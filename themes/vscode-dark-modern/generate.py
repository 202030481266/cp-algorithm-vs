"""Generate the VS Code Dark Modern color settings for Visual Studio.

    python generate.py
        -> rebuilds the files listed in VARIANTS (Vivid and VX2762)

    python generate.py --profile C:\\Windows\\System32\\spool\\drivers\\color\\X.icm -o out.vssettings
        -> colors converted the way VS Code (Chromium) converts them for a display
           with that ICC profile. Visual Studio does not color-manage, so this makes
           both editors look the same on that display.

    python generate.py --vivid 1.4 -o out.vssettings
        -> syntax colors with OKLCH chroma multiplied by 1.4 (lightness and hue kept,
           clipped to the sRGB gamut); grays and editor UI colors unchanged.
"""
import argparse
import struct
from pathlib import Path

BG = "1F1F1F"          # dark_modern.json editor.background
AUTO = None

# Dark+ / Dark Modern token colors (VS Code 1.142 theme-defaults)
FG        = "CCCCCC"   # editor.foreground (Dark Modern)
OPERATOR  = "D4D4D4"   # keyword.operator (dark_vs)
COMMENT   = "6A9955"
KEYWORD   = "569CD6"
CONTROL   = "C586C0"   # keyword.control, newOperator, #include
NUMBER    = "B5CEA8"
STRING    = "CE9178"
ESCAPE    = "D7BA7D"   # constant.character.escape
TYPE      = "4EC9B0"   # entity.name.type / namespace / attribute
FUNCTION  = "DCDCAA"   # entity.name.function, customLiteral
VARIABLE  = "9CDCFE"   # variable / parameter / property
ENUMMEM   = "4FC1FF"   # variable.other.enummember / constant
LABEL     = "C8C8C8"   # entity.name.label


def rgb(h):
    return tuple(int(h[i:i + 2], 16) for i in (0, 2, 4))


def blend(fg, alpha):
    """Composite #RRGGBB at alpha over the editor background (VS has no alpha)."""
    return "".join(f"{round(f * alpha + b * (1 - alpha)):02X}" for f, b in zip(rgb(fg), rgb(BG)))


WORD_HL   = blend("575757", 0xB8 / 255)   # editor.wordHighlightBackground
WORD_HL_W = blend("004972", 0xB8 / 255)   # editor.wordHighlightStrongBackground
FIND_HL   = blend("EA5C00", 0x55 / 255)   # editor.findMatchHighlightBackground
BRACE_BG  = blend("006400", 0x1A / 255)   # editorBracketMatch.background
WS        = blend("E3E4E2", 0x29 / 255)   # editorWhitespace.foreground
INACTIVE  = blend(FG, 0.55)               # C_Cpp.inactiveRegionOpacity
UNMATCHED = blend("FF1212", 0.8)          # editorBracketHighlight.unexpectedBracket.foreground
INLAY_BG  = blend("616161", 0.1)          # editorInlayHint.background = badge.background @ 10%

