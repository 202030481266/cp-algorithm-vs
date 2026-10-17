"""Build and test the Editor Font Styles extension.

    python build.py
        -> restores the Visual Studio SDK from NuGet, builds Release, runs the tests and copies
           EditorFontStyles.vsix next to this script (double-click it to install).

    python build.py --exp
        -> also deploys to the experimental instance; try it with  devenv /rootSuffix Exp

    python build.py --no-test
    python build.py --corpus ..\\..
        -> the tests also analyze every .cpp/.hpp/.h under the directory and print statistics.

Needs Visual Studio 2022 or 2026 with MSBuild (any edition or workload); the extension SDK comes from NuGet.
"""
import argparse
import os
import shutil
import subprocess
import sys
from pathlib import Path

HERE = Path(__file__).resolve().parent
PROJECT = HERE / "EditorFontStyles.csproj"
OUTPUT = HERE / "EditorFontStyles.vsix"
TEST_DIR = HERE / "obj" / "tests"


def vswhere(*args):
    root = os.environ.get("ProgramFiles(x86)", r"C:\Program Files (x86)")
    exe = Path(root) / "Microsoft Visual Studio" / "Installer" / "vswhere.exe"
    if not exe.is_file():
        sys.exit("vswhere.exe not found: install Visual Studio 2022 or 2026")
    return subprocess.run([str(exe), "-latest", "-prerelease", "-products", "*", "-requires", "Microsoft.Component.MSBuild", *args],
                          capture_output=True, text=True, check=True).stdout.strip()


def find_tools():
    install = vswhere("-property", "installationPath")
    if not install:
        sys.exit("no Visual Studio with MSBuild found")
    msbuild = Path(install) / "MSBuild" / "Current" / "Bin" / "MSBuild.exe"
    csc = msbuild.parent / "Roslyn" / "csc.exe"
    return msbuild, csc, vswhere("-property", "instanceId")


def newtonsoft():
    packages = Path(os.environ.get("NUGET_PACKAGES") or Path.home() / ".nuget" / "packages")
    found = sorted((packages / "newtonsoft.json").glob("*/lib/net45/Newtonsoft.Json.dll"),
                   key=lambda p: [int(x) for x in p.parts[-4].split(".") if x.isdigit()])
    if not found:
        sys.exit("Newtonsoft.Json not in the NuGet cache; run the build first")
    return found[-1]


def build(msbuild, instance, deploy):
    command = [str(msbuild), str(PROJECT), "-restore", "-p:Configuration=Release", "-v:minimal", "-nologo"]
    if deploy:
        command += ["-p:DeployExtension=true", "-p:VSSDKTargetPlatformRegRootSuffix=Exp", f"-p:DeployTargetInstanceId={instance}"]
    if subprocess.run(command).returncode != 0:
        sys.exit("build failed")
    shutil.copy2(HERE / "bin" / "Release" / "EditorFontStyles.vsix", OUTPUT)
    print(f"{OUTPUT.name}: {OUTPUT.stat().st_size // 1024} KB", flush=True)


def test(csc, corpus):
    TEST_DIR.mkdir(parents=True, exist_ok=True)
    library = shutil.copy2(newtonsoft(), TEST_DIR)
    exe = TEST_DIR / "EditorFontStyles.Tests.exe"
    sources = sorted(str(p) for p in [*(HERE / "src" / "Cpp").glob("*.cs"), HERE / "src" / "Configuration" / "StyleConfig.cs",
                                      HERE / "src" / "Configuration" / "ColorTheme.cs",
                                      *(HERE / "tests").glob("*.cs")])
    compile_command = [str(csc), "-nologo", "-langversion:latest", "-target:exe", "-warn:4", f"-out:{exe}", f"-r:{library}",
                       f"-resource:{HERE / 'resources' / 'system-symbols.txt'},EditorFontStyles.system-symbols.txt", *sources]
    if subprocess.run(compile_command).returncode != 0:
        sys.exit("test build failed")
    run = [str(exe), str(HERE)] + (["--corpus", str(Path(corpus).resolve())] if corpus else [])
    # The test process caps its own memory at 1 GB and stops itself after two minutes.
    if subprocess.run(run, timeout=180).returncode != 0:
        sys.exit("tests failed")


def main():
    parser = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    parser.add_argument("--exp", action="store_true", help="deploy to the experimental instance (devenv /rootSuffix Exp)")
    parser.add_argument("--no-test", action="store_true", help="skip the tests")
    parser.add_argument("--corpus", help="also analyze the C++ files under this directory")
    args = parser.parse_args()
    msbuild, csc, instance = find_tools()
    build(msbuild, instance, args.exp)
    if not args.no_test:
        test(csc, args.corpus)


if __name__ == "__main__":
    main()
