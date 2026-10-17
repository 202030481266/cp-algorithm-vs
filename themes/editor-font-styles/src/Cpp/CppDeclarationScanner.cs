using System;
using System.Collections.Generic;

namespace EditorFontStyles.Cpp
{
    /// <summary>
    /// Collects the names a C++ file declares: types, functions, variables, parameters, members,
    /// enumerators, template parameters, aliases, namespaces and macros.
    /// </summary>
    /// <remarks>
    /// This is a heuristic parser over tokens, not a compiler front end. It recognizes the usual
    /// "decl-specifiers type declarator-list" shape at statement starts and inside parentheses
    /// (parameters, for/if/catch heads), so "int a, b[N];", "void f(T x)", "auto [u, w] : g[v]" and
    /// "struct S { ... } s;" all work, while expressions such as "cout &lt;&lt; x" or "a[i] = b" declare
    /// nothing. Unbalanced brackets while typing end the enclosing group instead of the whole file.
    /// </remarks>
    internal sealed class CppDeclarationScanner
    {
        private enum Scope
        {
            File,
            Namespace,
            Class,
            Block,
            Enum,
        }

        private const int MaxNesting = 200;

        private static readonly HashSet<string> DeclarationSpecifiers = new HashSet<string>(StringComparer.Ordinal)
        {
            "const", "constexpr", "consteval", "constinit", "static", "inline", "extern", "mutable", "volatile",
            "register", "thread_local", "virtual", "explicit", "friend", "typedef",
        };

        private static readonly HashSet<string> BuiltinTypes = new HashSet<string>(StringComparer.Ordinal)
        {
            "auto", "bool", "char", "char8_t", "char16_t", "char32_t", "double", "float", "int", "long", "short",
            "signed", "unsigned", "void", "wchar_t", "__int8", "__int16", "__int32", "__int64", "__int128",
        };

        /// <summary>Microsoft keywords that are lexed as identifiers but behave like specifiers.</summary>
        private static readonly HashSet<string> Modifiers = new HashSet<string>(StringComparer.Ordinal)
        {
            "__forceinline", "__inline", "__cdecl", "__stdcall", "__fastcall", "__vectorcall", "__thiscall",
            "__clrcall", "__restrict", "__ptr32", "__ptr64", "__unaligned", "__w64",
        };

        private readonly List<CppToken> tokens;
        private readonly ISet<string> systemNames;
        private readonly HashSet<string> declared;
        private readonly HashSet<int> sites;
        private int[] code = Array.Empty<int>();
        private int[] match = Array.Empty<int>();
        private int count;
        private int nesting;

        private CppDeclarationScanner(List<CppToken> tokens, ISet<string> systemNames, HashSet<string> declared, HashSet<int> sites)
        {
            this.tokens = tokens;
            this.systemNames = systemNames;
            this.declared = declared;
            this.sites = sites;
        }

        /// <summary>
        /// Adds every declared name to <paramref name="declared"/> and the token index of each declaration to
        /// <paramref name="sites"/>. Reopening a namespace listed in <paramref name="systemNames"/> (namespace std)
        /// declares nothing.
        /// </summary>
        public static void Scan(CppLexResult lex, ISet<string> systemNames, HashSet<string> declared, HashSet<int> sites)
        {
            var scanner = new CppDeclarationScanner(lex.Tokens, systemNames, declared, sites);
            var main = new List<int>();
            for (int t = 0; t < lex.Tokens.Count; t++)
            {
                var token = lex.Tokens[t];
                if (token.Kind != CppTokenKind.Comment && !token.Has(CppTokenFlags.InDirective))
                {
                    main.Add(t);
                }
            }

            scanner.Run(main);
            foreach (var macro in lex.Macros)
            {
                scanner.DeclareToken(macro.NameToken);
                foreach (int parameter in macro.ParameterTokens)
                {
                    scanner.DeclareToken(parameter);
                }

                var body = new List<int>();
                for (int t = macro.BodyStart; t < macro.BodyEnd; t++)
                {
                    if (lex.Tokens[t].Kind != CppTokenKind.Comment)
                    {
                        body.Add(t);
                    }
                }

                scanner.Run(body);
            }
        }

