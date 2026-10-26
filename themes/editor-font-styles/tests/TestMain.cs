using System;
using System.Collections.Generic;
using System.IO;
using System.Threading;
using EditorFontStyles.Cpp;

namespace EditorFontStyles.Tests
{
    /// <summary>
    /// Tests for the parts of the extension that do not need Visual Studio (src/Cpp, src/Configuration/StyleConfig.cs and ColorTheme.cs).
    ///   EditorFontStyles.Tests.exe EXTENSION_DIRECTORY [--corpus DIRECTORY]
    /// The process puts itself in a job object with a 1 GB memory cap and stops after two minutes.
    /// </summary>
    internal static partial class Tests
    {
        private static int failures;
        private static int checks;
        private static ISet<string> systemNames;
        private static string extensionDirectory;

        private static int Main(string[] args)
        {
            MemoryCap.Apply(1024);
            using (new Timer(_ => { Console.Error.WriteLine("time limit exceeded"); Environment.Exit(124); }, null, 120000, Timeout.Infinite))
            {
                extensionDirectory = args.Length > 0 && !args[0].StartsWith("--") ? args[0] : Directory.GetCurrentDirectory();
                using (var reader = new StreamReader(Path.Combine(extensionDirectory, "resources", "system-symbols.txt")))
                {
                    systemNames = SystemSymbolTable.Parse(reader);
                }

                Run(LexerHandlesLiterals);
                Run(LexerHandlesDirectives);
                Run(DeclarationsOfTypicalSolution);
                Run(ExpressionsDeclareNothing);
                Run(StylesOfTypicalSolution);
                Run(ShadowedLibraryNames);
                Run(IncludedHeadersAreNeitherLocalNorSystem);
                Run(CommentTags);
                Run(ChangedRangeOfEdits);
                Run(ProjectIncludeDirectoriesParse);
                Run(SurvivesRandomInput);
                Run(BracePairsAtTheCaret);
                Run(BracePairsSkipLiteralsAndDirectives);
                Run(BracePairsRecoverFromMismatches);
                Run(BracePairsSurviveRandomInput);
                Run(DefaultConfigIsValid);
                Run(UserConfigOverlaysDefaults);
                Run(StyleRulesMatchNamesAndWildcards);
                Run(InvalidConfigIsReported);
                Run(ObliqueIsParsed);
                Run(FontIsParsed);
                Run(DecodesVsSettingsColors);
                Run(ParsesRepositoryThemes);
                Run(ParsesOtherCategoriesAndErrors);
                Run(ColorsSettingResolvesPaths);
                int corpus = Array.IndexOf(args, "--corpus");
                if (corpus >= 0 && corpus + 1 < args.Length)
                {
                    RunCorpus(args[corpus + 1]);
                }

                Console.WriteLine(failures == 0 ? $"all {checks} checks passed" : $"{failures} of {checks} checks failed");
                return failures == 0 ? 0 : 1;
            }
        }

        private static void Run(Action test)
        {
            try
            {
                test();
            }
            catch (Exception e)
            {
                failures++;
                Console.WriteLine($"FAIL {test.Method.Name}: {e}");
            }
        }

        private static void Check(bool condition, string what)
        {
            checks++;
            if (!condition)
            {
                failures++;
                Console.WriteLine("FAIL " + what);
            }
        }

        private static void Equal<T>(T expected, T actual, string what)
        {
            Check(EqualityComparer<T>.Default.Equals(expected, actual), $"{what}: expected {expected}, got {actual}");
        }
    }
}
