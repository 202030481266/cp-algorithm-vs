#!/usr/bin/env python3
"""ICPC workspace helper. Requires Python 3.10+ and a C++ compiler for compilation."""
from __future__ import annotations

import argparse
from dataclasses import dataclass
from datetime import datetime
import json
import locale
import math
from pathlib import Path
import re
import shutil
import subprocess
import sys
import tempfile
import time
import xml.etree.ElementTree as ET

ROOT = Path(__file__).resolve().parents[1]
NS = "http://schemas.microsoft.com/developer/msbuild/2003"
ET.register_namespace("", NS)
WORK_FILES = ("main.cpp", "data/input.txt", "data/expected.txt", "data/output.txt")
GROUPS = {
    "include": "02 公共头文件", "solutions": "03 题解归档",
    "cp-stl": "04 算法库 cp-stl",
    "archive": "04 历史恢复", "templates": "05 模板",
    "examples": "06 示例", "data": "07 测试数据",
    "tools": "08 工具", "docs": "09 使用说明",
}


def q(name: str) -> str:
    return f"{{{NS}}}{name}"


def local_path(value: str | Path) -> Path:
    path = (ROOT / value).resolve()
    if not path.is_relative_to(ROOT):
        raise ValueError(f"Path must stay inside the workspace: {value}")
    return path


def write_xml(path: Path, root: ET.Element) -> None:
    ET.indent(root, space="  ")
    temporary = path.with_name(path.name + ".tmp")
    ET.ElementTree(root).write(temporary, encoding="utf-8", xml_declaration=True)
    temporary.replace(path)


def sync_project() -> None:
    """Register browsable files without compiling archived main() functions."""
    entries = [("main.cpp", "ClCompile", "01 当前题目")]
    for folder, title in GROUPS.items():
        for path in sorted((ROOT / folder).rglob("*")):
            if not path.is_file() or "__pycache__" in path.parts:
                continue
            if path.suffix in {".pyc", ".pyo", ".tmp"}:
                continue
            relative = path.relative_to(ROOT)
            if folder == "data" and (path.name == "output.txt" or ".stderr." in path.name):
                continue
            parents = path.parent.relative_to(ROOT / folder).parts
            group = "\\".join((title, *parents))
            kind = "ClInclude" if folder in {"include", "cp-stl"} and path.suffix in {".h", ".hpp"} else "None"
            entries.append((str(relative).replace("/", "\\"), kind, group))
    for filename in ("README.md", ".gitignore", ".editorconfig"):
        if (ROOT / filename).exists():
            entries.append((filename, "None", "09 使用说明"))

    project_path = ROOT / "cp-algorithm.vcxproj"
    project = ET.parse(project_path).getroot()
    for element in list(project):
        if element.tag == q("ItemGroup") and element.get("Label") == "WorkspaceFiles":
            project.remove(element)
    items = ET.Element(q("ItemGroup"), Label="WorkspaceFiles")
    filters = ET.Element(q("Project"), ToolsVersion="4.0")
    definitions = ET.SubElement(filters, q("ItemGroup"))
    filter_items = ET.SubElement(filters, q("ItemGroup"))
    filter_names = set()
    for relative, kind, group in entries:
        ET.SubElement(items, q(kind), Include=relative)
        item = ET.SubElement(filter_items, q(kind), Include=relative)
        ET.SubElement(item, q("Filter")).text = group
        parts = group.split("\\")
        filter_names.update("\\".join(parts[:i]) for i in range(1, len(parts) + 1))
    for name in sorted(filter_names):
        ET.SubElement(definitions, q("Filter"), Include=name)
    index = next(i for i, child in enumerate(project)
                 if child.tag == q("Import") and child.get("Project", "").endswith("Microsoft.Cpp.targets"))
    project.insert(index, items)
    write_xml(project_path, project)
    write_xml(ROOT / "cp-algorithm.vcxproj.filters", filters)
    print(f"VS file list refreshed: {len(entries)} files; only main.cpp is compiled.")