        private void Run(List<int> codeTokens)
        {
            code = codeTokens.ToArray();
            count = code.Length;
            match = ComputeMatches();
            nesting = 0;
            ParseScope(0, count, Scope.File);
        }

        // ---- token helpers (k is an index into code) ----

        private CppToken Tok(int k) => tokens[code[k]];

        private bool Punct(int k, string value) => k < count && Tok(k).Kind == CppTokenKind.Punctuator && Tok(k).Value == value;

        private bool Keyword(int k, string value) => k < count && Tok(k).Kind == CppTokenKind.Keyword && Tok(k).Value == value;

        private bool Ident(int k) => k < count && Tok(k).Kind == CppTokenKind.Identifier && !Modifiers.Contains(Tok(k).Value);

        private bool IsModifier(int k) => k < count && Tok(k).Kind == CppTokenKind.Identifier && Modifiers.Contains(Tok(k).Value);

        private bool IsAttribute(int k) => k < count && Tok(k).Has(CppTokenFlags.Attribute);

        private bool IsOpener(int k) => Punct(k, "(") || Punct(k, "[") || Punct(k, "{");

        /// <summary>Index after the group opened at <paramref name="k"/>.</summary>
        private int After(int k) => Math.Min(match[k] + 1, count);

        private void Declare(int k)
        {
            DeclareToken(code[k]);
        }

        private void DeclareToken(int tokenIndex)
        {
            var token = tokens[tokenIndex];
            if (token.Kind == CppTokenKind.Identifier)
            {
                declared.Add(token.Value);
                sites.Add(tokenIndex);
            }
        }

        private int[] ComputeMatches()
        {
            var result = new int[count];
            var stack = new List<int>();
            for (int k = 0; k < count; k++)
            {
                result[k] = -1;
                var token = Tok(k);
                if (token.Kind != CppTokenKind.Punctuator)
                {
                    continue;
                }

                switch (token.Value)
                {
                    case "(":
                    case "[":
                    case "{":
                        stack.Add(k);
                        break;
                    case ")":
                    case "]":
                    case "}":
                        string open = token.Value == ")" ? "(" : token.Value == "]" ? "[" : "{";
                        int s = stack.Count - 1;
                        while (s >= 0 && Tok(stack[s]).Value != open)
                        {
                            s--;
                        }

                        if (s < 0)
                        {
                            break;  // stray closer
                        }

                        for (int t = stack.Count - 1; t >= s; t--)
                        {
                            result[stack[t]] = k;  // openers left open inside end here too
                        }

                        result[k] = stack[s];
                        stack.RemoveRange(s, stack.Count - s);
                        break;
                }
            }

            foreach (int open in stack)
            {
                result[open] = count;
            }

            return result;
        }

        private int SkipAttributes(int k, int end)
        {
            while (k < end && IsAttribute(k))
            {
                k++;
            }

            return k;
        }

        /// <summary>Index of the '>' (or '>>') closing the template argument list opened at <paramref name="k"/>, or -1.</summary>
        private int MatchAngle(int k, int end)
        {
            int depth = 0;
            for (int j = k; j < end; j++)
            {
                var token = Tok(j);
                if (token.Kind == CppTokenKind.Keyword && depth == 1 && (token.Value == "and" || token.Value == "or"))
                {
                    return -1;
                }

                if (token.Kind != CppTokenKind.Punctuator)
                {
                    continue;
                }

                switch (token.Value)
                {
                    case "<":
                        depth++;
                        break;
                    case ">":
                        if (--depth == 0)
                        {
                            return j;
                        }

                        break;
                    case ">>":
                        depth -= 2;
                        if (depth <= 0)
                        {
                            return j;
                        }

                        break;
                    case "(":
                    case "[":
                        j = After(j) - 1;
                        break;
                    case "{":
                    case "}":
                    case ")":
                    case "]":
                    case ";":
                        return -1;
                    case "&&":
                    case "||":
                        if (depth == 1)
                        {
                            return -1;  // a < b && c > d is a comparison
                        }

                        break;
                }
            }

            return -1;
        }

        // ---- statements ----

        private void ParseScope(int k, int end, Scope scope)
        {
            while (k < end)
            {
                int next = ParseStatement(k, end, scope);
                k = next > k ? next : k + 1;
            }
        }

