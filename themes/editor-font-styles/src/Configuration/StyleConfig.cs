using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text.RegularExpressions;
using EditorFontStyles.Cpp;
using Newtonsoft.Json;
using Newtonsoft.Json.Linq;

namespace EditorFontStyles.Configuration
{
    /// <summary>Font changes for one Fonts and Colors item; null members leave the item unchanged.</summary>
    internal sealed class TextStyle
    {
        public bool? Italic;
        public bool? Bold;
        public string FontFamily;
        public double? FontScale;
        public bool? Underline;
        public bool? Strikethrough;
        public double? Opacity;

        /// <summary>Slant the regular glyphs instead of using the font's italic face.</summary>
        public bool? Oblique;

        public bool IsEmpty => Italic == null && Bold == null && FontFamily == null && FontScale == null
            && Underline == null && Strikethrough == null && Opacity == null && Oblique == null;

        public void OverrideWith(TextStyle other)
        {
            Italic = other.Italic ?? Italic;
            Bold = other.Bold ?? Bold;
            FontFamily = other.FontFamily ?? FontFamily;
            FontScale = other.FontScale ?? FontScale;
            Underline = other.Underline ?? Underline;
            Strikethrough = other.Strikethrough ?? Strikethrough;
            Opacity = other.Opacity ?? Opacity;
            Oblique = other.Oblique ?? Oblique;
        }
    }

    /// <summary>A "styles" entry: an item name (case-insensitive, * and ? wildcards) and its style.</summary>
    internal sealed class StyleRule
    {
        private readonly Regex pattern;

        public StyleRule(string name, TextStyle style)
        {
            Name = name;
            Style = style;
            if (name.IndexOf('*') >= 0 || name.IndexOf('?') >= 0)
            {
                string regex = "^" + Regex.Escape(name).Replace(@"\*", ".*").Replace(@"\?", ".") + "$";
                pattern = new Regex(regex, RegexOptions.IgnoreCase | RegexOptions.CultureInvariant);
            }
        }

        public string Name { get; }
        public TextStyle Style { get; }

        /// <summary>Matches the Fonts and Colors item name (CppFunctionSemanticTokenFormat) or the classification name (cppFunction).</summary>
        public bool Matches(string formatKey, string classification)
        {
            if (pattern != null)
            {
                return (formatKey != null && pattern.IsMatch(formatKey)) || (classification != null && pattern.IsMatch(classification));
            }

            return string.Equals(Name, formatKey, StringComparison.OrdinalIgnoreCase)
                || string.Equals(Name, classification, StringComparison.OrdinalIgnoreCase);
        }
    }

    /// <summary>
    /// The built-in defaults (resources/default-config.json) overlaid with a user's .editor-font-styles.json:
    /// "styles" entries merge property by property (null removes an entry), sections merge key by key.
    /// </summary>
    internal sealed class StyleConfig
    {
        public const string FileName = ".editor-font-styles.json";

        private static readonly string[] Sections = { "colors", "styles", "systemSymbols", "localSymbols", "commentTags", "includeDirectories", "listClassifications", "$schema" };

        private ISet<string> systemNames;

        private StyleConfig()
        {
        }

        /// <summary>Path of the user file, or null for the built-in defaults.</summary>
        public string SourcePath { get; private set; }

        /// <summary>Absolute paths of the .vssettings files whose Text Editor colors are applied, in order.</summary>
        public IReadOnlyList<string> ColorFiles { get; private set; } = Array.Empty<string>();

        public IReadOnlyList<StyleRule> Rules { get; private set; } = Array.Empty<StyleRule>();
        public bool SystemSymbols { get; private set; }
        public IReadOnlyList<string> ExtraSystemNames { get; private set; } = Array.Empty<string>();
        public IReadOnlyList<string> ExcludedSystemNames { get; private set; } = Array.Empty<string>();
        public bool LocalSymbols { get; private set; }
        public bool CommentTags { get; private set; }
        public IReadOnlyList<string> CommentTagWords { get; private set; } = Array.Empty<string>();

        /// <summary>Absolute directories searched for quoted includes after the document's own directory.</summary>
        public IReadOnlyList<string> IncludeDirectories { get; private set; } = Array.Empty<string>();
        public bool ListClassifications { get; private set; }

        public ISet<string> SystemNames => systemNames ?? (systemNames = SystemSymbolTable.Compose(
            ExtraSystemNames.ToList(), ExcludedSystemNames.ToList()));

        public string Description => SourcePath ?? "built-in defaults";

        /// <summary>Merged style of all rules matching an item, later rules winning; null when none matches.</summary>
        public TextStyle ResolveStyle(string formatKey, string classification)
        {
            TextStyle result = null;
            foreach (var rule in Rules)
            {
                if (rule.Matches(formatKey, classification))
                {
                    (result ?? (result = new TextStyle())).OverrideWith(rule.Style);
                }
            }

            return result;
        }

