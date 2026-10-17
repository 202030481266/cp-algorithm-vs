using System;
using System.Collections.Generic;
using System.ComponentModel.Composition;
using System.IO;
using System.Linq;
using System.Security;
using System.Windows;
using System.Windows.Threading;
using EditorFontStyles.Diagnostics;
using Microsoft.VisualStudio.Shell;

namespace EditorFontStyles.Configuration
{
    /// <summary>
    /// Finds the configuration for a document: the nearest .editor-font-styles.json in its folder or a parent
    /// folder (so a repository can carry one), else %USERPROFILE%\.editor-font-styles.json, else the built-in
    /// defaults. Loaded files, including the .vssettings files named by "colors", are watched and edits take
    /// effect immediately.
    /// </summary>
    [Export(typeof(ConfigService))]
    internal sealed class ConfigService
    {
        private const string DefaultResource = "EditorFontStyles.default-config.json";

        private readonly OutputLog log;
        private readonly Dispatcher dispatcher;
        private readonly string defaultJson;
        private readonly object gate = new object();
        private readonly Dictionary<string, CachedConfig> cache = new Dictionary<string, CachedConfig>(StringComparer.OrdinalIgnoreCase);
        private readonly Dictionary<string, CachedTheme> themes = new Dictionary<string, CachedTheme>(StringComparer.OrdinalIgnoreCase);
        private readonly Dictionary<string, FileSystemWatcher> watchers = new Dictionary<string, FileSystemWatcher>(StringComparer.OrdinalIgnoreCase);
        private readonly DispatcherTimer reloadTimer;
        private StyleConfig builtIn;

        [ImportingConstructor]
        public ConfigService(OutputLog log)
        {
            this.log = log;
            dispatcher = Application.Current?.Dispatcher ?? Dispatcher.CurrentDispatcher;
            using (var stream = typeof(ConfigService).Assembly.GetManifestResourceStream(DefaultResource))
            using (var reader = new StreamReader(stream))
            {
                defaultJson = reader.ReadToEnd();
            }

            reloadTimer = new DispatcherTimer(TimeSpan.FromMilliseconds(300), DispatcherPriority.Background, OnReloadTimer, dispatcher);
            reloadTimer.Stop();
            Watch(ProfileConfigPath);
        }

        /// <summary>Raised on the UI thread after a watched configuration file was created, changed or deleted.</summary>
        public event EventHandler ConfigChanged;

        public static string ProfileConfigPath =>
            Path.Combine(Environment.GetFolderPath(Environment.SpecialFolder.UserProfile), StyleConfig.FileName);

        public StyleConfig BuiltIn
        {
            get
            {
                lock (gate)
                {
                    return builtIn ?? (builtIn = Create(null, null));
                }
            }
        }

        public StyleConfig GetConfig(string documentPath)
        {
            string path = FindConfigFile(documentPath);
            return path == null ? BuiltIn : Load(path);
        }

        public static bool IsProfileConfig(string path) =>
            string.Equals(path, ProfileConfigPath, StringComparison.OrdinalIgnoreCase);

        /// <summary>The Text Editor colors of the configuration's "colors" files; reloaded when a file changes.</summary>
        public ColorTheme GetColorTheme(StyleConfig config)
        {
            if (config.ColorFiles.Count == 0)
            {
                return ColorTheme.Empty;
            }

            lock (gate)
            {
                string key = string.Join("|", config.ColorFiles);
                var stamps = config.ColorFiles.Select(Stamp).ToList();
                if (themes.TryGetValue(key, out var cached) && cached.Stamps.SequenceEqual(stamps))
                {
                    return cached.Theme;
                }

                var problems = new List<string>();
                var theme = ColorTheme.Load(config.ColorFiles, problems);
                foreach (string problem in problems)
                {
                    log.WriteLine(problem);
                }

                themes[key] = new CachedTheme(stamps, theme);
                foreach (string file in config.ColorFiles)
                {
                    Watch(file);
                }

                return theme;
            }
        }

