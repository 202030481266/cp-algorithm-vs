using System;
using System.Collections.Generic;
using System.Linq;
using System.Windows;
using System.Windows.Media;
using EditorFontStyles.Configuration;
using EditorFontStyles.Diagnostics;
using Microsoft.VisualStudio.Text.Classification;
using Microsoft.VisualStudio.Text.Formatting;

namespace EditorFontStyles.Styling
{
    [Flags]
    internal enum FontProperty
    {
        None = 0,
        Italic = 1,
        Bold = 2,
        Typeface = 4,
        Size = 8,
        Decorations = 16,
        Opacity = 32,
        Effects = 64,
    }

    /// <summary>The font properties of a classification that styles can change; null means "not set".</summary>
    internal struct FontValues
    {
        public bool? Italic;
        public bool? Bold;
        public Typeface Typeface;
        public double? Size;
        public TextDecorationCollection Decorations;
        public double? Opacity;
        public TextEffectCollection Effects;

        public static FontValues Read(TextFormattingRunProperties p) => new FontValues
        {
            Italic = p.ItalicEmpty ? (bool?)null : p.Italic,
            Bold = p.BoldEmpty ? (bool?)null : p.Bold,
            Typeface = p.TypefaceEmpty ? null : p.Typeface,
            Size = p.FontRenderingEmSizeEmpty ? (double?)null : p.FontRenderingEmSize,
            Decorations = p.TextDecorationsEmpty ? null : p.TextDecorations,
            Opacity = p.ForegroundOpacityEmpty ? (double?)null : p.ForegroundOpacity,
            Effects = p.TextEffectsEmpty ? null : p.TextEffects,
        };

        public static FontProperty Differences(FontValues a, FontValues b, FontProperty mask)
        {
            var result = FontProperty.None;
            if (a.Italic != b.Italic)
            {
                result |= FontProperty.Italic;
            }

            if (a.Bold != b.Bold)
            {
                result |= FontProperty.Bold;
            }

            if (!SameTypeface(a.Typeface, b.Typeface))
            {
                result |= FontProperty.Typeface;
            }

            if (!Close(a.Size, b.Size))
            {
                result |= FontProperty.Size;
            }

            if (DecorationMask(a.Decorations) != DecorationMask(b.Decorations))
            {
                result |= FontProperty.Decorations;
            }

            if (!Close(a.Opacity, b.Opacity))
            {
                result |= FontProperty.Opacity;
            }

            if (EffectsKey(a.Effects) != EffectsKey(b.Effects))
            {
                result |= FontProperty.Effects;
            }

            return result & mask;
        }

        public TextFormattingRunProperties WriteTo(TextFormattingRunProperties p, FontProperty mask)
        {
            if ((mask & FontProperty.Italic) != 0)
            {
                p = Italic.HasValue ? p.SetItalic(Italic.Value) : p.ClearItalic();
            }

            if ((mask & FontProperty.Bold) != 0)
            {
                p = Bold.HasValue ? p.SetBold(Bold.Value) : p.ClearBold();
            }

            if ((mask & FontProperty.Typeface) != 0)
            {
                p = Typeface != null ? p.SetTypeface(Typeface) : p.ClearTypeface();
            }

            if ((mask & FontProperty.Size) != 0)
            {
                p = Size.HasValue
                    ? p.SetFontRenderingEmSize(Size.Value).SetFontHintingEmSize(Size.Value)
                    : p.ClearFontRenderingEmSize().ClearFontHintingEmSize();
            }

            if ((mask & FontProperty.Decorations) != 0)
            {
                p = Decorations != null ? p.SetTextDecorations(Decorations) : p.ClearTextDecorations();
            }

            if ((mask & FontProperty.Opacity) != 0)
            {
                p = Opacity.HasValue ? p.SetForegroundOpacity(Opacity.Value) : p.ClearForegroundOpacity();
            }

            if ((mask & FontProperty.Effects) != 0)
            {
                p = Effects != null ? p.SetTextEffects(Effects) : p.ClearTextEffects();
            }

            return p;
        }

