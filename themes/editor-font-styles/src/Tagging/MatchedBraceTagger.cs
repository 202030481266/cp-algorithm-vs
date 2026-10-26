using System;
using System.Collections.Generic;
using System.ComponentModel.Composition;
using System.Threading;
using EditorFontStyles.Configuration;
using EditorFontStyles.Cpp;
using EditorFontStyles.Diagnostics;
using Microsoft.VisualStudio.Shell;
using Microsoft.VisualStudio.Text;
using Microsoft.VisualStudio.Text.Classification;
using Microsoft.VisualStudio.Text.Editor;
using Microsoft.VisualStudio.Text.Tagging;
using Microsoft.VisualStudio.Utilities;

namespace EditorFontStyles.Tagging
{
    [Export(typeof(IViewTaggerProvider))]
    [ContentType("C/C++")]
    [TagType(typeof(IClassificationTag))]
    [TextViewRole(PredefinedTextViewRoles.Interactive)]
    internal sealed class MatchedBraceTaggerProvider : IViewTaggerProvider
    {
        [Import]
        internal IClassificationTypeRegistryService ClassificationRegistry = null;

        [Import]
        internal ITextDocumentFactoryService DocumentFactory = null;

        [Import]
        internal ConfigService Configs = null;

        [Import]
        internal OutputLog Log = null;

        public ITagger<T> CreateTagger<T>(ITextView textView, ITextBuffer buffer) where T : ITag
        {
            if (textView == null || buffer == null || buffer != textView.TextBuffer)
            {
                return null;  // the caret position is in the view's own buffer
            }

            var tagger = textView.Properties.GetOrCreateSingletonProperty(
                typeof(MatchedBraceTagger), () => new MatchedBraceTagger(textView, this));
            tagger.AddReference();
            return tagger as ITagger<T>;
        }
    }

    /// <summary>
    /// Classifies the bracket pair next to the caret as Matched Brace (turquoise, bold and underlined by default).
    /// Visual Studio's own brace highlight only draws a box, and only with the caret before an opening bracket or
    /// after a closing one; this also covers the caret just inside the pair, like VS Code. The pairs come from the
    /// lexer (see <see cref="BracePairs"/>), so brackets in comments and literals never match.
    /// </summary>
    internal sealed class MatchedBraceTagger : ITagger<IClassificationTag>, IDisposable
    {
        private static readonly TimeSpan ConfigRecheck = TimeSpan.FromSeconds(1);

        private readonly ITextView view;
        private readonly MatchedBraceTaggerProvider provider;
        private readonly IClassificationTag tag;
        private int references;
        private bool attached;
        private bool scheduled;
        private bool failed;
        private bool enabled;
        private string configDocument;
        private DateTime configCheckedAt;
        private ITextSnapshot pairsSnapshot;
        private BracePairs pairs;
        private Pair shown;  // the highlighted pair, or null

        public MatchedBraceTagger(ITextView view, MatchedBraceTaggerProvider provider)
        {
            this.view = view;
            this.provider = provider;
            tag = new ClassificationTag(provider.ClassificationRegistry.GetClassificationType(StyleClassificationNames.MatchedBrace));
            view.Caret.PositionChanged += OnCaretMoved;
            view.LayoutChanged += OnLayoutChanged;
            view.Closed += OnClosed;
            provider.Configs.ConfigChanged += OnConfigChanged;
            attached = true;
            ScheduleUpdate();  // taggers are created during a layout
        }

        public event EventHandler<SnapshotSpanEventArgs> TagsChanged;

        private string DocumentPath =>
            provider.DocumentFactory.TryGetTextDocument(view.TextDataModel.DocumentBuffer, out var document) ? document.FilePath : null;

        public void AddReference()
        {
            Interlocked.Increment(ref references);
        }

        public void Dispose()
        {
            if (Interlocked.Decrement(ref references) > 0)
            {
                return;
            }

            Detach();
            view.Properties.RemoveProperty(typeof(MatchedBraceTagger));
        }

        public IEnumerable<ITagSpan<IClassificationTag>> GetTags(NormalizedSnapshotSpanCollection spans)
        {
            var pair = shown;
            if (spans.Count == 0 || pair == null)
            {
                yield break;
            }

            var snapshot = spans[0].Snapshot;
            foreach (int position in new[] { pair.Open, pair.Close })
            {
                var bracket = BracketIn(pair.Snapshot, position, snapshot);
                if (bracket.HasValue && spans.IntersectsWith(bracket.Value))
                {
                    yield return new TagSpan<IClassificationTag>(bracket.Value, tag);
                }
            }
        }

        /// <summary>The bracket at <paramref name="position"/> of <paramref name="from"/>, in <paramref name="snapshot"/>; null when an edit removed it.</summary>
        private static SnapshotSpan? BracketIn(ITextSnapshot from, int position, ITextSnapshot snapshot)
        {
            var span = new SnapshotSpan(from, position, 1);
            if (snapshot != from)
            {
                span = span.TranslateTo(snapshot, SpanTrackingMode.EdgeExclusive);
                if (span.Length != 1 || !BracePairs.IsBracket(span.Start.GetChar()))
                {
                    return null;
                }
            }

            return span;
        }