ITEMS = [
    # Editor surface
    ("Plain Text", FG, BG),
    ("TextView Background", AUTO, BG),
    ("Indicator Margin", AUTO, BG),
    ("Line Number", "6E7681", BG),
    ("Selected Line Number", "CCCCCC", BG),
    ("Selected Text", AUTO, "264F78"),
    ("Inactive Selected Text", AUTO, "3A3D41"),
    ("CurrentLineActiveFormat", "282828", BG),
    ("CurrentLineInactiveFormat", "282828", BG),
    ("Caret (Primary)", "AEAFAD", AUTO),
    ("Caret (Secondary)", "AEAFAD", AUTO),
    ("Visible Whitespace", WS, AUTO),
    ("StickyScroll Background", AUTO, BG),
    # Brackets and highlights (marker items: Foreground = border, Background = fill)
    # VS paints nesting depth 2 with "level three" and depth 3 with "level two" (measured),
    # so the two colors are swapped to get VS Code's gold -> orchid -> blue order.
    ("brace pair level one", "FFD700", AUTO),
    ("brace pair level two", "179FFF", AUTO),
    ("brace pair level three", "DA70D6", AUTO),
    ("mismatched brace", UNMATCHED, AUTO),
    ("Brace Matching (Rectangle)", "888888", BRACE_BG),
    ("brace matching", "888888", BRACE_BG),
    ("MarkerFormatDefinition/HighlightedReference", WORD_HL, WORD_HL),
    ("MarkerFormatDefinition/HighlightedDefinition", WORD_HL, WORD_HL),
    ("MarkerFormatDefinition/CppHighlightedDefinition", WORD_HL, WORD_HL),
    ("MarkerFormatDefinition/HighlightedWrittenReference", WORD_HL_W, WORD_HL_W),
    ("MarkerFormatDefinition/FindHighlight", FIND_HL, FIND_HL),
    # General syntax
    ("Identifier", VARIABLE, AUTO),
    ("Comment", COMMENT, AUTO),
    ("Keyword", KEYWORD, AUTO),
    ("Number", NUMBER, AUTO),
    ("String", STRING, AUTO),
    ("Operator", OPERATOR, AUTO),
    ("Preprocessor Keyword", CONTROL, AUTO),
    ("Excluded Code", INACTIVE, AUTO),
    ("Type", TYPE, AUTO),
    ("Method", FUNCTION, AUTO),
    ("XML Doc Comment", COMMENT, AUTO),
    ("XML Doc Tag", COMMENT, AUTO),
    # C++
    ("C/C++ User Keywords", KEYWORD, AUTO),
    ("CppControlKeywordSyntacticTokenFormat", CONTROL, AUTO),
    ("CppNewDeleteSemanticTokenFormat", CONTROL, AUTO),
    ("CppMacroSemanticTokenFormat", KEYWORD, AUTO),
    ("CppNamespaceSemanticTokenFormat", TYPE, AUTO),
    ("CppTypeSemanticTokenFormat", TYPE, AUTO),
    ("CppRefTypeSemanticTokenFormat", TYPE, AUTO),
    ("CppValueTypeSemanticTokenFormat", TYPE, AUTO),
    ("CppClassTemplateSemanticTokenFormat", TYPE, AUTO),
    ("CppGenericTypeSemanticTokenFormat", TYPE, AUTO),
    ("CppAttributesSyntacticTokenFormat", TYPE, AUTO),
    ("CppFunctionSemanticTokenFormat", FUNCTION, AUTO),
    ("CppMemberFunctionSemanticTokenFormat", FUNCTION, AUTO),
    ("CppStaticMemberFunctionSemanticTokenFormat", FUNCTION, AUTO),
    ("CppFunctionTemplateSemanticTokenFormat", FUNCTION, AUTO),
    # VS Code colors overloaded operators like functions (measured: cin >>, cout <<, string +, iterator ==)
    ("CppOperatorSemanticTokenFormat", FUNCTION, AUTO),
    ("CppMemberOperatorSemanticTokenFormat", FUNCTION, AUTO),
    ("CppGlobalVariableSemanticTokenFormat", VARIABLE, AUTO),
    ("CppLocalVariableSemanticTokenFormat", VARIABLE, AUTO),
    ("CppParameterSemanticTokenFormat", VARIABLE, AUTO),
    ("CppMemberFieldSemanticTokenFormat", VARIABLE, AUTO),
    ("CppStaticMemberFieldSemanticTokenFormat", VARIABLE, AUTO),
    ("CppPropertySemanticTokenFormat", VARIABLE, AUTO),
    ("CppEventSemanticTokenFormat", VARIABLE, AUTO),
    ("CppEnumSemanticTokenFormat", ENUMMEM, AUTO),
    ("CppLabelSemanticTokenFormat", LABEL, AUTO),
    ("CppUDLRawSemanticTokenFormat", FUNCTION, AUTO),
    ("CppUDLNumberSemanticTokenFormat", NUMBER, AUTO),
    ("CppUDLStringSemanticTokenFormat", STRING, AUTO),
    ("CppStringEscapeCharacterSyntacticTokenFormat", ESCAPE, AUTO),
    ("CppStringDelimiterCharacterSyntacticTokenFormat", STRING, AUTO),
    ("CppInlineHintsFormat", "969696", INLAY_BG),
    # Roslyn / shared classifications
    ("keyword - control", CONTROL, AUTO),
    ("punctuation", FG, AUTO),
    ("operator - overloaded", FUNCTION, AUTO),
    ("string - escape character", ESCAPE, AUTO),
    ("string - verbatim", STRING, AUTO),
    ("namespace name", TYPE, AUTO),
    ("class name", TYPE, AUTO),
    ("record class name", TYPE, AUTO),
    ("struct name", TYPE, AUTO),
    ("record struct name", TYPE, AUTO),
    ("interface name", TYPE, AUTO),
    ("enum name", TYPE, AUTO),
    ("delegate name", TYPE, AUTO),
    ("module name", TYPE, AUTO),
    ("type parameter name", TYPE, AUTO),
    ("method name", FUNCTION, AUTO),
    ("extension method name", FUNCTION, AUTO),
    ("property name", VARIABLE, AUTO),
    ("field name", VARIABLE, AUTO),
    ("event name", VARIABLE, AUTO),
    ("local name", VARIABLE, AUTO),
    ("parameter name", VARIABLE, AUTO),
    ("enum member name", ENUMMEM, AUTO),
    ("constant name", ENUMMEM, AUTO),
    ("label name", LABEL, AUTO),
    ("inline hints", "969696", INLAY_BG),
    ("xml doc comment - text", COMMENT, AUTO),
    ("xml doc comment - delimiter", COMMENT, AUTO),
    ("xml doc comment - name", COMMENT, AUTO),
]

