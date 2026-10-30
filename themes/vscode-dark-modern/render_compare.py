"""Render vivid-compare.png: the same C++ code in the Vivid and the Dark Modern palette.

    python render_compare.py        (needs Pillow: pip install pillow)

The colors come from generate.py, so the picture follows the palettes. Tokens get the colors Visual Studio
gives them with semantic colorization and brace pair colorization on; the classification below is written
by hand for this sample, not computed by a C++ parser.
"""
import re
from pathlib import Path

from PIL import Image, ImageDraw, ImageFont

from generate import BG, DARK_MODERN, VIVID

PANELS = [("鲜艳版", VIVID), ("Dark Modern 原色", DARK_MODERN)]

CODE = r"""#include <iostream>
#include <vector>
#define rep(i, l, r) for (int i = l; i < r; ++i)
using namespace std;
using ll = long long;

constexpr int INF = 0x3f3f3f3f;
enum class Dir { up, down };

// point update on a bottom-up segment tree
template <typename T>
struct SegTree {
    int n;
    vector<T> t;
    explicit SegTree(int sz) : n(sz), t(2 * sz) {}
    void update(int p, T value) {
        for (t[p += n] = value; p > 1; p >>= 1)
            t[p >> 1] = t[p] + t[p ^ 1];
    }
};

void solve() {
    ll n, m;
    cin >> n >> m;
    string s;
    cin >> s;
    ll one = 0;
    rep(i, 0, n) {
        if (s[i] == '1') ++one;
    }
    if (m * one % n) {
        cout << -1 << '\n';
        return;
    }
    vector<ll> cnt(n + 1, INF);
    rep(i, 0, n) cnt[i + 1] = cnt[i] + (s[i] - '0');
    auto dir = cnt[n] > 0 ? Dir::up : Dir::down;
    sort(cnt.begin(), cnt.end(), greater<ll>());
    printf("%lld\t%lld\n", n, m);
    if (dir == Dir::up) cout << "Yes\n";
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);
    int tc = 1;
    cin >> tc;
    while (tc--) solve();
}
"""

KEYWORDS = {"auto", "class", "const", "constexpr", "enum", "explicit", "false", "int", "long", "namespace",
            "nullptr", "struct", "template", "typename", "using", "void"}
CONTROL = {"for", "if", "return", "while"}
TYPES = {"Dir", "SegTree", "T", "greater", "ios", "ll", "std", "string", "vector"}
FUNCTIONS = {"begin", "end", "main", "printf", "solve", "sort", "sync_with_stdio", "tie", "update"}
MACROS = {"rep"}
ENUMS = {"up", "down"}
MEMBERS = {"n", "t"}                # only inside struct SegTree; elsewhere n is a local variable
CONTAINERS = {"cnt", "s", "t"}      # their [] is the overloaded operator[]
STREAMS = {"cin", "cout"}           # << and >> on a line with them are overloaded

TOKEN = re.compile(r"""
    (?P<comment>//.*)
  | (?P<string>"(?:\\.|[^"\\])*"|'(?:\\.|[^'\\])*')
  | (?P<number>0[xX][0-9a-fA-F]+|\d+)
  | (?P<directive>\#\w+)
  | (?P<ident>[A-Za-z_]\w*)
  | (?P<bracket>[()\[\]{}])
  | (?P<op>>>=|<<=|<<|>>|::|\+\+|--|\+=|-=|==|!=|<=|>=|&&|\|\||[-+*/%^&|!~=<>?:;,.])
  | (?P<space>\s+)
""", re.X)
ESCAPE = re.compile(r"\\.")


def classify(code, p):
    """Lines of (column, text, color) for palette p."""
    lines, depth, overloads, in_struct = [], 0, [], False
    for line in code.splitlines():
        spans, prev = [], None
        streams = any(s in re.findall(r"\w+", line) for s in STREAMS)
        in_struct = in_struct or line.startswith("struct ")
        if line.startswith("#include"):
            spans += [(0, "#include", p.control), (9, line[9:], p.string)]
            lines.append(spans)
            continue
        for m in TOKEN.finditer(line):
            kind, text, col = m.lastgroup, m.group(), m.start()
            if kind == "space":
                continue
            if kind == "comment":
                color = p.comment
            elif kind == "string":
                last = 0
                for e in ESCAPE.finditer(text):
                    spans += [(col + last, text[last:e.start()], p.string), (col + e.start(), e.group(), p.escape)]
                    last = e.end()
                text, col, color = text[last:], col + last, p.string
            elif kind == "number":
                color = p.number
            elif kind == "directive":
                color = p.control
            elif kind == "ident":
                color = (p.keyword if text in KEYWORDS else p.control if text in CONTROL
                         else p.type if text in TYPES else p.function if text in FUNCTIONS
                         else p.macro if text in MACROS else p.enum if text in ENUMS
                         else p.member if in_struct and text in MEMBERS else p.variable)
            elif kind == "bracket":
                if text in "([{":
                    overloads.append(text == "[" and prev in CONTAINERS)
                    color = p.overload if overloads[-1] else p.braces[depth % 3]
                    depth += 1
                else:
                    depth -= 1
                    color = p.overload if overloads.pop() else p.braces[depth % 3]
            else:
                color = p.overload if text in ("<<", ">>") and streams else p.operator
            spans.append((col, text, color))
            prev = text
        in_struct = in_struct and line != "};"
        lines.append(spans)
    return lines


def render(out, scale=2):
    fonts = Path("C:/Windows/Fonts")
    code_font = ImageFont.truetype(str(fonts / "consola.ttf"), 16 * scale)
    title_font = ImageFont.truetype(str(fonts / "msyh.ttc"), 15 * scale)
    advance = code_font.getlength("M")
    line_h, pad_x, pad_y, header, margin, gap = 23 * scale, 20 * scale, 14 * scale, 40 * scale, 20 * scale, 20 * scale

    rows = CODE.splitlines()
    panel_w = round(2 * pad_x + max(map(len, rows)) * advance)
    panel_h = header + 2 * pad_y + len(rows) * line_h
    image = Image.new("RGB", (2 * margin + len(PANELS) * panel_w + (len(PANELS) - 1) * gap, 2 * margin + panel_h),
                      "#181818")
    draw = ImageDraw.Draw(image)
    for i, (title, palette) in enumerate(PANELS):
        x0, y0 = margin + i * (panel_w + gap), margin
        draw.rectangle([x0, y0, x0 + panel_w - 1, y0 + panel_h - 1], fill=f"#{BG}", outline="#2B2B2B", width=scale)
        draw.rectangle([x0 + scale, y0 + scale, x0 + panel_w - 1 - scale, y0 + header], fill="#181818")
        draw.line([x0, y0 + header, x0 + panel_w - 1, y0 + header], fill="#2B2B2B", width=scale)
        draw.text((x0 + pad_x, y0 + header // 2), title, font=title_font, fill="#CCCCCC", anchor="lm")
        for row, spans in enumerate(classify(CODE, palette)):
            baseline = y0 + header + pad_y + row * line_h + round(line_h * 0.75)
            for col, text, color in spans:
                draw.text((x0 + pad_x + col * advance, baseline), text, font=code_font, fill=f"#{color}", anchor="ls")
    image.save(out, optimize=True)
    print(f"{out.name}: {image.width}x{image.height}")


if __name__ == "__main__":
    render(Path(__file__).resolve().parent / "vivid-compare.png")