def snapshot() -> Path:
    parent = ROOT / "backups"
    parent.mkdir(exist_ok=True)
    destination = Path(tempfile.mkdtemp(
        prefix="work-" + datetime.now().strftime("%Y%m%d-%H%M%S-"), dir=parent))
    for relative in WORK_FILES:
        source = ROOT / relative
        if source.is_file():
            target = destination / relative
            target.parent.mkdir(parents=True, exist_ok=True)
            shutil.copy2(source, target)
    print(f"Current work backed up: {destination.relative_to(ROOT)}")
    return destination


def problem_path(name: str) -> Path:
    parts = name.replace("\\", "/").split("/")
    if any(not re.fullmatch(r"[A-Za-z0-9][A-Za-z0-9_.-]*", part) for part in parts):
        raise ValueError("Use a path such as codeforces/2100/A (letters, digits, _, -, .).")
    return local_path(Path("solutions").joinpath(*parts))


def save_problem(name: str) -> int:
    destination = problem_path(name)
    if destination.exists():
        raise ValueError(f"Archive already exists; choose another name: {destination}")
    destination.mkdir(parents=True)
    for relative in WORK_FILES[:3]:
        source = ROOT / relative
        if source.is_file():
            shutil.copy2(source, destination / source.name)
    (destination / "README.md").write_text(
        f"# {name}\n\n- 题目链接：\n- 算法标签：\n- 状态：待填写（归档不代表已 AC）\n"
        "- 时间复杂度：\n- 易错点：\n", encoding="utf-8")
    sync_project()
    print(f"Saved: {destination.relative_to(ROOT)}")
    return 0


def switch_problem(name: str | None) -> int:
    source = problem_path(name) if name is not None else ROOT / "templates"
    if not (source / "main.cpp").is_file():
        raise ValueError(f"Missing solution/template: {source / 'main.cpp'}")
    snapshot()
    shutil.copy2(source / "main.cpp", ROOT / "main.cpp")
    (ROOT / "data").mkdir(exist_ok=True)
    for filename in ("input.txt", "expected.txt"):
        target = ROOT / "data" / filename
        if name is not None and (source / filename).is_file():
            shutil.copy2(source / filename, target)
        else:
            target.write_bytes(b"")
    (ROOT / "data/output.txt").write_bytes(b"")
    print("main.cpp and data files updated. Reload changed files in Visual Studio.")
    return 0


def compile_source(source: Path, target: Path, args: argparse.Namespace) -> list[str]:
    if not source.is_file():
        raise ValueError(f"Missing source: {source}")
    compiler = shutil.which(args.cxx)
    if not compiler:
        raise ValueError(f"Compiler not found: {args.cxx}. Add g++ to PATH, use a VS developer terminal for cl, or use --cxx FULL_PATH.")
    target.parent.mkdir(parents=True, exist_ok=True)
    includes = [ROOT / "include", ROOT / "cp-stl", ROOT]
    includes.extend(Path(directory).resolve() for directory in args.include)
    if Path(compiler).stem.lower() == "cl":
        standard = "c++latest" if args.std == "c++23" else args.std
        command = [compiler, "/nologo", "/EHsc", "/utf-8", "/permissive-",
                   f"/std:{standard}", "/O2", "/W3", "/DLOCAL", "/D_CRT_SECURE_NO_WARNINGS"]
        command.extend("/I" + str(directory) for directory in includes)
        command.extend([str(source), "/Fe:" + str(target),
                        "/Fo:" + str(target.with_suffix(".obj"))])
    else:
        command = [compiler, f"-std={args.std}", "-O2", "-Wall", "-Wextra", "-DLOCAL"]
        for directory in includes:
            command.extend(["-I", str(directory)])
        command.extend([str(source), "-o", target.name])
    result = subprocess.run(command, cwd=target.parent, stdout=subprocess.PIPE,
                            stderr=subprocess.STDOUT, timeout=120)
    if result.stdout:
        try:
            diagnostics = result.stdout.decode("utf-8")
        except UnicodeDecodeError:
            diagnostics = result.stdout.decode(locale.getpreferredencoding(False), errors="replace")
        print(diagnostics, end="")
    if result.returncode:
        raise ValueError(f"Compilation failed: {source.relative_to(ROOT)} (exit {result.returncode})")
    return command


