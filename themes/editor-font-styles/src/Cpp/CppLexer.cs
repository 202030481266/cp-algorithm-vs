using System;
using System.Collections.Generic;

namespace EditorFontStyles.Cpp
{
    internal enum CppTokenKind : byte
    {
        Identifier,
        Keyword,
        Number,
        String,
        Character,
        Comment,
        Punctuator,
        Directive,   // the '#' of a preprocessor directive and the directive name
        HeaderName,  // <file> or "file" after #include
    }

    [Flags]
    internal enum CppTokenFlags : byte
    {
        None = 0,
        InDirective = 1,  // part of a preprocessor directive line
        Unstyled = 2,     // never styled: the rest of #include / #pragma / #error lines
        Attribute = 4,    // [[...]], __declspec(...), __attribute__((...)), alignas(...)
    }

    internal readonly struct CppToken
    {
        public CppToken(CppTokenKind kind, int start, int length, CppTokenFlags flags, string value)
        {
            Kind = kind;
            Start = start;
            Length = length;
            Flags = flags;
            Value = value;
        }

        public CppTokenKind Kind { get; }
        public int Start { get; }
        public int Length { get; }
        public CppTokenFlags Flags { get; }

        /// <summary>Text of identifiers, keywords, punctuators and directive names; null for other kinds.</summary>
        public string Value { get; }

        public int End => Start + Length;

        public bool Has(CppTokenFlags flag) => (Flags & flag) != 0;

        public CppToken With(CppTokenFlags flags) => new CppToken(Kind, Start, Length, Flags | flags, Value);
    }

    internal sealed class MacroDefinition
    {
        public int NameToken;
        public readonly List<int> ParameterTokens = new List<int>();
        public int BodyStart;  // first token of the replacement list
        public int BodyEnd;    // one past its last token
    }

    internal sealed class CppLexResult
    {
        public readonly List<CppToken> Tokens = new List<CppToken>();
        public readonly List<string> QuotedIncludes = new List<string>();
        public readonly List<MacroDefinition> Macros = new List<MacroDefinition>();
    }

    /// <summary>
    /// Tokenizer for C and C++ source that tolerates code being edited: unterminated literals and comments
    /// end at the line or file end. Groups after "#if 0" / "#elif 0" are skipped like the preprocessor does.
    /// </summary>
    internal static class CppLexer
    {
        public static readonly HashSet<string> Keywords = new HashSet<string>(StringComparer.Ordinal)
        {
            "alignas", "alignof", "and", "and_eq", "asm", "auto", "bitand", "bitor", "bool", "break", "case",
            "catch", "char", "char8_t", "char16_t", "char32_t", "class", "compl", "concept", "const", "consteval",
            "constexpr", "constinit", "const_cast", "continue", "co_await", "co_return", "co_yield", "decltype",
            "default", "delete", "do", "double", "dynamic_cast", "else", "enum", "explicit", "export", "extern",
            "false", "final", "float", "for", "friend", "goto", "if", "inline", "int", "long", "mutable",
            "namespace", "new", "noexcept", "not", "not_eq", "nullptr", "operator", "or", "or_eq", "override",
            "private", "protected", "public", "register", "reinterpret_cast", "requires", "return", "short",
            "signed", "sizeof", "static", "static_assert", "static_cast", "struct", "switch", "template", "this",
            "thread_local", "throw", "true", "try", "typedef", "typeid", "typename", "union", "unsigned", "using",
            "virtual", "void", "volatile", "wchar_t", "while", "xor", "xor_eq",
            "__int8", "__int16", "__int32", "__int64", "__int128",
        };

        private static readonly string[] Punctuators3 = { "<=>", "->*", "...", "<<=", ">>=" };

        private static readonly string[] Punctuators2 =
        {
            "::", "->", ".*", "++", "--", "<<", ">>", "<=", ">=", "==", "!=", "&&", "||",
            "+=", "-=", "*=", "/=", "%=", "&=", "|=", "^=", "##",
        };

        private static readonly string[] SingleCharacters = BuildSingleCharacters();

        public static CppLexResult Lex(string text) => new Lexer(text).Run();