        public static string FindConfigFile(string documentPath)
        {
            try
            {
                if (!string.IsNullOrEmpty(documentPath) && Path.IsPathRooted(documentPath))
                {
                    for (string directory = Path.GetDirectoryName(Path.GetFullPath(documentPath)); !string.IsNullOrEmpty(directory); directory = Path.GetDirectoryName(directory))
                    {
                        string candidate = Path.Combine(directory, StyleConfig.FileName);
                        if (File.Exists(candidate))
                        {
                            return candidate;
                        }
                    }
                }

                return File.Exists(ProfileConfigPath) ? ProfileConfigPath : null;
            }
            catch (Exception e) when (e is ArgumentException || e is IOException || e is NotSupportedException || e is SecurityException || e is UnauthorizedAccessException)
            {
                return null;
            }
        }

        private StyleConfig Load(string path)
        {
            lock (gate)
            {
                var stamp = Stamp(path);
                if (cache.TryGetValue(path, out var cached) && cached.Stamp == stamp)
                {
                    return cached.Config;
                }

                string json;
                try
                {
                    json = File.ReadAllText(path);
                }
                catch (Exception e) when (e is IOException || e is UnauthorizedAccessException)
                {
                    log.WriteLine($"{path}: cannot be read now ({e.Message}).");
                    return cached?.Config ?? BuiltIn;
                }

                var config = Create(json, path);
                cache[path] = new CachedConfig(stamp, config);
                Watch(path);
                log.WriteLine($"Loaded {path}.");
                return config;
            }
        }

        private StyleConfig Create(string userJson, string path)
        {
            var problems = new List<string>();
            var config = StyleConfig.Load(defaultJson, userJson, path, problems);
            foreach (string problem in problems)
            {
                log.WriteLine(problem);
            }

            return config;
        }

        private static (DateTime, long) Stamp(string path)
        {
            var info = new FileInfo(path);
            return info.Exists ? (info.LastWriteTimeUtc, info.Length) : (DateTime.MinValue, -1L);
        }

        private void Watch(string path)
        {
            lock (gate)
            {
                if (watchers.ContainsKey(path))
                {
                    return;
                }

                try
                {
                    var watcher = new FileSystemWatcher(Path.GetDirectoryName(path), Path.GetFileName(path))
                    {
                        NotifyFilter = NotifyFilters.LastWrite | NotifyFilters.FileName | NotifyFilters.Size | NotifyFilters.CreationTime,
                    };
                    watcher.Changed += OnFileEvent;
                    watcher.Created += OnFileEvent;
                    watcher.Deleted += OnFileEvent;
                    watcher.Renamed += OnFileEvent;
                    watcher.EnableRaisingEvents = true;
                    watchers.Add(path, watcher);
                }
                catch (Exception e) when (e is ArgumentException || e is IOException || e is PlatformNotSupportedException)
                {
                    log.WriteLine($"{path}: changes will apply after restarting Visual Studio ({e.Message}).");
                }
            }
        }

        private void OnFileEvent(object sender, FileSystemEventArgs e)
        {
            ExtensionThreading.Factory.RunAsync(async () =>
            {
                await ExtensionThreading.Factory.SwitchToMainThreadAsync();
                reloadTimer.Stop();
                reloadTimer.Start();  // editors save in several steps; reload once they are done
            }).FileAndForget("EditorFontStyles/ConfigReload");
        }

        private void OnReloadTimer(object sender, EventArgs e)
        {
            reloadTimer.Stop();
            lock (gate)
            {
                cache.Clear();
                themes.Clear();
            }

            ConfigChanged?.Invoke(this, EventArgs.Empty);
        }

        private sealed class CachedTheme
        {
            public CachedTheme(List<(DateTime, long)> stamps, ColorTheme theme)
            {
                Stamps = stamps;
                Theme = theme;
            }

            public List<(DateTime, long)> Stamps { get; }
            public ColorTheme Theme { get; }
        }

        private sealed class CachedConfig
        {
            public CachedConfig((DateTime, long) stamp, StyleConfig config)
            {
                Stamp = stamp;
                Config = config;
            }

            public (DateTime, long) Stamp { get; }
            public StyleConfig Config { get; }
        }
    }
}
