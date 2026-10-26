using System;
using System.Collections.Generic;
using System.Linq;
using EditorFontStyles.Cpp;

namespace EditorFontStyles.Tests
{
    /// <summary>Tests of brace matching (src/Cpp/BracePairs.cs).</summary>
    internal static partial class Tests
    {
        /// <summary>
        /// The code of <paramref name="marked"/> without its '|' (the caret), with '@' before each bracket of the
        /// pair found for that caret, or "none".
        /// </summary>
        private static string PairAt(string marked, bool virtualSpace = false)
        {
            int caret = marked.IndexOf('|');
            string code = marked.Remove(caret, 1);
            var pairs = BracePairs.Find(CppLexer.Lex(code));
            if (!pairs.TryGetPair(caret, virtualSpace, out int open, out int close))
            {
                return "none";
            }

            return code.Insert(close, "@").Insert(open, "@");
        }

        private static void BracePairsAtTheCaret()
        {
            Equal("f@(a@)", PairAt("f|(a)"), "before an opening bracket");
            Equal("f@(a@)", PairAt("f(|a)"), "after an opening bracket");
            Equal("f@(a@)", PairAt("f(a|)"), "before a closing bracket");
            Equal("f@(a@)", PairAt("f(a)|"), "after a closing bracket");
            Equal("none", PairAt("f(a|b)"), "not next to a bracket");
            Equal("none", PairAt("|"), "empty text");
            Equal("@(@)", PairAt("(|)"), "between the brackets of a pair");
            Equal("@(a@)(b)", PairAt("(a)|(b)"), "a closing bracket before the caret comes first");
            Equal("{@{@}}", PairAt("{|{}}"), "an opening bracket after the caret comes before one before it");
            Equal("{@{@}}", PairAt("{{}|}"), "a closing bracket before the caret comes before one after it");
            Equal("none", PairAt("{ }|", virtualSpace: true), "in virtual space the bracket before the line end is not next to the caret");
            Equal("@{ @}", PairAt("{ }|"), "at the line end");

            const string loop = "rep(i, 1, n + 1) {\n    if (b[i] == 3) {\n        cout << 1;\n    }\n}";
            Equal("rep(i, 1, n + 1) @{\n    if (b[i] == 3) {\n        cout << 1;\n    }\n@}", PairAt(loop + "|"), "outer block");
            Equal("rep(i, 1, n + 1) {\n    if (b[i] == 3) @{\n        cout << 1;\n    @}\n}", PairAt(loop.Insert(loop.LastIndexOf("}\n}"), "|")), "inner block from inside");
            Equal("rep(i, 1, n + 1) {\n    if (b@[i@] == 3) {\n        cout << 1;\n    }\n}", PairAt(loop.Replace("b[i]", "b[i|]")), "subscript");
            Equal("auto f = [&](int x) @{ return x; @};", PairAt("auto f = [&](int x) |{ return x; };"), "lambda body");
            Equal("@[[nodiscard]@] int f();", PairAt("|[[nodiscard]] int f();"), "attribute brackets");
            Equal("none", PairAt("vector<pair<int, int>>|"), "angle brackets are not brackets");
        }

        private static void BracePairsSkipLiteralsAndDirectives()
        {
            Equal("f@(\")\", ')', /* ) */ x@) // )", PairAt("f|(\")\", ')', /* ) */ x) // )"), "brackets in literals and comments");
            Equal("@(R\"x( ) )x\"@)", PairAt("|(R\"x( ) )x\")"), "brackets in a raw string");
            Equal("void f() @{\n#if 0\n}\n#endif\n@}", PairAt("void f() |{\n#if 0\n}\n#endif\n}"), "#if 0 group");
            Equal("#define BEGIN {\nint main() @{\n@}\n", PairAt("#define BEGIN {\nint main() |{\n}\n"), "a directive does not open a code bracket");
            Equal("none", PairAt("int f() |{\n#define END }\n"), "a directive does not close a code bracket");
            Equal("none", PairAt("int f() {\n#define END }|\n"), "a directive bracket does not match a code bracket");
            Equal("#define rep(i, a, b) for (int i = @(a@); i < (b); ++i)\n", PairAt("#define rep(i, a, b) for (int i = |(a); i < (b); ++i)\n"), "brackets within a directive");
            Equal("#define A (\n#define B @(x@)\n", PairAt("#define A (\n#define B |(x)\n"), "each directive on its own");
            Equal("#define F(x) \\\n    @((x) + 1@)\n", PairAt("#define F(x) \\\n    |((x) + 1)\n"), "directive continued on the next line");
        }

        private static void BracePairsRecoverFromMismatches()
        {
            Equal("@{ ( @}", PairAt("|{ ( }"), "a closing brace closes the block past an unclosed parenthesis");
            Equal("none", PairAt("{ |( }"), "the unclosed parenthesis stays unmatched");
            Equal("none", PairAt("( ]|"), "a closing bracket with none of its kind open");
            Equal("@( ] @)", PairAt("|( ] )"), "a stray closing bracket is skipped");
            Equal("none", PairAt("if (a[|i"), "being typed");
            Equal("for (int i = 0; i < n; i++) @{\n    if (a[i\n@}", PairAt("for (int i = 0; i < n; i++) |{\n    if (a[i\n}"), "a block around a line being typed");
        }

        private static void BracePairsSurviveRandomInput()
        {
            string[] pieces =
            {
                "(", ")", "[", "]", "{", "}", "(", ")", "{", "}", "<", ">", "x", ";", " ", "\n", "\"", "'", "\")\"", "'('",
                "/*", "*/", "//", "\n#define M(a) (a\n", "\n#if 0\n", "\n#endif\n", "R\"(", ")\"", "\\\n", "#", "[[", "]]",
            };
            var random = new Random(271828);
            for (int round = 0; round < 3000; round++)
            {
                var text = new System.Text.StringBuilder();
                int length = random.Next(1, 100);
                for (int i = 0; i < length; i++)
                {
                    text.Append(pieces[random.Next(pieces.Length)]);
                }

                string code = text.ToString();
                var pairs = BracePairs.Find(CppLexer.Lex(code));
                var found = new HashSet<(int Open, int Close)>();
                for (int caret = 0; caret <= code.Length; caret++)
                {
                    if (!pairs.TryGetPair(caret, false, out int open, out int close))
                    {
                        continue;
                    }

                    bool valid = open >= 0 && open < close && close < code.Length && "([{".IndexOf(code[open]) >= 0
                        && "([{".IndexOf(code[open]) == ")]}".IndexOf(code[close])
                        && (caret == open || caret == open + 1 || caret == close || caret == close + 1);
                    if (!valid)
                    {
                        Check(false, $"pair {open}, {close} for caret {caret} in {code.Replace("\n", "\\n")}");
                        return;
                    }

                    found.Add((open, close));
                }

                var crossing = found.FirstOrDefault(a => found.Any(b => a.Open < b.Open && b.Open < a.Close && a.Close < b.Close));
                if (crossing != default)
                {
                    Check(false, $"pairs cross at {crossing} in {code.Replace("\n", "\\n")}");
                    return;
                }
            }

            Check(true, "random input");
        }
    }
}