        public static bool IsIdentifierStart(char c) => c == '_' || (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '$' || c > 0x7F;

        public static bool IsIdentifierPart(char c) => IsIdentifierStart(c) || (c >= '0' && c <= '9');

        private static bool IsDigit(char c) => c >= '0' && c <= '9';

        private static string[] BuildSingleCharacters()
        {
            var table = new string[128];
            for (int c = 0; c < 128; c++)
            {
                table[c] = ((char)c).ToString();
            }

            return table;
        }

        private sealed class Lexer
        {
            private readonly string text;
            private readonly int n;
            private readonly CppLexResult result = new CppLexResult();
            private int i;
            private bool lineStart = true;  // only whitespace and comments since the last newline
            private bool inDirective;
            private string directive;
            private int directiveBodyStart;
            private CppTokenFlags lineFlags;
            private MacroDefinition macro;

            public Lexer(string text)
            {
                this.text = text ?? string.Empty;
                n = this.text.Length;
            }

            public CppLexResult Run()
            {
                while (i < n)
                {
                    char c = text[i];
                    if (c == '\n')
                    {
                        i++;
                        lineStart = true;
                        if (inDirective)
                        {
                            EndDirective();
                        }

                        continue;
                    }

                    if (c == ' ' || c == '\t' || c == '\r' || c == '\f' || c == '\v')
                    {
                        i++;
                        continue;
                    }

                    if (c == '\\' && NewlineLength(i + 1) > 0)
                    {
                        i += 1 + NewlineLength(i + 1);  // line splice: the logical line goes on
                        continue;
                    }

                    if (c == '/' && i + 1 < n && text[i + 1] == '/')
                    {
                        int end = LineCommentEnd(i + 2);
                        Add(CppTokenKind.Comment, i, end, null);
                        i = end;
                        continue;
                    }

                    if (c == '/' && i + 1 < n && text[i + 1] == '*')
                    {
                        int close = text.IndexOf("*/", i + 2, StringComparison.Ordinal);
                        int end = close < 0 ? n : close + 2;
                        Add(CppTokenKind.Comment, i, end, null);
                        i = end;
                        continue;
                    }

                    if (c == '#' && lineStart && !inDirective)
                    {
                        BeginDirective();
                        continue;
                    }

                    lineStart = false;
                    if (IsIdentifierStart(c))
                    {
                        LexIdentifier();
                    }
                    else if (IsDigit(c) || (c == '.' && i + 1 < n && IsDigit(text[i + 1])))
                    {
                        LexNumber();
                    }
                    else if (c == '"')
                    {
                        int end = SkipLiteralSuffix(QuotedEnd(i, '"'));
                        Add(CppTokenKind.String, i, end, null);
                        i = end;
                    }
                    else if (c == '\'')
                    {
                        int end = SkipLiteralSuffix(QuotedEnd(i, '\''));
                        Add(CppTokenKind.Character, i, end, null);
                        i = end;
                    }
                    else
                    {
                        string p = MatchPunctuator();
                        Add(CppTokenKind.Punctuator, i, i + p.Length, p);
                        i += p.Length;
                    }
                }

                if (inDirective)
                {
                    EndDirective();
                }

                MarkAttributes();
                return result;
            }

            private void Add(CppTokenKind kind, int start, int end, string value, CppTokenFlags extra = CppTokenFlags.None)
            {
                var flags = extra | (inDirective ? lineFlags : CppTokenFlags.None);
                result.Tokens.Add(new CppToken(kind, start, end - start, flags, value));
            }

            private int NewlineLength(int at)
            {
                if (at < n && text[at] == '\n')
                {
                    return 1;
                }

                return at + 1 < n && text[at] == '\r' && text[at + 1] == '\n' ? 2 : 0;
            }

            /// <summary>End of a // comment; a backslash at the end of the line continues it.</summary>
            private int LineCommentEnd(int from)
            {
                int j = from;
                while (true)
                {
                    int newline = text.IndexOf('\n', j);
                    if (newline < 0)
                    {
                        return n;
                    }

                    int last = newline - 1;
                    if (last >= from && text[last] == '\r')
                    {
                        last--;
                    }

                    if (last >= from && text[last] == '\\')
                    {
                        j = newline + 1;
                        continue;
                    }

                    return newline > 0 && text[newline - 1] == '\r' ? newline - 1 : newline;
                }
            }

            private void LexIdentifier()
            {
                int start = i;
                while (i < n && IsIdentifierPart(text[i]))
                {
                    i++;
                }

                if (i < n && (text[i] == '"' || text[i] == '\'') && IsLiteralPrefix(start, i - start))
                {
                    bool raw = text[i - 1] == 'R';
                    int end = text[i] == '\'' ? QuotedEnd(i, '\'') : raw ? RawStringEnd(i) : QuotedEnd(i, '"');
                    end = SkipLiteralSuffix(end);
                    Add(text[i] == '\'' ? CppTokenKind.Character : CppTokenKind.String, start, end, null);
                    i = end;
                    return;
                }

                string value = text.Substring(start, i - start);
                bool keyword = Keywords.Contains(value) || (inDirective && value == "defined");
                Add(keyword ? CppTokenKind.Keyword : CppTokenKind.Identifier, start, i, value);
            }

            private bool IsLiteralPrefix(int start, int length)
            {
                string prefix = text.Substring(start, length);
                switch (prefix)
                {
                    case "L": case "u": case "U": case "u8":
                    case "R": case "LR": case "uR": case "UR": case "u8R":
                        return true;
                    default:
                        return false;
                }
            }

            private void LexNumber()
            {
                int start = i++;
                while (i < n)
                {
                    char d = text[i];
                    if ((d == '+' || d == '-') && "eEpP".IndexOf(text[i - 1]) >= 0)
                    {
                        i++;
                    }
                    else if (IsIdentifierPart(d) || d == '.')
                    {
                        i++;
                    }
                    else if (d == '\'' && i + 1 < n && IsIdentifierPart(text[i + 1]))
                    {
                        i += 2;  // digit separator
                    }
                    else
                    {
                        break;
                    }
                }

                Add(CppTokenKind.Number, start, i, null);
            }

            /// <summary>End of a "..." or '...' literal starting at <paramref name="open"/>; stops at an unescaped newline.</summary>
            private int QuotedEnd(int open, char quote)
            {
                int j = open + 1;
                while (j < n)
                {
                    char c = text[j];
                    if (c == '\\')
                    {
                        j += j + 1 < n && text[j + 1] == '\r' && j + 2 < n && text[j + 2] == '\n' ? 3 : 2;
                    }
                    else if (c == quote)
                    {
                        return j + 1;
                    }
                    else if (c == '\n')
                    {
                        return j;
                    }
                    else
                    {
                        j++;
                    }
                }

                return n;
            }

            private int RawStringEnd(int open)
            {
                int paren = open + 1;
                while (paren < n && paren - open <= 17 && text[paren] != '(')
                {
                    char c = text[paren];
                    if (c == ')' || c == '\\' || c == '"' || char.IsWhiteSpace(c))
                    {
                        return QuotedEnd(open, '"');
                    }

                    paren++;
                }

                if (paren >= n || text[paren] != '(')
                {
                    return QuotedEnd(open, '"');
                }

                string close = ")" + text.Substring(open + 1, paren - open - 1) + "\"";
                int end = text.IndexOf(close, paren + 1, StringComparison.Ordinal);
                return end < 0 ? n : end + close.Length;
            }

            /// <summary>User-defined literal suffix such as "abc"s or 'x'_c.</summary>
            private int SkipLiteralSuffix(int end)
            {
                if (end < n && IsIdentifierStart(text[end]))
                {
                    while (end < n && IsIdentifierPart(text[end]))
                    {
                        end++;
                    }
                }

                return end;
            }

            private string MatchPunctuator()
            {
                foreach (string p in Punctuators3)
                {
                    if (string.CompareOrdinal(text, i, p, 0, 3) == 0)
                    {
                        return p;
                    }
                }

                if (i + 1 < n)
                {
                    foreach (string p in Punctuators2)
                    {
                        if (text[i] == p[0] && text[i + 1] == p[1])
                        {
                            return p;
                        }
                    }
                }

                char c = text[i];
                return c < 128 ? SingleCharacters[c] : c.ToString();
            }

            private void SkipHorizontalSpace()
            {
                while (i < n && (text[i] == ' ' || text[i] == '\t'))
                {
                    i++;
                }
            }

            private void BeginDirective()
            {
                inDirective = true;
                lineFlags = CppTokenFlags.InDirective;
                Add(CppTokenKind.Directive, i, i + 1, "#", CppTokenFlags.Unstyled);
                i++;
                SkipHorizontalSpace();
                directive = null;
                if (i < n && IsIdentifierStart(text[i]))
                {
                    int start = i;
                    while (i < n && IsIdentifierPart(text[i]))
                    {
                        i++;
                    }

                    directive = text.Substring(start, i - start);
                    Add(CppTokenKind.Directive, start, i, directive, CppTokenFlags.Unstyled);
                }

                switch (directive)
                {
                    case "include":
                    case "include_next":
                    case "import":
                    case "embed":
                        lineFlags |= CppTokenFlags.Unstyled;
                        LexHeaderName();
                        break;
                    case "pragma":
                    case "error":
                    case "warning":
                    case "line":
                    case "ident":
                    case "region":
                    case "endregion":
                    case "using":
                        lineFlags |= CppTokenFlags.Unstyled;
                        break;
                    case "define":
                        LexMacroHead();
                        break;
                }

                directiveBodyStart = result.Tokens.Count;
            }

            private void LexHeaderName()
            {
                SkipHorizontalSpace();
                if (i >= n || (text[i] != '<' && text[i] != '"'))
                {
                    return;
                }

                char close = text[i] == '<' ? '>' : '"';
                int end = i + 1;
                while (end < n && text[end] != close && text[end] != '\n')
                {
                    end++;
                }

                if (end < n && text[end] == close)
                {
                    if (close == '"')
                    {
                        result.QuotedIncludes.Add(text.Substring(i + 1, end - i - 1));
                    }

                    end++;
                }

                Add(CppTokenKind.HeaderName, i, end, null);
                i = end;
            }

            private void LexMacroHead()
            {
                SkipHorizontalSpace();
                if (i >= n || !IsIdentifierStart(text[i]))
                {
                    return;
                }

                int start = i;
                while (i < n && IsIdentifierPart(text[i]))
                {
                    i++;
                }

                string name = text.Substring(start, i - start);
                macro = new MacroDefinition { NameToken = result.Tokens.Count };
                Add(Keywords.Contains(name) ? CppTokenKind.Keyword : CppTokenKind.Identifier, start, i, name);
                if (i >= n || text[i] != '(')
                {
                    return;  // object-like macro
                }

                Add(CppTokenKind.Punctuator, i, i + 1, "(");
                i++;
                while (i < n && text[i] != ')' && text[i] != '\n')
                {
                    char c = text[i];
                    if (IsIdentifierStart(c))
                    {
                        int s = i;
                        while (i < n && IsIdentifierPart(text[i]))
                        {
                            i++;
                        }

                        macro.ParameterTokens.Add(result.Tokens.Count);
                        Add(CppTokenKind.Identifier, s, i, text.Substring(s, i - s));
                    }
                    else if (c == '\\' && NewlineLength(i + 1) > 0)
                    {
                        i += 1 + NewlineLength(i + 1);
                    }
                    else
                    {
                        i++;
                    }
                }

                if (i < n && text[i] == ')')
                {
                    Add(CppTokenKind.Punctuator, i, i + 1, ")");
                    i++;
                }
            }

            private void EndDirective()
            {
                inDirective = false;
                if (macro != null)
                {
                    macro.BodyStart = directiveBodyStart;
                    macro.BodyEnd = result.Tokens.Count;
                    result.Macros.Add(macro);
                    macro = null;
                }

                if ((directive == "if" || directive == "elif") && ConditionIsFalse())
                {
                    SkipInactiveGroup();
                }

                directive = null;
            }

            private bool ConditionIsFalse()
            {
                int count = 0;
                CppToken only = default;
                for (int t = directiveBodyStart; t < result.Tokens.Count; t++)
                {
                    if (result.Tokens[t].Kind != CppTokenKind.Comment)
                    {
                        only = result.Tokens[t];
                        count++;
                    }
                }

                return count == 1
                    && ((only.Kind == CppTokenKind.Number && only.Length == 1 && text[only.Start] == '0')
                        || (only.Kind == CppTokenKind.Keyword && only.Value == "false"));
            }

            /// <summary>Skips lines up to the matching #else / #elif / #endif; <see cref="i"/> is at a line start.</summary>
            private void SkipInactiveGroup()
            {
                int depth = 0;
                int line = i;
                while (line < n)
                {
                    int p = line;
                    while (p < n && (text[p] == ' ' || text[p] == '\t'))
                    {
                        p++;
                    }

                    if (p < n && text[p] == '#')
                    {
                        int s = p + 1;
                        while (s < n && (text[s] == ' ' || text[s] == '\t'))
                        {
                            s++;
                        }

                        int e = s;
                        while (e < n && IsIdentifierPart(text[e]))
                        {
                            e++;
                        }

                        string name = text.Substring(s, e - s);
                        if (name == "if" || name == "ifdef" || name == "ifndef")
                        {
                            depth++;
                        }
                        else if (name == "endif" && depth > 0)
                        {
                            depth--;
                        }
                        else if (depth == 0 && (name == "endif" || name == "else" || name == "elif" || name == "elifdef" || name == "elifndef"))
                        {
                            i = line;
                            lineStart = true;
                            return;
                        }
                    }

                    line = NextInactiveLine(p);
                }

                i = n;
            }

            /// <summary>Start of the next line in a skipped group; comments may hide newlines.</summary>
            private int NextInactiveLine(int p)
            {
                while (p < n)
                {
                    char c = text[p];
                    if (c == '\n')
                    {
                        return p + 1;
                    }

                    if (c == '/' && p + 1 < n && text[p + 1] == '*')
                    {
                        int close = text.IndexOf("*/", p + 2, StringComparison.Ordinal);
                        p = close < 0 ? n : close + 2;
                    }
                    else if (c == '/' && p + 1 < n && text[p + 1] == '/')
                    {
                        p = LineCommentEnd(p + 2);
                    }
                    else if (c == '\\' && NewlineLength(p + 1) > 0)
                    {
                        p += 1 + NewlineLength(p + 1);
                    }
                    else
                    {
                        p++;
                    }
                }

                return n;
            }

            private void MarkAttributes()
            {
                var tokens = result.Tokens;
                for (int t = 0; t < tokens.Count; t++)
                {
                    var token = tokens[t];
                    int last = -1;
                    if (token.Kind == CppTokenKind.Punctuator && token.Value == "[")
                    {
                        int next = NextCodeToken(t + 1);
                        if (next >= 0 && tokens[next].Value == "[" && tokens[next].Start == token.End)
                        {
                            last = MatchingClose(t, "[", "]");
                        }
                    }
                    else if ((token.Kind == CppTokenKind.Identifier && (token.Value == "__declspec" || token.Value == "__attribute__"))
                        || (token.Kind == CppTokenKind.Keyword && token.Value == "alignas"))
                    {
                        int next = NextCodeToken(t + 1);
                        if (next >= 0 && tokens[next].Kind == CppTokenKind.Punctuator && tokens[next].Value == "(")
                        {
                            last = MatchingClose(next, "(", ")");
                        }
                    }

                    if (last < 0)
                    {
                        continue;
                    }

                    for (int a = t; a <= last; a++)
                    {
                        tokens[a] = tokens[a].With(CppTokenFlags.Attribute);
                    }

                    t = last;
                }
            }

            private int NextCodeToken(int t)
            {
                var tokens = result.Tokens;
                while (t < tokens.Count && tokens[t].Kind == CppTokenKind.Comment)
                {
                    t++;
                }

                return t < tokens.Count ? t : -1;
            }

            private int MatchingClose(int open, string openValue, string closeValue)
            {
                var tokens = result.Tokens;
                int depth = 0;
                for (int t = open; t < tokens.Count; t++)
                {
                    if (tokens[t].Kind != CppTokenKind.Punctuator)
                    {
                        continue;
                    }

                    if (tokens[t].Value == openValue)
                    {
                        depth++;
                    }
                    else if (tokens[t].Value == closeValue && --depth == 0)
                    {
                        return t;
                    }
                }

                return tokens.Count - 1;
            }
        }
    }
}