        private int ParseBlock(int k, Scope scope)
        {
            int close = Math.Min(match[k], count);
            if (nesting < MaxNesting)
            {
                nesting++;
                ParseScope(k + 1, close, scope);
                nesting--;
            }

            return Math.Min(close + 1, count);
        }

        private int ParseStatement(int k, int end, Scope scope)
        {
            k = SkipAttributes(k, end);
            if (k >= end)
            {
                return end;
            }

            if (scope == Scope.Enum)
            {
                return ParseEnumerators(k, end);
            }

            var token = Tok(k);
            if (token.Kind == CppTokenKind.Punctuator)
            {
                switch (token.Value)
                {
                    case ";":
                    case ")":
                    case "]":
                    case "}":
                        return k + 1;
                    case "{":
                        return ParseBlock(k, Scope.Block);
                    default:
                        return SkipExpression(k, end, false);
                }
            }

            if (token.Kind == CppTokenKind.Keyword)
            {
                switch (token.Value)
                {
                    case "template":
                        int afterHeader = SkipTemplateHeader(k, end);
                        return afterHeader > k ? ParseStatement(afterHeader, end, scope) : k + 1;
                    case "namespace":
                        return ParseNamespace(k, end);
                    case "inline":
                        if (Keyword(k + 1, "namespace"))
                        {
                            return ParseNamespace(k + 1, end);
                        }

                        break;
                    case "using":
                        return ParseUsing(k, end);
                    case "public":
                    case "private":
                    case "protected":
                        return Punct(k + 1, ":") ? k + 2 : k + 1;
                    case "if":
                    case "while":
                    case "switch":
                    case "for":
                    case "catch":
                        return ParseControl(k, end);
                    case "else":
                    case "do":
                    case "try":
                        return ParseBody(k + 1, end);
                    case "case":
                    case "default":
                        return SkipPast(k, end, ":");
                    case "extern":
                        if (k + 1 < end && Tok(k + 1).Kind == CppTokenKind.String)
                        {
                            return Punct(k + 2, "{") ? ParseBlock(k + 2, Scope.Namespace) : ParseStatement(k + 2, end, scope);
                        }

                        break;
                    case "concept":
                        if (Ident(k + 1))
                        {
                            Declare(k + 1);
                        }

                        return SkipExpression(k + 1, end, false);
                    case "static_assert":
                    case "return":
                    case "co_return":
                    case "co_yield":
                    case "throw":
                    case "goto":
                    case "break":
                    case "continue":
                    case "delete":
                    case "asm":
                    case "requires":
                        return SkipExpression(k, end, false);
                }
            }

            if (scope == Scope.Block && Ident(k) && Punct(k + 1, ":"))
            {
                return k + 2;  // label
            }

            return ParseDeclarationOrExpression(k, end, false);
        }

        private int ParseBody(int k, int end)
        {
            if (k >= end)
            {
                return end;
            }

            return Punct(k, "{") ? ParseBlock(k, Scope.Block) : ParseStatement(k, end, Scope.Block);
        }

        private int ParseControl(int k, int end)
        {
            k++;
            if (Keyword(k, "constexpr") || Punct(k, "!"))
            {
                k++;
            }

            if (Keyword(k, "consteval"))
            {
                k++;
            }

            if (Punct(k, "("))
            {
                k = ParseParenGroup(k);
            }

            return ParseBody(k, end);
        }

        private int ParseNamespace(int k, int end)
        {
            k = SkipAttributes(k + 1, end);
            var names = new List<int>();
            while (Ident(k))
            {
                names.Add(k++);
                if (!Punct(k, "::"))
                {
                    break;
                }

                k++;
                if (Keyword(k, "inline"))
                {
                    k++;
                }
            }

            if (Punct(k, "="))
            {
                foreach (int name in names)
                {
                    Declare(name);  // namespace alias
                }

                return SkipExpression(k + 1, end, false);
            }

            foreach (int name in names)
            {
                if (systemNames == null || !systemNames.Contains(Tok(name).Value))
                {
                    Declare(name);
                }
            }

            return Punct(k, "{") ? ParseBlock(k, Scope.Namespace) : k;
        }

