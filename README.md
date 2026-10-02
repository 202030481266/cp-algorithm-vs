# cp-algorithm-vs

ICPC 训练用的 Visual Studio C++ 工作区：在根目录的 main.cpp 写当前题目，用 `tools/cp.py` 抓样例、测样例、归档、对拍和导出提交代码。

- [样例抓取](docs/FETCH.md)：支持 Codeforces、AtCoder、LeetCode
- [工作流教程](docs/WORKFLOW.md)：VS 设置、F5 读文件、对拍、模板宏
- [CP-STL 模板库](docs/CP_STL.md)：[模板索引](cp-stl/docs/usage/README.md) · [左程云课程对照表](cp-stl/docs/course-map.md)

## 环境

- Visual Studio（v145 工具集，C++23），打开 cp-algorithm.sln
- Python 3.10+ 和 g++（工具默认用 PATH 中的 g++，`-std=c++23`）
- 项目属性中 AtCoder Library 的包含目录是本机绝对路径，换电脑后需要修改

## 日常流程

工具读写磁盘上的文件，运行命令前先在 VS 中保存（Ctrl+Shift+S）；工具改动 main.cpp 或项目文件后，在 VS 中选择重新加载。

```powershell
python tools/cp.py new 2100A        # 备份当前工作，载入模板，抓取样例
python tools/cp.py test             # 编译 main.cpp 并跑全部样例
python tools/cp.py save             # 归档到 solutions/cf/2100/A
python tools/cp.py load cf/2100/A   # 载入以前的题目
```

- 题目可以写链接或简写（`2100A`、`abc400_e`、`lc/two-sum`），见[题目写法](docs/FETCH.md#题目写法)。
- `fetch <题目>` 只补抓样例，不改 main.cpp；不带参数的 `new` 只换成空白模板，此时 save 要写出归档名。
- save 覆盖已有归档前会确认，题目下的 README.md 笔记会保留。
- LeetCode 题会生成代码框架和本地驱动，可以直接 test 和 F5 调试；提交时只复制两条分隔线之间的代码，见[说明](docs/FETCH.md#leetcode)。
- 手动增删归档或模板后，运行 `sync` 刷新 VS 中的文件分类。

## 命令

| 命令 | 作用 |
| --- | --- |
| `test` | 编译 main.cpp，运行 data/samples 中的全部样例并判定 |
| `run` | 用 data/input.txt 运行一次，输出写到 data/output.txt；加 `--expected <文件>` 同时比较 |
| `stress --brute <暴力> --gen <生成器>` | 对拍，默认以 main.cpp 为待测程序，反例保存在 build/stress/failures/ |
| `example <模板>` | 运行模板库自带的例子，如 `example data_structures/fenwick` |
| `export` | 展开本地头文件，生成可提交的 build/submission.cpp |

常用选项：`--timeout 5`（默认 2 秒）、`--exact`（逐字节比较，默认按 token 比较）、`--exe <路径>`（改用 VS 生成的程序）、`--cxx <编译器>`、`--std c++17`、`-I <目录>`。

对拍可以先跑自带示例：

```powershell
python tools/cp.py stress --solution examples/stress/solution.cpp --brute examples/stress/brute.cpp --gen examples/stress/gen.cpp --iterations 1000
```

使用模板库时直接 `#include "data_structures/fenwick.hpp"`，通过 `cp::Fenwick<long long>` 使用，VS 和工具都已配好包含路径。

## 目录

| 路径 | 用途 |
| --- | --- |
| main.cpp | 当前题目，VS 中唯一参与编译的 .cpp |
| data/ | 当前题目的输入、期望输出和样例（samples/） |
| solutions/ | 已归档的题目，按 `平台/比赛/题号/` 存放，笔记写在各题的 README.md |
| templates/main.cpp | 新题模板 |
| include/ | 公共头文件 pch.h、dbg.h 和 LeetCode 驱动 leetcode.h |
| cp-stl/ | 算法模板库，已适配 MSVC |
| examples/stress/ | 对拍示例（最大非空子段和） |
| tools/ | cp.py 及其测试 |
| build/、backups/ | 编译产物和自动备份，不进 Git |
