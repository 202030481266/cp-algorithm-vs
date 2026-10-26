using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using EditorFontStyles.Configuration;

namespace EditorFontStyles.Tests
{
    /// <summary>Tests of configuration parsing and merging (src/Configuration/StyleConfig.cs).</summary>
    internal static partial class Tests
    {
        private static string DefaultJson => File.ReadAllText(Path.Combine(extensionDirectory, "resources", "default-config.json"));

        private static void DefaultConfigIsValid()
        {
            var problems = new List<string>();
            var config = StyleConfig.Load(DefaultJson, null, null, problems);
            Equal(string.Empty, string.Join("; ", problems), "problems in the built-in defaults");
            Check(config.SystemSymbols && config.LocalSymbols && config.CommentTags && config.MatchedBraces, "C/C++ features on by default");
            Equal(true, config.ResolveStyle("Comment", "comment")?.Italic, "comments italic");
            Equal(true, config.ResolveStyle("xml doc comment - text", "xml doc comment - text")?.Italic, "doc comments italic");
            Equal(true, config.ResolveStyle("EditorFontStyles.SystemSymbol", "EditorFontStyles.SystemSymbol")?.Italic, "system symbols italic");
            Equal(true, config.ResolveStyle("EditorFontStyles.LocalSymbol", "EditorFontStyles.LocalSymbol")?.Bold, "local symbols bold");
            var brace = config.ResolveStyle("EditorFontStyles.MatchedBrace", "EditorFontStyles.MatchedBrace");
            Check(brace?.Bold == true && brace.Underline == true, "matched braces bold and underlined");
            Check(config.ResolveStyle("Keyword", "keyword") == null, "keywords untouched");
            Check(config.CommentTagWords.Contains("TODO"), "TODO tag");
            Check(ReferenceEquals(config.SystemNames, Cpp.SystemSymbolTable.Default), "no extra names: shared table");
        }

        private static void UserConfigOverlaysDefaults()
        {
            string directory = Path.Combine(Path.GetTempPath(), "efs-config-" + Guid.NewGuid().ToString("N"));
            Directory.CreateDirectory(Path.Combine(directory, "cp-stl"));
            try
            {
                const string user = @"// user file
{
  ""styles"": {
    ""*comment*"": { ""fontFamily"": ""Cascadia Code"" },      // merged into the default entry
    ""EditorFontStyles.LocalSymbol"": null,                  // removed
    ""CppControlKeywordSyntacticTokenFormat"": { ""italic"": true, ""opacity"": 0.8 }
  },
  ""localSymbols"": { ""enabled"": false },
  ""systemSymbols"": { ""extraNames"": [ ""gp_hash_table"" ], ""excludeNames"": [ ""count"" ] },
  ""commentTags"": { ""words"": [ ""TODO"", ""PERF"" ] },
  ""matchedBraces"": { ""enabled"": false },
  ""includeDirectories"": [ ""cp-stl"" ]
}";
                var problems = new List<string>();
                string path = Path.Combine(directory, StyleConfig.FileName);
                var config = StyleConfig.Load(DefaultJson, user, path, problems);
                Equal(string.Empty, string.Join("; ", problems), "problems");
                var comment = config.ResolveStyle("Comment", "comment");
                Check(comment.Italic == true && comment.FontFamily == "Cascadia Code", "comment keeps italic and gains the font");
                Check(config.ResolveStyle("EditorFontStyles.LocalSymbol", null) == null, "null removes a default entry");
                var control = config.ResolveStyle("CppControlKeywordSyntacticTokenFormat", "cppControlKeyword");
                Check(control.Italic == true && Math.Abs(control.Opacity.Value - 0.8) < 1e-9, "new entry");
                Check(!config.LocalSymbols && config.SystemSymbols && config.CommentTags, "sections merge key by key");
                Check(!config.MatchedBraces, "matched braces turned off");
                Check(config.SystemNames.Contains("gp_hash_table") && !config.SystemNames.Contains("count") && config.SystemNames.Contains("vector"), "extra and excluded names");
                Equal("TODO|PERF", string.Join("|", config.CommentTagWords), "arrays replace");
                Equal(Path.Combine(directory, "cp-stl"), config.IncludeDirectories.SingleOrDefault(), "include directory relative to the file");
            }
            finally
            {
                Directory.Delete(directory, true);
            }
        }