# sRGB colorants adapted to D50, the ICC profile connection space
SRGB_D50 = [
    [0.4360747, 0.3850649, 0.1430804],
    [0.2225045, 0.7168786, 0.0606169],
    [0.0139322, 0.0971045, 0.7141733],
]


def srgb_decode(v):
    return v / 12.92 if v <= 0.04045 else ((v + 0.055) / 1.055) ** 2.4


def srgb_encode(v):
    return 12.92 * v if v <= 0.0031308 else 1.055 * v ** (1 / 2.4) - 0.055


def read_colorants(path):
    """Return the rXYZ/gXYZ/bXYZ matrix of a matrix-based display profile."""
    data = Path(path).read_bytes()
    count = struct.unpack_from(">I", data, 128)[0]
    tags = {}
    for i in range(count):
        sig, off, _size = struct.unpack_from(">4sII", data, 132 + 12 * i)
        tags[sig] = off
    cols = []
    for sig in (b"rXYZ", b"gXYZ", b"bXYZ"):
        off = tags[sig]
        cols.append([struct.unpack_from(">i", data, off + 8 + 4 * j)[0] / 65536 for j in range(3)])
    return [[cols[c][r] for c in range(3)] for r in range(3)]


def inverse(m):
    a, b, c = m
    det = (a[0] * (b[1] * c[2] - b[2] * c[1]) - a[1] * (b[0] * c[2] - b[2] * c[0])
           + a[2] * (b[0] * c[1] - b[1] * c[0]))
    return [
        [(b[1] * c[2] - b[2] * c[1]) / det, (a[2] * c[1] - a[1] * c[2]) / det, (a[1] * b[2] - a[2] * b[1]) / det],
        [(b[2] * c[0] - b[0] * c[2]) / det, (a[0] * c[2] - a[2] * c[0]) / det, (a[2] * b[0] - a[0] * b[2]) / det],
        [(b[0] * c[1] - b[1] * c[0]) / det, (a[1] * c[0] - a[0] * c[1]) / det, (a[0] * b[1] - a[1] * b[0]) / det],
    ]


def display_converter(profile):
    """Map sRGB colors to display RGB like Chromium: gamut by the profile, transfer curve kept as sRGB.

    Checked against a VS Code screenshot on a VX2762-2K-HD profile: every sampled color within +-1.
    """
    to_display = inverse(read_colorants(profile))

    def convert(h):
        lin = [srgb_decode(v / 255) for v in rgb(h)]
        pcs = [sum(SRGB_D50[r][c] * lin[c] for c in range(3)) for r in range(3)]
        out = [sum(to_display[r][c] * pcs[c] for c in range(3)) for r in range(3)]
        return "".join(f"{round(255 * srgb_encode(min(max(v, 0.0), 1.0))):02X}" for v in out)

    return convert


SYNTAX = {COMMENT, KEYWORD, CONTROL, NUMBER, STRING, ESCAPE, TYPE, FUNCTION, VARIABLE, ENUMMEM}


def to_oklab(h):
    r, g, b = (srgb_decode(v / 255) for v in rgb(h))
    l = (0.4122214708 * r + 0.5363325363 * g + 0.0514459929 * b) ** (1 / 3)
    m = (0.2119034982 * r + 0.6806995451 * g + 0.1073969566 * b) ** (1 / 3)
    s = (0.0883024619 * r + 0.2817188376 * g + 0.6299787005 * b) ** (1 / 3)
    return (0.2104542553 * l + 0.7936177850 * m - 0.0040720468 * s,
            1.9779984951 * l - 2.4285922050 * m + 0.4505937099 * s,
            0.0259040371 * l + 0.7827717662 * m - 0.8086757660 * s)


def from_oklab(lab):
    """OKLab -> linear sRGB, not clipped."""
    L, a, b = lab
    l = (L + 0.3963377774 * a + 0.2158037573 * b) ** 3
    m = (L - 0.1055613458 * a - 0.0638541728 * b) ** 3
    s = (L - 0.0894841775 * a - 1.2914855480 * b) ** 3
    return (4.0767416621 * l - 3.3077115913 * m + 0.2309699292 * s,
            -1.2684380046 * l + 2.6097574011 * m - 0.3413193965 * s,
            -0.0041960863 * l - 0.7034186147 * m + 1.7076147010 * s)


