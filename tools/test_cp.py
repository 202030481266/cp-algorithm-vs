"""Regression checks in isolated workspace copies, including Chinese/space paths."""
from argparse import Namespace
from contextlib import redirect_stdout
import io
import json
from pathlib import Path
import shutil
import sys
import tempfile
import unittest
from unittest.mock import patch
import xml.etree.ElementTree as ET

import cp

REAL_ROOT = cp.ROOT


class WorkspaceTests(unittest.TestCase):
    def setUp(self):
        checks = REAL_ROOT / "build/checks"
        checks.mkdir(parents=True, exist_ok=True)
        self.temporary = tempfile.TemporaryDirectory(prefix="验证 space-", dir=checks)
        self.root = Path(self.temporary.name).resolve()
        self.root_patch = patch.object(cp, "ROOT", self.root)
        self.root_patch.start()
        self.addCleanup(self.root_patch.stop)
        self.addCleanup(self.temporary.cleanup)
        self.stdout = io.StringIO()
        self.quiet = redirect_stdout(self.stdout)
        self.quiet.__enter__()
        self.addCleanup(self.quiet.__exit__, None, None, None)
        for folder in ("data", "templates", "include"):
            (self.root / folder).mkdir()
        shutil.copy2(REAL_ROOT / "cp-algorithm.vcxproj", self.root / "cp-algorithm.vcxproj")
        self.original = "// 当前题目\nint main() { return 7; }\n".encode()
        (self.root / "main.cpp").write_bytes(self.original)
        (self.root / "data/input.txt").write_bytes(b"1\n-2\n")
        (self.root / "data/expected.txt").write_bytes(b"-2\n")
        (self.root / "data/output.txt").write_bytes(b"previous output\n")
        (self.root / "templates/main.cpp").write_text("int main() {}\n", encoding="utf-8")

    def test_save_new_load_and_backups(self):
        cp.save_problem("codeforces/2100/A")
        archived = self.root / "solutions/codeforces/2100/A/main.cpp"
        self.assertEqual(archived.read_bytes(), self.original)
        cp.switch_problem(None)
        backups = list((self.root / "backups").iterdir())
        self.assertEqual(len(backups), 1)
        self.assertEqual((backups[0] / "main.cpp").read_bytes(), self.original)
        self.assertEqual((backups[0] / "data/output.txt").read_bytes(), b"previous output\n")
        self.assertEqual((self.root / "data/input.txt").read_bytes(), b"")
        cp.switch_problem("codeforces/2100/A")
        self.assertEqual((self.root / "main.cpp").read_bytes(), self.original)
        self.assertEqual((self.root / "data/input.txt").read_bytes(), b"1\n-2\n")
        self.assertEqual((self.root / "data/expected.txt").read_bytes(), b"-2\n")
        self.assertEqual(len(list((self.root / "backups").iterdir())), 2)

    def test_refuses_archive_overwrite(self):
        cp.save_problem("training/A")
        (self.root / "main.cpp").write_bytes(b"new work\n")
        with self.assertRaises(ValueError):
            cp.save_problem("training/A")
        self.assertEqual((self.root / "solutions/training/A/main.cpp").read_bytes(), self.original)
        self.assertEqual((self.root / "main.cpp").read_bytes(), b"new work\n")

    def test_missing_load_does_not_modify_work(self):
        with self.assertRaises(ValueError):
            cp.switch_problem("missing/A")
        self.assertEqual((self.root / "main.cpp").read_bytes(), self.original)
        self.assertFalse((self.root / "backups").exists())

    def test_path_escape_rejected(self):
        for name in ("../oops", "codeforces/../../oops", "D:/oops", "/absolute", "A//B"):
            with self.subTest(name=name), self.assertRaises(ValueError):
                cp.problem_path(name)
        with self.assertRaises(ValueError):
            cp.local_path("../outside.txt")

    def test_sync_only_compiles_main_and_is_repeatable(self):
        cp.save_problem("training/A")
        cp.sync_project()
        project = self.root / "cp-algorithm.vcxproj"
        first = project.read_bytes()
        cp.sync_project()
        self.assertEqual(first, project.read_bytes())
        tree = ET.parse(project)
        compiled = tree.findall(f".//{cp.q('ClCompile')}[@Include]")
        self.assertEqual([node.get("Include") for node in compiled], ["main.cpp"])
        archives = tree.findall(f".//{cp.q('None')}[@Include]")
        self.assertTrue(any(node.get("Include") == r"solutions\training\A\main.cpp" for node in archives))

    @unittest.skipUnless(shutil.which("g++"), "g++ is required for subprocess integration")
    def test_actual_wrong_answer_and_timeout_are_saved(self):
        (self.root / "gen.cpp").write_text(
            '#include <iostream>\nint main(){std::cout << "1\\n-5\\n";}\n', encoding="utf-8")
        (self.root / "brute.cpp").write_text(
            '#include <iostream>\nint main(){int n,x; std::cin>>n>>x; std::cout<<x<<"\\n";}\n',
            encoding="utf-8")
        wrong = self.root / "wrong.cpp"
        wrong.write_text('#include <iostream>\nint main(){std::cout<<0<<"\\n";}\n', encoding="utf-8")
        args = Namespace(solution="wrong.cpp", brute="brute.cpp", gen="gen.cpp",
                         cxx="g++", std="c++23", include=[], timeout=1.0,
                         iterations=1, seed=42, exact=False)
        self.assertEqual(cp.stress(args), 1)
        failure = next((self.root / "build/stress/failures").iterdir())
        metadata = json.loads((failure / "metadata.json").read_text(encoding="utf-8"))
        self.assertEqual(metadata["status"], "WRONG_ANSWER")
        self.assertEqual(metadata["seed"], 42)
        self.assertEqual((failure / "input.txt").read_bytes().split(), [b"1", b"-5"])
        self.assertEqual((failure / "brute.out").read_bytes().split(), [b"-5"])
        self.assertEqual((failure / "solution.out").read_bytes().split(), [b"0"])
        self.assertEqual((failure / "sources/solution.cpp").read_bytes(), wrong.read_bytes())

        wrong.write_text(
            "#include <thread>\n#include <chrono>\n"
            "int main(){std::this_thread::sleep_for(std::chrono::seconds(5));}\n", encoding="utf-8")
        args.timeout = 0.25
        self.assertEqual(cp.stress(args), 1)
        failures = list((self.root / "build/stress/failures").iterdir())
        self.assertEqual(len(failures), 2)
        statuses = {json.loads((p / "metadata.json").read_text(encoding="utf-8"))["status"]
                    for p in failures}
        self.assertEqual(statuses, {"WRONG_ANSWER", "SOLUTION_TLE"})
        self.assertEqual((self.root / "main.cpp").read_bytes(), self.original)

    def test_input_file_is_not_overwritten(self):
        args = Namespace(input="data/input.txt", output="data/input.txt", expected=None)
        with self.assertRaises(ValueError):
            cp.run_solution(args)
        self.assertEqual((self.root / "data/input.txt").read_bytes(), b"1\n-2\n")

    def test_process_stdout_stderr_and_exit_status(self):
        result = cp.execute([sys.executable, "-c",
                             "import sys; print('answer'); print('debug', file=sys.stderr); sys.exit(3)"],
                            b"", 5)
        self.assertEqual(result.stdout.strip(), b"answer")
        self.assertEqual(result.stderr.strip(), b"debug")
        self.assertEqual(result.status("BRUTE"), "BRUTE_RE")

    def test_process_timeout_preserves_partial_output(self):
        result = cp.execute([sys.executable, "-u", "-c",
                             "import time; print('partial'); time.sleep(5)"], b"", 0.25)
        self.assertTrue(result.timed_out)
        self.assertEqual(result.stdout.strip(), b"partial")
        self.assertEqual(result.status("GENERATOR"), "GENERATOR_TLE")

    def test_output_comparison_rules(self):
        self.assertTrue(cp.equal_output(b"YES 1\r\n2 \n", b"YES\n1 2", False))
        self.assertFalse(cp.equal_output(b"YES", b"yes", False))
        self.assertFalse(cp.equal_output(b"1.0", b"1", False))
        self.assertFalse(cp.equal_output(b"1 2", b"12", False))
        self.assertFalse(cp.equal_output(b"1\r\n", b"1\n", True))


if __name__ == "__main__":
    unittest.main()

