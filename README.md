# cp-algorithm-vs

用于 ICPC 训练的 Visual Studio C++ 写题工作区。根目录 **main.cpp** 写当前题目，
`tools/cp.py` 负责抓样例、测样例、归档、对拍和导出提交代码。

- 从 **Codeforces、AtCoder、LeetCode** 自动抓取样例，一条命令跑完全部样例：[样例抓取说明](docs/FETCH.md)
- 已接入 **CP-STL 的 36 个算法模板**，支持 Visual Studio / MSVC：[使用说明](docs/CP_STL.md) · [模板索引](cp-stl/docs/usage/README.md)
- Visual Studio 设置、文件输入输出、对拍与模板宏：[工作流教程](docs/WORKFLOW.md)

用 Visual Studio 打开 **cp-algorithm.sln**。工具在外部修改 main.cpp 或项目文件后，VS 会提示重新加载，选择重新加载即可。
解决方案资源管理器中建议关闭“显示所有文件”，按已配置的分类浏览。

## 目录

| 路径 | 用途 |
| --- | --- |
| main.cpp | 当前题目，VS 中唯一参与编译的 .cpp |
| data/input.txt、expected.txt | 当前题目的输入与期望输出；抓取样例后是第 1 组样例 |
| data/samples/ | 抓取到的全部样例：1.in、1.ans、2.in… |
| data/problem.json | 当前题目的归档名、链接和标题，由工具维护 |
| solutions/平台/比赛/题号/ | 已归档的代码、样例和笔记 README.md |
| templates/main.cpp | 新题模板；modules/ 保留早期的 C++ 模块实验 |
| include/ | 公共头文件 pch.h、dbg.h，以及 LeetCode 本地驱动 leetcode.h |
| cp-stl/ | 算法模板库、使用手册和示例，已适配 MSVC |
| examples/stress/ | 可以直接运行的对拍示例（最大非空子段和） |
| tools/ | 命令行工具 cp.py 及其测试 |
| docs/ | 详细教程、整理记录与哈希清单 |
| build/ | 编译产物、测试输出、对拍反例，不进 Git |
| backups/ | 每次 new/load/fetch 前自动备份的当前工作，不进 Git |

归档按平台和题号存放，例如 solutions/cf/2100/A/、solutions/atcoder/abc400/E/、solutions/leetcode/two-sum/。
“二分、图论、DP”等算法标签写在该题的 README.md 里，同一道题不必为多个标签复制多份。

## 日常流程

在 VS 的“视图 → 终端”中打开 PowerShell（位于项目根目录）。工具读写的是磁盘上的文件，执行命令前先 Ctrl+Shift+S 保存。

```powershell
# 1. 开新题：备份当前工作，载入模板，抓取全部样例
python tools/cp.py new https://codeforces.com/contest/2100/problem/A

# 2. 写完后编译一次，检查所有样例
python tools/cp.py test

# 3. 归档到 solutions/cf/2100/A（名字取自抓取的题目）
python tools/cp.py save

# 回到以前的题目：先备份当前工作，再载入归档的代码和样例
python tools/cp.py load cf/2100/A
```

- 题目也可以写成简写，如 `2100A`、`abc400_e`、`leetcode/two-sum`；支持的写法和限制见[样例抓取说明](docs/FETCH.md)。
- 已经在写代码、只想补抓样例：`python tools/cp.py fetch 2100A`，不会改动 main.cpp。
- 不抓样例、只换成空白模板：`python tools/cp.py new`。没有抓取过题目时，save 要写出归档名，如 `save cf/2100/A`。
- save 遇到已有归档会询问 `[y/N]`：输入 `y` 或 `yes`（不区分大小写）才会覆盖代码和样例，回车或其他输入取消。
  已有的 README.md 笔记和其他文件会保留；想保留多个版本，可以另起名字，如 `save cf/2100/A-v2`。
- 想改新题的默认代码，编辑 templates/main.cpp。
- 手工新增或移动归档、模板文件后，运行 `python tools/cp.py sync` 刷新 VS 中的分类。
  归档、模板和对拍代码可以在 VS 中打开查看，但不参与主项目编译。

## LeetCode

```powershell
python tools/cp.py new https://leetcode.cn/problems/two-sum/
```