        private int ParseUsing(int k, int end)
        {
            k++;
            if (Ident(k))
            {
                int afterName = SkipAttributes(k + 1, end);
                if (Punct(afterName, "="))
                {
                    Declare(k);  // alias declaration
                    return SkipExpression(afterName + 1, end, false);
                }
            }

            return SkipPast(k, end, ";");  // using-directive or using-declaration
        }

        private int SkipTemplateHeader(int k, int end)
        {
            int open = k + 1;
            if (!Punct(open, "<"))
            {
                return open;  // explicit instantiation: "template class X<int>;"
            }

            int close = MatchAngle(open, end);
            if (close < 0)
            {
                return open + 1;
            }

            int element = open + 1;
            while (element < close)
            {
                int elementEnd = TopLevelComma(element, close);
                ParseTemplateParameter(element, elementEnd);
                element = elementEnd + 1;
            }

            return close + 1;
        }

        private int TopLevelComma(int k, int end)
        {
            int angle = 0;
            while (k < end)
            {
                if (IsOpener(k))
                {
                    k = After(k);
                    continue;
                }

                if (Punct(k, "<"))
                {
                    angle++;
                }
                else if (Punct(k, ">"))
                {
                    angle--;
                }
                else if (Punct(k, ">>"))
                {
                    angle -= 2;
                }
                else if (Punct(k, ",") && angle <= 0)
                {
                    return k;
                }

                k++;
            }

            return end;
        }

        private void ParseTemplateParameter(int k, int end)
        {
            k = SkipAttributes(k, end);
            if (Keyword(k, "template"))
            {
                k = SkipTemplateHeader(k, end);  // template template parameter
            }

            if (Keyword(k, "class") || Keyword(k, "typename"))
            {
                k++;
                if (Punct(k, "..."))
                {
                    k++;
                }

                if (Ident(k) && k < end)
                {
                    Declare(k);
                }

                return;
            }

            ParseDeclarationOrExpression(k, end, true);  // int N, auto V, std::integral T
        }

        private int ParseEnumerators(int k, int end)
        {
            while (k < end)
            {
                k = SkipAttributes(k, end);
                if (Ident(k))
                {
                    Declare(k);
                    k++;
                }

                while (k < end && !Punct(k, ","))
                {
                    k = IsOpener(k) ? After(k) : k + 1;
                }

                k++;
            }

            return end;
        }

        // ---- declarations ----

        /// <summary>
        /// Parses "specifiers type declarators" at <paramref name="k"/>, or skips an expression. Inside
        /// parentheses (<paramref name="inParens"/>) it handles one element and stops at ',' or ';'.
        /// </summary>
        private int ParseDeclarationOrExpression(int k, int end, bool inParens)
        {
            int start = k;
            bool sawType = false;
            bool autoType = false;
            while (k < end)
            {
                k = SkipAttributes(k, end);
                if (k >= end)
                {
                    break;
                }

                var token = Tok(k);
                if (token.Kind == CppTokenKind.Keyword)
                {
                    string word = token.Value;
                    if (DeclarationSpecifiers.Contains(word))
                    {
                        k = word == "explicit" && Punct(k + 1, "(") ? After(k + 1) : k + 1;
                        continue;
                    }

                    if (BuiltinTypes.Contains(word))
                    {
                        sawType = true;
                        autoType = word == "auto";
                        k++;
                        continue;
                    }

                    if (word == "decltype" && Punct(k + 1, "("))
                    {
                        sawType = true;
                        autoType = false;
                        k = After(k + 1);
                        continue;
                    }

                    if (word == "typename" && !sawType)
                    {
                        k++;
                        continue;
                    }

                    if ((word == "struct" || word == "class" || word == "union" || word == "enum") && !sawType)
                    {
                        k = ParseClassSpecifier(k, end, out bool hadBody);
                        sawType = true;
                        if (hadBody && Punct(k, ";"))
                        {
                            return inParens ? k : k + 1;
                        }

                        continue;
                    }

                    break;
                }

                if (IsModifier(k))
                {
                    k++;
                    continue;
                }

                if (!sawType && (Ident(k) || Punct(k, "::")))
                {
                    int afterType = ParseTypeName(k, end);
                    if (afterType < 0)
                    {
                        break;
                    }

                    sawType = true;
                    autoType = false;
                    k = afterType;
                    continue;
                }

                break;
            }

            if (!sawType)
            {
                return SkipExpression(start, end, inParens);
            }

            bool first = true;
            while (k < end)
            {
                int afterDeclarator = ParseDeclarator(k, end, autoType, out int name);
                if (afterDeclarator < 0)
                {
                    if (first)
                    {
                        return SkipExpression(start, end, inParens);
                    }

                    break;
                }

                if (name >= 0)
                {
                    Declare(name);
                }

                first = false;
                k = afterDeclarator;
                if (Punct(k, "("))
                {
                    k = SkipFunctionSuffix(ParseParenGroup(k), end);
                    if (Punct(k, "{"))
                    {
                        return ParseBlock(k, Scope.Block);  // function body ends the declaration
                    }

                    if (Punct(k, ":") && !inParens)
                    {
                        k = SkipConstructorInitializers(k, end);
                        if (Punct(k, "{"))
                        {
                            return ParseBlock(k, Scope.Block);
                        }
                    }
                }

                while (Punct(k, "["))
                {
                    k = After(k);
                }

                bool initialized = Punct(k, "=");
                if (Punct(k, ":") || Punct(k, "="))
                {
                    k = SkipInitializer(k + 1, end);  // bit-field width, range-for range or initializer
                }
                else if (Punct(k, "{"))
                {
                    k = SkipInitializer(k, end);
                }

                if (inParens)
                {
                    if (initialized && Punct(k, ",") && Ident(k + 1) && Punct(k + 2, "="))
                    {
                        k++;  // for (int i = 0, j = n - 1; ...): parameters never look like this
                        continue;
                    }

                    return k;
                }

                if (Punct(k, ","))
                {
                    k++;
                    continue;
                }

                return Punct(k, ";") ? k + 1 : k;
            }

            return k;
        }