@dataclass
class Run:
    stdout: bytes
    stderr: bytes
    returncode: int | None
    timed_out: bool
    seconds: float

    def status(self, role: str) -> str | None:
        if self.timed_out:
            return role + "_TLE"
        if self.returncode != 0:
            return role + "_RE"
        return None

    def metadata(self) -> dict:
        return {"exit_code": self.returncode, "timed_out": self.timed_out,
                "seconds": round(self.seconds, 6)}


def execute(command: list[str], data: bytes, timeout: float) -> Run:
    started = time.perf_counter()
    try:
        result = subprocess.run(command, cwd=ROOT, input=data, stdout=subprocess.PIPE,
                                stderr=subprocess.PIPE, timeout=timeout)
        return Run(result.stdout, result.stderr, result.returncode, False,
                   time.perf_counter() - started)
    except subprocess.TimeoutExpired as error:
        return Run(error.stdout or b"", error.stderr or b"", None, True,
                   time.perf_counter() - started)


def equal_output(left: bytes, right: bytes, exact: bool) -> bool:
    return left == right if exact else left.split() == right.split()


def run_solution(args: argparse.Namespace) -> int:
    input_path = local_path(args.input)
    output_path = local_path(args.output)
    error_path = output_path.with_suffix(".stderr.txt")
    expected = local_path(args.expected) if args.expected else None
    if (input_path in {output_path, error_path} or output_path == error_path
            or expected in {output_path, error_path}):
        raise ValueError("Input/expected files must be different from output/log files.")
    data = input_path.read_bytes()
    expected_data = expected.read_bytes() if expected else None
    if args.exe:
        executable = local_path(args.exe)
        if not executable.is_file():
            raise ValueError(f"Executable not found: {executable}")
    else:
        executable = ROOT / "build/tools/run/solution.exe"
        compile_source(local_path(args.source), executable, args)
    result = execute([str(executable)], data, args.timeout)
    output_path.parent.mkdir(parents=True, exist_ok=True)
    output_path.write_bytes(result.stdout)
    error_path.write_bytes(result.stderr)
    print(f"stdout: {output_path.relative_to(ROOT)}; stderr: {error_path.relative_to(ROOT)}")
    status = result.status("SOLUTION")
    if status:
        print(f"{status}: exit={result.returncode}, elapsed={result.seconds:.3f}s")
        return 1
    if expected_data is not None:
        matched = equal_output(result.stdout, expected_data, args.exact)
        print("MATCH" if matched else "WRONG ANSWER")
        return 0 if matched else 1
    print(f"Finished in {result.seconds:.3f}s (output not checked).")
    return 0


def save_failure(status: str, seed: int, runs: dict[str, Run],
                 sources: dict[str, Path], commands: dict, args: argparse.Namespace) -> Path:
    parent = ROOT / "build/stress/failures"
    parent.mkdir(parents=True, exist_ok=True)
    destination = Path(tempfile.mkdtemp(
        prefix=datetime.now().strftime("%Y%m%d-%H%M%S-") + f"seed-{seed}-", dir=parent))
    (destination / "input.txt").write_bytes(runs["generator"].stdout)
    for role, result in runs.items():
        if role != "generator":
            (destination / f"{role}.out").write_bytes(result.stdout)
        (destination / f"{role}.stderr.txt").write_bytes(result.stderr)
    (destination / "sources").mkdir()
    for role, source in sources.items():
        shutil.copy2(source, destination / "sources" / f"{role}.cpp")
    metadata = {
        "status": status, "seed": seed, "timeout_seconds_per_program": args.timeout,
        "comparison": "exact bytes" if args.exact else "whitespace-separated tokens",
        "sources": {role: str(path.relative_to(ROOT)) for role, path in sources.items()},
        "compile_commands": commands,
        "compile_working_directories": {role: str(ROOT / "build/stress/bin") for role in sources},
        "results": {role: result.metadata() for role, result in runs.items()},
    }
    (destination / "metadata.json").write_text(
        json.dumps(metadata, indent=2, ensure_ascii=False) + "\n", encoding="utf-8")
    return destination