        /// <summary>A copy with the properties in <paramref name="mask"/> taken from <paramref name="other"/>.</summary>
        public FontValues With(FontValues other, FontProperty mask)
        {
            var result = this;
            if ((mask & FontProperty.Italic) != 0)
            {
                result.Italic = other.Italic;
            }

            if ((mask & FontProperty.Bold) != 0)
            {
                result.Bold = other.Bold;
            }

            if ((mask & FontProperty.Typeface) != 0)
            {
                result.Typeface = other.Typeface;
            }

            if ((mask & FontProperty.Size) != 0)
            {
                result.Size = other.Size;
            }

            if ((mask & FontProperty.Decorations) != 0)
            {
                result.Decorations = other.Decorations;
            }

            if ((mask & FontProperty.Opacity) != 0)
            {
                result.Opacity = other.Opacity;
            }

            if ((mask & FontProperty.Effects) != 0)
            {
                result.Effects = other.Effects;
            }

            return result;
        }

        private static bool SameTypeface(Typeface a, Typeface b)
        {
            if (a == null || b == null)
            {
                return a == b;
            }

            return a.FontFamily.Source == b.FontFamily.Source && a.Style == b.Style && a.Weight == b.Weight && a.Stretch == b.Stretch;
        }

        private static bool Close(double? a, double? b) =>
            a.HasValue == b.HasValue && (!a.HasValue || Math.Abs(a.Value - b.Value) < 1e-6);

        /// <summary>Compares effect collections by their transforms (the only effect this extension writes).</summary>
        private static string EffectsKey(TextEffectCollection effects)
        {
            if (effects == null)
            {
                return null;
            }

            return string.Join(";", effects.Select(e => e.Transform == null ? "-" : e.Transform.Value.ToString(System.Globalization.CultureInfo.InvariantCulture)));
        }

        private static int DecorationMask(TextDecorationCollection decorations)
        {
            if (decorations == null)
            {
                return -1;
            }

            int mask = 0;
            foreach (var decoration in decorations)
            {
                mask |= 1 << (int)decoration.Location;
            }

            return mask;
        }
    }

    /// <summary>
    /// Applies the "font" and "styles" of a configuration to one classification format map (the "text" map
    /// shared by code editors). The font replaces the default text properties; styles change only the explicit
    /// properties of matching classifications; colors stay with Fonts and Colors. When Fonts and Colors or a
    /// theme switch rewrites the font or an item, the configuration is applied again, and when the font or a
    /// style is removed the value from before is restored.
    /// </summary>
    internal sealed class FormatMapStyler
    {
        private const double DipsPerPoint = 96.0 / 72.0;

        /// <summary>
        /// Italic runs are drawn 0.01% larger than the text around them. WPF shapes adjacent runs of the same
        /// size and "same" font together, and it treats the regular and italic faces of one TrueType collection
        /// (.ttc, e.g. Sarasa Mono SC) as the same font: the italic run then gets the glyph numbers of the
        /// regular face and "std" after a keyword is drawn as "OPB". A different size keeps the run separate.
        /// </summary>
        private const double ItalicSizeNudge = 1.0001;

        private readonly OutputLog log;
        private readonly IReadOnlyList<IClassificationType> topTypes;
        private readonly Dictionary<IClassificationType, Managed> managed = new Dictionary<IClassificationType, Managed>();
        private readonly Managed editorFont = new Managed();
        private StyleConfig config;
        private bool applying;
        private bool listed;

        /// <param name="topTypes">
        /// Classifications kept above all others, so that their bold/italic wins over the explicit "not bold"
        /// that Fonts and Colors stores for every item (C++ parameters would otherwise stay regular).
        /// </param>
        public FormatMapStyler(IClassificationFormatMap map, IReadOnlyList<IClassificationType> topTypes, OutputLog log)
        {
            Map = map;
            this.topTypes = topTypes;
            this.log = log;
            map.ClassificationFormatMappingChanged += OnMappingChanged;
        }

        public IClassificationFormatMap Map { get; }

        public void Apply(StyleConfig newConfig)
        {
            config = newConfig;
            listed = false;
            Reapply();
        }

