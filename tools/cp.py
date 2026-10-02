#!/usr/bin/env python3
"""ICPC workspace helper. Requires Python 3.10+ and a C++ compiler for compilation."""
from __future__ import annotations

import argparse
from dataclasses import dataclass
from datetime import datetime
import json
import locale
import math
import os
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
WORK_FILES = ("main.cpp", "data/input.txt", "data/expected.txt", "data/output.txt", "data/problem.json")
SAMPLES = "data/samples"          # Fetched samples: 1.in, 1.ans, 2.in, ...
PROBLEM_FILE = "data/problem.json"  # Current problem: archive name, URL, title
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
            if folder == "data" and (path.name in {"output.txt", "problem.json"} or ".stderr." in path.name
                                     or path.is_relative_to(ROOT / SAMPLES)):
                continue  # Changes with every problem; listing it would churn the project file.
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
    if (ROOT / SAMPLES).is_dir():
        shutil.copytree(ROOT / SAMPLES, destination / SAMPLES)
    print(f"Current work backed up: {destination.relative_to(ROOT)}")
    return destination


def problem_name(name: str) -> str:
    parts = name.replace("\\", "/").split("/")
    if any(not re.fullmatch(r"[A-Za-z0-9][A-Za-z0-9_.-]*", part) for part in parts):
        raise ValueError("Use a path such as cf/2100/A (letters, digits, _, -, .).")
    return "/".join(parts)


def problem_path(name: str) -> Path:
    return local_path(Path("solutions").joinpath(*problem_name(name).split("/")))


def write_text(path: Path, text: str) -> None:
    path.parent.mkdir(parents=True, exist_ok=True)
    path.write_text(text, encoding="utf-8", newline="\n")


def read_info(path: Path) -> dict:
    if not path.is_file():
        return {}
    try:
        info = json.loads(path.read_text(encoding="utf-8-sig"))
    except ValueError:
        raise ValueError(f"Cannot parse {path.relative_to(ROOT)}; fix or delete it.") from None
    return info if isinstance(info, dict) else {}


def write_info(info: dict | None) -> None:
    path = ROOT / PROBLEM_FILE
    if info is None:
        path.unlink(missing_ok=True)
    else:
        write_text(path, json.dumps(info, indent=2, ensure_ascii=False) + "\n")


def replace_samples(source: Path | None) -> None:
    """Make data/samples a copy of an archive's samples/ folder, or remove it."""
    target = ROOT / SAMPLES
    if target.exists():
        shutil.rmtree(target)
    if source is not None and source.is_dir():
        shutil.copytree(source, target)


def save_problem(name: str | None = None) -> int:
    info = read_info(ROOT / PROBLEM_FILE)
    if name is None:
        name = info.get("id")
        if not isinstance(name, str) or not name:
            raise ValueError("Give an archive name such as cf/2100/A. "
                             "(After fetch/new <problem>, save needs no name.)")
    name = problem_name(name)
    destination = problem_path(name)
    if destination.exists():
        if not destination.is_dir():
            raise ValueError(f"Archive path is not a directory: {destination}")
        try:
            answer = input(
                f"Archive already exists: {destination.relative_to(ROOT)}. "
                "Overwrite code and sample data? [y/N] ")
        except EOFError:
            answer = ""
        if answer.strip().lower() not in {"y", "yes"}:
            print("Save cancelled.")
            return 0
    destination.mkdir(parents=True, exist_ok=True)
    for relative in WORK_FILES[:3]:
        source = ROOT / relative
        if source.is_file():
            shutil.copy2(source, destination / source.name)
    if (ROOT / SAMPLES).is_dir():
        if (destination / "samples").exists():
            shutil.rmtree(destination / "samples")
        shutil.copytree(ROOT / SAMPLES, destination / "samples")
    info["id"] = name  # Later saves without a name go to the same archive.
    write_info(info)
    shutil.copy2(ROOT / PROBLEM_FILE, destination / "problem.json")
    url, title = info.get("url"), info.get("title")
    link = f"[{title}]({url})" if url and title else url or ""
    notes = destination / "README.md"
    if not notes.exists():
        write_text(notes, f"# {name}\n\n- 题目链接：{link}\n- 算法标签：\n- 状态：待填写（归档不代表已 AC）\n"
                          "- 时间复杂度：\n- 易错点：\n")
    elif link:
        text = notes.read_bytes().decode("utf-8")
        filled = re.sub(r"^- 题目链接：[ \t]*(?=\r?$)", lambda _: "- 题目链接：" + link, text,
                        count=1, flags=re.M)
        if filled != text:
            notes.write_bytes(filled.encode("utf-8"))
    sync_project()
    print(f"Saved: {destination.relative_to(ROOT)}")
    return 0


def switch_problem(name: str | None) -> int:
    source = problem_path(name) if name is not None else ROOT / "templates"
    if not (source / "main.cpp").is_file():
        raise ValueError(f"Missing solution/template: {source / 'main.cpp'}")
    info = None
    if name is not None:
        info = read_info(source / "problem.json")
        info["id"] = problem_name(name)
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
    replace_samples(source / "samples" if name is not None else None)
    write_info(info)
    print("main.cpp and data files updated. Reload changed files in Visual Studio.")
    return 0