        public static StyleConfig Load(string defaultJson, string userJson, string sourcePath, ICollection<string> problems)
        {
            var defaults = Parse(defaultJson, "built-in defaults", problems) ?? new JObject();
            var user = userJson == null ? null : Parse(userJson, sourcePath, problems);
            return FromJson(Merge(defaults, user), sourcePath, problems);
        }

        private static JObject Parse(string json, string origin, ICollection<string> problems)
        {
            try
            {
                var settings = new JsonLoadSettings { CommentHandling = CommentHandling.Ignore, LineInfoHandling = LineInfoHandling.Load };
                return JObject.Parse(json, settings);
            }
            catch (JsonException e)
            {
                problems.Add($"{origin}: {e.Message} The file is ignored until the error is fixed.");
                return null;
            }
        }

        private static JObject Merge(JObject defaults, JObject user)
        {
            var result = (JObject)defaults.DeepClone();
            if (user == null)
            {
                return result;
            }

            foreach (var property in user.Properties())
            {
                if (property.Name == "styles" && property.Value is JObject userStyles)
                {
                    if (!(result["styles"] is JObject styles))
                    {
                        result["styles"] = styles = new JObject();
                    }

                    foreach (var style in userStyles.Properties())
                    {
                        if (style.Value.Type == JTokenType.Null)
                        {
                            styles.Remove(style.Name);
                        }
                        else if (styles[style.Name] is JObject existing && style.Value is JObject changes)
                        {
                            foreach (var change in changes.Properties())
                            {
                                existing[change.Name] = change.Value.DeepClone();
                            }
                        }
                        else
                        {
                            styles[style.Name] = style.Value.DeepClone();
                        }
                    }
                }
                else if (property.Value is JObject section && result[property.Name] is JObject existingSection)
                {
                    foreach (var change in section.Properties())
                    {
                        existingSection[change.Name] = change.Value.DeepClone();
                    }
                }
                else
                {
                    result[property.Name] = property.Value.DeepClone();
                }
            }

            return result;
        }

        private static StyleConfig FromJson(JObject json, string sourcePath, ICollection<string> problems)
        {
            string origin = sourcePath ?? "built-in defaults";
            var config = new StyleConfig { SourcePath = sourcePath };
            foreach (var property in json.Properties())
            {
                if (!Sections.Contains(property.Name))
                {
                    problems.Add($"{origin}: unknown setting \"{property.Name}\" is ignored.");
                }
            }

            var rules = new List<StyleRule>();
            if (json["styles"] is JObject styles)
            {
                foreach (var entry in styles.Properties())
                {
                    if (entry.Value is JObject body)
                    {
                        var style = ReadStyle(body, $"{origin}: styles.\"{entry.Name}\"", problems);
                        if (!style.IsEmpty)
                        {
                            rules.Add(new StyleRule(entry.Name, style));
                        }
                    }
                    else
                    {
                        problems.Add($"{origin}: styles.\"{entry.Name}\" must be an object such as {{ \"italic\": true }}.");
                    }
                }
            }

            config.Rules = rules;
            config.ColorFiles = ResolveColorFiles(json["colors"], sourcePath, origin, problems);
            var system = json["systemSymbols"] as JObject;
            config.SystemSymbols = ReadBool(system, "enabled", true, origin, problems);
            config.ExtraSystemNames = ReadStrings(system, "extraNames", origin, problems);
            config.ExcludedSystemNames = ReadStrings(system, "excludeNames", origin, problems);
            config.LocalSymbols = ReadBool(json["localSymbols"] as JObject, "enabled", true, origin, problems);
            var tags = json["commentTags"] as JObject;
            config.CommentTags = ReadBool(tags, "enabled", true, origin, problems);
            config.CommentTagWords = ReadStrings(tags, "words", origin, problems);
            config.ListClassifications = ReadBool(json, "listClassifications", false, origin, problems);
            config.IncludeDirectories = ResolveDirectories(ReadStrings(json, "includeDirectories", origin, problems), sourcePath, origin, problems);
            return config;
        }

        private static TextStyle ReadStyle(JObject body, string origin, ICollection<string> problems)
        {
            var style = new TextStyle();
            foreach (var property in body.Properties())
            {
                var value = property.Value;
                switch (property.Name)
                {
                    case "italic":
                        style.Italic = Bool(value, origin, property.Name, problems);
                        break;
                    case "bold":
                        style.Bold = Bool(value, origin, property.Name, problems);
                        break;
                    case "underline":
                        style.Underline = Bool(value, origin, property.Name, problems);
                        break;
                    case "strikethrough":
                        style.Strikethrough = Bool(value, origin, property.Name, problems);
                        break;
                    case "fontFamily":
                        if (value.Type == JTokenType.String && ((string)value).Trim().Length > 0)
                        {
                            style.FontFamily = ((string)value).Trim();
                        }
                        else if (value.Type != JTokenType.Null)
                        {
                            problems.Add($"{origin}.fontFamily must be a font name such as \"Cascadia Code\".");
                        }

                        break;
                    case "fontScale":
                        style.FontScale = Number(value, 0.25, 4, origin, property.Name, problems);
                        break;
                    case "opacity":
                        style.Opacity = Number(value, 0, 1, origin, property.Name, problems);
                        break;
                    case "oblique":
                        style.Oblique = Bool(value, origin, property.Name, problems);
                        break;
                    default:
                        problems.Add($"{origin}: unknown property \"{property.Name}\" (use italic, oblique, bold, fontFamily, fontScale, underline, strikethrough or opacity).");
                        break;
                }
            }

            return style;
        }