        /// <summary>Index after a possibly qualified type name with template arguments, or -1.</summary>
        private int ParseTypeName(int k, int end)
        {
            if (Punct(k, "::"))
            {
                k++;
            }

            if (!Ident(k) || k >= end)
            {
                return -1;
            }

            k++;
            while (true)
            {
                if (Punct(k, "<"))
                {
                    int close = MatchAngle(k, end);
                    if (close < 0)
                    {
                        return -1;
                    }

                    k = close + 1;
                }

                if (!Punct(k, "::"))
                {
                    return k;
                }

                int next = k + 1;
                if (Keyword(next, "template"))
                {
                    next++;
                }

                if (!Ident(next))
                {
                    return -1;  // Foo::~Foo, Foo::operator=, pointer to member
                }

                k = next + 1;
            }
        }

        /// <summary>
        /// Parses one declarator; returns the index after it (before parameters or an initializer) or -1.
        /// <paramref name="name"/> is the declared identifier, or -1 for operators and destructors.
        /// </summary>
        private int ParseDeclarator(int k, int end, bool allowStructuredBinding, out int name)
        {
            name = -1;
            while (k < end)
            {
                if (Punct(k, "*") || Punct(k, "&") || Punct(k, "&&") || Punct(k, "^")
                    || Keyword(k, "const") || Keyword(k, "volatile") || IsModifier(k) || IsAttribute(k))
                {
                    k++;
                    continue;
                }

                break;
            }

            if (Punct(k, "..."))
            {
                k++;
            }

            if (k >= end)
            {
                return -1;
            }

            if (Punct(k, "[") && allowStructuredBinding)
            {
                int close = Math.Min(match[k], count);
                bool any = false;
                for (int t = k + 1; t < close; t++)
                {
                    if (Ident(t))
                    {
                        Declare(t);
                        any = true;
                    }
                }

                return any ? close + 1 : -1;
            }

            if (Punct(k, "(") && (Punct(k + 1, "*") || Punct(k + 1, "&") || Punct(k + 1, "^")))
            {
                // (*fp)(args) and (&ref)[N]; "swap(*a, *b)" and "f(*p);" are calls, not declarators.
                int close = Math.Min(match[k], count);
                int inner = ParseDeclarator(k + 1, close, false, out name);
                while (inner >= 0 && inner < close && Punct(inner, "["))
                {
                    inner = After(inner);
                }

                if (inner == close && name >= 0 && (Punct(close + 1, "(") || Punct(close + 1, "[")))
                {
                    return close + 1;
                }

                name = -1;
                return -1;
            }

            if (Keyword(k, "operator"))
            {
                return SkipOperatorName(k, end);
            }

            if (Punct(k, "~") && Ident(k + 1))
            {
                return k + 2;
            }

            if (!Ident(k))
            {
                return -1;
            }

            int last = k++;
            while (true)
            {
                if (Punct(k, "<"))
                {
                    int close = MatchAngle(k, end);
                    if (close < 0 || !Punct(close + 1, "::"))
                    {
                        break;
                    }

                    k = close + 1;
                }

                if (!Punct(k, "::"))
                {
                    break;
                }

                int next = k + 1;
                if (Ident(next))
                {
                    last = next;
                    k = next + 1;
                    continue;
                }

                if (Keyword(next, "operator"))
                {
                    return SkipOperatorName(next, end);
                }

                if (Punct(next, "~") && Ident(next + 1))
                {
                    return next + 2;
                }

                return -1;
            }

            name = last;
            return k;
        }

