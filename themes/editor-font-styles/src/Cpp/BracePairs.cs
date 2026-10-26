using System;
using System.Collections.Generic;

namespace EditorFontStyles.Cpp
{
    /// <summary>
    /// The bracket pairs ((), [] and {}) of a lexed C/C++ file, for highlighting the pair at the caret.
    /// Brackets in comments, literals and "#if 0" groups are not tokens and never count. Brackets of a
    /// preprocessor directive pair only within that directive, so "#define BEGIN {" leaves the braces of the
    /// code alone. A closing bracket that does not fit the innermost open bracket closes the nearest open bracket
    /// of its kind and the ones in between stay unmatched: a "(" not closed yet inside a block does not unbalance
    /// the block. With no open bracket of its kind, the closing bracket stays unmatched.
    /// </summary>
    internal sealed class BracePairs
    {
        private const string Opening = "([{";
        private const string Closing = ")]}";

        private readonly int[] positions;  // offsets of the brackets, ascending
        private readonly int[] partners;   // index of the matching bracket, -1 when unmatched
        private readonly bool[] opening;

        private BracePairs(int[] positions, int[] partners, bool[] opening)
        {
            this.positions = positions;
            this.partners = partners;
            this.opening = opening;
        }

        public static bool IsBracket(char c) => Opening.IndexOf(c) >= 0 || Closing.IndexOf(c) >= 0;

        public static BracePairs Find(CppLexResult lex)
        {
            var positions = new List<int>();
            var partners = new List<int>();
            var opening = new List<bool>();
            var kinds = new List<int>();
            var code = new List<int>();       // open brackets of the code, innermost last
            var directive = new List<int>();  // open brackets of the current directive
            foreach (var token in lex.Tokens)
            {
                if (token.Kind == CppTokenKind.Directive && token.Value == "#")
                {
                    directive.Clear();  // brackets left open by the previous directive stay unmatched
                    continue;
                }

                if (token.Kind != CppTokenKind.Punctuator || token.Length != 1)
                {
                    continue;
                }

                int kind = Opening.IndexOf(token.Value[0]);
                bool open = kind >= 0;
                if (!open && (kind = Closing.IndexOf(token.Value[0])) < 0)
                {
                    continue;
                }

                int index = positions.Count;
                positions.Add(token.Start);
                partners.Add(-1);
                opening.Add(open);
                kinds.Add(kind);
                var stack = token.Has(CppTokenFlags.InDirective) ? directive : code;
                if (open)
                {
                    stack.Add(index);
                    continue;
                }

                int depth = stack.Count - 1;
                while (depth >= 0 && kinds[stack[depth]] != kind)
                {
                    depth--;
                }

                if (depth >= 0)
                {
                    partners[index] = stack[depth];
                    partners[stack[depth]] = index;
                    stack.RemoveRange(depth, stack.Count - depth);
                }
            }

            return new BracePairs(positions.ToArray(), partners.ToArray(), opening.ToArray());
        }

        /// <summary>
        /// The pair of a bracket next to the caret, which is before the character at <paramref name="caret"/>:
        /// first a closing bracket before the caret or an opening one after it (the brackets Visual Studio itself
        /// highlights), then a closing bracket after the caret or an opening one before it, like VS Code. In
        /// <paramref name="virtualSpace"/> past the end of the line, the character before the caret is not next to it.
        /// </summary>
        public bool TryGetPair(int caret, bool virtualSpace, out int open, out int close)
        {
            int before = virtualSpace ? -1 : IndexAt(caret - 1);
            int after = IndexAt(caret);
            return PairOf(before, false, out open, out close)
                || PairOf(after, true, out open, out close)
                || PairOf(after, false, out open, out close)
                || PairOf(before, true, out open, out close);
        }

        private int IndexAt(int offset)
        {
            int index = offset < 0 ? -1 : Array.BinarySearch(positions, offset);
            return index < 0 ? -1 : index;
        }

        private bool PairOf(int index, bool isOpening, out int open, out int close)
        {
            open = close = -1;
            if (index < 0 || opening[index] != isOpening || partners[index] < 0)
            {
                return false;
            }

            open = positions[Math.Min(index, partners[index])];
            close = positions[Math.Max(index, partners[index])];
            return true;
        }
    }
}
