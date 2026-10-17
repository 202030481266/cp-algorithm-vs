using System.ComponentModel.Composition;
using System.Windows.Media;
using Microsoft.VisualStudio.Text.Classification;
using Microsoft.VisualStudio.Utilities;

namespace EditorFontStyles.Tagging
{
    /// <summary>
    /// Classifications added on top of the language's own ones. They set only font properties (and a color
    /// for comment tags), so a system symbol keeps its function/type color and becomes italic. They appear in
    /// Tools > Options > Fonts and Colors and can be listed in .vssettings files like any other item.
    /// </summary>
    internal static class StyleClassificationNames
    {
        public const string SystemSymbol = "EditorFontStyles.SystemSymbol";
        public const string LocalSymbol = "EditorFontStyles.LocalSymbol";
        public const string CommentTag = "EditorFontStyles.CommentTag";
    }

    internal static class StyleClassificationTypes
    {
        [Export]
        [Name(StyleClassificationNames.SystemSymbol)]
        internal static ClassificationTypeDefinition SystemSymbol = null;

        [Export]
        [Name(StyleClassificationNames.LocalSymbol)]
        internal static ClassificationTypeDefinition LocalSymbol = null;

        [Export]
        [Name(StyleClassificationNames.CommentTag)]
        internal static ClassificationTypeDefinition CommentTag = null;
    }

    [Export(typeof(EditorFormatDefinition))]
    [ClassificationType(ClassificationTypeNames = StyleClassificationNames.SystemSymbol)]
    [Name(StyleClassificationNames.SystemSymbol)]
    [UserVisible(true)]
    [Order(After = Priority.High)]
    internal sealed class SystemSymbolFormat : ClassificationFormatDefinition
    {
        public SystemSymbolFormat()
        {
            DisplayName = "Editor Font Styles: System Symbol";
            IsItalic = true;
        }
    }

    [Export(typeof(EditorFormatDefinition))]
    [ClassificationType(ClassificationTypeNames = StyleClassificationNames.LocalSymbol)]
    [Name(StyleClassificationNames.LocalSymbol)]
    [UserVisible(true)]
    [Order(After = Priority.High)]
    internal sealed class LocalSymbolFormat : ClassificationFormatDefinition
    {
        public LocalSymbolFormat()
        {
            DisplayName = "Editor Font Styles: Local Symbol";
            IsBold = true;
        }
    }

    [Export(typeof(EditorFormatDefinition))]
    [ClassificationType(ClassificationTypeNames = StyleClassificationNames.CommentTag)]
    [Name(StyleClassificationNames.CommentTag)]
    [UserVisible(true)]
    [Order(After = Priority.High)]
    internal sealed class CommentTagFormat : ClassificationFormatDefinition
    {
        public CommentTagFormat()
        {
            DisplayName = "Editor Font Styles: Comment Tag";
            ForegroundColor = Color.FromRgb(0xFF, 0x8C, 0x00);
            IsBold = true;
        }
    }
}