生成的 main.cpp 包含 LeetCode 给出的代码框架和一个本地驱动：在框架里写解答，`test` 就能跑全部示例，F5 也能断点调试。
**提交时只复制两条分隔线之间的代码。** 支持的参数类型和注意事项见[样例抓取说明](docs/FETCH.md#leetcode)。

## 运行与调试

| 命令 | 作用 |
| --- | --- |
| `python tools/cp.py test` | 用 g++ 编译 main.cpp，逐个运行 data/samples 中的样例并判定 |
| `python tools/cp.py run` | 编译 main.cpp，运行一次 data/input.txt，输出写到 data/output.txt |
| `python tools/cp.py run --expected data/expected.txt` | 同上，并与期望输出比较 |
| 在 test 或 run 后加 `--exe build/bin/x64/Debug/cp-algorithm.exe` | 改用 VS 已生成的程序（先 Ctrl+Shift+B 生成） |

- 默认按空白分隔的 token 比较，忽略多余空白和换行风格，但区分大小写；`--exact` 改为逐字节比较。
- 每次运行默认限时 2 秒，可加 `--timeout 5`。
- run 的标准错误写到 data/output.stderr.txt，不会混进答案；test 的输出保存在 build/tools/test/output/。

想在 **F5 断点调试时自动读取 data/input.txt**，在项目属性中添加 LOCAL_FILE 宏，步骤见
[Visual Studio、文件 I/O 与对拍教程](docs/WORKFLOW.md#b-f5-断点调试时读文件)。

## 对拍

```powershell
# 先跑一次自带示例
python tools/cp.py stress --solution examples/stress/solution.cpp --brute examples/stress/brute.cpp --gen examples/stress/gen.cpp --iterations 1000

# 自己的题：在 data/ 写好 brute.cpp 和 gen.cpp，默认把 main.cpp 当作待测程序
python tools/cp.py stress --brute data/brute.cpp --gen data/gen.cpp --iterations 10000
```

工具用 g++ 编译三个程序；gen 按种子生成一份输入，交给正解和暴力，比较两者输出。
出现不一致、超时或异常退出就停止，把反例、标准错误、种子、编译命令和当时的源码保存到 build/stress/failures/。
三个程序都使用标准输入输出，不要在代码里写死 freopen。
示例中的暴力和生成器只适用于最大非空子段和，要按自己题目的输入格式和约束改写。详见[对拍教程](docs/WORKFLOW.md#5-怎样写对拍)。

## 算法模板与提交

在 main.cpp 顶部包含需要的头文件，例如 `#include "data_structures/fenwick.hpp"`，然后通过 `cp::Fenwick<long long>` 使用；
VS 与命令行工具都已配置好包含路径。

```powershell
# 运行库中自带的例子，自动读取输入并核对答案
python tools/cp.py example data_structures/fenwick

# 展开当前代码依赖的本地头文件，生成可提交的 build/submission.cpp
python tools/cp.py export
```

模板中的 rep/per、all/rall、debug、debug_matrix、print 等用法见[模板说明](docs/WORKFLOW.md#9-模板宏与数据结构输出)。

## 编译器

- VS 使用 MSVC（v145 工具集，C++23）。工具的 test/run/stress 默认使用 PATH 中的 g++ 和 `-std=c++23`。
- 新模板至少需要 C++20；只用 C++17 的其他源码可加 `--std c++17`。部分旧题解用到 format/concepts/ranges，需要编译器支持。
- `--cxx` 指定 GCC/Clang 的路径；在 VS Developer PowerShell 中用 `--cxx cl` 改用 MSVC。`-I` 添加第三方头文件目录。
- 工具自检：`python -m unittest discover -s tools -p "test_*.py"`，覆盖范围见[工作流教程](docs/WORKFLOW.md#8-工具自检)。

## GitHub 仓库与日常同步

每次完成一批题目或修改工具后，先在 VS 中保存文件，再运行：

```powershell
git status
git add .
git diff --cached --stat
git commit -m "Add solutions for today's practice"
git push
```

第一次在另一台电脑使用：

```powershell
git clone https://github.com/202030481266/cp-algorithm-vs.git
cd cp-algorithm-vs
```

然后用 Visual Studio 打开 cp-algorithm.sln。项目使用 v145 工具集；
项目属性里 AtCoder Library 的包含目录是本机绝对路径，需要按新电脑的实际位置调整。
工具的编译运行功能另需 Python 3.10+ 和 g++。

.gitignore 已排除 VS 缓存、个人配置、编译产物、Python 缓存、运行输出和本地备份；
源码、VS 共享项目配置、模板、样例、工具和文档都纳入版本管理。
backups/ 和 build/ 只存在于本机：重要的对拍反例请复制到对应题目目录再提交。
整理前的文件映射和备份位置见[整理记录](docs/REORGANIZATION.md)。
