using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.IO;
using System.Linq;
using EditorFontStyles.Cpp;

namespace EditorFontStyles.Tests
{
    /// <summary>Tests of the C++ analysis (src/Cpp).</summary>
    internal static partial class Tests
    {
        // ---- helpers ----

        private static string TokenText(string text, CppToken token) => text.Substring(token.Start, token.Length);

        private static Dictionary<int, SymbolStyle> StylesByOffset(string code, ISet<string> included = null)
        {
            var analysis = CppFileAnalysis.Analyze(code, systemNames);
            var options = new SymbolStyleOptions
            {
                SystemNames = systemNames,
                CommentTagWords = new[] { "TODO", "FIXME" },
            };
            var spans = SymbolStyler.Style(code, analysis, included ?? new HashSet<string>(), options);
            for (int i = 1; i < spans.Count; i++)
            {
                Check(spans[i - 1].End <= spans[i].Start, $"spans sorted and disjoint at {spans[i]}");
            }

            return spans.ToDictionary(s => s.Start, s => s.Style);
        }

        /// <summary>Style of the <paramref name="occurrence"/>-th (0-based) whole-word occurrence of <paramref name="word"/>.</summary>
        private static SymbolStyle StyleOf(string code, Dictionary<int, SymbolStyle> styles, string word, int occurrence = 0)
        {
            int at = -1;
            for (int seen = 0; seen <= occurrence; seen++)
            {
                do
                {
                    at = code.IndexOf(word, at + 1, StringComparison.Ordinal);
                }
                while (at >= 0 && ((at > 0 && CppLexer.IsIdentifierPart(code[at - 1]))
                    || (at + word.Length < code.Length && CppLexer.IsIdentifierPart(code[at + word.Length]))));

                if (at < 0)
                {
                    throw new ArgumentException($"'{word}' #{occurrence} not found");
                }
            }

            return styles.TryGetValue(at, out var style) ? style : SymbolStyle.None;
        }

        private static void ExpectStyle(string code, Dictionary<int, SymbolStyle> styles, string word, SymbolStyle expected, int occurrence = 0)
        {
            Equal(expected, StyleOf(code, styles, word, occurrence), $"style of '{word}' #{occurrence}");
        }

        // ---- tests ----

        private static void LexerHandlesLiterals()
        {
            const string code = "int x = 1'000'000; auto s = \"a\\\"b\"s; auto r = R\"x(a)\"b)x\"; char c = '\\'';\n"
                + "double d = 1e-9; double h = 0x1p+3; auto u = u8\"\\u00e9\"; /* block\ncomment */ int after;\n"
                + "// line comment \\\n continued\nint next;";
            var lex = CppLexer.Lex(code);
            var numbers = lex.Tokens.Where(t => t.Kind == CppTokenKind.Number).Select(t => TokenText(code, t)).ToList();
            Equal("1'000'000|1e-9|0x1p+3", string.Join("|", numbers), "numbers");
            var strings = lex.Tokens.Where(t => t.Kind == CppTokenKind.String).Select(t => TokenText(code, t)).ToList();
            Equal("\"a\\\"b\"s|R\"x(a)\"b)x\"|u8\"\\u00e9\"", string.Join("|", strings), "strings");
            Equal("'\\''", TokenText(code, lex.Tokens.Single(t => t.Kind == CppTokenKind.Character)), "character");
            var identifiers = lex.Tokens.Where(t => t.Kind == CppTokenKind.Identifier).Select(t => t.Value).ToList();
            Equal("x|s|r|c|d|h|u|after|next", string.Join("|", identifiers), "identifiers");
            Equal(2, lex.Tokens.Count(t => t.Kind == CppTokenKind.Comment), "comments (a spliced // comment is one)");
        }

