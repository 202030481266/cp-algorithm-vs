using System;
using System.Collections.Generic;

namespace EditorFontStyles.Cpp
{
    internal static class SpanDiff
    {
        /// <summary>
        /// The range [Start, End) of the new text whose styled spans differ from the old ones; empty when the
        /// edit only shifted them. Spans before the first difference keep their offsets, spans after the last
        /// one moved by the change in length, so one pass from each end finds the changed middle.
        /// </summary>
        public static (int Start, int End) ChangedRange(IReadOnlyList<StyledSpan> before, int beforeLength, IReadOnlyList<StyledSpan> after, int afterLength)
        {
            int front = 0;
            while (front < before.Count && front < after.Count && before[front].Equals(after[front]))
            {
                front++;
            }

            int delta = afterLength - beforeLength;
            int oldBack = before.Count - 1;
            int newBack = after.Count - 1;
            while (oldBack >= front && newBack >= front && SameShifted(before[oldBack], after[newBack], delta))
            {
                oldBack--;
                newBack--;
            }

            if (oldBack < front && newBack < front)
            {
                return (0, 0);
            }

            int start = int.MaxValue;
            int end = int.MinValue;
            if (newBack >= front)
            {
                start = Math.Min(start, after[front].Start);
                end = Math.Max(end, after[newBack].End);
            }

            if (oldBack >= front)
            {
                start = Math.Min(start, Math.Min(before[front].Start, before[front].Start + delta));
                end = Math.Max(end, Math.Max(before[oldBack].End, before[oldBack].End + delta));
            }

            start = Math.Max(0, Math.Min(start, afterLength));
            end = Math.Max(start, Math.Min(end, afterLength));
            return (start, end);
        }

        private static bool SameShifted(StyledSpan a, StyledSpan b, int delta) =>
            a.Start + delta == b.Start && a.Length == b.Length && a.Style == b.Style;
    }
}
