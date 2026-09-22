# 在 Visual Studio 中使用 CP-STL

已从 D:/CP-Templates-and-Solutions/cp-stl 导入全部 35 个头文件、使用手册、38 个示例和算法回归程序，保存于 workspace 的 `cp-stl/`。这是项目内的副本，克隆本 workspace 后可直接使用；原始目录未改动。导入源文件的 SHA-256 见 [cp-stl-import.json](cp-stl-import.json)。

本地新增了 [计数排序与基数排序](../cp-stl/docs/usage/basic/sort.md)；当前共 36 个头文件、39 个示例。

## 第一次打开

保存 VS 中尚未保存的编辑，接受项目文件的外部修改并重新加载 `cp-algorithm.sln`。
解决方案资源管理器的 **04 算法库 cp-stl** 中可以按 basic、data_structures、graph、string、math、dp、geometry、util 浏览源码；docs/usage 下有逐个模板的说明，examples 下有例子。

[完整算法索引](../cp-stl/docs/usage/README.md) · [常用入口](../cp-stl/README.md) · [完整示例](../cp-stl/examples/README.md)

项目仍只编译根目录 main.cpp；F5 运行的是它。浏览库中的例子不会改变 F5 的入口。
VS 的 Debug/Release、x64/Win32 共享 cp-stl 包含目录。命令行 run/stress/example 也已自动配置。

## 写题时直接 include

在 main.cpp 顶部按题目需要添加头文件，例如：

```cpp
#include "data_structures/fenwick.hpp"
#include "math/combinatorics.hpp"
```

在 solve() 中使用：

```cpp
vector<long long> a{1, 2, 3};
cp::Fenwick<long long> bit(a);
bit.add(1, 5);
cout << bit.sum(0, 3) << '\n'; // 11
```

算法位于 `cp::`，下标默认 0-based，区间通常为 `[l,r)`。具体前置条件看对应使用手册。
也支持带库前缀的包含方式，例如 `#include "cp-stl/data_structures/fenwick.hpp"`。

执行 `python tools/cp.py new` 仍使用 [templates/main.cpp](../templates/main.cpp)，其中已加上常用头文件入口提示。原有别名、debug/debug_matrix、print、多测开关和 LOCAL_FILE 保持原来的用法。当前工作入口与样例没有因导入而被替换。

默认模板已经有 debug，不必再包含 util/debug.hpp。独立程序可使用库的 debug（LOCAL 启用，ONLINE_JUDGE 或 NO_DEBUG 关闭）；已有 debug 宏时库会保留它。

## 运行一个例子

在 workspace 根目录终端执行：

```powershell
python tools/cp.py example data_structures/fenwick
python tools/cp.py example graph/dinic
python tools/cp.py example math/number_theory
python tools/cp.py example "中文 路径/求和"
```

自动读取例子旁的 .in（没有则使用空输入），与 .ans 比较并打印 MATCH；不占用当前题目的 data 文件。
程序输出在终端，编译产物放在 build/examples/。

默认用 PATH 中的 g++。若要用 MSVC，在 VS 的 Developer PowerShell 中执行：

```powershell
python tools/cp.py example data_structures/fenwick --cxx cl
python tools/cp.py run --cxx cl
```

run/stress/example 支持 `--std c++17`、`c++20`、`c++23`；MSVC 的 c++23 映射到 /std:c++latest。
纯 cp-stl 头文件至少需要 C++17；workspace 默认主模板至少需要 C++20。

## 导出 OJ 单文件

```powershell
python tools/cp.py export
```

生成 **build/submission.cpp**，提交这个文件。会递归展开本地头文件，保留标准库 include 和条件编译；包含保护避免相同头文件重复定义。此命令不修改 main.cpp。

也可指定归档或输出路径：

```powershell
python tools/cp.py export --source solutions/cf/1500/C/main.cpp --output build/cf1500c.cpp
```

外部库通过 `-I` 指定；例如使用 ACL 时加 `-I D:/CP-Templates-and-Solutions/ac-library`。
导出器支持普通的 `#include "..."` / `#include <...>` 指令，不展开宏计算出的头文件名。它不求值 #if，因此条件分支内引用的本地头文件也需要能找到。遇到缺失头文件会报错并保留上次导出。
导出后按题目平台选择 C++ 标准，并使用原模板的 ONLINE_JUDGE / NO_DEBUG 开关关闭调试输出。

## MSVC 兼容处理

- 二分中点用无符号差值计算，支持 LLONG_MIN 到 LLONG_MAX。
- 模幂与一般模数逆元保留完整的 long long 输入范围。GCC 使用原生 128 位模乘；MSVC 在乘积能放入无符号 64 位时直接求模，否则使用精确加倍法。大模数下单次模乘 O(log mod)，模幂最坏 O(log exponent · log mod)，逆元最坏 O(log² mod)。常用 998244353 / 1000000007 模数无需加倍法。
- 凸包沿用原库 `|x|,|y| <= 1e9` 的坐标约束。MSVC 下 CrossProduct 为 long long，在该范围内叉积精确；GCC 下保留原生 128 位类型。
- 轻量起手式与测试程序改用标准头文件；MSVC 不提供 i128 别名。

其余算法接口保留原库形式。没有引入额外的宽整数库。

## 检查与后续维护

```powershell
# GCC：全部头文件、39 个例子、原随机对拍、数学边界和导出检查
python tools/test_cp_stl.py

# MSVC：在 VS Developer PowerShell 中执行同一套检查
$env:CP_STL_CXX = "cl"
python tools/test_cp_stl.py

# 工作区工具与模板的全部回归
python -m unittest discover -s tools -p "test_*.py"
```

数学边界另用 Python 任意精度整数计算期望值；GCC 测试还强制关闭原生 128 位分支，检查 MSVC 所用的可移植路径。

新增模板后运行 `python tools/cp.py sync` 刷新 VS 分类。
这份适配副本不会自动从外部目录同步；更新时按导入清单比较，并保留这里的兼容修改。
上游的 VS Code 任务脚本、GDB 配置测试和基准报告没有迁入，本项目统一使用 tools/cp.py 和以上测试入口。