        private static void LexerHandlesDirectives()
        {
            const string code = "#include <vector>\n#include \"fenwick.hpp\"\n#pragma GCC optimize(\"O2\")\n"
                + "#define rep(i, a, b) for (int i = (a); i < (b); ++i)\n#define LONG_MACRO(x) \\\n    ((x) + 1)\n"
                + "#if 0\ndon't parse this; int hidden;\n#if 1\nint nested;\n#endif\n#else\nint shown;\n#endif\n"
                + "[[nodiscard]] int f();\n# define SPACED 1\nint tail;";
            var lex = CppLexer.Lex(code);
            Equal("fenwick.hpp", string.Join("|", lex.QuotedIncludes), "quoted includes");
            Equal(3, lex.Macros.Count, "macro count");
            var rep = lex.Macros[0];
            Equal("rep", lex.Tokens[rep.NameToken].Value, "macro name");
            Equal("i|a|b", string.Join("|", rep.ParameterTokens.Select(t => lex.Tokens[t].Value)), "macro parameters");
            Check(lex.Tokens.Skip(rep.BodyStart).Take(rep.BodyEnd - rep.BodyStart).Any(t => t.Value == "for"), "macro body contains for");
            var spliced = lex.Macros[1];
            Check(lex.Tokens.Skip(spliced.BodyStart).Take(spliced.BodyEnd - spliced.BodyStart).Any(t => t.Value == "x"), "spliced macro body continues on the next line");
            Equal("SPACED", lex.Tokens[lex.Macros[2].NameToken].Value, "# define with a space");
            var identifiers = lex.Tokens.Where(t => t.Kind == CppTokenKind.Identifier).Select(t => t.Value).ToList();
            Check(!identifiers.Contains("hidden") && !identifiers.Contains("nested"), "#if 0 group skipped");
            Check(identifiers.Contains("shown") && identifiers.Contains("tail"), "#else group and following code lexed");
            Check(lex.Tokens.Where(t => t.Value == "GCC" || t.Value == "optimize").All(t => t.Has(CppTokenFlags.Unstyled)), "#pragma unstyled");
            Check(lex.Tokens.Single(t => t.Value == "nodiscard").Has(CppTokenFlags.Attribute), "attribute flagged");
            Check(!lex.Tokens.Single(t => t.Value == "f").Has(CppTokenFlags.Attribute), "declaration after attribute not flagged");
        }

