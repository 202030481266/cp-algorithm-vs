"""CP-STL integration checks. CP_STL_CXX=cl selects a VS developer terminal."""
from concurrent.futures import ThreadPoolExecutor
from pathlib import Path
import math
import os
import random
import re
import shutil
import subprocess
import tempfile
import unittest

ROOT = Path(__file__).resolve().parents[1]
LIB = ROOT / "cp-stl"


class CPSTLTests(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        cls.compiler = shutil.which(os.environ.get("CP_STL_CXX", "g++"))
        if not cls.compiler:
            raise unittest.SkipTest("Selected C++ compiler is not on PATH")
        cls.msvc = Path(cls.compiler).stem.lower() == "cl"
        checks = ROOT / "build/checks"
        checks.mkdir(parents=True, exist_ok=True)
        cls.temp = tempfile.TemporaryDirectory(prefix="cp-stl 验证-", dir=checks)
        cls.addClassCleanup(cls.temp.cleanup)
        cls.build = Path(cls.temp.name)
        cls.headers = sorted(LIB.rglob("*.hpp"))

    def compile(self, source, name, *, syntax=False, local=True, extra=(), std="c++17", includes=True):
        directory = self.build / name
        directory.mkdir(parents=True, exist_ok=True)
        exe = directory / "test.exe"
        if self.msvc:
            command = [self.compiler, "/nologo", "/EHsc", "/utf-8", "/permissive-",
                       "/std:" + std, "/O2", "/W3"]
            if local:
                command.append("/DLOCAL")
            if includes:
                command += ["/I" + str(LIB), "/I" + str(ROOT / "include"), "/I" + str(ROOT)]
            command += ["/Zs"] if syntax else ["/Fe:" + str(exe), "/Fo:" + str(directory / "test.obj")]
        else:
            command = [self.compiler, "-std=" + std, "-O2", "-Wall", "-Wextra"]
            if local:
                command.append("-DLOCAL")
            if includes:
                command += ["-I", str(LIB), "-I", str(ROOT / "include"), "-I", str(ROOT)]
            command += ["-fsyntax-only"] if syntax else ["-o", exe.name]
        command += list(extra) + [str(source)]
        compiled = subprocess.run(command, cwd=directory, capture_output=True, timeout=120)
        self.assertEqual(compiled.returncode, 0,
                         f"{source}\n" + (compiled.stdout + compiled.stderr).decode("utf-8", "replace"))
        return exe

    def run_program(self, exe, data=b""):
        result = subprocess.run([str(exe)], input=data, capture_output=True, timeout=60, cwd=exe.parent)
        self.assertEqual(result.returncode, 0, result.stderr.decode("utf-8", "replace"))
        return result

    def test_all_headers_are_self_contained(self):
        self.assertEqual(len(self.headers), 36)
        def check(header):
            relative = header.relative_to(LIB)
            source = self.build / ("header_" + "_".join(relative.parts) + ".cpp")
            source.write_text(f'#include "{relative.as_posix()}"\nint main() {{}}\n', encoding="utf-8")
            self.compile(source, "headers/" + relative.as_posix(), syntax=True)
        with ThreadPoolExecutor(max_workers=3) as pool:
            list(pool.map(check, self.headers))

    def test_all_examples_and_debug_switch(self):
        sources = sorted((LIB / "examples").rglob("*.cpp"))
        self.assertEqual(len(sources), 39)
        def check(source):
            relative = source.relative_to(LIB / "examples")
            exe = self.compile(source, "examples/" + relative.as_posix())
            input_file = source.with_suffix(".in")
            result = self.run_program(exe, input_file.read_bytes() if input_file.exists() else b"")
            self.assertEqual(result.stdout.replace(b"\r\n", b"\n"),
                             source.with_suffix(".ans").read_bytes().replace(b"\r\n", b"\n"), str(relative))
            error = source.with_suffix(".err")
            self.assertEqual(result.stderr.replace(b"\r\n", b"\n"),
                             error.read_bytes().replace(b"\r\n", b"\n") if error.exists() else b"", str(relative))
        with ThreadPoolExecutor(max_workers=3) as pool:
            list(pool.map(check, sources))
        source = LIB / "examples/util/debug.cpp"
        for name, local, extra in [
            ("judge", False, []),
            ("online", True, ["/DONLINE_JUDGE"] if self.msvc else ["-DONLINE_JUDGE"]),
        ]:
            exe = self.compile(source, "debug-" + name, local=local, extra=extra)
            self.assertEqual(self.run_program(exe).stderr, b"")

    def test_original_random_algorithm_checks(self):
        exe = self.compile(LIB / "tests/algorithms.cpp", "algorithms")
        self.assertIn(b"All template tests passed", self.run_program(exe).stdout)

    def test_portability_against_python_integers(self):
        source = self.build / "boundaries.cpp"
        source.write_text(r"""
#include <iostream>
#include <algorithm>
#include <cassert>
#include <limits>
#include <optional>
#include <utility>
#include <vector>
// Load MinGW's standard headers before hiding its builtin feature macro.
#ifdef CP_STL_TEST_PORTABLE
#undef __SIZEOF_INT128__
#endif
#include "basic/search.hpp"
#include "math/number_theory.hpp"
#include "geometry/convex_hull.hpp"
int main() {
    char kind;
    while (std::cin >> kind) {
        long long a, b, c;
        if (kind == 'C') {
            cp::Point p, q, r;
            std::cin >> p.x >> p.y >> q.x >> q.y >> r.x >> r.y;
            std::cout << static_cast<long long>(cp::cross(p, q, r)) << '\n';
        } else {
            std::cin >> a >> b >> c;
            if (kind == 'P') std::cout << cp::pow_mod(a, b, c) << '\n';
            if (kind == 'I') {
                auto inverse = cp::inverse_mod(a, c);
                std::cout << (inverse ? *inverse : -1) << '\n';
            }
            if (kind == 'S') std::cout << cp::first_true(a, b, [c](long long x) { return x >= c; }) << '\n';
        }
    }
}
""", encoding="utf-8")
        rng = random.Random(20260922)
        low, high = -(2**63), 2**63 - 1
        cases, expected = [], []
        values = [low, high, -1, 0, 1, 2**32, -(2**32)]
        moduli = [1, 2, 12, 998244353, 2**32 + 15, high - 1, high]
        for a in values + [rng.randint(low, high) for _ in range(150)]:
            for mod in moduli:
                exponent = rng.choice([0, 1, 2, high, rng.randrange(2**63)])
                cases.append(f"P {a} {exponent} {mod}")
                expected.append(pow(a, exponent, mod))
                if mod > 1:
                    cases.append(f"I {a} 0 {mod}")
                    expected.append(pow(a, -1, mod) if math.gcd(a, mod) == 1 else -1)
        intervals = [(low, high), (low, low), (high, high), (low, low+1), (high-1, high), (-3, 4)]
        intervals += [tuple(sorted([rng.randint(low, high), rng.randint(low, high)])) for _ in range(100)]
        for lo, hi in intervals:
            for threshold in [low, high, 0, rng.randint(low, high)]:
                cases.append(f"S {lo} {hi} {threshold}")
                expected.append(max(lo, min(hi, threshold)))
        points = [[-10**9, -10**9, 10**9, -10**9, -10**9, 10**9],
                  [10**9, 10**9, -10**9, -10**9, 10**9, -10**9]]
        points += [[rng.randint(-10**9, 10**9) for _ in range(6)] for _ in range(200)]
        for ax, ay, bx, by, cx, cy in points:
            cases.append(f"C {ax} {ay} {bx} {by} {cx} {cy}")
            expected.append((bx-ax)*(cy-ay) - (by-ay)*(cx-ax))
        data = ("\n".join(cases) + "\n").encode()
        modes = [("native", [])]
        if not self.msvc:
            modes.append(("portable", ["-DCP_STL_TEST_PORTABLE"]))
        for name, extra in modes:
            exe = self.compile(source, "boundaries-" + name, extra=extra)
            actual = [int(value) for value in self.run_program(exe, data).stdout.split()]
            self.assertEqual(actual, expected, name)

    def test_workspace_template_and_export(self):
        from export_submission import expand_submission
        source = self.build / "workspace.cpp"
        source.write_text(
            ''.join(f'#include "{header.relative_to(LIB).as_posix()}"\n'
                    for header in self.headers if header.name != "debug.hpp") +
            '#define main starter_main\n#include "templates/main.cpp"\n#undef main\n'
            '#include "util/debug.hpp"\n'
            'int main() { const std::vector<long long> keys{3, 1, 2}; '
            'print(cp::radix_sort_ids(keys)); print(cp::counting_sort_ids(keys)); '
            'std::vector<long long> a = keys; '
            'cp::radix_sort(a); cp::counting_sort(a); cp::Fenwick<long long> f(a); '
            'f.add(1, 5); print(std::vector<long long>{f.sum(0, 3)}); }\n', encoding="utf-8")
        exe = self.compile(source, "workspace", std="c++20")
        self.assertEqual(self.run_program(exe).stdout.split(), [b"1", b"2", b"0", b"1", b"2", b"0", b"11"])
        output = self.build / "submission.cpp"
        expand_submission(source, output, [LIB, ROOT / "include", ROOT], ROOT)
        exe = self.compile(output, "submission", std="c++20", includes=False)
        self.assertEqual(self.run_program(exe).stdout.split(), [b"1", b"2", b"0", b"1", b"2", b"0", b"11"])

    def test_usage_examples_and_links(self):
        fence = chr(96) * 3
        for header in self.headers:
            relative = header.relative_to(LIB)
            source = LIB / "examples" / relative.with_suffix(".cpp")
            guide = LIB / "docs/usage" / relative.with_suffix(".md")
            code = re.findall(fence + r"cpp\n(.*?)\n" + fence, guide.read_text(encoding="utf-8"), re.S)
            self.assertIn(source.read_text(encoding="utf-8").rstrip("\n"), code, str(guide))
        for path in LIB.rglob("*.md"):
            text = re.sub(fence + r".*?" + fence, "", path.read_text(encoding="utf-8"), flags=re.S)
            for target in re.findall(r"\[[^\]]+\]\(([^)]+)\)", text):
                target = target.strip().strip("<>")
                if "://" in target or target.startswith("#"):
                    continue
                target = target.split("#", 1)[0]
                self.assertTrue((path.parent / target).exists(), f"{path}: {target}")


if __name__ == "__main__":
    unittest.main(verbosity=2)
