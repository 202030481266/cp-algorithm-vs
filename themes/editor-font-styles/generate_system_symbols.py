"""Generate resources/system-symbols.txt: the public names declared by the C++ standard library headers.

    python generate_system_symbols.py
        -> finds the newest MSVC toolset (via vswhere) and the newest Windows SDK UCRT headers,
           follows #include from every standard header and writes every non-reserved identifier
           (not starting with "_") that those headers contain outside comments, strings and directives,
           plus the names of the macros they #define.

    python generate_system_symbols.py --msvc-include DIR --ucrt-include DIR
        -> uses the given header directories instead.

The extension treats these names as "system symbols" (Visual Assist: System symbols in italics)
unless the current file or a header it includes with quotes declares the same name.
"""
import argparse
import os
import re
import subprocess
import winreg
from pathlib import Path

HERE = Path(__file__).resolve().parent
OUTPUT = HERE / "resources" / "system-symbols.txt"

KEYWORDS = set("""
    alignas alignof and and_eq asm auto bitand bitor bool break case catch char char8_t char16_t char32_t
    class compl concept const consteval constexpr constinit const_cast continue co_await co_return co_yield
    decltype default delete do double dynamic_cast else enum explicit export extern false float for friend
    goto if import inline int long module mutable namespace new noexcept not not_eq nullptr operator or
    or_eq override final private protected public register reinterpret_cast requires return short signed
    sizeof static static_assert static_cast struct switch template this thread_local throw true try typedef
    typeid typename union unsigned using virtual void volatile wchar_t while xor xor_eq
""".split())

# Namespaces that MSVC opens through macros such as _STD_BEGIN, so they may never appear as plain tokens.
EXTRA_NAMES = """
    std chrono execution filesystem literals numbers placeholders pmr ranges regex_constants rel_ops
    this_thread views chrono_literals complex_literals string_literals string_view_literals
""".split()

TOKEN = re.compile(r"""
      (?P<comment>//[^\n]*|/\*.*?\*/)
    | (?P<string>(?:u8|[uUL])?R"(?P<delim>[^()\\\s"]{0,16})\(.*?\)(?P=delim)"
               |(?:u8|[uUL])?"(?:\\.|[^"\\\n])*"
               |(?:u8|[uUL])?'(?:\\.|[^'\\\n])*')
    | (?P<number>\.?\d(?:[\w.]|'\w|[eEpP][+-])*)
    | (?P<ident>[A-Za-z_][A-Za-z0-9_]*)
""", re.S | re.X)
IDENT = re.compile(r"[A-Za-z_][A-Za-z0-9_]*")
DIRECTIVE = re.compile(r"\s*#\s*(\w+)\s*(.*)", re.S)
INCLUDE = re.compile(r"""[<"]([^>"]+)[>"]""")
# Arguments of __declspec / __pragma / _Pragma are compiler syntax, not library names.
COMPILER_SYNTAX = re.compile(r"\b(?:__declspec|__pragma|_Pragma)\s*\((?:[^()]|\([^()]*\))*\)")
# Compiler intrinsics and C++/CX headers reached from <atomic>, <bit> etc.; their parameter names
# (a, b, k, x, ...) are not library API and would collide with ordinary variable names.
SKIPPED_HEADERS = re.compile(r"(?i).*intrin.*\.h|.*neon.*\.h|agile\.h|vccorlib\.h")


def vswhere():
    root = os.environ.get("ProgramFiles(x86)", r"C:\Program Files (x86)")
    return Path(root) / "Microsoft Visual Studio" / "Installer" / "vswhere.exe"


def find_msvc_include():
    out = subprocess.run([str(vswhere()), "-all", "-prerelease", "-products", "*", "-property", "installationPath"],
                         capture_output=True, text=True, check=True).stdout.splitlines()
    candidates = []
    for install in filter(None, (line.strip() for line in out)):
        tools = Path(install) / "VC" / "Tools" / "MSVC"
        if tools.is_dir():
            candidates += [d / "include" for d in tools.iterdir() if (d / "include" / "vector").is_file()]
    if not candidates:
        raise SystemExit("no MSVC toolset with C++ headers found; pass --msvc-include")
    return max(candidates, key=lambda p: version_key(p.parent.name))