        private void OnCaretMoved(object sender, CaretPositionChangedEventArgs e)
        {
            UpdateNowOrLater();
        }

        private void OnLayoutChanged(object sender, TextViewLayoutChangedEventArgs e)
        {
            if (e.NewSnapshot != e.OldSnapshot)
            {
                UpdateNowOrLater();  // an edit that did not move the caret can still change the pairs
            }
        }

        private void OnConfigChanged(object sender, EventArgs e)
        {
            configCheckedAt = default;
            failed = false;
            UpdateNowOrLater();
        }

        private void OnClosed(object sender, EventArgs e)
        {
            Detach();
        }

        private void Detach()
        {
            if (!attached)
            {
                return;
            }

            attached = false;
            view.Caret.PositionChanged -= OnCaretMoved;
            view.LayoutChanged -= OnLayoutChanged;
            view.Closed -= OnClosed;
            provider.Configs.ConfigChanged -= OnConfigChanged;
        }

        private void UpdateNowOrLater()
        {
            if (view.InLayout)
            {
                ScheduleUpdate();
            }
            else
            {
                Update();
            }
        }

        /// <summary>Updates after the current layout pass; requests made before it runs are merged.</summary>
        private void ScheduleUpdate()
        {
            if (scheduled)
            {
                return;
            }

            scheduled = true;
            ExtensionThreading.Factory.RunAsync(async () =>
            {
                await ExtensionThreading.Factory.SwitchToMainThreadAsync(alwaysYield: true);
                scheduled = false;
                Update();
            }).FileAndForget("EditorFontStyles/MatchedBrace");
        }

        private void Update()
        {
            if (!attached || failed)
            {
                return;
            }

            try
            {
                RefreshConfig();
                var caret = view.Caret.Position;
                var point = caret.BufferPosition;
                int newOpen = -1;
                int newClose = -1;
                bool found = enabled && point.Snapshot.Length <= SymbolStyleTagger.MaxDocumentLength
                    && PairsFor(point.Snapshot).TryGetPair(point.Position, caret.VirtualSpaces > 0, out newOpen, out newClose);
                Show(found ? point.Snapshot : null, newOpen, newClose);
            }
            catch (Exception e)
            {
                failed = true;  // never break the editor: stop highlighting in this view and report once
                shown = null;
                provider.Log.WriteLine($"Brace matching stopped for {DocumentPath ?? "a view"}: {e}");
            }
        }

        private void RefreshConfig()
        {
            var now = DateTime.UtcNow;
            string path = DocumentPath;
            if (path == configDocument && now - configCheckedAt < ConfigRecheck)
            {
                return;
            }

            configCheckedAt = now;
            configDocument = path;
            enabled = provider.Configs.GetConfig(path).MatchedBraces;
        }

        private BracePairs PairsFor(ITextSnapshot snapshot)
        {
            if (snapshot != pairsSnapshot)
            {
                pairs = BracePairs.Find(LexedSnapshot.Of(snapshot).Lex);
                pairsSnapshot = snapshot;
            }

            return pairs;
        }

        /// <summary>Highlights the brackets at <paramref name="newOpen"/> and <paramref name="newClose"/> of <paramref name="snapshot"/>, or none when it is null.</summary>
        private void Show(ITextSnapshot snapshot, int newOpen, int newClose)
        {
            var target = snapshot ?? view.TextSnapshot;
            var changed = new List<SnapshotSpan>(4);
            var old = shown;
            if (old != null)
            {
                foreach (int position in new[] { old.Open, old.Close })
                {
                    var bracket = BracketIn(old.Snapshot, position, target);
                    if (bracket.HasValue)
                    {
                        changed.Add(bracket.Value);
                    }
                }
            }

            if (snapshot != null)
            {
                changed.Add(new SnapshotSpan(snapshot, newOpen, 1));
                changed.Add(new SnapshotSpan(snapshot, newClose, 1));
            }

            // The same brackets as before (the caret went to their other side, or an edit moved them): nothing to redraw.
            bool same = changed.Count == 4 && snapshot != null && changed[0] == changed[2] && changed[1] == changed[3];
            shown = snapshot == null ? null : new Pair(snapshot, newOpen, newClose);
            if (same)
            {
                return;
            }

            foreach (var span in changed)
            {
                TagsChanged?.Invoke(this, new SnapshotSpanEventArgs(span));
            }
        }

        /// <summary>Replaced as a whole, so GetTags never sees the brackets of one snapshot with another snapshot.</summary>
        private sealed class Pair
        {
            public Pair(ITextSnapshot snapshot, int open, int close)
            {
                Snapshot = snapshot;
                Open = open;
                Close = close;
            }

            public ITextSnapshot Snapshot { get; }
            public int Open { get; }
            public int Close { get; }
        }
    }
}
