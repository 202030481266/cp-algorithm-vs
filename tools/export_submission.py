"""Expand local C++ headers without evaluating conditional compilation."""
from __future__ import annotations

import hashlib
from pathlib import Path
import re

INCLUDE = re.compile(r'^\s*#\s*include\s*([<"])([^>"]+)[>"]')
ONCE = re.compile(r'^\s*#\s*pragma\s+once\b')


def expand_submission(source: Path, output: Path, directories: list[Path],
                      root: Path) -> int:
    source, output, root = source.resolve(), output.resolve(), root.resolve()
    directories = [path.resolve() for path in directories]
    active: set[Path] = set()
    dependencies: set[Path] = set()
    guards: dict[Path, str] = {}

    def label(path: Path) -> str:
        return path.relative_to(root).as_posix() if path.is_relative_to(root) else path.name

    def expand(path: Path, header: bool) -> str:
        path = path.resolve()
        dependencies.add(path)
        if path in active:
            return f"// Recursive include: {label(path)}\n"
        active.add(path)
        try:
            lines = path.read_text(encoding="utf-8-sig").splitlines()
            # A guard at each inclusion site preserves #ifdef behavior. A global
            # 'seen files' set would lose headers first seen in inactive branches.
            once = header and any(ONCE.match(line) for line in lines)
            guard = guards.setdefault(path, "CP_EXPANDED_" + hashlib.sha256(path.as_posix().encode()).hexdigest().upper())
            result = [f"// Begin: {label(path)}"]
            if once:
                result += [f"#ifndef {guard}", f"#define {guard}"]
            for line in lines:
                if ONCE.match(line):
                    continue
                match = INCLUDE.match(line)
                if match:
                    kind, name = match.groups()
                    search = ([path.parent] if kind == '"' else []) + directories
                    found = next((candidate.resolve() for directory in search
                                  if (candidate := directory / name).is_file()), None)
                    if found is not None:
                        result.append(expand(found, True))
                        continue
                    if kind == '"' or name.startswith("atcoder/"):
                        raise ValueError(f"Cannot expand {name!r} included by {label(path)}; add -I DIR.")
                result.append(line)
            if once:
                result.append(f"#endif // {guard}")
            result.append(f"// End: {label(path)}")
            return "\n".join(result) + "\n"
        finally:
            active.remove(path)

    text = expand(source, False)
    if output in dependencies:
        raise ValueError("Export output must not overwrite the source or any included header.")
    output.parent.mkdir(parents=True, exist_ok=True)
    output.write_text(text, encoding="utf-8", newline="\n")
    return len(dependencies) - 1
