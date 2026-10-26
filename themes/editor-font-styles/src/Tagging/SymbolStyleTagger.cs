using System;
using System.Collections.Generic;
using System.ComponentModel.Composition;
using System.IO;
using System.Linq;
using System.Threading;
using EditorFontStyles.Configuration;
using EditorFontStyles.Cpp;
using EditorFontStyles.Diagnostics;
using Microsoft.VisualStudio.Shell;
using Microsoft.VisualStudio.Text;
using Microsoft.VisualStudio.Text.Classification;
using Microsoft.VisualStudio.Text.Tagging;
using Microsoft.VisualStudio.Utilities;

namespace EditorFontStyles.Tagging
{
    [Export(typeof(ITaggerProvider))]
    [ContentType("C/C++")]
    [TagType(typeof(IClassificationTag))]
    internal sealed class SymbolStyleTaggerProvider : ITaggerProvider
    {
        [Import]
        internal IClassificationTypeRegistryService ClassificationRegistry = null;

        [Import]
        internal ITextDocumentFactoryService DocumentFactory = null;

        [Import]
        internal ConfigService Configs = null;

        [Import]
        internal OutputLog Log = null;

        public ITagger<T> CreateTagger<T>(ITextBuffer buffer) where T : ITag
        {
            if (buffer == null)
            {
                return null;
            }

            var tagger = buffer.Properties.GetOrCreateSingletonProperty(
                typeof(SymbolStyleTagger), () => new SymbolStyleTagger(buffer, this));
            tagger.AddReference();
            return tagger as ITagger<T>;
        }
    }

    /// <summary>
    /// Adds the System Symbol, Local Symbol and Comment Tag classifications to a C/C++ buffer. The whole file
    /// is analyzed again for each new snapshot (a few milliseconds for contest-sized files); when a new
    /// declaration restyles text elsewhere, only the range that changed is refreshed.
    /// </summary>
    internal sealed class SymbolStyleTagger : ITagger<IClassificationTag>, IDisposable
    {
        internal const int MaxDocumentLength = 2 * 1024 * 1024;
        private static readonly TimeSpan ConfigRecheck = TimeSpan.FromSeconds(1);
        private static readonly List<StyledSpan> NoSpans = new List<StyledSpan>();

        private readonly ITextBuffer buffer;
        private readonly SymbolStyleTaggerProvider provider;
        private readonly IClassificationTag systemTag;
        private readonly IClassificationTag localTag;
        private readonly IClassificationTag commentTag;
        private readonly object gate = new object();
        private int references;
        private bool failed;
        private StyleConfig config;
        private string configDocument;
        private DateTime configCheckedAt;
        private IReadOnlyList<string> includeDirectories = Array.Empty<string>();
        private ITextSnapshot analyzedSnapshot;
        private List<StyledSpan> spans = NoSpans;

        public SymbolStyleTagger(ITextBuffer buffer, SymbolStyleTaggerProvider provider)
        {
            this.buffer = buffer;
            this.provider = provider;
            var registry = provider.ClassificationRegistry;
            systemTag = new ClassificationTag(registry.GetClassificationType(StyleClassificationNames.SystemSymbol));
            localTag = new ClassificationTag(registry.GetClassificationType(StyleClassificationNames.LocalSymbol));
            commentTag = new ClassificationTag(registry.GetClassificationType(StyleClassificationNames.CommentTag));
            provider.Configs.ConfigChanged += OnConfigChanged;
        }

        public event EventHandler<SnapshotSpanEventArgs> TagsChanged;

        private string DocumentPath =>
            provider.DocumentFactory.TryGetTextDocument(buffer, out var document) ? document.FilePath : null;

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

            provider.Configs.ConfigChanged -= OnConfigChanged;
            buffer.Properties.RemoveProperty(typeof(SymbolStyleTagger));
        }

        public IEnumerable<ITagSpan<IClassificationTag>> GetTags(NormalizedSnapshotSpanCollection requested)
        {
            if (requested.Count == 0)
            {
                yield break;
            }

            var snapshot = requested[0].Snapshot;
            var current = SpansFor(snapshot);
            foreach (var span in requested)
            {
                int end = span.End.Position;
                for (int i = FirstEndingAfter(current, span.Start.Position); i < current.Count && current[i].Start < end; i++)
                {
                    var styled = current[i];
                    yield return new TagSpan<IClassificationTag>(new SnapshotSpan(snapshot, styled.Start, styled.Length), TagFor(styled.Style));
                }
            }
        }

