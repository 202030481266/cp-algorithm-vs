"""Regression checks in isolated workspace copies, including Chinese/space paths."""
from argparse import Namespace
from contextlib import redirect_stdout
import io
import json
import os
from pathlib import Path
import shutil
import sys
import tempfile
import unittest
from unittest.mock import patch
import xml.etree.ElementTree as ET

import cp
from fetch_problem import Problem, Sample

REAL_ROOT = cp.ROOT


def fake_fetch(target, code=None):
    return Problem(target, title="A. Sum", time_limit="1 second", code=code,
                   samples=[Sample("1 2\n", "3\n"), Sample("5 5\n", None)])


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
        with patch("builtins.input", side_effect=AssertionError("New archives need no confirmation")):
            self.assertEqual(cp.save_problem("codeforces/2100/A"), 0)
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

    def test_confirmed_overwrite_updates_code_and_samples_and_preserves_notes(self):
        cp.save_problem("training/A")
        archive = self.root / "solutions/training/A"
        notes = "# 我的题解\n\n- 状态：AC\n- 算法标签：贪心\n".encode()
        (archive / "README.md").write_bytes(notes)
        (archive / "brute.cpp").write_bytes(b"// keep brute force\n")
        for index, answer in enumerate(("y", "yes", " Y ", " YES ")):
            with self.subTest(answer=answer):
                updates = {
                    "main.cpp": f"// revision {index}\nint main() {{}}\n".encode(),
                    "data/input.txt": f"{index}\n".encode(),
                    "data/expected.txt": f"{index + 1}\n".encode(),
                }
                for relative, content in updates.items():
                    (self.root / relative).write_bytes(content)
                with patch("builtins.input", return_value=answer) as prompt:
                    self.assertEqual(cp.save_problem("training/A"), 0)
                prompt.assert_called_once()
                for relative, content in updates.items():
                    self.assertEqual((archive / Path(relative).name).read_bytes(), content)
                    self.assertEqual((self.root / relative).read_bytes(), content)
                self.assertEqual((archive / "README.md").read_bytes(), notes)
                self.assertEqual((archive / "brute.cpp").read_bytes(), b"// keep brute force\n")
                self.assertFalse((archive / "output.txt").exists())

    def test_cancelled_overwrite_leaves_workspace_unchanged(self):
        cp.save_problem("training/A")
        (self.root / "main.cpp").write_bytes(b"new work\n")
        (self.root / "data/input.txt").write_bytes(b"new input\n")
        (self.root / "data/expected.txt").write_bytes(b"new answer\n")
        before = {path.relative_to(self.root): path.read_bytes()
                  for path in self.root.rglob("*") if path.is_file()}
        for answer in ("", "n", "NO", "maybe", EOFError()):
            with self.subTest(answer=answer):
                with patch("builtins.input", side_effect=[answer]) as prompt:
                    self.assertEqual(cp.save_problem("training/A"), 0)
                prompt.assert_called_once()
                after = {path.relative_to(self.root): path.read_bytes()
                         for path in self.root.rglob("*") if path.is_file()}
                self.assertEqual(after, before)
                self.assertIn("Save cancelled.", self.stdout.getvalue())

    def test_overwrite_creates_notes_when_missing(self):
        cp.save_problem("training/A")
        notes = self.root / "solutions/training/A/README.md"
        notes.unlink()
        with patch("builtins.input", return_value="y"):
            self.assertEqual(cp.save_problem("training/A"), 0)
        self.assertIn("# training/A", notes.read_text(encoding="utf-8"))

    def test_save_rejects_file_as_archive_directory(self):
        archive = self.root / "solutions/training/A"
        archive.parent.mkdir(parents=True)
        archive.write_bytes(b"existing file\n")
        with patch("builtins.input", side_effect=AssertionError("Invalid paths need no confirmation")):
            with self.assertRaisesRegex(ValueError, "not a directory"):
                cp.save_problem("training/A")
        self.assertEqual(archive.read_bytes(), b"existing file\n")

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

    def files(self, folder):
        return sorted(path.name for path in (self.root / folder).iterdir())

    def info(self, path="data/problem.json"):
        return json.loads((self.root / path).read_text(encoding="utf-8"))

    def test_failed_fetch_changes_nothing(self):
        with patch("fetch_problem.fetch", side_effect=ValueError("offline")):
            for command in (cp.new_problem, cp.fetch_samples):
                with self.subTest(command=command.__name__), self.assertRaisesRegex(ValueError, "offline"):
                    command("1610F", None)
        self.assertFalse((self.root / "backups").exists())
        self.assertEqual((self.root / "main.cpp").read_bytes(), self.original)
        self.assertEqual((self.root / "data/input.txt").read_bytes(), b"1\n-2\n")

    def test_fetch_writes_samples_and_keeps_code(self):
        with patch("fetch_problem.fetch", side_effect=fake_fetch):
            self.assertEqual(cp.fetch_samples("1610F", None), 0)
        self.assertEqual((self.root / "main.cpp").read_bytes(), self.original)
        self.assertEqual(self.files("data/samples"), ["1.ans", "1.in", "2.in"])
        self.assertEqual((self.root / "data/samples/2.in").read_bytes(), b"5 5\n")
        self.assertEqual((self.root / "data/input.txt").read_bytes(), b"1 2\n")
        self.assertEqual((self.root / "data/expected.txt").read_bytes(), b"3\n")
        self.assertEqual((self.root / "data/output.txt").read_bytes(), b"")
        self.assertEqual(self.info(), {"id": "cf/1610/F", "platform": "codeforces",
                                       "url": "https://codeforces.com/contest/1610/problem/F",
                                       "title": "A. Sum", "time_limit": "1 second"})
        backup = next((self.root / "backups").iterdir())
        self.assertEqual((backup / "data/input.txt").read_bytes(), b"1\n-2\n")

    def test_new_with_problem_uses_template_or_generated_code(self):
        with patch("fetch_problem.fetch", side_effect=fake_fetch):
            cp.new_problem("abc400_e", "training/E")
        self.assertEqual((self.root / "main.cpp").read_bytes(), (self.root / "templates/main.cpp").read_bytes())
        self.assertEqual(self.info()["id"], "training/E")
        with patch("fetch_problem.fetch", side_effect=lambda target: fake_fetch(target, "// 生成\n")):
            cp.new_problem("leetcode/two-sum", None)
        self.assertEqual((self.root / "main.cpp").read_text(encoding="utf-8"), "// 生成\n")
        self.assertEqual(self.info()["id"], "leetcode/two-sum")
        with self.assertRaisesRegex(ValueError, "--name"):
            cp.new_problem(None, "cf/1/A")

    def test_save_without_name_and_round_trip_with_samples(self):
        with self.assertRaisesRegex(ValueError, "archive name"):
            cp.save_problem()
        with patch("fetch_problem.fetch", side_effect=fake_fetch):
            cp.fetch_samples("1610F", None)
        with patch("builtins.input", side_effect=AssertionError("New archives need no confirmation")):
            cp.save_problem()
        archive = self.root / "solutions/cf/1610/F"
        self.assertEqual(self.files(archive / "samples"), ["1.ans", "1.in", "2.in"])
        self.assertEqual(self.info(archive / "problem.json")["id"], "cf/1610/F")
        self.assertIn("- 题目链接：[A. Sum](https://codeforces.com/contest/1610/problem/F)\n",
                      (archive / "README.md").read_text(encoding="utf-8"))

        cp.switch_problem(None)
        self.assertFalse((self.root / "data/samples").exists())
        self.assertFalse((self.root / "data/problem.json").exists())
        backup, = [path for path in (self.root / "backups").iterdir() if (path / "data/samples").is_dir()]
        self.assertEqual((backup / "data/samples/1.ans").read_bytes(), b"3\n")
        self.assertEqual(self.info(backup / "data/problem.json")["id"], "cf/1610/F")
        with self.assertRaisesRegex(ValueError, "archive name"):
            cp.save_problem()

        cp.switch_problem("cf\\1610\\F")
        self.assertEqual(self.files("data/samples"), ["1.ans", "1.in", "2.in"])
        self.assertEqual(self.info()["url"], "https://codeforces.com/contest/1610/problem/F")
        cp.save_problem("training/B")  # An explicit name becomes the current problem.
        self.assertEqual(self.info()["id"], "training/B")
        self.assertEqual(self.info("solutions/training/B/problem.json")["id"], "training/B")

    def test_load_old_archive_without_samples_or_metadata(self):
        cp.save_problem("training/A")
        (self.root / "solutions/training/A/problem.json").unlink()
        with patch("fetch_problem.fetch", side_effect=fake_fetch):
            cp.fetch_samples("1610F", None)
        cp.switch_problem("training/A")
        self.assertFalse((self.root / "data/samples").exists())
        self.assertEqual(self.info(), {"id": "training/A"})

    def test_save_fills_only_an_empty_link_line(self):
        cp.save_problem("training/A")
        notes = self.root / "solutions/training/A/README.md"
        self.assertIn("- 题目链接：\n", notes.read_text(encoding="utf-8"))
        notes.write_bytes("# 笔记\r\n- 题目链接：  \r\n- 状态：AC\r\n".encode())
        (self.root / "data/problem.json").write_text(
            json.dumps({"id": "training/A", "url": "https://x/1", "title": "T"}), encoding="utf-8")
        with patch("builtins.input", return_value="y"):
            cp.save_problem()
        self.assertEqual(notes.read_bytes(), "# 笔记\r\n- 题目链接：[T](https://x/1)\r\n- 状态：AC\r\n".encode())
        notes.write_bytes("- 题目链接：mine\n".encode())
        with patch("builtins.input", return_value="y"):
            cp.save_problem()
        self.assertEqual(notes.read_bytes(), "- 题目链接：mine\n".encode())

    def test_sync_skips_current_samples_but_lists_archived_ones(self):
        with patch("fetch_problem.fetch", side_effect=fake_fetch):
            cp.fetch_samples("1610F", None)
        cp.save_problem()
        listed = {node.get("Include") for node in ET.parse(self.root / "cp-algorithm.vcxproj").iter()
                  if node.get("Include")}
        self.assertIn(r"solutions\cf\1610\F\samples\1.in", listed)
        self.assertIn(r"data\input.txt", listed)
        self.assertFalse(any(name.startswith(r"data\samples") for name in listed))
        self.assertNotIn(r"data\problem.json", listed)

    @unittest.skipUnless(shutil.which("g++"), "g++ is required for subprocess integration")
    def test_test_command_reports_each_sample(self):
        (self.root / "main.cpp").write_text(
            "#include <chrono>\n#include <iostream>\n#include <thread>\n"
            "int main() { long long a, b; std::cin >> a >> b;\n"
            "  if (a == 7) return 3;\n"
            "  if (a == 9) std::this_thread::sleep_for(std::chrono::seconds(5));\n"
            "  std::cerr << \"debug a=\" << a << '\\n';\n"
            "  std::cout << a + b + (a == 2) << '\\n'; }\n", encoding="utf-8")
        samples = self.root / "data/samples"
        samples.mkdir()
        for name, data, answer in (("1", "1 2", "3"), ("2", "2 2", "4"), ("3", "7 0", "7"),
                                   ("4", "9 0", "9"), ("5", "4 4", None), ("10", "3 4", "7")):
            (samples / f"{name}.in").write_text(data + "\n", encoding="utf-8")
            if answer is not None:
                (samples / f"{name}.ans").write_text(answer + "\n", encoding="utf-8")
        args = Namespace(source="main.cpp", exe=None, exact=False, timeout=0.5,
                         cxx="g++", std="c++17", include=[])
        self.assertEqual(cp.test_samples(args), 1)
        report = self.stdout.getvalue()
        verdicts = [line for line in report.splitlines() if line.startswith("Sample ")]
        self.assertEqual([line.split("  [")[0] for line in verdicts], [
            "Sample 1: OK", "Sample 2: WRONG ANSWER", "Sample 3: RUNTIME ERROR (exit 3)",
            "Sample 4: TIME LIMIT", "Sample 5: NO ANSWER TO COMPARE", "Sample 10: OK"])
        self.assertIn("first difference: token 1: expected '4', got '5'", report)
        self.assertIn("debug a=2", report)
        self.assertIn("2/5 samples passed, 1 without an answer.", report)
        self.assertEqual((self.root / "build/tools/test/output/2.out").read_bytes().split(), [b"5"])

        shutil.rmtree(samples)  # Without samples, test checks data/input.txt.
        (self.root / "data/input.txt").write_bytes(b"1 2\n")
        (self.root / "data/expected.txt").write_bytes(b"3\n")
        self.assertEqual(cp.test_samples(args), 0)
        (self.root / "data/expected.txt").write_bytes(b"")
        self.assertEqual(cp.test_samples(args), 0)
        self.assertIn("Ran without checking", self.stdout.getvalue())

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

    def test_toolchain_directory_comes_first_on_path(self):
        env = cp.toolchain_env(sys.executable)
        self.assertEqual(env["PATH"].split(os.pathsep)[0], os.path.dirname(os.path.abspath(sys.executable)))
        self.assertIsNone(cp.toolchain_env("no-such-compiler-for-cp-tests"))

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