        private static void StyleRulesMatchNamesAndWildcards()
        {
            var problems = new List<string>();
            const string user = @"{ ""styles"": { ""cppFunction"": { ""bold"": true }, ""Cpp*Variable*"": { ""italic"": true }, ""keyword"": { ""underline"": true } } }";
            var config = StyleConfig.Load("{}", user, "C:\\x\\" + StyleConfig.FileName, problems);
            Equal(true, config.ResolveStyle("CppFunctionSemanticTokenFormat", "cppFunction")?.Bold, "classification name");
            Equal(true, config.ResolveStyle("CppLocalVariableSemanticTokenFormat", "cppLocalVariable")?.Italic, "wildcard");
            Equal(true, config.ResolveStyle("Keyword", "keyword")?.Underline, "case-insensitive item name");
            Check(config.ResolveStyle("CppParameterSemanticTokenFormat", "cppParameter") == null, "no match");
        }

        private static void ObliqueIsParsed()
        {
            var problems = new List<string>();
            var config = StyleConfig.Load(DefaultJson, "{ \"styles\": { \"*comment*\": { \"oblique\": true }, \"Keyword\": { \"oblique\": \"yes\" } } }", "C:\\x\\" + StyleConfig.FileName, problems);
            var comment = config.ResolveStyle("Comment", "comment");
            Check(comment.Oblique == true && comment.Italic == true, "oblique merged into the default comment style (applied instead of the italic face)");
            Check(problems.Count == 1 && problems[0].Contains("oblique"), "non-boolean oblique reported");
        }

        private static void FontIsParsed()
        {
            var problems = new List<string>();
            string path = "C:\\x\\" + StyleConfig.FileName;
            var config = StyleConfig.Load(DefaultJson, null, null, problems);
            Check(config.FontFamily == null && config.FontSize == null, "no editor font by default");

            config = StyleConfig.Load(DefaultJson, "{ \"font\": { \"family\": \" Maple Mono NF CN \", \"size\": 13.5 } }", path, problems);
            Equal(string.Empty, string.Join("; ", problems), "problems");
            Check(config.FontFamily == "Maple Mono NF CN" && config.FontSize == 13.5, "family trimmed, size in points");

            config = StyleConfig.Load(DefaultJson, "{ \"font\": { \"size\": 12 } }", path, problems);
            Check(config.FontFamily == null && config.FontSize == 12, "font merges key by key");

            config = StyleConfig.Load(DefaultJson, "{ \"font\": { \"family\": 3, \"size\": 100, \"weight\": 400 } }", path, problems);
            Check(problems.Count == 3 && config.FontFamily == null && config.FontSize == null, "bad family, size and unknown property reported");

            problems.Clear();
            StyleConfig.Load(DefaultJson, "{ \"font\": \"Consolas\" }", path, problems);
            Check(problems.Count == 1 && problems[0].Contains("must be an object"), "font that is not an object reported");
        }

        private static void InvalidConfigIsReported()
        {
            var problems = new List<string>();
            var config = StyleConfig.Load(DefaultJson, "{ \"styles\": { \"Comment\": { \"italic\": 1, \"colour\": \"red\" } } }", "C:\\x\\" + StyleConfig.FileName, problems);
            Equal(2, problems.Count, "bad value and unknown property reported");
            Equal(true, config.ResolveStyle("Comment", "comment")?.Italic, "a bad value keeps the default");
            problems.Clear();
            config = StyleConfig.Load(DefaultJson, "{ \"styles\": ", "C:\\x\\" + StyleConfig.FileName, problems);
            Check(problems.Count == 1 && problems[0].Contains("ignored"), "syntax error reported");
            Check(config.SystemSymbols && config.ResolveStyle("Comment", "comment")?.Italic == true, "broken file falls back to the defaults");
        }
    }
}
