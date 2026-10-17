using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;

namespace EditorFontStyles.Cpp
{
    /// <summary>Names declared by the standard library headers (resources/system-symbols.txt).</summary>
    internal static class SystemSymbolTable
    {
        public const string ResourceName = "EditorFontStyles.system-symbols.txt";

        private static readonly Lazy<HashSet<string>> DefaultNames = new Lazy<HashSet<string>>(LoadEmbedded);

        public static HashSet<string> Default => DefaultNames.Value;

        public static HashSet<string> Parse(TextReader reader)
        {
            var names = new HashSet<string>(StringComparer.Ordinal);
            string line;
            while ((line = reader.ReadLine()) != null)
            {
                line = line.Trim();
                if (line.Length > 0 && line[0] != '#')
                {
                    names.Add(line);
                }
            }

            return names;
        }

        /// <summary>The default table plus <paramref name="extra"/> minus <paramref name="excluded"/>.</summary>
        public static ISet<string> Compose(IReadOnlyCollection<string> extra, IReadOnlyCollection<string> excluded)
        {
            if ((extra == null || extra.Count == 0) && (excluded == null || excluded.Count == 0))
            {
                return Default;
            }

            var names = new HashSet<string>(Default, StringComparer.Ordinal);
            names.UnionWith(extra ?? Enumerable.Empty<string>());
            names.ExceptWith(excluded ?? Enumerable.Empty<string>());
            return names;
        }

        private static HashSet<string> LoadEmbedded()
        {
            using (var stream = typeof(SystemSymbolTable).Assembly.GetManifestResourceStream(ResourceName))
            {
                if (stream == null)
                {
                    return new HashSet<string>(StringComparer.Ordinal);
                }

                using (var reader = new StreamReader(stream))
                {
                    return Parse(reader);
                }
            }
        }
    }
}
