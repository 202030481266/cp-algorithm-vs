using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text.RegularExpressions;

namespace EditorFontStyles.Cpp
{
    /// <summary>
    /// Names declared by the project headers a file includes with quotes (#include "fenwick.hpp"), so
    /// that a project function named like a library one (size, count) is not shown as a system symbol.
    /// Headers are resolved like the compiler does for quoted includes: next to the including file first,
    /// then in the include directories. Results are cached per file and refreshed when the file changes.
    /// </summary>
    internal sealed class HeaderIndex
    {
        public static readonly HeaderIndex Shared = new HeaderIndex();

        private const int MaxDepth = 4;
        private const int MaxFiles = 64;
        private const long MaxFileSize = 2 << 20;
        private static readonly TimeSpan RecheckInterval = TimeSpan.FromSeconds(2);

        private readonly object gate = new object();
        private readonly Dictionary<string, Entry> entries = new Dictionary<string, Entry>(StringComparer.OrdinalIgnoreCase);

        public HashSet<string> CollectDeclaredNames(IReadOnlyList<string> includes, string directory, IReadOnlyList<string> includeDirectories, ISet<string> systemNames)
        {
            var names = new HashSet<string>(StringComparer.Ordinal);
            if (includes.Count == 0)
            {
                return names;
            }

            var visited = new HashSet<string>(StringComparer.OrdinalIgnoreCase);
            var queue = new Queue<(string Include, string Directory, int Depth)>();
            foreach (string include in includes)
            {
                queue.Enqueue((include, directory, 0));
            }

            while (queue.Count > 0 && visited.Count < MaxFiles)
            {
                var (include, baseDirectory, depth) = queue.Dequeue();
                string path = Resolve(include, baseDirectory, includeDirectories);
                if (path == null || !visited.Add(path))
                {
                    continue;
                }

                var entry = GetEntry(path, systemNames);
                if (entry == null)
                {
                    continue;
                }

                names.UnionWith(entry.Names);
                if (depth + 1 < MaxDepth)
                {
                    foreach (string nested in entry.Includes)
                    {
                        queue.Enqueue((nested, Path.GetDirectoryName(path), depth + 1));
                    }
                }
            }

            return names;
        }

        private static string Resolve(string include, string baseDirectory, IReadOnlyList<string> includeDirectories)
        {
            try
            {
                if (Path.IsPathRooted(include))
                {
                    return File.Exists(include) ? Path.GetFullPath(include) : null;
                }

                if (!string.IsNullOrEmpty(baseDirectory))
                {
                    string local = Path.Combine(baseDirectory, include);
                    if (File.Exists(local))
                    {
                        return Path.GetFullPath(local);
                    }
                }

                foreach (string directory in includeDirectories)
                {
                    string candidate = Path.Combine(directory, include);
                    if (File.Exists(candidate))
                    {
                        return Path.GetFullPath(candidate);
                    }
                }
            }
            catch (Exception e) when (e is ArgumentException || e is IOException || e is NotSupportedException || e is UnauthorizedAccessException)
            {
            }

            return null;
        }

        private Entry GetEntry(string path, ISet<string> systemNames)
        {
            lock (gate)
            {
                var now = DateTime.UtcNow;
                if (entries.TryGetValue(path, out var cached) && now - cached.CheckedAt < RecheckInterval)
                {
                    return cached;
                }

                try
                {
                    var info = new FileInfo(path);
                    if (!info.Exists || info.Length > MaxFileSize)
                    {
                        entries.Remove(path);
                        return null;
                    }

                    if (cached != null && cached.Stamp == info.LastWriteTimeUtc && cached.Length == info.Length)
                    {
                        cached.CheckedAt = now;
                        return cached;
                    }

                    var analysis = CppFileAnalysis.Analyze(File.ReadAllText(path), systemNames);
                    var entry = new Entry
                    {
                        Stamp = info.LastWriteTimeUtc,
                        Length = info.Length,
                        CheckedAt = now,
                        Names = analysis.DeclaredNames,
                        Includes = analysis.Lex.QuotedIncludes,
                    };
                    entries[path] = entry;
                    return entry;
                }
                catch (Exception e) when (e is IOException || e is UnauthorizedAccessException || e is ArgumentException)
                {
                    return cached;
                }
            }
        }

