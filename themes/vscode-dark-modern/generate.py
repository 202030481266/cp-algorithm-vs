"""Generate the VS Code Dark Modern color settings for Visual Studio.

    python generate.py
        -> rebuilds the files listed in VARIANTS (Vivid and VX2762)

    python generate.py --profile C:\\Windows\\System32\\spool\\drivers\\color\\X.icm -o out.vssettings
        -> Dark Modern colors converted the way VS Code (Chromium) converts them for a display
           with that ICC profile. Visual Studio does not color-manage, so this makes
           both editors look the same on that display.

    python generate.py --palette vivid --chroma 1.15 -o out.vssettings
        -> the Vivid palette with the OKLCH chroma of its syntax colors multiplied by 1.15
           (lightness and hue kept, clipped to the sRGB gamut); editor UI colors unchanged.
"""
import argparse
import struct
from dataclasses import dataclass
from pathlib import Path

BG = "1F1F1F"          # dark_modern.json editor.background
AUTO = None


@dataclass(frozen=True)
class Palette:
    text: str          # plain text, punctuation, current line number
    operator: str      # VS also classifies ; , . and template <> as operators
    overload: str      # overloaded operators: cin >>, cout <<, vector []
    comment: str
    keyword: str
    control: str       # if / for / return, preprocessor directives, new / delete
    macro: str
    type: str          # types, namespaces, attributes
    function: str
    variable: str      # locals, globals, parameters
    member: str        # member fields, properties
    enum: str          # enum members, constants
    number: str
    string: str
    escape: str
    label: str
    braces: tuple      # brace pair colors at nesting depth 1, 2, 3


# Dark+ / Dark Modern token colors (VS Code 1.142 theme-defaults)
DARK_MODERN = Palette(
    text="CCCCCC",      # editor.foreground (Dark Modern)
    operator="D4D4D4",  # keyword.operator (dark_vs)
    overload="DCDCAA",  # like functions (measured: cin >>, cout <<, string +, iterator ==)
    comment="6A9955",
    keyword="569CD6",
    control="C586C0",   # keyword.control, newOperator, #include
    macro="569CD6",     # entity.name.function.preprocessor
    type="4EC9B0",      # entity.name.type / namespace / attribute
    function="DCDCAA",  # entity.name.function, customLiteral
    variable="9CDCFE",  # variable / parameter
    member="9CDCFE",    # variable.other.property
    enum="4FC1FF",      # variable.other.enummember / constant
    number="B5CEA8",
    string="CE9178",
    escape="D7BA7D",    # constant.character.escape
    label="C8C8C8",     # entity.name.label
    braces=("FFD700", "DA70D6", "179FFF"),  # editorBracketHighlight.foreground1-3
)

# Hand-picked: One Dark Pro roles, Gruvbox's warm off-white text, Monokai's pink control flow.
# OKLCH lightness / chroma / hue in the comments; every color has at least 4.5:1 contrast on BG.
VIVID = Palette(
    text="D8D0BF",      # 0.86 0.025  85  also variables and parameters, so plain code stays calm
    operator="A29E96",  # 0.70 0.012  80  dimmer than variables
    overload="A29E96",  # same as built-in operators
    comment="8C8579",   # 0.62 0.020  85
    keyword="B484E8",   # 0.70 0.150 305
    control="F26A90",   # 0.70 0.170   5
    macro="56C4CA",     # 0.76 0.100 200
    type="E6BE68",      # 0.82 0.115  85
    function="61B2F4",  # 0.74 0.125 245
    variable="D8D0BF",
    member="EAACA1",    # 0.80 0.075  30
    enum="EE9B5F",      # same as numbers
    number="EE9B5F",    # 0.76 0.125  55
    string="93CA73",    # 0.78 0.130 135
    escape="56C4CA",    # same as macros
    label="D8D0BF",
    braces=("F2CE59", "E176C1", "6CCDEA"),  # 0.86 0.14 92 / 0.71 0.16 340 / 0.80 0.10 220
)

PALETTES = {"dark-modern": DARK_MODERN, "vivid": VIVID}


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
UNMATCHED = blend("FF1212", 0.8)          # editorBracketHighlight.unexpectedBracket.foreground
INLAY_BG  = blend("616161", 0.1)          # editorInlayHint.background = badge.background @ 10%