def stress(args: argparse.Namespace) -> int:
    if args.seed < 0 or args.seed + args.iterations > 2**64:
        raise ValueError("Seeds must stay in the unsigned 64-bit range.")
    sources = {"solution": local_path(args.solution), "brute": local_path(args.brute),
               "generator": local_path(args.gen)}
    binaries = {role: ROOT / "build/stress/bin" / f"{role}.exe" for role in sources}
    commands = {role: compile_source(path, binaries[role], args) for role, path in sources.items()}
    for index in range(args.iterations):
        seed = args.seed + index
        runs = {"generator": execute([str(binaries["generator"]), str(seed)], b"", args.timeout)}
        status = runs["generator"].status("GENERATOR")
        if status is None:
            data = runs["generator"].stdout
            runs["brute"] = execute([str(binaries["brute"])], data, args.timeout)
            status = runs["brute"].status("BRUTE")
            if status is None:
                runs["solution"] = execute([str(binaries["solution"])], data, args.timeout)
                status = runs["solution"].status("SOLUTION")
                if status is None and not equal_output(runs["solution"].stdout,
                                                       runs["brute"].stdout, args.exact):
                    status = "WRONG_ANSWER"
        if status is not None:
            destination = save_failure(status, seed, runs, sources, commands, args)
            print(f"{status}, seed={seed}. Saved: {destination.relative_to(ROOT)}", flush=True)
            return 1
        if (index + 1) % 100 == 0:
            print(f"Passed {index + 1}/{args.iterations} (seed={seed})", flush=True)
    print(f"PASS: {args.iterations} cases, seeds {args.seed}..{args.seed + args.iterations - 1}.")
    return 0


def export_solution(args: argparse.Namespace) -> int:
    from export_submission import expand_submission
    source, output = local_path(args.source), local_path(args.output)
    directories = [ROOT / "include", ROOT / "cp-stl", ROOT]
    directories.extend(Path(directory).resolve() for directory in args.include)
    count = expand_submission(source, output, directories, ROOT)
    print(f"Exported: {output.relative_to(ROOT)} ({count} local headers expanded).")
    return 0


def run_example(args: argparse.Namespace) -> int:
    examples = (ROOT / "cp-stl/examples").resolve()
    relative = Path(args.name)
    if relative.suffix != ".cpp":
        relative = relative.with_suffix(".cpp")
    source = (examples / relative).resolve()
    if not source.is_relative_to(examples) or not source.is_file():
        raise ValueError("Use a cp-stl example name such as data_structures/fenwick.")
    relative = source.relative_to(examples)
    executable = ROOT / "build/examples" / relative.with_suffix("") / "program.exe"
    compile_source(source, executable, args)
    input_file, answer = source.with_suffix(".in"), source.with_suffix(".ans")
    result = execute([str(executable)], input_file.read_bytes() if input_file.exists() else b"", args.timeout)
    if result.stdout:
        sys.stdout.write(result.stdout.decode("utf-8", errors="replace").replace("\r\n", "\n"))
    if result.stderr:
        sys.stderr.write(result.stderr.decode("utf-8", errors="replace").replace("\r\n", "\n"))
    status = result.status("EXAMPLE")
    if status:
        print(status)
        return 1
    if answer.exists():
        matched = equal_output(result.stdout, answer.read_bytes(), False)
        print("MATCH" if matched else "WRONG ANSWER")
        return 0 if matched else 1
    return 0


