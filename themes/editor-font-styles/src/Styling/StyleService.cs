using System;
using System.Collections.Generic;
using System.ComponentModel.Composition;
using System.IO;
using System.Linq;
using EditorFontStyles.Configuration;
using EditorFontStyles.Diagnostics;
using EditorFontStyles.Tagging;
using Microsoft.VisualStudio.Text;
using Microsoft.VisualStudio.Text.Classification;
using Microsoft.VisualStudio.Text.Editor;
using Microsoft.VisualStudio.Utilities;

namespace EditorFontStyles.Styling
{
    /// <summary>
    /// Keeps the editor format maps colored and styled with one configuration at a time. Format maps are
    /// shared by all editors, so the configuration follows the focused document, except that a document
    /// with no configuration file of its own (a library header reached by Go To Definition) keeps the
    /// project configuration that is already active instead of switching to the defaults.
    /// </summary>
    [Export(typeof(StyleService))]
    internal sealed class StyleService
    {
        private readonly ConfigService configs;
        private readonly OutputLog log;
        private readonly List<ColorApplier> appliers = new List<ColorApplier>();
        private readonly List<FormatMapStyler> stylers = new List<FormatMapStyler>();
        private readonly IReadOnlyList<IClassificationType> topTypes;
        private StyleConfig active;
        private ColorTheme activeTheme = ColorTheme.Empty;
        private string activeDocument;

        [ImportingConstructor]
        public StyleService(ConfigService configs, OutputLog log, IClassificationTypeRegistryService registry)
        {
            this.configs = configs;
            this.log = log;
            topTypes = new[] { StyleClassificationNames.SystemSymbol, StyleClassificationNames.LocalSymbol, StyleClassificationNames.CommentTag }
                .Select(registry.GetClassificationType)
                .Where(type => type != null)
                .ToList();
            configs.ConfigChanged += OnConfigChanged;
        }

        private void OnConfigChanged(object sender, EventArgs e)
        {
            // Raised from a dispatcher timer: an exception escaping here would terminate Visual Studio.
            try
            {
                Activate(activeDocument, force: true);
            }
            catch (Exception error)
            {
                log.WriteLine($"Reloading the configuration failed: {error}");
            }
        }

        public void Attach(IWpfTextView view, IEditorFormatMap editorMap, IClassificationFormatMap classificationMap, Func<string> documentPath)
        {
            if (!appliers.Any(a => a.Map == editorMap))
            {
                var applier = new ColorApplier(editorMap);
                appliers.Add(applier);
                if (active != null)
                {
                    applier.Apply(activeTheme);
                }
            }

            if (!stylers.Any(s => s.Map == classificationMap))
            {
                var styler = new FormatMapStyler(classificationMap, topTypes, log);
                stylers.Add(styler);
                if (active != null)
                {
                    styler.Apply(active);
                }
            }

            Activate(documentPath(), force: false);
            EventHandler focus = (sender, e) => Activate(documentPath(), force: false);
            view.GotAggregateFocus += focus;
            view.Closed += (sender, e) => view.GotAggregateFocus -= focus;
        }

        private static bool IsProjectConfig(StyleConfig config) =>
            config?.SourcePath != null && !ConfigService.IsProfileConfig(config.SourcePath);

        private void Activate(string documentPath, bool force)
        {
            var config = configs.GetConfig(documentPath);
            if (!force && !IsProjectConfig(config) && IsProjectConfig(active) && File.Exists(active.SourcePath))
            {
                return;  // keep the project's look while a header outside the project has the focus
            }

            activeDocument = documentPath;
            var theme = configs.GetColorTheme(config);
            if (ReferenceEquals(config, active) && ReferenceEquals(theme, activeTheme))
            {
                return;
            }

            if (active == null)
            {
                log.WriteLine($"Editor Font Styles {typeof(StyleService).Assembly.GetName().Version.ToString(3)} started.");
            }

            log.WriteLine($"Using {config.Description}.");
            active = config;
            activeTheme = theme;
            List<string> unknown = null;
            foreach (var applier in appliers)
            {
                var missing = applier.Apply(theme);
                unknown = unknown ?? missing;
            }

            if (theme.Items.Count > 0)
            {
                string files = string.Join(", ", theme.Files.Select(Path.GetFileName));
                string note = unknown != null && unknown.Count > 0
                    ? $"; not defined in this Visual Studio: {string.Join(", ", unknown)}"
                    : string.Empty;
                log.WriteLine($"Colors: {theme.Items.Count} items from {files}{note}.");
            }

            foreach (var styler in stylers)
            {
                styler.Apply(config);
            }
        }
    }

    [Export(typeof(IWpfTextViewCreationListener))]
    [ContentType("text")]
    [TextViewRole(PredefinedTextViewRoles.Document)]
    internal sealed class TextViewStyleListener : IWpfTextViewCreationListener
    {
        [Import]
        internal IEditorFormatMapService EditorFormatMapService = null;

        [Import]
        internal IClassificationFormatMapService FormatMapService = null;

        [Import]
        internal ITextDocumentFactoryService DocumentFactory = null;

        [Import]
        internal StyleService Styles = null;

        public void TextViewCreated(IWpfTextView textView)
        {
            string DocumentPath() =>
                DocumentFactory.TryGetTextDocument(textView.TextDataModel.DocumentBuffer, out var document) ? document.FilePath : null;

            Styles.Attach(
                textView,
                EditorFormatMapService.GetEditorFormatMap(textView),
                FormatMapService.GetClassificationFormatMap(textView),
                DocumentPath);
        }
    }
}