# VS paints the selection at 40% opacity (measured: #264F78 over #1E1E1E shows as #213242), so these
# items get the color that shows as the VS Code one at that opacity.
SELECTION_OPACITY = 0.4
SELECTIONS = ("Selected Text", "Inactive Selected Text")


def see_through(target, under):
    """The color that VS, painting it at SELECTION_OPACITY over `under`, shows as `target`."""
    return "".join(f"{min(255, max(0, round((t - u * (1 - SELECTION_OPACITY)) / SELECTION_OPACITY))):02X}"
                   for t, u in zip(rgb(target), rgb(under)))


def items(p):
    return [
        # Editor surface. A Plain Text background would be painted under every piece of text, on top
        # of the selection, so that only line ends showed it; the background is the TextView's alone.
        ("Plain Text", p.text, AUTO),
        ("TextView Background", AUTO, BG),
        ("Indicator Margin", AUTO, BG),
        ("Line Number", "6E7681", BG),
        ("Selected Line Number", p.text, BG),
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
        # so the two colors are swapped to keep the palette's depth order.
        ("brace pair level one", p.braces[0], AUTO),
        ("brace pair level two", p.braces[2], AUTO),
        ("brace pair level three", p.braces[1], AUTO),
        ("mismatched brace", UNMATCHED, AUTO),
        ("Brace Matching (Rectangle)", "888888", BRACE_BG),
        ("brace matching", "888888", BRACE_BG),
        ("MarkerFormatDefinition/HighlightedReference", WORD_HL, WORD_HL),
        ("MarkerFormatDefinition/HighlightedDefinition", WORD_HL, WORD_HL),
        ("MarkerFormatDefinition/CppHighlightedDefinition", WORD_HL, WORD_HL),
        ("MarkerFormatDefinition/HighlightedWrittenReference", WORD_HL_W, WORD_HL_W),
        ("MarkerFormatDefinition/FindHighlight", FIND_HL, FIND_HL),
        # General syntax
        ("Identifier", p.variable, AUTO),
        ("Comment", p.comment, AUTO),
        ("Keyword", p.keyword, AUTO),
        ("Number", p.number, AUTO),
        ("String", p.string, AUTO),
        ("Operator", p.operator, AUTO),
        ("Preprocessor Keyword", p.control, AUTO),
        ("Excluded Code", blend(p.text, 0.55), AUTO),  # C_Cpp.inactiveRegionOpacity
        ("Type", p.type, AUTO),
        ("Method", p.function, AUTO),
        ("XML Doc Comment", p.comment, AUTO),
        ("XML Doc Tag", p.comment, AUTO),
        # C++
        ("C/C++ User Keywords", p.keyword, AUTO),
        ("CppControlKeywordSyntacticTokenFormat", p.control, AUTO),
        ("CppNewDeleteSemanticTokenFormat", p.control, AUTO),
        ("CppMacroSemanticTokenFormat", p.macro, AUTO),
        ("CppNamespaceSemanticTokenFormat", p.type, AUTO),
        ("CppTypeSemanticTokenFormat", p.type, AUTO),
        ("CppRefTypeSemanticTokenFormat", p.type, AUTO),
        ("CppValueTypeSemanticTokenFormat", p.type, AUTO),
        ("CppClassTemplateSemanticTokenFormat", p.type, AUTO),
        ("CppGenericTypeSemanticTokenFormat", p.type, AUTO),
        ("CppAttributesSyntacticTokenFormat", p.type, AUTO),
        ("CppFunctionSemanticTokenFormat", p.function, AUTO),
        ("CppMemberFunctionSemanticTokenFormat", p.function, AUTO),
        ("CppStaticMemberFunctionSemanticTokenFormat", p.function, AUTO),
        ("CppFunctionTemplateSemanticTokenFormat", p.function, AUTO),
        ("CppOperatorSemanticTokenFormat", p.overload, AUTO),
        ("CppMemberOperatorSemanticTokenFormat", p.overload, AUTO),
        ("CppGlobalVariableSemanticTokenFormat", p.variable, AUTO),
        ("CppLocalVariableSemanticTokenFormat", p.variable, AUTO),
        ("CppParameterSemanticTokenFormat", p.variable, AUTO),
        ("CppMemberFieldSemanticTokenFormat", p.member, AUTO),
        ("CppStaticMemberFieldSemanticTokenFormat", p.member, AUTO),
        ("CppPropertySemanticTokenFormat", p.member, AUTO),
        ("CppEventSemanticTokenFormat", p.member, AUTO),
        ("CppEnumSemanticTokenFormat", p.enum, AUTO),
        ("CppLabelSemanticTokenFormat", p.label, AUTO),
        ("CppUDLRawSemanticTokenFormat", p.function, AUTO),
        ("CppUDLNumberSemanticTokenFormat", p.number, AUTO),
        ("CppUDLStringSemanticTokenFormat", p.string, AUTO),
        ("CppStringEscapeCharacterSyntacticTokenFormat", p.escape, AUTO),
        ("CppStringDelimiterCharacterSyntacticTokenFormat", p.string, AUTO),
        ("CppInlineHintsFormat", "969696", INLAY_BG),
        # Roslyn / shared classifications
        ("keyword - control", p.control, AUTO),
        ("punctuation", p.text, AUTO),
        ("operator - overloaded", p.overload, AUTO),
        ("string - escape character", p.escape, AUTO),
        ("string - verbatim", p.string, AUTO),
        ("namespace name", p.type, AUTO),
        ("class name", p.type, AUTO),
        ("record class name", p.type, AUTO),
        ("struct name", p.type, AUTO),
        ("record struct name", p.type, AUTO),
        ("interface name", p.type, AUTO),
        ("enum name", p.type, AUTO),
        ("delegate name", p.type, AUTO),
        ("module name", p.type, AUTO),
        ("type parameter name", p.type, AUTO),
        ("method name", p.function, AUTO),
        ("extension method name", p.function, AUTO),
        ("property name", p.member, AUTO),
        ("field name", p.member, AUTO),
        ("event name", p.member, AUTO),
        ("local name", p.variable, AUTO),
        ("parameter name", p.variable, AUTO),
        ("enum member name", p.enum, AUTO),
        ("constant name", p.enum, AUTO),
        ("label name", p.label, AUTO),
        ("inline hints", "969696", INLAY_BG),
        ("xml doc comment - text", p.comment, AUTO),
        ("xml doc comment - delimiter", p.comment, AUTO),
        ("xml doc comment - name", p.comment, AUTO),
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


SYNTAX_ROLES = ("comment", "keyword", "control", "macro", "type", "function", "variable", "member",
                "enum", "number", "string", "escape", "overload")


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


def chroma_converter(factor, syntax):
    """Multiply OKLCH chroma of the colors in syntax, keeping hue.

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
        if h not in syntax:
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


def render(palette, convert=lambda h: h):
    rows = items(palette)
    names = [n for n, _, _ in rows]
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
    for name, fg, bg in rows:
        fg = None if fg is None else convert(fg)
        bg = None if bg is None else convert(bg)
        if name in SELECTIONS:
            bg = see_through(bg, convert(BG))
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
    "VSCode-Dark-Modern-Vivid.vssettings": {"palette": VIVID},
    "VSCode-Dark-Modern-VX2762.vssettings": {"profile": r"C:\Windows\System32\spool\drivers\color\VX2762-2K-HD.ICM"},
}


def write(out, palette=DARK_MODERN, chroma=1.0, profile=None):
    steps = [chroma_converter(chroma, {getattr(palette, r) for r in SYNTAX_ROLES})] if chroma != 1.0 else []
    if profile:
        steps.append(display_converter(profile))

    def convert(h):
        for step in steps:
            h = step(h)
        return h

    out.write_text(render(palette, convert), encoding="utf-8", newline="\n")
    print(f"{out.name}: {len(items(palette))} items")


def main():
    parser = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    parser.add_argument("--palette", choices=PALETTES, help="syntax colors (default: dark-modern)")
    parser.add_argument("--profile", help="display ICC profile used by VS Code on the target monitor")
    parser.add_argument("--chroma", type=float, default=1.0, help="chroma multiplier for syntax colors")
    parser.add_argument("-o", "--output", help="output .vssettings path")
    args = parser.parse_args()
    here = Path(__file__).resolve().parent
    if args.output is None:
        if args.palette or args.profile or args.chroma != 1.0:
            parser.error("--palette/--profile/--chroma need -o; without options all VARIANTS are rebuilt")
        for name, options in VARIANTS.items():
            write(here / name, **options)
        return
    write(Path(args.output), PALETTES[args.palette or "dark-modern"], args.chroma, args.profile)


if __name__ == "__main__":
    main()