        private IClassificationTag TagFor(SymbolStyle style)
        {
            switch (style)
            {
                case SymbolStyle.System:
                    return systemTag;
                case SymbolStyle.Local:
                    return localTag;
                default:
                    return commentTag;
            }
        }

        private static int FirstEndingAfter(List<StyledSpan> sorted, int position)
        {
            int low = 0;
            int high = sorted.Count;
            while (low < high)
            {
                int middle = (low + high) / 2;
                if (sorted[middle].End <= position)
                {
                    low = middle + 1;
                }
                else
                {
                    high = middle;
                }
            }

            return low;
        }

        private List<StyledSpan> SpansFor(ITextSnapshot snapshot)
        {
            lock (gate)
            {
                if (snapshot == analyzedSnapshot)
                {
                    return spans;
                }

                RefreshConfig(false);
                var fresh = Analyze(snapshot);
                if (analyzedSnapshot != null && snapshot.Version.VersionNumber < analyzedSnapshot.Version.VersionNumber)
                {
                    return fresh;  // an old snapshot; keep the newer analysis cached
                }

                var before = spans;
                int beforeLength = analyzedSnapshot?.Length ?? -1;
                spans = fresh;
                analyzedSnapshot = snapshot;
                if (beforeLength >= 0)
                {
                    var (start, end) = SpanDiff.ChangedRange(before, beforeLength, fresh, snapshot.Length);
                    if (end > start)
                    {
                        RaiseTagsChangedLater(new SnapshotSpan(snapshot, start, end - start));
                    }
                }

                return fresh;
            }
        }

        private List<StyledSpan> Analyze(ITextSnapshot snapshot)
        {
            if (failed || config == null || !(config.SystemSymbols || config.LocalSymbols || config.CommentTags) || snapshot.Length > MaxDocumentLength)
            {
                return NoSpans;
            }

            try
            {
                var lexed = LexedSnapshot.Of(snapshot);
                var names = config.SystemNames;
                var analysis = CppFileAnalysis.Analyze(lexed.Lex, names);
                string path = configDocument;
                var included = HeaderIndex.Shared.CollectDeclaredNames(
                    analysis.Lex.QuotedIncludes, path == null ? null : Path.GetDirectoryName(path), includeDirectories, names);
                var options = new SymbolStyleOptions
                {
                    SystemSymbols = config.SystemSymbols,
                    LocalSymbols = config.LocalSymbols,
                    CommentTags = config.CommentTags,
                    SystemNames = names,
                    CommentTagWords = config.CommentTagWords,
                };
                return SymbolStyler.Style(lexed.Text, analysis, included, options);
            }
            catch (Exception e)
            {
                failed = true;  // never break the editor: stop styling this buffer and report once
                provider.Log.WriteLine($"Symbol styling stopped for {DocumentPath ?? "a buffer"}: {e}");
                return NoSpans;
            }
        }

        /// <summary>Re-resolves the configuration (at most once a second unless forced); true when it changed.</summary>
        private bool RefreshConfig(bool force)
        {
            var now = DateTime.UtcNow;
            string path = DocumentPath;
            if (!force && config != null && path == configDocument && now - configCheckedAt < ConfigRecheck)
            {
                return false;
            }

            configCheckedAt = now;
            var resolved = provider.Configs.GetConfig(path);
            if (ReferenceEquals(resolved, config) && path == configDocument)
            {
                return false;
            }

            config = resolved;
            configDocument = path;
            includeDirectories = resolved.IncludeDirectories
                .Concat(ProjectIncludeDirectories.Find(path))
                .Distinct(StringComparer.OrdinalIgnoreCase)
                .ToList();
            return true;
        }

        private void OnConfigChanged(object sender, EventArgs e)
        {
            lock (gate)
            {
                if (!RefreshConfig(true))
                {
                    return;
                }

                failed = false;
                analyzedSnapshot = null;
                spans = NoSpans;
            }

            var snapshot = buffer.CurrentSnapshot;
            TagsChanged?.Invoke(this, new SnapshotSpanEventArgs(new SnapshotSpan(snapshot, 0, snapshot.Length)));
        }

        /// <summary>Raised after the current layout pass: other lines use names this edit declared or removed.</summary>
        private void RaiseTagsChangedLater(SnapshotSpan span)
        {
            ExtensionThreading.Factory.RunAsync(async () =>
            {
                await ExtensionThreading.Factory.SwitchToMainThreadAsync(alwaysYield: true);
                var current = span.TranslateTo(buffer.CurrentSnapshot, SpanTrackingMode.EdgeInclusive);
                TagsChanged?.Invoke(this, new SnapshotSpanEventArgs(current));
            }).FileAndForget("EditorFontStyles/TagsChanged");
        }
    }
}