        /// <summary>Skips "operator+", "operator()", "operator new[]", "operator bool"; returns the index of the '('.</summary>
        private int SkipOperatorName(int k, int end)
        {
            k++;
            if (Punct(k, "(") && Punct(k + 1, ")"))
            {
                return k + 2;
            }

            while (k < end && !Punct(k, "("))
            {
                k = Punct(k, "[") ? After(k) : k + 1;
            }

            return k;
        }

        private int SkipFunctionSuffix(int k, int end)
        {
            while (k < end)
            {
                if (Keyword(k, "const") || Keyword(k, "volatile") || Keyword(k, "override") || Keyword(k, "final")
                    || Keyword(k, "mutable") || Keyword(k, "constexpr") || Keyword(k, "consteval")
                    || Punct(k, "&") || Punct(k, "&&") || IsAttribute(k) || IsModifier(k))
                {
                    k++;
                }
                else if (Keyword(k, "noexcept") || Keyword(k, "throw") || Keyword(k, "requires"))
                {
                    k++;
                    if (Punct(k, "("))
                    {
                        k = ParseParenGroup(k);
                    }
                }
                else if (Punct(k, "->"))
                {
                    k = SkipTrailingReturnType(k + 1, end);
                }
                else
                {
                    break;
                }
            }

            return k;
        }

        private int SkipTrailingReturnType(int k, int end)
        {
            while (k < end)
            {
                if (Punct(k, "{") || Punct(k, ";") || Punct(k, "=") || Punct(k, ",") || Punct(k, ")")
                    || Punct(k, "]") || Punct(k, "}") || Keyword(k, "requires"))
                {
                    break;
                }

                if (Punct(k, "(") || Punct(k, "["))
                {
                    k = After(k);
                }
                else if (Punct(k, "<"))
                {
                    int close = MatchAngle(k, end);
                    k = close < 0 ? k + 1 : close + 1;
                }
                else
                {
                    k++;
                }
            }

            return k;
        }

        /// <summary>Skips ": a(x), b{y}" and returns the index of the constructor body '{'.</summary>
        private int SkipConstructorInitializers(int k, int end)
        {
            k++;
            while (k < end)
            {
                if (Punct(k, "{") && (Punct(k - 1, ")") || Punct(k - 1, "}") || Punct(k - 1, "...")))
                {
                    return k;
                }

                if (Punct(k, "(") )
                {
                    k = ParseParenGroup(k);
                }
                else if (Punct(k, "{"))
                {
                    k = ParseBlock(k, Scope.Block);
                }
                else if (Punct(k, ";") || Punct(k, "}"))
                {
                    return k;
                }
                else
                {
                    k++;
                }
            }

            return k;
        }