def find_ucrt_include():
    try:
        with winreg.OpenKey(winreg.HKEY_LOCAL_MACHINE, r"SOFTWARE\Microsoft\Windows Kits\Installed Roots") as key:
            root = Path(winreg.QueryValueEx(key, "KitsRoot10")[0])
    except OSError:
        root = Path(os.environ.get("ProgramFiles(x86)", r"C:\Program Files (x86)")) / "Windows Kits" / "10"
    versions = [d for d in (root / "Include").iterdir() if (d / "ucrt" / "stdio.h").is_file()]
    if not versions:
        raise SystemExit("no Windows SDK UCRT headers found; pass --ucrt-include")
    return max(versions, key=lambda p: version_key(p.name)) / "ucrt"


def version_key(name):
    return [int(part) for part in re.findall(r"\d+", name)]


def logical_lines(text):
    """Comments become a space and strings an empty literal, then backslash-newlines are joined."""
    def replace(m):
        if m.group("comment"):
            return " "
        if m.group("string"):
            return '""'
        return m.group(0)

    cleaned = TOKEN.sub(replace, text)
    return cleaned.replace("\\\r\n", "").replace("\\\n", "").splitlines()


def scan_header(path, names, includes):
    for line in logical_lines(path.read_text(encoding="utf-8", errors="replace")):
        directive = DIRECTIVE.match(line)
        if directive:
            name, rest = directive.groups()
            if name == "define":
                macro = IDENT.match(rest)
                if macro:
                    names.add(macro.group(0))
            elif name in ("include", "include_next"):
                header = INCLUDE.search(rest)
                if header:
                    includes.append(header.group(1))
            continue
        for m in TOKEN.finditer(COMPILER_SYNTAX.sub(" ", line)):
            if m.group("ident"):
                names.add(m.group("ident"))


def collect(msvc_include, ucrt_include):
    search = [msvc_include, ucrt_include]
    pending = [p for p in msvc_include.iterdir() if p.is_file() and not p.suffix]  # the standard headers
    seen, names = set(), set()
    while pending:
        path = pending.pop()
        key = str(path).lower()
        if key in seen:
            continue
        seen.add(key)
        includes = []
        scan_header(path, names, includes)
        for header in includes:
            if SKIPPED_HEADERS.fullmatch(Path(header).name):
                continue
            for base in [path.parent] + search:
                candidate = base / header
                if candidate.is_file():
                    pending.append(candidate.resolve())
                    break
    # Single letters are template/function parameter names in the few headers that do not uglify them
    # (std::uniform_int_distribution::a(), complex literal i): never worth italicizing a loop variable for.
    public = {n for n in names if len(n) > 1 and not n.startswith("_") and n not in KEYWORDS}
    return sorted(public | set(EXTRA_NAMES)), len(seen)


def main():
    parser = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    parser.add_argument("--msvc-include", type=Path, help="MSVC include directory (contains <vector>)")
    parser.add_argument("--ucrt-include", type=Path, help="Windows SDK ucrt include directory (contains stdio.h)")
    parser.add_argument("-o", "--output", type=Path, default=OUTPUT)
    args = parser.parse_args()
    msvc = args.msvc_include or find_msvc_include()
    ucrt = args.ucrt_include or find_ucrt_include()
    names, headers = collect(msvc, ucrt)
    header = [
        "# Public names declared by the C++ standard library and the C runtime headers (system symbols).",
        f"# Generated by generate_system_symbols.py from MSVC {msvc.parent.name} and UCRT {ucrt.parent.name}"
        f" ({headers} headers); do not edit by hand.",
    ]
    args.output.parent.mkdir(parents=True, exist_ok=True)
    args.output.write_text("\n".join(header + names) + "\n", encoding="utf-8", newline="\n")
    print(f"{args.output.name}: {len(names)} names from {headers} headers")


if __name__ == "__main__":
    main()
