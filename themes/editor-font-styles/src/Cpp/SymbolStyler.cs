using System;
using System.Collections.Generic;

namespace EditorFontStyles.Cpp
{
    internal enum SymbolStyle : byte
    {
        None,
        System,      // declared by the standard library / C runtime headers
        Local,       // declared in the current file
        CommentTag,  // TODO, FIXME, ... inside a comment
    }

    internal readonly struct StyledSpan : IEquatable<StyledSpan>
    {
        public StyledSpan(int start, int length, SymbolStyle style)
        {
            Start = start;
            Length = length;
            Style = style;
        }

        public int Start { get; }
        public int Length { get; }
        public SymbolStyle Style { get; }
        public int End => Start + Length;

        public bool Equals(StyledSpan other) => Start == other.Start && Length == other.Length && Style == other.Style;

        public override bool Equals(object obj) => obj is StyledSpan other && Equals(other);

        public override int GetHashCode() => (Start * 397) ^ (Length * 31) ^ (int)Style;

        public override string ToString() => $"{Style}@{Start}+{Length}";
    }

    internal sealed class SymbolStyleOptions
    {
        public bool SystemSymbols = true;
        public bool LocalSymbols = true;
        public bool CommentTags = true;
        public ISet<string> SystemNames = new HashSet<string>();
        public IReadOnlyList<string> CommentTagWords = Array.Empty<string>();
    }

    /// <summary>Tokens and declarations of one C++ file.</summary>
    internal sealed class CppFileAnalysis
    {
        private CppFileAnalysis(CppLexResult lex, HashSet<string> declaredNames, HashSet<int> declarationSites)
        {
            Lex = lex;
            DeclaredNames = declaredNames;
            DeclarationSites = declarationSites;
        }

        public CppLexResult Lex { get; }
        public HashSet<string> DeclaredNames { get; }
        public HashSet<int> DeclarationSites { get; }

        public static CppFileAnalysis Analyze(string text, ISet<string> systemNames)
        {
            var lex = CppLexer.Lex(text);
            var declared = new HashSet<string>(StringComparer.Ordinal);
            var sites = new HashSet<int>();
            CppDeclarationScanner.Scan(lex, systemNames, declared, sites);
            return new CppFileAnalysis(lex, declared, sites);
        }
    }

    /// <summary>
    /// Decides which identifiers are system symbols (Visual Assist: "System symbols in italics") and which
    /// are local symbols ("Local symbols in bold"), by name like Visual Assist does:
    /// <list type="bullet">
    /// <item>a name qualified by a system name (std::max, ios::sync_with_stdio, vector&lt;int&gt;::iterator) is a system symbol;</item>
    /// <item>otherwise a name declared anywhere in the current file is a local symbol;</item>
    /// <item>otherwise a name declared in a header the file includes with quotes is neither;</item>
    /// <item>otherwise a name from the system symbol table (cin, sort, vector, push_back) is a system symbol.</item>
    /// </list>
    /// </summary>
    internal static class SymbolStyler
    {
        public static List<StyledSpan> Style(string text, CppFileAnalysis analysis, ISet<string> includedNames, SymbolStyleOptions options)
        {
            var spans = new List<StyledSpan>();
            var tokens = analysis.Lex.Tokens;
            for (int t = 0; t < tokens.Count; t++)
            {
                var token = tokens[t];
                if (token.Kind == CppTokenKind.Comment)
                {
                    if (options.CommentTags)
                    {
                        AddCommentTags(text, token, options.CommentTagWords, spans);
                    }

                    continue;
                }

                if (token.Kind != CppTokenKind.Identifier || token.Has(CppTokenFlags.Unstyled | CppTokenFlags.Attribute))
                {
                    continue;
                }

                var style = Classify(t, analysis, includedNames, options.SystemNames);
                if ((style == SymbolStyle.System && options.SystemSymbols) || (style == SymbolStyle.Local && options.LocalSymbols))
                {
                    spans.Add(new StyledSpan(token.Start, token.Length, style));
                }
            }

            return spans;
        }