        private sealed class Entry
        {
            public DateTime Stamp;
            public long Length;
            public DateTime CheckedAt;
            public HashSet<string> Names;
            public List<string> Includes;
        }
    }

    /// <summary>Include directories of the nearest .vcxproj above a document (AdditionalIncludeDirectories, IncludePath).</summary>
    internal static class ProjectIncludeDirectories
    {
        private static readonly Regex Setting = new Regex(
            @"<(?:AdditionalIncludeDirectories|IncludePath)(?:\s[^>]*)?>([^<]*)</", RegexOptions.CultureInvariant);

        private static readonly object Gate = new object();
        private static readonly Dictionary<string, (DateTime Stamp, List<string> Directories)> Cache =
            new Dictionary<string, (DateTime, List<string>)>(StringComparer.OrdinalIgnoreCase);

        public static IReadOnlyList<string> Find(string documentPath)
        {
            if (string.IsNullOrEmpty(documentPath))
            {
                return Array.Empty<string>();
            }

            try
            {
                for (string directory = Path.GetDirectoryName(Path.GetFullPath(documentPath)); !string.IsNullOrEmpty(directory); directory = Path.GetDirectoryName(directory))
                {
                    string[] projects = Directory.GetFiles(directory, "*.vcxproj");
                    if (projects.Length == 0)
                    {
                        continue;
                    }

                    var directories = new List<string>();
                    foreach (string project in projects)
                    {
                        foreach (string include in Load(project))
                        {
                            if (!directories.Contains(include, StringComparer.OrdinalIgnoreCase))
                            {
                                directories.Add(include);
                            }
                        }
                    }

                    return directories;
                }
            }
            catch (Exception e) when (e is ArgumentException || e is IOException || e is NotSupportedException || e is UnauthorizedAccessException)
            {
            }

            return Array.Empty<string>();
        }

        /// <summary>Directories listed by the project; entries with macros other than $(ProjectDir)/$(SolutionDir) are skipped.</summary>
        public static List<string> Parse(string projectXml, string projectDirectory)
        {
            var result = new List<string>();
            string root = projectDirectory.TrimEnd('\\', '/') + "\\";
            foreach (Match setting in Setting.Matches(projectXml))
            {
                foreach (string raw in setting.Groups[1].Value.Split(';'))
                {
                    string entry = raw.Trim()
                        .Replace("$(ProjectDir)", root)
                        .Replace("$(SolutionDir)", root)
                        .Replace("$(MSBuildProjectDirectory)", root)
                        .Replace("$(MSBuildThisFileDirectory)", root);
                    if (entry.Length == 0 || entry.Contains("$(") || entry.Contains("%("))
                    {
                        continue;
                    }

                    try
                    {
                        string full = Path.GetFullPath(Path.Combine(projectDirectory, entry));
                        if (Directory.Exists(full) && !result.Contains(full, StringComparer.OrdinalIgnoreCase))
                        {
                            result.Add(full);
                        }
                    }
                    catch (Exception e) when (e is ArgumentException || e is NotSupportedException || e is IOException)
                    {
                    }
                }
            }

            return result;
        }

        private static List<string> Load(string project)
        {
            var stamp = File.GetLastWriteTimeUtc(project);
            lock (Gate)
            {
                if (Cache.TryGetValue(project, out var cached) && cached.Stamp == stamp)
                {
                    return cached.Directories;
                }
            }

            var directories = Parse(File.ReadAllText(project), Path.GetDirectoryName(project));
            lock (Gate)
            {
                Cache[project] = (stamp, directories);
            }

            return directories;
        }
    }
}