MAX_DARKEN = 0.06  # OKLab lightness the boost may give up when the sRGB gamut is the limit


def vivid_converter(factor):
    """Multiply OKLCH chroma of syntax colors, keeping hue.

    Lightness is kept unless the boosted chroma does not fit in sRGB (light blues such as
    #9CDCFE already sit on the gamut edge); then lightness is lowered by at most MAX_DARKEN.
    """
    def in_gamut(L, a, b):
        return all(-1e-7 <= v <= 1 + 1e-7 for v in from_oklab((L, a, b)))

    def max_scale(L, a, b, limit):
        lo, hi = 0.0, limit
        if in_gamut(L, a * hi, b * hi):
            return hi
        for _ in range(40):
            mid = (lo + hi) / 2
            lo, hi = (mid, hi) if in_gamut(L, a * mid, b * mid) else (lo, mid)
        return lo

    def convert(h):
        if h not in SYNTAX:
            return h
        L, a, b = to_oklab(h)
        best = None
        for i in range(int(MAX_DARKEN * 1000) + 1):
            Lt = L - i / 1000
            f = max_scale(Lt, a, b, factor)
            if best is None or f > best[1] + 1e-9:
                best = (Lt, f)
            if f >= factor - 1e-9:
                break
        Lt, f = best
        lin = from_oklab((Lt, a * f, b * f))
        return "".join(f"{round(255 * srgb_encode(min(max(v, 0.0), 1.0))):02X}" for v in lin)

    return convert


def colorref(h):
    if h is None:
        return "0x02000000"
    r, g, b = rgb(h)
    return f"0x00{b:02X}{g:02X}{r:02X}"


def render(convert=lambda h: h):
    names = [n for n, _, _ in ITEMS]
    assert len(names) == len(set(names)), "duplicate item"
    lines = [
        "<?xml version='1.0' encoding='utf-8'?>",
        "<UserSettings>",
        '  <ApplicationIdentity version="17.0" />',
        "  <ToolsOptions />",
        '  <Category name="Environment_Group" RegisteredName="Environment_Group">',
        '    <Category name="Environment_FontsAndColors" Category="{1EDA5DD4-927A-43A7-810E-7FD247D0DA1D}" Package="{DA9FB551-C724-11D0-AE1F-00A0C90FFFC3}" RegisteredName="Environment_FontsAndColors" PackageName="CVsShellPackage">',
        '      <PropertyValue name="Version">2</PropertyValue>',
        '      <FontsAndColors Version="2.0">',
        "        <Categories>",
        '          <Category GUID="{A27B4E24-A735-4D1D-B8E7-9716E1E3D8E0}">',
        "            <Items>",
    ]
    for name, fg, bg in ITEMS:
        fg = None if fg is None else convert(fg)
        bg = None if bg is None else convert(bg)
        lines.append(f'              <Item Name="{name}" Foreground="{colorref(fg)}" Background="{colorref(bg)}" BoldFont="No" />')
    lines += [
        "            </Items>",
        "          </Category>",
        "        </Categories>",
        "      </FontsAndColors>",
        "    </Category>",
        "  </Category>",
        "</UserSettings>",
        "",
    ]
    return "\n".join(lines)


VARIANTS = {
    "VSCode-Dark-Modern-Vivid.vssettings": {"vivid": 1.4},
    "VSCode-Dark-Modern-VX2762.vssettings": {"profile": r"C:\Windows\System32\spool\drivers\color\VX2762-2K-HD.ICM"},
}


def write(out, vivid=1.0, profile=None):
    steps = [vivid_converter(vivid)] if vivid != 1.0 else []
    if profile:
        steps.append(display_converter(profile))

    def convert(h):
        for step in steps:
            h = step(h)
        return h

    out.write_text(render(convert), encoding="utf-8", newline="\n")
    print(f"{out.name}: {len(ITEMS)} items")


def main():
    parser = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    parser.add_argument("--profile", help="display ICC profile used by VS Code on the target monitor")
    parser.add_argument("--vivid", type=float, default=1.0, help="chroma multiplier for syntax colors")
    parser.add_argument("-o", "--output", help="output .vssettings path")
    args = parser.parse_args()
    here = Path(__file__).resolve().parent
    if args.output is None:
        if args.profile or args.vivid != 1.0:
            parser.error("--profile/--vivid need -o; without options all VARIANTS are rebuilt")
        for name, options in VARIANTS.items():
            write(here / name, **options)
        return
    write(Path(args.output), args.vivid, args.profile)


if __name__ == "__main__":
    main()
