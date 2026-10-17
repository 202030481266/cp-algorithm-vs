using System;
using System.Collections;
using System.Collections.Generic;
using System.Linq;
using System.Windows;
using System.Windows.Media;
using EditorFontStyles.Configuration;
using EditorFontStyles.Diagnostics;
using Microsoft.VisualStudio.Shell;
using Microsoft.VisualStudio.Text.Classification;

namespace EditorFontStyles.Styling
{
    /// <summary>Foreground, background and bold of an editor format entry; null means "not set" (Automatic).</summary>
    internal struct ColorValues
    {
        public Color? Foreground;
        public Color? Background;
        public bool? Bold;

        public static ColorValues Read(ResourceDictionary properties) => new ColorValues
        {
            Foreground = ColorOf(properties, EditorFormatDefinition.ForegroundColorId, EditorFormatDefinition.ForegroundBrushId),
            Background = ColorOf(properties, EditorFormatDefinition.BackgroundColorId, EditorFormatDefinition.BackgroundBrushId),
            Bold = properties.Contains(ClassificationFormatDefinition.IsBoldId) && properties[ClassificationFormatDefinition.IsBoldId] is bool bold
                ? bold
                : (bool?)null,
        };

        public bool SameColors(ColorValues other) => Foreground == other.Foreground && Background == other.Background;

        public bool Same(ColorValues other) => SameColors(other) && Bold == other.Bold;

        /// <summary>A copy with the values a .vssettings item defines.</summary>
        public ColorValues With(ThemeItem item)
        {
            var result = this;
            if (item.HasForeground)
            {
                result.Foreground = ToColor(item.Foreground);
            }

            if (item.HasBackground)
            {
                result.Background = ToColor(item.Background);
            }

            if (item.Bold.HasValue)
            {
                result.Bold = item.Bold;
            }

            return result;
        }

        /// <summary>A copy of <paramref name="current"/> with these values; italic, typeface and the rest are kept.</summary>
        public ResourceDictionary WriteTo(ResourceDictionary current)
        {
            var result = new ResourceDictionary();
            foreach (DictionaryEntry entry in current)
            {
                result[entry.Key] = entry.Value;
            }

            Set(result, EditorFormatDefinition.ForegroundColorId, EditorFormatDefinition.ForegroundBrushId, Foreground);
            Set(result, EditorFormatDefinition.BackgroundColorId, EditorFormatDefinition.BackgroundBrushId, Background);
            if (Bold.HasValue)
            {
                result[ClassificationFormatDefinition.IsBoldId] = Bold.Value;
            }
            else
            {
                result.Remove(ClassificationFormatDefinition.IsBoldId);
            }

            return result;
        }

        private static Color? ToColor(uint? rgb) =>
            rgb.HasValue ? Color.FromRgb((byte)(rgb.Value >> 16), (byte)(rgb.Value >> 8), (byte)rgb.Value) : (Color?)null;

        private static Color? ColorOf(ResourceDictionary properties, string colorKey, string brushKey)
        {
            if (properties.Contains(colorKey) && properties[colorKey] is Color color)
            {
                return color;
            }

            return properties.Contains(brushKey) && properties[brushKey] is SolidColorBrush brush ? brush.Color : (Color?)null;
        }

        private static void Set(ResourceDictionary properties, string colorKey, string brushKey, Color? color)
        {
            if (color.HasValue)
            {
                var brush = new SolidColorBrush(color.Value);
                brush.Freeze();
                properties[colorKey] = color.Value;
                properties[brushKey] = brush;
            }
            else
            {
                properties.Remove(colorKey);
                properties.Remove(brushKey);
            }
        }
    }

    /// <summary>
    /// Applies the Text Editor colors of the "colors" .vssettings files to an editor format map, the way
    /// importing the file would, but only in memory: Fonts and Colors keeps its own values. When Fonts and
    /// Colors rewrites an item (dialog, theme switch, settings import) the theme color is put back, and when
    /// the theme stops naming an item its previous value is restored.
    /// </summary>
    internal sealed class ColorApplier
    {
        private readonly Dictionary<string, Managed> managed = new Dictionary<string, Managed>(StringComparer.OrdinalIgnoreCase);
        private ColorTheme theme = ColorTheme.Empty;
        private bool applying;
        private bool scheduled;

        public ColorApplier(IEditorFormatMap map)
        {
            Map = map;
            map.FormatMappingChanged += OnFormatMappingChanged;
        }

        public IEditorFormatMap Map { get; }

        /// <summary>Applies <paramref name="newTheme"/>; returns the items that had no value in this map (unknown names).</summary>
        public List<string> Apply(ColorTheme newTheme)
        {
            theme = newTheme;
            return Reapply();
        }

        private void OnFormatMappingChanged(object sender, FormatItemsEventArgs e)
        {
            if (applying || scheduled || managed.Count == 0)
            {
                return;
            }

            foreach (string key in e.ChangedItems)
            {
                // Only color changes count: bold is also written by the styles, which win over the theme.
                if (managed.TryGetValue(key, out var state) && !state.Applied.SameColors(ColorValues.Read(Map.GetProperties(key))))
                {
                    scheduled = true;
                    ExtensionThreading.Factory.RunAsync(async () =>
                    {
                        await ExtensionThreading.Factory.SwitchToMainThreadAsync(alwaysYield: true);
                        scheduled = false;
                        Reapply();
                    }).FileAndForget("EditorFontStyles/ColorReapply");
                    return;
                }
            }
        }

        private List<string> Reapply()
        {
            var unknown = new List<string>();
            applying = true;
            bool batch = !Map.IsInBatchUpdate;
            if (batch)
            {
                Map.BeginBatchUpdate();
            }

            try
            {
                foreach (string key in managed.Keys.Where(key => !theme.Items.ContainsKey(key)).ToList())
                {
                    Restore(key);
                }

                foreach (var item in theme.Items.Values)
                {
                    if (!ApplyItem(item))
                    {
                        unknown.Add(item.Name);
                    }
                }
            }
            finally
            {
                if (batch)
                {
                    Map.EndBatchUpdate();
                }

                applying = false;
            }

            return unknown;
        }

        private bool ApplyItem(ThemeItem item)
        {
            var properties = Map.GetProperties(item.Name);
            bool known = properties.Count > 0 || managed.ContainsKey(item.Name);
            var current = ColorValues.Read(properties);
            if (!managed.TryGetValue(item.Name, out var state))
            {
                state = new Managed { Original = current };
                managed.Add(item.Name, state);
            }
            else if (!state.Applied.SameColors(current))
            {
                state.Original = current;  // Fonts and Colors rewrote the item after we did
            }

            var target = state.Original.With(item);
            if (!target.Same(current))
            {
                Map.SetProperties(item.Name, target.WriteTo(properties));
            }

            state.Applied = target;
            return known;
        }

        private void Restore(string key)
        {
            var state = managed[key];
            managed.Remove(key);
            var properties = Map.GetProperties(key);
            var current = ColorValues.Read(properties);
            if (state.Applied.SameColors(current) && !state.Original.Same(current))
            {
                Map.SetProperties(key, state.Original.WriteTo(properties));
            }
        }

        private sealed class Managed
        {
            public ColorValues Original;  // values before the theme was applied
            public ColorValues Applied;   // values the theme wrote
        }
    }
}