        private void OnMappingChanged(object sender, EventArgs e)
        {
            if (!applying)
            {
                Reapply();  // new classifications registered, Fonts and Colors changed or the theme switched
            }
        }

        private void Reapply()
        {
            if (config == null)
            {
                return;
            }

            applying = true;
            bool batch = !Map.IsInBatchUpdate;
            if (batch)
            {
                Map.BeginBatchUpdate();
            }

            try
            {
                ApplyEditorFont();  // first: style sizes are relative to the editor font
                foreach (var type in Map.CurrentPriorityOrder)
                {
                    if (type != null)
                    {
                        Restyle(type);
                    }
                }

                KeepTopTypesOnTop();
            }
            finally
            {
                if (batch)
                {
                    Map.EndBatchUpdate();
                }

                applying = false;
            }

            if (config.ListClassifications && !listed)
            {
                listed = true;
                var names = Map.CurrentPriorityOrder.Where(t => t != null)
                    .Select((t, i) => $"{i,4}  {Map.GetEditorFormatMapKey(t)} ({t.Classification})");
                log.WriteLine("Classifications, lowest priority first:" + Environment.NewLine + string.Join(Environment.NewLine, names));
            }
        }

        /// <summary>Moves each top type to the end of the priority order (highest priority) with adjacent swaps.</summary>
        private void KeepTopTypesOnTop()
        {
            var order = Map.CurrentPriorityOrder.Where(t => t != null).ToList();
            int expected = order.Count - topTypes.Count;
            bool onTop = expected >= 0;
            for (int i = 0; onTop && i < topTypes.Count; i++)
            {
                onTop = order[expected + i] == topTypes[i];
            }

            if (onTop)
            {
                return;
            }

            foreach (var type in topTypes)
            {
                order = Map.CurrentPriorityOrder.Where(t => t != null).ToList();
                for (int i = order.IndexOf(type); i >= 0 && i + 1 < order.Count; i++)
                {
                    Map.SwapPriorities(type, order[i + 1]);
                }
            }
        }

        private void ApplyEditorFont()
        {
            var desired = default(FontValues);
            var mask = FontProperty.None;
            if (config.FontFamily != null && FontCatalog.IsInstalled(config.FontFamily, log))
            {
                desired.Typeface = new Typeface(new FontFamily(config.FontFamily), FontStyles.Normal, FontWeights.Normal, FontStretches.Normal);
                mask |= FontProperty.Typeface;
            }

            if (config.FontSize.HasValue)
            {
                desired.Size = config.FontSize.Value * DipsPerPoint;
                mask |= FontProperty.Size;
            }

            if (mask != FontProperty.None || editorFont.Mask != FontProperty.None)
            {
                Update(editorFont, desired, mask, Map.DefaultTextProperties, p => Map.DefaultTextProperties = p);
            }
        }

        private void Restyle(IClassificationType type)
        {
            var style = config.ResolveStyle(Map.GetEditorFormatMapKey(type), type.Classification);
            managed.TryGetValue(type, out var state);
            if (style == null && state == null)
            {
                return;
            }

            var desired = Desired(style, out var mask);
            state = state ?? new Managed();
            Update(state, desired, mask, Map.GetExplicitTextProperties(type), p => Map.SetExplicitTextProperties(type, p));
            if (mask == FontProperty.None)
            {
                managed.Remove(type);
            }
            else
            {
                managed[type] = state;
            }
        }

        /// <summary>Writes the <paramref name="mask"/> properties of <paramref name="desired"/> and restores the ones no longer set.</summary>
        private static void Update(Managed state, FontValues desired, FontProperty mask, TextFormattingRunProperties properties, Action<TextFormattingRunProperties> write)
        {
            var current = FontValues.Read(properties);

            // Values someone else changed after we wrote them (Fonts and Colors, a theme) become the originals.
            state.Original = state.Original.With(current, FontValues.Differences(current, state.Applied, state.Mask));
            state.Original = state.Original.With(current, mask & ~state.Mask);
            var released = state.Mask & ~mask;
            var target = desired.With(state.Original, released);
            var changed = mask | released;
            if (FontValues.Differences(current, target, changed) != FontProperty.None)
            {
                write(target.WriteTo(properties, changed));
            }

            state.Applied = desired;
            state.Mask = mask;
        }