        public static SymbolStyle Classify(int t, CppFileAnalysis analysis, ISet<string> includedNames, ISet<string> systemNames)
        {
            var tokens = analysis.Lex.Tokens;
            string name = tokens[t].Value;
            var declared = analysis.DeclaredNames;
            int qualifier = Qualifier(tokens, t);
            if (qualifier >= 0)
            {
                string scope = tokens[qualifier].Value;
                bool systemScope = systemNames.Contains(scope) && !declared.Contains(scope) && !includedNames.Contains(scope);
                if (systemScope)
                {
                    return SymbolStyle.System;
                }

                return declared.Contains(name) ? SymbolStyle.Local : SymbolStyle.None;
            }

            if (analysis.DeclarationSites.Contains(t) || declared.Contains(name))
            {
                return SymbolStyle.Local;
            }

            if (includedNames.Contains(name))
            {
                return SymbolStyle.None;
            }

            return systemNames.Contains(name) ? SymbolStyle.System : SymbolStyle.None;
        }

        /// <summary>
        /// For "A::name" or "A&lt;T&gt;::name" returns the token index of A; -1 when the name is unqualified,
        /// globally qualified ("::name") or qualified by something else ("decltype(x)::name").
        /// </summary>
        private static int Qualifier(List<CppToken> tokens, int t)
        {
            int p = Previous(tokens, t);
            if (p < 0 || tokens[p].Kind != CppTokenKind.Punctuator || tokens[p].Value != "::")
            {
                return -1;
            }

            bool inDirective = tokens[t].Has(CppTokenFlags.InDirective);
            int q = Previous(tokens, p);
            if (q >= 0 && tokens[q].Kind == CppTokenKind.Punctuator && (tokens[q].Value == ">" || tokens[q].Value == ">>"))
            {
                int depth = 0;
                int limit = Math.Max(0, q - 128);
                for (; q >= limit; q--)
                {
                    var token = tokens[q];
                    if (token.Kind != CppTokenKind.Punctuator)
                    {
                        continue;
                    }

                    if (token.Value == ">")
                    {
                        depth++;
                    }
                    else if (token.Value == ">>")
                    {
                        depth += 2;
                    }
                    else if (token.Value == "<" && --depth == 0)
                    {
                        break;
                    }
                    else if (token.Value == ";" || token.Value == "{" || token.Value == "}")
                    {
                        return -1;
                    }
                }

                q = q >= limit ? Previous(tokens, q) : -1;
            }

            if (q < 0 || tokens[q].Kind != CppTokenKind.Identifier || tokens[q].Has(CppTokenFlags.InDirective) != inDirective)
            {
                return -1;
            }

            return q;
        }

        private static int Previous(List<CppToken> tokens, int t)
        {
            for (int p = t - 1; p >= 0; p--)
            {
                if (tokens[p].Kind != CppTokenKind.Comment)
                {
                    return p;
                }
            }

            return -1;
        }

        private static void AddCommentTags(string text, CppToken comment, IReadOnlyList<string> words, List<StyledSpan> spans)
        {
            int first = spans.Count;
            foreach (string word in words)
            {
                if (string.IsNullOrEmpty(word))
                {
                    continue;
                }

                int from = comment.Start;
                while (from < comment.End)
                {
                    int at = text.IndexOf(word, from, comment.End - from, StringComparison.Ordinal);
                    if (at < 0)
                    {
                        break;
                    }

                    int after = at + word.Length;
                    bool wordStart = at == comment.Start || !CppLexer.IsIdentifierPart(text[at - 1]);
                    bool wordEnd = after >= comment.End || !CppLexer.IsIdentifierPart(text[after]);
                    if (wordStart && wordEnd)
                    {
                        spans.Add(new StyledSpan(at, word.Length, SymbolStyle.CommentTag));
                    }

                    from = after;
                }
            }

            if (spans.Count - first > 1)
            {
                spans.Sort(first, spans.Count - first, Comparer<StyledSpan>.Create((a, b) => a.Start.CompareTo(b.Start)));
            }
        }
    }
}