def download(spec: str, name: str | None):
    """Fetch before touching any file, so a network error changes nothing."""
    from fetch_problem import fetch, parse_target
    target = parse_target(spec)
    if name:
        target.id = problem_name(name)
    print(f"Fetching {target.url}", flush=True)
    return fetch(target)


def write_samples(problem) -> None:
    """Store every sample in data/samples; sample 1 also becomes input.txt/expected.txt."""
    replace_samples(None)
    for index, sample in enumerate(problem.samples, 1):
        write_text(ROOT / SAMPLES / f"{index}.in", sample.input)
        if sample.output is not None:
            write_text(ROOT / SAMPLES / f"{index}.ans", sample.output)
    write_text(ROOT / "data/input.txt", problem.samples[0].input)
    write_text(ROOT / "data/expected.txt", problem.samples[0].output or "")
    (ROOT / "data/output.txt").write_bytes(b"")
    write_info(problem.metadata())
    limit = f" (time limit: {problem.time_limit})" if problem.time_limit else ""
    print(f"{problem.target.id}: {problem.title}{limit}")
    print(f"{len(problem.samples)} sample(s) saved to {SAMPLES}/; sample 1 is also in "
          "data/input.txt and data/expected.txt.")
    for note in problem.notes:
        print(f"Note: {note}")


def fetch_samples(spec: str, name: str | None) -> int:
    problem = download(spec, name)
    snapshot()
    write_samples(problem)
    if problem.code is not None:
        print("main.cpp unchanged; 'new <problem>' would generate the LeetCode driver.")
    print("Check all samples: python tools/cp.py test")
    return 0


def new_problem(spec: str | None, name: str | None) -> int:
    if spec is None:
        if name:
            raise ValueError("--name needs a problem to fetch, e.g. new 1610F --name cf/1610/F")
        return switch_problem(None)
    problem = download(spec, name)
    template = ROOT / "templates/main.cpp"
    if problem.code is None and not template.is_file():
        raise ValueError(f"Missing template: {template}")
    snapshot()
    if problem.code is None:
        shutil.copy2(template, ROOT / "main.cpp")
    else:
        write_text(ROOT / "main.cpp", problem.code)
    write_samples(problem)
    print("main.cpp " + ("now holds the LeetCode code and local driver." if problem.code
                         else "reset to templates/main.cpp.") + " Reload changed files in Visual Studio.")
    if problem_path(problem.target.id).exists():
        print(f"Note: solutions/{problem.target.id} already exists; "
              f"'load {problem.target.id}' restores that code.")
    return 0


def toolchain_env(cxx: str) -> dict[str, str] | None:
    """Environment with the compiler's own directory first on PATH.

    On Windows, MinGW's ld runs from a folder without its DLLs, and g++-built programs need
    libstdc++/libwinpthread DLLs; both are looked up on PATH. Copies from Git for Windows or
    Anaconda that come earlier on PATH mix C runtimes and can crash the linker or the program.
    Nothing is hard-coded: the directory is wherever this machine's compiler was found.
    """
    compiler = shutil.which(cxx)
    if not compiler:
        return None
    env = os.environ.copy()
    env["PATH"] = os.path.dirname(os.path.abspath(compiler)) + os.pathsep + env.get("PATH", "")
    return env


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
                            stderr=subprocess.STDOUT, timeout=120, env=toolchain_env(compiler))
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


def execute(command: list[str], data: bytes, timeout: float,
            env: dict[str, str] | None = None) -> Run:
    started = time.perf_counter()
    try:
        result = subprocess.run(command, cwd=ROOT, input=data, stdout=subprocess.PIPE,
                                stderr=subprocess.PIPE, timeout=timeout, env=env)
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
    result = execute([str(executable)], data, args.timeout, toolchain_env(args.cxx))
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


def sample_files() -> list[tuple[str, Path, Path | None]]:
    """data/samples/N.in with N.ans; without samples, data/input.txt and a non-empty expected.txt."""
    folder = ROOT / SAMPLES
    numbered = lambda path: (not path.stem.isdigit(), int(path.stem) if path.stem.isdigit() else 0, path.stem)
    inputs = sorted(folder.glob("*.in"), key=numbered) if folder.is_dir() else []
    if inputs:
        return [(path.stem, path, path.with_suffix(".ans") if path.with_suffix(".ans").is_file() else None)
                for path in inputs]
    expected = ROOT / "data/expected.txt"
    answer = expected if expected.is_file() and expected.read_bytes().strip() else None
    return [("input.txt", ROOT / "data/input.txt", answer)]


def excerpt(data: bytes, limit: int = 12) -> str:
    lines = data.decode("utf-8", errors="replace").replace("\r\n", "\n").splitlines()
    shown = "\n".join("    " + line for line in lines[:limit])
    if len(lines) > limit:
        shown += f"\n    ... ({len(lines) - limit} more lines)"
    return shown or "    (empty)"