        private FontValues Desired(TextStyle style, out FontProperty mask)
        {
            var values = default(FontValues);
            mask = FontProperty.None;
            if (style == null)
            {
                return values;
            }

            if (style.Italic.HasValue)
            {
                values.Italic = style.Italic;
                mask |= FontProperty.Italic;
            }

            if (style.Bold.HasValue)
            {
                values.Bold = style.Bold;
                mask |= FontProperty.Bold;
            }

            if (style.FontFamily != null && FontCatalog.IsInstalled(style.FontFamily, log))
            {
                values.Typeface = new Typeface(new FontFamily(style.FontFamily), FontStyles.Normal, FontWeights.Normal, FontStretches.Normal);
                mask |= FontProperty.Typeface;
            }

            bool italic = style.Italic == true && style.Oblique != true;
            if (style.FontScale.HasValue || italic)
            {
                double scale = (style.FontScale ?? 1.0) * (italic ? ItalicSizeNudge : 1.0);
                values.Size = Map.DefaultTextProperties.FontRenderingEmSize * scale;
                mask |= FontProperty.Size;
            }

            if (style.Underline.HasValue || style.Strikethrough.HasValue)
            {
                var decorations = new TextDecorationCollection();
                if (style.Underline == true)
                {
                    foreach (var decoration in TextDecorations.Underline)
                    {
                        decorations.Add(decoration);
                    }
                }

                if (style.Strikethrough == true)
                {
                    foreach (var decoration in TextDecorations.Strikethrough)
                    {
                        decorations.Add(decoration);
                    }
                }

                decorations.Freeze();
                values.Decorations = decorations;
                mask |= FontProperty.Decorations;
            }

            if (style.Opacity.HasValue)
            {
                values.Opacity = style.Opacity;
                mask |= FontProperty.Opacity;
            }

            if (style.Oblique.HasValue)
            {
                values.Effects = style.Oblique.Value ? ObliqueEffects() : null;
                mask |= FontProperty.Effects;
                if (style.Oblique.Value)
                {
                    values.Italic = false;  // slant the regular glyphs; the italic face is not used at all
                    mask |= FontProperty.Italic;
                }
            }

            return values;
        }

        /// <summary>
        /// Slants text by 12 degrees around its baseline, like an oblique font. The glyphs stay those of the
        /// regular face, so fonts whose italic face renders wrongly in Visual Studio (Sarasa Mono at some
        /// sizes shows "std" as "OPB") still read correctly, and columns stay aligned.
        /// </summary>
        private TextEffectCollection ObliqueEffects()
        {
            var text = Map.DefaultTextProperties;
            double baseline = text.Typeface.FontFamily.Baseline * text.FontRenderingEmSize;
            var effect = new TextEffect
            {
                Transform = new SkewTransform(-12, 0, 0, baseline),
                PositionStart = 0,
                PositionCount = int.MaxValue,
            };
            var effects = new TextEffectCollection { effect };
            effects.Freeze();
            return effects;
        }

        private sealed class Managed
        {
            public FontValues Original;  // values before this extension changed them
            public FontValues Applied;   // values this extension wrote last
            public FontProperty Mask;    // properties this extension controls
        }
    }

    internal static class FontCatalog
    {
        private static readonly HashSet<string> Warned = new HashSet<string>(StringComparer.OrdinalIgnoreCase);
        private static HashSet<string> installed;

        public static bool IsInstalled(string family, OutputLog log)
        {
            if (installed == null)
            {
                installed = new HashSet<string>(StringComparer.OrdinalIgnoreCase);
                foreach (var font in Fonts.SystemFontFamilies)
                {
                    installed.Add(font.Source);
                    foreach (string name in font.FamilyNames.Values)
                    {
                        installed.Add(name);
                    }
                }
            }

            if (installed.Contains(family))
            {
                return true;
            }

            if (Warned.Add(family))
            {
                log.WriteLine($"Font \"{family}\" is not installed; the editor font is used instead.");
            }

            return false;
        }
    }
}