        private static bool? Bool(JToken value, string origin, string name, ICollection<string> problems)
        {
            if (value.Type == JTokenType.Boolean)
            {
                return (bool)value;
            }

            if (value.Type != JTokenType.Null)
            {
                problems.Add($"{origin}.{name} must be true or false.");
            }

            return null;
        }

        private static double? Number(JToken value, double min, double max, string origin, string name, ICollection<string> problems)
        {
            if ((value.Type == JTokenType.Float || value.Type == JTokenType.Integer) && (double)value >= min && (double)value <= max)
            {
                return (double)value;
            }

            if (value.Type != JTokenType.Null)
            {
                problems.Add($"{origin}.{name} must be a number from {min} to {max}.");
            }

            return null;
        }

        private static bool ReadBool(JObject section, string name, bool fallback, string origin, ICollection<string> problems)
        {
            var value = section?[name];
            if (value == null || value.Type == JTokenType.Null)
            {
                return fallback;
            }

            if (value.Type == JTokenType.Boolean)
            {
                return (bool)value;
            }

            problems.Add($"{origin}: {name} must be true or false.");
            return fallback;
        }

        private static IReadOnlyList<string> ReadStrings(JObject section, string name, string origin, ICollection<string> problems)
        {
            var value = section?[name];
            if (value == null || value.Type == JTokenType.Null)
            {
                return Array.Empty<string>();
            }

            if (value is JArray array && array.All(item => item.Type == JTokenType.String))
            {
                return array.Select(item => (string)item).Where(item => item.Length > 0).ToList();
            }

            problems.Add($"{origin}: {name} must be a list of strings.");
            return Array.Empty<string>();
        }

        private static IReadOnlyList<string> ResolveColorFiles(JToken value, string sourcePath, string origin, ICollection<string> problems)
        {
            var entries = new List<string>();
            if (value == null || value.Type == JTokenType.Null)
            {
                return entries;
            }

            if (value.Type == JTokenType.String)
            {
                entries.Add((string)value);
            }
            else if (value is JArray array && array.All(item => item.Type == JTokenType.String))
            {
                entries.AddRange(array.Select(item => (string)item));
            }
            else
            {
                problems.Add($"{origin}: colors must be a .vssettings path or a list of paths.");
                return Array.Empty<string>();
            }

            var result = new List<string>();
            string baseDirectory = sourcePath == null ? null : Path.GetDirectoryName(sourcePath);
            foreach (string entry in entries.Where(e => e.Trim().Length > 0))
            {
                try
                {
                    string expanded = Environment.ExpandEnvironmentVariables(entry.Trim());
                    if (!Path.IsPathRooted(expanded) && baseDirectory == null)
                    {
                        problems.Add($"{origin}: colors \"{entry}\" must be an absolute path here.");
                        continue;
                    }

                    string full = Path.GetFullPath(Path.IsPathRooted(expanded) ? expanded : Path.Combine(baseDirectory, expanded));
                    if (!File.Exists(full))
                    {
                        problems.Add($"{origin}: colors file \"{entry}\" does not exist (yet); it is used as soon as it does.");
                    }

                    result.Add(full);
                }
                catch (Exception e) when (e is ArgumentException || e is NotSupportedException || e is IOException)
                {
                    problems.Add($"{origin}: colors \"{entry}\" is not a valid path.");
                }
            }

            return result;
        }

        private static IReadOnlyList<string> ResolveDirectories(IReadOnlyList<string> entries, string sourcePath, string origin, ICollection<string> problems)
        {
            var result = new List<string>();
            string baseDirectory = sourcePath == null ? null : Path.GetDirectoryName(sourcePath);
            foreach (string entry in entries)
            {
                try
                {
                    string expanded = Environment.ExpandEnvironmentVariables(entry);
                    if (!Path.IsPathRooted(expanded) && baseDirectory == null)
                    {
                        continue;
                    }

                    string full = Path.GetFullPath(Path.IsPathRooted(expanded) ? expanded : Path.Combine(baseDirectory, expanded));
                    if (Directory.Exists(full))
                    {
                        result.Add(full);
                    }
                    else
                    {
                        problems.Add($"{origin}: include directory \"{entry}\" does not exist.");
                    }
                }
                catch (Exception e) when (e is ArgumentException || e is NotSupportedException || e is IOException)
                {
                    problems.Add($"{origin}: include directory \"{entry}\" is not a valid path.");
                }
            }

            return result;
        }
    }
}
