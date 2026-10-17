using System;
using System.Collections.Generic;
using System.Globalization;
using System.IO;
using System.Linq;
using System.Xml;
using System.Xml.Linq;

namespace EditorFontStyles.Configuration
{
    /// <summary>One Fonts and Colors item of a .vssettings file. Colors are 0xRRGGBB; null means "Automatic".</summary>
    internal sealed class ThemeItem
    {
        public ThemeItem(string name, bool hasForeground, uint? foreground, bool hasBackground, uint? background, bool? bold)
        {
            Name = name;
            HasForeground = hasForeground;
            Foreground = foreground;
            HasBackground = hasBackground;
            Background = background;
            Bold = bold;
        }

        public string Name { get; }
        public bool HasForeground { get; }
        public uint? Foreground { get; }
        public bool HasBackground { get; }
        public uint? Background { get; }
        public bool? Bold { get; }
    }

    /// <summary>
    /// The Text Editor colors of one or more .vssettings files (the "colors" setting). Later files override
    /// items of earlier ones. Items of other Fonts and Colors categories (Output window, printer) are counted
    /// but not used.
    /// </summary>
    internal sealed class ColorTheme
    {
        public const string TextEditorCategory = "{A27B4E24-A735-4D1D-B8E7-9716E1E3D8E0}";

        public static readonly ColorTheme Empty = new ColorTheme(Array.Empty<string>(), new Dictionary<string, ThemeItem>(StringComparer.OrdinalIgnoreCase), 0);

        private ColorTheme(IReadOnlyList<string> files, Dictionary<string, ThemeItem> items, int otherCategoryItems)
        {
            Files = files;
            Items = items;
            OtherCategoryItems = otherCategoryItems;
        }

        public IReadOnlyList<string> Files { get; }

        /// <summary>Items by Fonts and Colors name, case-insensitive.</summary>
        public IReadOnlyDictionary<string, ThemeItem> Items { get; }

        public int OtherCategoryItems { get; }

        public static ColorTheme Load(IReadOnlyList<string> files, ICollection<string> problems)
        {
            if (files.Count == 0)
            {
                return Empty;
            }

            var items = new Dictionary<string, ThemeItem>(StringComparer.OrdinalIgnoreCase);
            int other = 0;
            foreach (string file in files)
            {
                string xml;
                try
                {
                    xml = File.ReadAllText(file);
                }
                catch (Exception e) when (e is IOException || e is UnauthorizedAccessException || e is ArgumentException || e is NotSupportedException)
                {
                    problems.Add($"{file}: cannot read the colors file ({e.Message}).");
                    continue;
                }

                other += Parse(xml, file, items, problems);
            }

            return new ColorTheme(files, items, other);
        }

        /// <summary>Adds the Text Editor items of <paramref name="xml"/>; returns the number of items of other categories.</summary>
        public static int Parse(string xml, string origin, Dictionary<string, ThemeItem> items, ICollection<string> problems)
        {
            XDocument document;
            try
            {
                document = XDocument.Parse(xml);
            }
            catch (XmlException e)
            {
                problems.Add($"{origin}: not a valid .vssettings file ({e.Message}).");
                return 0;
            }

            int other = 0;
            bool found = false;
            foreach (var category in document.Descendants("FontsAndColors").Elements("Categories").Elements("Category"))
            {
                var list = category.Element("Items")?.Elements("Item").ToList() ?? new List<XElement>();
                string guid = ((string)category.Attribute("GUID") ?? string.Empty).Trim();
                if (!string.Equals(guid.Trim('{', '}'), TextEditorCategory.Trim('{', '}'), StringComparison.OrdinalIgnoreCase))
                {
                    other += list.Count;
                    continue;
                }

                found = true;
                foreach (var element in list)
                {
                    string name = (string)element.Attribute("Name");
                    if (string.IsNullOrWhiteSpace(name))
                    {
                        continue;
                    }

                    var foreground = ReadColor(element, "Foreground", origin, name, problems, out bool hasForeground);
                    var background = ReadColor(element, "Background", origin, name, problems, out bool hasBackground);
                    string bold = (string)element.Attribute("BoldFont");
                    bool? isBold = bold == null ? (bool?)null : string.Equals(bold, "Yes", StringComparison.OrdinalIgnoreCase);
                    items[name] = new ThemeItem(name, hasForeground, foreground, hasBackground, background, isBold);
                }
            }

            if (!found)
            {
                problems.Add($"{origin}: no Text Editor colors (Fonts and Colors category {TextEditorCategory}) in this file.");
            }

            return other;
        }

        /// <summary>
        /// Decodes a .vssettings color: 0x00BBGGRR is an RGB value, any other high byte (0x02000000 "Automatic",
        /// system colors) means the item takes its color from elsewhere.
        /// </summary>
        public static bool TryDecodeColor(string text, out uint? rgb)
        {
            rgb = null;
            string digits = text.Trim();
            if (digits.StartsWith("0x", StringComparison.OrdinalIgnoreCase))
            {
                digits = digits.Substring(2);
            }

            if (!uint.TryParse(digits, NumberStyles.HexNumber, CultureInfo.InvariantCulture, out uint value))
            {
                return false;
            }

            if (value >> 24 == 0)
            {
                uint red = value & 0xFF;
                uint green = (value >> 8) & 0xFF;
                uint blue = (value >> 16) & 0xFF;
                rgb = (red << 16) | (green << 8) | blue;
            }

            return true;
        }

        private static uint? ReadColor(XElement item, string attribute, string origin, string name, ICollection<string> problems, out bool present)
        {
            string text = (string)item.Attribute(attribute);
            present = text != null;
            if (!present)
            {
                return null;
            }

            if (TryDecodeColor(text, out uint? rgb))
            {
                return rgb;
            }

            present = false;
            problems.Add($"{origin}: {name} {attribute}=\"{text}\" is not a color such as 0x00BBGGRR.");
            return null;
        }
    }
}
