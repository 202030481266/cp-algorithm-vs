using Microsoft.VisualStudio.Shell;
using Microsoft.VisualStudio.Threading;

namespace EditorFontStyles.Diagnostics
{
    /// <summary>
    /// The extension's own JoinableTaskFactory for fire-and-forget work on the UI thread (logging, raising
    /// events after a file change). Its tasks are tracked in a collection, as the VS threading rules require
    /// for components that have no AsyncPackage.
    /// </summary>
    internal static class ExtensionThreading
    {
        private static readonly JoinableTaskCollection Tasks = ThreadHelper.JoinableTaskContext.CreateCollection();

        public static JoinableTaskFactory Factory { get; } = ThreadHelper.JoinableTaskContext.CreateFactory(Tasks);
    }
}