        private const string Solution = @"#include <bits/stdc++.h>
using namespace std;
#define fi first
#define rep(i, a, b) for (int i = (a); i < (b); ++i)
typedef long long ll;
typedef pair<int, int> pii;
using vi = vector<int>;
const int N = 2e5 + 5, MOD = 998244353;
int n, m, cnt[N];
ll dp[N][2], ans;
struct Edge { int to, w; };
vector<Edge> g[N];
struct Node { int l, r; ll sum; } tr[N << 2];
enum Color { RED, GREEN };
template <class T> void chmax(T& a, T b) { a = max(a, b); }
int (*compare)(const void*, const void*);
namespace util { int helper(int value) { return value * 2; } }
bool cmp(const Edge& lhs, const Edge& rhs) { return lhs.w < rhs.w; }
int count_ones(int x) { return __builtin_popcount(x); }
void dfs(int u, int parent) {
    for (auto [to, w] : g[u]) if (to != parent) dfs(to, u);
}
int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);
    cin >> n >> m;
    rep(i, 0, n) cin >> cnt[i];
    for (int k = 0, step = 1; k < n; k += step) ans += cnt[k];
    auto lambda = [&](int left, int right) -> ll { ll total = 0; return total + left + right; };
    sort(cnt, cnt + n, [](int a, int b) { return a > b; });
    pii p{1, 2};
    cout << p.first << ' ' << p.fi << ' ' << max(1, 2) << ' ' << std::max(3, 4) << endl;
    int size = 0; // TODO: rename
    size++;
    vector<int>::iterator it;
    auto now = std::chrono::steady_clock::now();
    if (auto found = find(cnt, cnt + n, 3); found != cnt + n) ans++;
    try { throw runtime_error(""x""); } catch (const exception& error) { cerr << error.what(); }
    return 0;
}
";

        private static void DeclarationsOfTypicalSolution()
        {
            var analysis = CppFileAnalysis.Analyze(Solution, systemNames);
            var declared = analysis.DeclaredNames;
            foreach (string name in new[]
            {
                "fi", "rep", "i", "a", "b", "ll", "pii", "vi", "N", "MOD", "n", "m", "cnt", "dp", "ans", "Edge", "to", "w",
                "g", "Node", "l", "r", "sum", "tr", "Color", "RED", "GREEN", "T", "chmax", "compare", "util", "helper",
                "value", "cmp", "lhs", "rhs", "count_ones", "x", "dfs", "u", "parent", "main", "k", "step", "lambda",
                "left", "right", "total", "p", "size", "it", "now", "found", "error",
            })
            {
                Check(declared.Contains(name), $"declares {name}");
            }

            foreach (string name in new[]
            {
                "std", "first", "pair", "vector", "max", "ios", "sync_with_stdio", "cin", "tie", "sort", "cout", "endl",
                "chrono", "steady_clock", "find", "runtime_error", "exception", "cerr", "what", "iterator", "__builtin_popcount",
            })
            {
                Check(!declared.Contains(name), $"does not declare {name}");
            }
        }

        private static void ExpressionsDeclareNothing()
        {
            const string code = "void f(int* p, int* q, int c, int d) {\n"
                + "    swap(*p, *q); show(*p); arr[idx] = value; cout << (c < d && c > limit); x = y < z;\n"
                + "    obj.method(arg); ptr->field = other; ++counter; (*p)++; call(a1, b1);\n"
                + "    if (c < lo || d > hi) return; while (cnt--) work(); new Widget(size1); delete p;\n"
                + "}\n";
            var declared = CppFileAnalysis.Analyze(code, systemNames).DeclaredNames;
            Equal("c|d|f|p|q", string.Join("|", declared.OrderBy(s => s, StringComparer.Ordinal)), "declared names");
        }

        private static void StylesOfTypicalSolution()
        {
            string code = Solution;
            var styles = StylesByOffset(code);
            ExpectStyle(code, styles, "std", SymbolStyle.System);
            ExpectStyle(code, styles, "fi", SymbolStyle.Local);
            ExpectStyle(code, styles, "first", SymbolStyle.System);
            ExpectStyle(code, styles, "rep", SymbolStyle.Local, 1);
            ExpectStyle(code, styles, "ll", SymbolStyle.Local);
            ExpectStyle(code, styles, "pair", SymbolStyle.System);
            ExpectStyle(code, styles, "vector", SymbolStyle.System);
            ExpectStyle(code, styles, "N", SymbolStyle.Local, 1);
            ExpectStyle(code, styles, "Edge", SymbolStyle.Local, 1);
            ExpectStyle(code, styles, "to", SymbolStyle.Local, 1);
            ExpectStyle(code, styles, "max", SymbolStyle.System);
            ExpectStyle(code, styles, "__builtin_popcount", SymbolStyle.None);
            ExpectStyle(code, styles, "main", SymbolStyle.Local);
            ExpectStyle(code, styles, "ios", SymbolStyle.System);
            ExpectStyle(code, styles, "sync_with_stdio", SymbolStyle.System);
            ExpectStyle(code, styles, "cin", SymbolStyle.System);
            ExpectStyle(code, styles, "tie", SymbolStyle.System);
            ExpectStyle(code, styles, "sort", SymbolStyle.System);
            ExpectStyle(code, styles, "first", SymbolStyle.System, 1);
            ExpectStyle(code, styles, "fi", SymbolStyle.Local, 1);
            ExpectStyle(code, styles, "max", SymbolStyle.System, 2);
            ExpectStyle(code, styles, "endl", SymbolStyle.System);
            ExpectStyle(code, styles, "size", SymbolStyle.Local);
            ExpectStyle(code, styles, "size", SymbolStyle.Local, 1);
            ExpectStyle(code, styles, "iterator", SymbolStyle.System);
            ExpectStyle(code, styles, "chrono", SymbolStyle.System);
            ExpectStyle(code, styles, "steady_clock", SymbolStyle.System);
            ExpectStyle(code, styles, "now", SymbolStyle.System, 1);
            ExpectStyle(code, styles, "now", SymbolStyle.Local, 0);
            ExpectStyle(code, styles, "what", SymbolStyle.System);
            ExpectStyle(code, styles, "TODO", SymbolStyle.CommentTag);
            ExpectStyle(code, styles, "bits", SymbolStyle.None);
        }

        private static void ShadowedLibraryNames()
        {
            const string code = "int max(int a, int b) { return a > b ? a : b; }\nint next[10];\n"
                + "int main() { int x = max(1, 2); int y = std::max(1, 2); next[0] = *std::next(&x); }\n";
            var styles = StylesByOffset(code);
            ExpectStyle(code, styles, "max", SymbolStyle.Local, 1);
            ExpectStyle(code, styles, "max", SymbolStyle.System, 2);
            ExpectStyle(code, styles, "next", SymbolStyle.Local, 1);
            ExpectStyle(code, styles, "next", SymbolStyle.System, 2);
        }

        private static void IncludedHeadersAreNeitherLocalNorSystem()
        {
            const string code = "#include \"fenwick.hpp\"\nint main() { FenwickTree tree(5); tree.add(1, 2); return tree.size(); }\n";
            var included = new HashSet<string> { "FenwickTree", "add", "size" };
            var styles = StylesByOffset(code, included);
            ExpectStyle(code, styles, "FenwickTree", SymbolStyle.None);
            ExpectStyle(code, styles, "size", SymbolStyle.None);
            ExpectStyle(code, styles, "tree", SymbolStyle.Local);
            ExpectStyle(code, styles, "fenwick", SymbolStyle.None);
        }

        private static void CommentTags()
        {
            const string code = "// TODO fix, NOTTODO, TODOS, FIXME:\n/* multi\n TODO */ int TODO_COUNT; \"TODO in string\"\n";
            var styles = StylesByOffset(code);
            Equal(3, styles.Values.Count(s => s == SymbolStyle.CommentTag), "comment tags (whole words, comments only)");
            ExpectStyle(code, styles, "FIXME", SymbolStyle.CommentTag);
        }

        private static void ChangedRangeOfEdits()
        {
            var before = new List<StyledSpan> { new StyledSpan(0, 3, SymbolStyle.Local), new StyledSpan(10, 3, SymbolStyle.System), new StyledSpan(20, 3, SymbolStyle.Local) };
            var shifted = before.Select(s => new StyledSpan(s.Start + 2, s.Length, s.Style)).ToList();
            Equal((0, 0), SpanDiff.ChangedRange(before, 30, shifted, 32), "pure shift");
            var restyled = new List<StyledSpan> { before[0], new StyledSpan(10, 3, SymbolStyle.Local), before[2] };
            Equal((10, 13), SpanDiff.ChangedRange(before, 30, restyled, 30), "one restyled span");
            var removed = new List<StyledSpan> { before[0], new StyledSpan(17, 3, SymbolStyle.Local) };
            var range = SpanDiff.ChangedRange(before, 30, removed, 27);
            Check(range.Start <= 10 && range.End >= 10, $"removed span covered: {range}");
            Equal((0, 0), SpanDiff.ChangedRange(before, 30, before, 30), "no change");
        }

        private static void ProjectIncludeDirectoriesParse()
        {
            string temp = Path.Combine(Path.GetTempPath(), "efs-tests-" + Guid.NewGuid().ToString("N"));
            Directory.CreateDirectory(Path.Combine(temp, "include"));
            Directory.CreateDirectory(Path.Combine(temp, "cp-stl"));
            try
            {
                const string xml = "<AdditionalIncludeDirectories>$(ProjectDir)include;$(ProjectDir)cp-stl;$(ProjectDir);"
                    + "D:\\missing\\ac-library;$(VC_IncludePath);%(AdditionalIncludeDirectories)</AdditionalIncludeDirectories>";
                var directories = ProjectIncludeDirectories.Parse(xml, temp);
                Equal(3, directories.Count, "existing directories without unknown macros");
                Check(directories[0].EndsWith("include", StringComparison.OrdinalIgnoreCase), "include first");
            }
            finally
            {
                Directory.Delete(temp, true);
            }
        }

        private static void SurvivesRandomInput()
        {
            string[] pieces =
            {
                "(", ")", "{", "}", "[", "]", "<", ">", ">>", "::", ",", ";", ":", "*", "&", "=", "int", "auto", "x", "y",
                "std", "vector", "template", "struct", "class", "enum", "namespace", "using", "typedef", "operator", "for",
                "if", "return", "\n#define M(a) a\n", "\n#if 0\n", "\n#endif\n", "\"", "'", "/*", "*/", "//", "\n", " ",
                "R\"(", ")\"", "[[", "]]", "...", "->", "~", "1'0", "0x1p+", "\\\n", "#", "decltype", "typename",
            };
            var random = new Random(12345);
            var watch = Stopwatch.StartNew();
            for (int round = 0; round < 3000; round++)
            {
                var text = new System.Text.StringBuilder();
                int length = random.Next(1, 120);
                for (int i = 0; i < length; i++)
                {
                    text.Append(pieces[random.Next(pieces.Length)]).Append(random.Next(3) == 0 ? " " : string.Empty);
                }

                string code = text.ToString();
                var analysis = CppFileAnalysis.Analyze(code, systemNames);
                var spans = SymbolStyler.Style(code, analysis, new HashSet<string>(), new SymbolStyleOptions { SystemNames = systemNames, CommentTagWords = new[] { "x" } });
                foreach (var span in spans)
                {
                    if (span.Start < 0 || span.End > code.Length)
                    {
                        Check(false, $"span {span} outside text of length {code.Length}");
                        return;
                    }
                }
            }

            Check(watch.ElapsedMilliseconds < 20000, $"random input analyzed in {watch.ElapsedMilliseconds} ms");
        }

        private static void RunCorpus(string directory)
        {
            var files = Directory.EnumerateFiles(directory, "*.*", SearchOption.AllDirectories)
                .Where(f => f.EndsWith(".cpp", StringComparison.OrdinalIgnoreCase) || f.EndsWith(".hpp", StringComparison.OrdinalIgnoreCase) || f.EndsWith(".h", StringComparison.OrdinalIgnoreCase))
                .Where(f => f.IndexOf("\\.vs\\", StringComparison.OrdinalIgnoreCase) < 0 && f.IndexOf("\\build\\", StringComparison.OrdinalIgnoreCase) < 0)
                .ToList();
            var systemCounts = new Dictionary<string, int>(StringComparer.Ordinal);
            long characters = 0;
            var slowest = (Time: 0.0, File: string.Empty);
            var watch = Stopwatch.StartNew();
            foreach (string file in files)
            {
                string code = File.ReadAllText(file);
                characters += code.Length;
                var one = Stopwatch.StartNew();
                var analysis = CppFileAnalysis.Analyze(code, systemNames);
                var included = HeaderIndex.Shared.CollectDeclaredNames(analysis.Lex.QuotedIncludes, Path.GetDirectoryName(file), ProjectIncludeDirectories.Find(file), systemNames);
                var spans = SymbolStyler.Style(code, analysis, included, new SymbolStyleOptions { SystemNames = systemNames, CommentTagWords = new[] { "TODO" } });
                double ms = one.Elapsed.TotalMilliseconds;
                if (ms > slowest.Time)
                {
                    slowest = (ms, file);
                }

                foreach (var span in spans.Where(s => s.Style == SymbolStyle.System))
                {
                    string name = code.Substring(span.Start, span.Length);
                    systemCounts[name] = systemCounts.TryGetValue(name, out int c) ? c + 1 : 1;
                }
            }

            Console.WriteLine($"corpus: {files.Count} files, {characters / 1024} KB in {watch.ElapsedMilliseconds} ms; slowest {slowest.Time:F1} ms ({Path.GetFileName(slowest.File)})");
            Console.WriteLine("most frequent system symbols: " + string.Join(" ", systemCounts.OrderByDescending(p => p.Value).Take(80).Select(p => $"{p.Key}:{p.Value}")));
        }
    }
}