        private int ParseClassSpecifier(int k, int end, out bool hadBody)
        {
            hadBody = false;
            bool isEnum = Keyword(k, "enum");
            k++;
            if (isEnum && (Keyword(k, "class") || Keyword(k, "struct")))
            {
                k++;
            }

            k = SkipAttributes(k, end);
            int name = -1;
            bool specialization = false;
            if (Punct(k, "::"))
            {
                k++;
            }

            while (Ident(k) && k < end)
            {
                name = k++;
                specialization = false;
                if (Punct(k, "<"))
                {
                    int close = MatchAngle(k, end);
                    if (close >= 0)
                    {
                        k = close + 1;
                        specialization = true;  // struct hash<P> { ... } adds no new name
                    }
                }

                if (!Punct(k, "::"))
                {
                    break;
                }

                k++;
            }

            if (Keyword(k, "final"))
            {
                k++;
            }

            if (Punct(k, ":"))
            {
                while (k < end && !Punct(k, "{") && !Punct(k, ";"))
                {
                    k = Punct(k, "(") || Punct(k, "[") ? After(k) : k + 1;  // base clause or enum base
                }
            }

            if (Punct(k, "{"))
            {
                if (name >= 0 && !specialization)
                {
                    Declare(name);
                }

                hadBody = true;
                return ParseBlock(k, isEnum ? Scope.Enum : Scope.Class);
            }

            if (name >= 0 && !specialization && Punct(k, ";"))
            {
                Declare(name);  // forward declaration
            }

            return k;
        }

        // ---- expressions ----

        /// <summary>Parses "( ... )": every comma- or semicolon-separated element may be a declaration.</summary>
        private int ParseParenGroup(int k)
        {
            int close = Math.Min(match[k], count);
            if (nesting >= MaxNesting)
            {
                return Math.Min(close + 1, count);
            }

            nesting++;
            int j = k + 1;
            while (j < close)
            {
                int next = ParseDeclarationOrExpression(j, close, true);
                if (next < close && (Punct(next, ",") || Punct(next, ";")))
                {
                    next++;
                }

                j = next > j ? next : j + 1;
            }

            nesting--;
            return Math.Min(close + 1, count);
        }

        /// <summary>
        /// Skips an expression to ';' (statement) or to ',' / ';' (inside parentheses), looking into nested
        /// groups for lambda parameters and bodies. At statement level a "{ ... }" right after ')' is a body
        /// (rep(i, 0, n) { ... }, constructors) and ends the statement.
        /// </summary>
        private int SkipExpression(int k, int end, bool inParens)
        {
            int start = k;
            while (k < end)
            {
                var token = Tok(k);
                if (token.Kind == CppTokenKind.Punctuator)
                {
                    switch (token.Value)
                    {
                        case ";":
                            return inParens ? k : k + 1;
                        case ",":
                            if (inParens)
                            {
                                return k;
                            }

                            break;
                        case "(":
                            k = ParseParenGroup(k);
                            continue;
                        case "[":
                            k = After(k);
                            continue;
                        case "{":
                            bool body = !inParens && k > start
                                && (Punct(k - 1, ")") || Keyword(k - 1, "const") || Keyword(k - 1, "noexcept")
                                    || Keyword(k - 1, "override") || Keyword(k - 1, "mutable") || Keyword(k - 1, "final"));
                            k = ParseBlock(k, Scope.Block);
                            if (body)
                            {
                                return k;
                            }

                            continue;
                        case ")":
                        case "]":
                        case "}":
                            return k;
                    }
                }

                k++;
            }

            return k;
        }

        /// <summary>Skips an initializer to the next top-level ',' or ';', parsing lambdas inside.</summary>
        private int SkipInitializer(int k, int end)
        {
            while (k < end)
            {
                if (Punct(k, ",") || Punct(k, ";") || Punct(k, ")") || Punct(k, "]") || Punct(k, "}"))
                {
                    return k;
                }

                if (Punct(k, "("))
                {
                    k = ParseParenGroup(k);
                }
                else if (Punct(k, "{"))
                {
                    k = ParseBlock(k, Scope.Block);
                }
                else if (Punct(k, "["))
                {
                    k = After(k);
                }
                else
                {
                    k++;
                }
            }

            return k;
        }

        private int SkipPast(int k, int end, string terminator)
        {
            while (k < end)
            {
                if (Punct(k, terminator))
                {
                    return k + 1;
                }

                if (Punct(k, "}") || Punct(k, ")") || Punct(k, "]"))
                {
                    return k;
                }

                k = IsOpener(k) ? After(k) : k + 1;
            }

            return k;
        }
    }
}
