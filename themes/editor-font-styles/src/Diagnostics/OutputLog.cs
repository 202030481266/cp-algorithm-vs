using System;
using System.ComponentModel.Composition;
using Microsoft.VisualStudio;
using Microsoft.VisualStudio.Shell;
using Microsoft.VisualStudio.Shell.Interop;

namespace EditorFontStyles.Diagnostics
{
    /// <summary>
    /// Writes to the "Editor Font Styles" pane of the Output window (configuration errors, loaded files).
    /// Failures to create the pane go to the Visual Studio activity log.
    /// </summary>
    [Export(typeof(OutputLog))]
    internal sealed class OutputLog
    {
        private const string Source = "Editor Font Styles";
        private static readonly Guid PaneId = new Guid("5B8F2C47-9D1E-4E3A-8C6B-2F7D0A9E1B34");

        private readonly IServiceProvider services;
        private IVsOutputWindowPane pane;

        [ImportingConstructor]
        public OutputLog(SVsServiceProvider services)
        {
            this.services = services;
        }

        /// <summary>Callable from any thread; on the UI thread the line is written before this returns.</summary>
        public void WriteLine(string message)
        {
            string line = $"[{DateTime.Now:HH:mm:ss}] {message}{Environment.NewLine}";
            ExtensionThreading.Factory.RunAsync(async () =>
            {
                await ExtensionThreading.Factory.SwitchToMainThreadAsync();
                try
                {
                    if (pane == null)
                    {
                        if (!(services.GetService(typeof(SVsOutputWindow)) is IVsOutputWindow window))
                        {
                            ActivityLog.TryLogWarning(Source, "Output window service not available: " + message);
                            return;
                        }

                        var id = PaneId;
                        int created = window.CreatePane(ref id, Source, 1, 0);
                        int found = window.GetPane(ref id, out pane);
                        if (ErrorHandler.Failed(created) || ErrorHandler.Failed(found) || pane == null)
                        {
                            ActivityLog.TryLogWarning(Source, $"Output pane not created (0x{created:X8}, 0x{found:X8}): {message}");
                            return;
                        }
                    }

                    pane.OutputStringThreadSafe(line);
                }
                catch (Exception e)
                {
                    ActivityLog.TryLogError(Source, $"{message}{Environment.NewLine}{e}");
                }
            }).FileAndForget("EditorFontStyles/OutputLog");
        }
    }
}
