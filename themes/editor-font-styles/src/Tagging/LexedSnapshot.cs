using EditorFontStyles.Cpp;
using Microsoft.VisualStudio.Text;

namespace EditorFontStyles.Tagging
{
    /// <summary>
    /// The text and tokens of a snapshot. The latest one of each buffer is kept, so the symbol styling and brace
    /// matching taggers lex a snapshot only once between them.
    /// </summary>
    internal sealed class LexedSnapshot
    {
        private LexedSnapshot(ITextSnapshot snapshot, string text)
        {
            Snapshot = snapshot;
            Text = text;
            Lex = CppLexer.Lex(text);
        }

        public ITextSnapshot Snapshot { get; }
        public string Text { get; }
        public CppLexResult Lex { get; }

        public static LexedSnapshot Of(ITextSnapshot snapshot)
        {
            var latest = snapshot.TextBuffer.Properties.GetOrCreateSingletonProperty(typeof(LexedSnapshot), () => new Latest());
            lock (latest)
            {
                if (latest.Value != null && latest.Value.Snapshot == snapshot)
                {
                    return latest.Value;
                }

                var lexed = new LexedSnapshot(snapshot, snapshot.GetText());
                if (latest.Value == null || snapshot.Version.VersionNumber > latest.Value.Snapshot.Version.VersionNumber)
                {
                    latest.Value = lexed;
                }

                return lexed;
            }
        }

        private sealed class Latest
        {
            public LexedSnapshot Value;
        }
    }
}