def first_difference(actual: bytes, expected: bytes) -> str:
    got, want = actual.split(), expected.split()
    for index, (left, right) in enumerate(zip(got, want), 1):
        if left != right:
            return f"token {index}: expected {right.decode(errors='replace')!r}, got {left.decode(errors='replace')!r}"
    return f"expected {len(want)} tokens, got {len(got)}"


def test_samples(args: argparse.Namespace) -> int:
    samples = sample_files()
    if not samples[0][1].is_file():
        raise ValueError("No samples. Run 'fetch <problem>' or fill data/input.txt first.")
    if args.exe:
        executable = local_path(args.exe)
        if not executable.is_file():
            raise ValueError(f"Executable not found: {executable}")
    else:
        executable = ROOT / "build/tools/test/solution.exe"
        compile_source(local_path(args.source), executable, args)
    outputs = ROOT / "build/tools/test/output"
    if outputs.exists():
        shutil.rmtree(outputs)
    outputs.mkdir(parents=True)
    failed = unchecked = 0
    env = toolchain_env(args.cxx)
    for name, input_path, answer_path in samples:
        data = input_path.read_bytes()
        result = execute([str(executable)], data, args.timeout, env)
        (outputs / f"{name}.out").write_bytes(result.stdout)
        (outputs / f"{name}.stderr.txt").write_bytes(result.stderr)
        expected = answer_path.read_bytes() if answer_path else None
        if result.timed_out:
            verdict = "TIME LIMIT"
        elif result.returncode != 0:
            verdict = f"RUNTIME ERROR (exit {result.returncode})"
        elif expected is None:
            verdict = "NO ANSWER TO COMPARE"
        elif equal_output(result.stdout, expected, args.exact):
            verdict = "OK"
        else:
            verdict = "WRONG ANSWER"
        print(f"Sample {name}: {verdict}  [{result.seconds:.3f}s]")
        if verdict == "OK":
            continue
        if verdict == "NO ANSWER TO COMPARE":
            unchecked += 1
        else:
            failed += 1
            print("  input:\n" + excerpt(data))
            if expected is not None:
                print("  expected:\n" + excerpt(expected))
        print("  output:\n" + excerpt(result.stdout))
        if verdict == "WRONG ANSWER" and not args.exact:
            print("  first difference: " + first_difference(result.stdout, expected))
        if result.stderr.strip():
            print("  stderr:\n" + excerpt(result.stderr, 8))
    checked = len(samples) - unchecked
    if checked:
        summary = f"{checked - failed}/{checked} samples passed"
        summary += f", {unchecked} without an answer" if unchecked else ""
    else:
        summary = "Ran without checking (no expected output)"
    print(f"{summary}. Outputs: {outputs.relative_to(ROOT)}")
    return 1 if failed else 0


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
    env = toolchain_env(args.cxx)
    for index in range(args.iterations):
        seed = args.seed + index
        runs = {"generator": execute([str(binaries["generator"]), str(seed)], b"", args.timeout, env)}
        status = runs["generator"].status("GENERATOR")
        if status is None:
            data = runs["generator"].stdout
            runs["brute"] = execute([str(binaries["brute"])], data, args.timeout, env)
            status = runs["brute"].status("BRUTE")
            if status is None:
                runs["solution"] = execute([str(binaries["solution"])], data, args.timeout, env)
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
    data = input_file.read_bytes() if input_file.exists() else b""
    result = execute([str(executable)], data, args.timeout, toolchain_env(args.cxx))
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
    problem_help = ("problem URL, or shorthand such as 1610F, cf/1610/F, abc400_e, "
                    "atcoder/abc400/E, leetcode/two-sum")
    starter = commands.add_parser("new", help="Back up current work; load templates/main.cpp, "
                                              "or fetch a problem and start it")
    starter.add_argument("problem", nargs="?", help=problem_help)
    fetcher = commands.add_parser("fetch", help="Download samples into data/ without changing main.cpp")
    fetcher.add_argument("problem", help=problem_help)
    for child in (starter, fetcher):
        child.add_argument("--name", help="archive name instead of the default, e.g. cf/1610/F")
    saver = commands.add_parser("save", help="Archive current code and data; confirm before overwriting")
    saver.add_argument("problem", nargs="?", help="e.g. cf/2100/A; defaults to the fetched/loaded problem")
    loader = commands.add_parser("load", help="Back up current work and load an archived problem")
    loader.add_argument("problem", help="e.g. cf/2100/A")
    checker = commands.add_parser("test", help="Compile once and check every sample in data/samples")
    checker.add_argument("--source", default="main.cpp")
    checker.add_argument("--exe", help="Use an existing executable instead of compiling")
    checker.add_argument("--exact", action="store_true")
    checker.add_argument("--timeout", type=positive_float, default=2.0)
    compiler_options(checker)
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
        if args.command == "load":
            return switch_problem(args.problem)
        if args.command == "new":
            return new_problem(args.problem, args.name)
        if args.command == "fetch":
            return fetch_samples(args.problem, args.name)
        if args.command == "test":
            return test_samples(args)
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

