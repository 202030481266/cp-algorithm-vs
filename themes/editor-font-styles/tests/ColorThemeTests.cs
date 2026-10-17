using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using EditorFontStyles.Configuration;

namespace EditorFontStyles.Tests
{
    /// <summary>Tests of .vssettings color parsing (src/Configuration/ColorTheme.cs) and the "colors" setting.</summary>
    internal static partial class Tests
    {
        private static void DecodesVsSettingsColors()
        {
            Check(ColorTheme.TryDecodeColor("0x00AADCDC", out uint? rgb) && rgb == 0xDCDCAA, "0x00BBGGRR is RGB DCDCAA");
            Check(ColorTheme.TryDecodeColor("0x02000000", out rgb) && rgb == null, "0x02000000 is Automatic");
            Check(ColorTheme.TryDecodeColor("0x01000005", out rgb) && rgb == null, "system color index is Automatic");
            Check(!ColorTheme.TryDecodeColor("red", out _), "not a color");
        }

        private static void ParsesRepositoryThemes()
        {
            string themes = Path.GetFullPath(Path.Combine(extensionDirectory, ".."));
            var problems = new List<string>();
            var native = ColorTheme.Load(new[] { Path.Combine(themes, "visual-assist-dark", "Visual-Assist-Dark.vssettings") }, problems);
            Equal(string.Empty, string.Join("; ", problems), "problems");
            Equal(70, native.Items.Count, "Visual Assist Dark items");
            var plain = native.Items["plain text"];
            Check(plain.Foreground == 0xDCDCDC && plain.Background == 0x1E1E1E && plain.Bold == false, "Plain Text DCDCDC on 1E1E1E");
            var comment = native.Items["Comment"];
            Check(comment.Foreground == 0x57A64A && comment.HasBackground && comment.Background == null, "Comment 57A64A, automatic background");
            Equal(true, native.Items["CppLocalVariableSemanticTokenFormat"].Bold, "local variables bold");

            var layered = ColorTheme.Load(new[]
            {
                Path.Combine(themes, "visual-assist-dark", "Visual-Assist-Dark.vssettings"),
                Path.Combine(themes, "vscode-dark-modern", "VSCode-Dark-Modern-Vivid.vssettings"),
            }, problems);
            Equal(string.Empty, string.Join("; ", problems), "problems of two files");
            Check(layered.Items.Count >= 97, $"union of both files ({layered.Items.Count} items)");
            Equal(0x1F1F1FU, layered.Items["Plain Text"].Background, "later file wins");

            var expected = new Dictionary<string, int>
            {
                ["Visual-Assist-Dark.vssettings"] = 70,
                ["Visual-Assist-Dark-with-VA.vssettings"] = 81,
                ["VSCode-Dark-Modern-Vivid.vssettings"] = 97,
                ["VSCode-Dark-Modern-VX2762.vssettings"] = 97,
            };
            foreach (string file in Directory.GetFiles(themes, "*.vssettings", SearchOption.AllDirectories))
            {
                var theme = ColorTheme.Load(new[] { file }, problems);
                string name = Path.GetFileName(file);
                Check(problems.Count == 0 && theme.OtherCategoryItems == 0, $"{name} parses cleanly");
                if (expected.TryGetValue(name, out int count))
                {
                    Equal(count, theme.Items.Count, $"{name} items");
                }
            }
        }

        private static void ParsesOtherCategoriesAndErrors()
        {
            const string xml = @"<UserSettings><Category name='Environment_Group'><Category name='Environment_FontsAndColors'>
<FontsAndColors Version='2.0'><Categories>
  <Category GUID='{a27b4e24-a735-4d1d-b8e7-9716e1e3d8e0}' FontName='Consolas' FontSize='10'><Items>
    <Item Name='Keyword' Foreground='0x00D69C56' Background='0x02000000' BoldFont='Yes' />
    <Item Name='Broken' Foreground='blue' />
  </Items></Category>
  <Category GUID='{9973EFDF-317D-431C-8BC1-5E88CBFD4F7F}'><Items><Item Name='Plain Text' Foreground='0x00FFFFFF' /></Items></Category>
</Categories></FontsAndColors></Category></Category></UserSettings>";
            var items = new Dictionary<string, ThemeItem>(StringComparer.OrdinalIgnoreCase);
            var problems = new List<string>();
            int other = ColorTheme.Parse(xml, "test.vssettings", items, problems);
            Equal(1, other, "items of other categories");
            Check(items["Keyword"].Foreground == 0x569CD6 && items["Keyword"].Bold == true, "lower-case GUID accepted");
            Check(!items["Broken"].HasForeground && problems.Count == 1, "bad color reported and ignored");
            problems.Clear();
            ColorTheme.Parse("<UserSettings>", "broken.vssettings", items, problems);
            Check(problems.Count == 1 && problems[0].Contains("not a valid"), "invalid XML reported");
            problems.Clear();
            ColorTheme.Parse("<UserSettings />", "empty.vssettings", items, problems);
            Check(problems.Count == 1 && problems[0].Contains("no Text Editor colors"), "file without editor colors reported");
        }

        private static void ColorsSettingResolvesPaths()
        {
            string directory = Path.Combine(Path.GetTempPath(), "efs-colors-" + Guid.NewGuid().ToString("N"));
            Directory.CreateDirectory(Path.Combine(directory, "themes"));
            File.WriteAllText(Path.Combine(directory, "themes", "a.vssettings"), "<UserSettings />");
            try
            {
                string path = Path.Combine(directory, StyleConfig.FileName);
                var problems = new List<string>();
                var single = StyleConfig.Load(DefaultJson, "{ \"colors\": \"themes/a.vssettings\" }", path, problems);
                Equal(string.Empty, string.Join("; ", problems), "problems");
                Equal(Path.Combine(directory, "themes", "a.vssettings"), single.ColorFiles.SingleOrDefault(), "relative to the configuration file");

                var list = StyleConfig.Load(DefaultJson, "{ \"colors\": [ \"themes/a.vssettings\", \"themes/missing.vssettings\" ] }", path, problems);
                Equal(2, list.ColorFiles.Count, "list keeps a missing file so that creating it later works");
                Check(problems.Count == 1 && problems[0].Contains("does not exist"), "missing file reported");

                problems.Clear();
                var none = StyleConfig.Load(DefaultJson, "{ \"colors\": 3 }", path, problems);
                Check(none.ColorFiles.Count == 0 && problems.Count == 1, "wrong type reported");
                Check(StyleConfig.Load(DefaultJson, null, null, new List<string>()).ColorFiles.Count == 0, "no colors by default");
            }
            finally
            {
                Directory.Delete(directory, true);
            }
        }
    }
}