def positive_int(value: str) -> int:
    result = int(value)
    if result <= 0:
        raise argparse.ArgumentTypeError("Must be positive.")
    return result


def positive_float(value: str) -> float:
    result = float(value)
    if not math.isfinite(result) or result <= 0:
        raise argparse.ArgumentTypeError("Must be finite and positive.")
    return result


def compiler_options(parser: argparse.ArgumentParser) -> None:
    parser.add_argument("--cxx", default="g++", help="g++/clang++/cl command or full path (cl needs a VS developer terminal)")
    parser.add_argument("--std", choices=("c++17", "c++20", "c++23"), default="c++23")
    parser.add_argument("-I", "--include", action="append", default=[], help="Extra include directory")


def main() -> int:
    for stream in (sys.stdout, sys.stderr):
        if hasattr(stream, "reconfigure"):
            stream.reconfigure(encoding="utf-8", errors="backslashreplace")
    parser = argparse.ArgumentParser(description=__doc__)
    commands = parser.add_subparsers(dest="command", required=True)
    commands.add_parser("sync", help="Refresh Visual Studio file groups")
    commands.add_parser("new", help="Back up current work and load templates/main.cpp")
    for name, description in (("save", "Archive current code and data; refuse overwrites"),
                              ("load", "Back up current work and load an archived problem")):
        child = commands.add_parser(name, help=description)
        child.add_argument("problem", help="e.g. codeforces/2100/A")
    runner = commands.add_parser("run", help="Compile and run one input file, or run an existing exe")
    runner.add_argument("--source", default="main.cpp")
    runner.add_argument("--exe", help="Use an existing executable instead of compiling")
    runner.add_argument("--input", default="data/input.txt")
    runner.add_argument("--output", default="data/output.txt")
    runner.add_argument("--expected", help="Optional expected output file")
    runner.add_argument("--exact", action="store_true")
    runner.add_argument("--timeout", type=positive_float, default=2.0)
    compiler_options(runner)
    tester = commands.add_parser("stress", help="Compare solution with brute force on seeded random inputs")
    tester.add_argument("--solution", default="main.cpp")
    tester.add_argument("--brute", required=True)
    tester.add_argument("--gen", required=True)
    tester.add_argument("--iterations", type=positive_int, default=1000)
    tester.add_argument("--seed", type=int, default=1)
    tester.add_argument("--timeout", type=positive_float, default=2.0)
    tester.add_argument("--exact", action="store_true")
    compiler_options(tester)
    exporter = commands.add_parser("export", help="Expand local headers into one OJ submission")
    exporter.add_argument("--source", default="main.cpp")
    exporter.add_argument("--output", default="build/submission.cpp")
    exporter.add_argument("-I", "--include", action="append", default=[], help="Extra header directory")
    example = commands.add_parser("example", help="Compile a cp-stl example and compare its bundled answer")
    example.add_argument("name", help="e.g. data_structures/fenwick (relative to cp-stl/examples)")
    example.add_argument("--timeout", type=positive_float, default=2.0)
    compiler_options(example)
    args = parser.parse_args()
    try:
        if args.command == "sync":
            sync_project()
            return 0
        if args.command == "save":
            return save_problem(args.problem)
        if args.command in {"new", "load"}:
            return switch_problem(args.problem if args.command == "load" else None)
        if args.command == "run":
            return run_solution(args)
        if args.command == "export":
            return export_solution(args)
        if args.command == "example":
            return run_example(args)
        return stress(args)
    except (OSError, ValueError, ET.ParseError, subprocess.TimeoutExpired) as error:
        print(f"ERROR: {error}", file=sys.stderr)
        return 2
    except KeyboardInterrupt:
        print("\nStopped.", file=sys.stderr)
        return 130


if __name__ == "__main__":
    sys.exit(main())

