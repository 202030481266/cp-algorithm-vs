# 逐个模板的使用手册

这里覆盖全部 **36 个算法/工具头文件**，另有 [起手式 template.cpp](template.md)。每篇说明包含适用场景、接口、完整程序、预期输出和使用限制；文档里的程序与对应 `.cpp` 保持一致。

## 如何使用

1. 从下面按类别找模板，先读“使用方法”。
2. 打开同一行的“完整示例”，在 workspace 终端执行文末的 example 命令。这 36 个示例都内置数据，不需要输入；`debug.cpp` 还会向标准错误输出调试信息。
3. 按题目修改示例数据或在自己的解答中引用头文件。数组与字符串区间通常为 `[l,r)`，图的点从 `0` 开始；具体约束以对应说明为准。
4. 提交到 OJ 前执行 `python tools/cp.py export`，展开引用的本地头文件。

每个示例旁的 `.ans` 是预期标准输出，`util/debug.err` 是本地模式的预期标准错误输出。也可以在仓库根目录运行，例如：

```powershell
python tools/cp.py example "data_structures/fenwick"
```

[全部示例目录](../../examples/README.md) · [Visual Studio 使用说明](../../../docs/CP_STL.md) · [主索引](../../README.md)

## 基础

| 模板源码 | 使用方法 | 完整示例 |
| --- | --- | --- |
| [basic/search.hpp](../../basic/search.hpp) | [二分答案与有符号整除](basic/search.md) | [运行示例](../../examples/basic/search.cpp) |
| [basic/compress.hpp](../../basic/compress.hpp) | [离散化 Compressor](basic/compress.md) | [运行示例](../../examples/basic/compress.cpp) |
| [basic/sort.hpp](../../basic/sort.hpp) | [计数排序与基数排序](basic/sort.md) | [运行示例](../../examples/basic/sort.cpp) |

## 数据结构

| 模板源码 | 使用方法 | 完整示例 |
| --- | --- | --- |
| [data_structures/dsu.hpp](../../data_structures/dsu.hpp) | [并查集 DSU](data_structures/dsu.md) | [运行示例](../../examples/data_structures/dsu.cpp) |
| [data_structures/fenwick.hpp](../../data_structures/fenwick.hpp) | [树状数组 Fenwick](data_structures/fenwick.md) | [运行示例](../../examples/data_structures/fenwick.cpp) |
| [data_structures/segment_tree.hpp](../../data_structures/segment_tree.hpp) | [泛型线段树 SegmentTree](data_structures/segment_tree.md) | [运行示例](../../examples/data_structures/segment_tree.cpp) |
| [data_structures/lazy_segment_tree.hpp](../../data_structures/lazy_segment_tree.hpp) | [区间加与区间和 RangeAddSum](data_structures/lazy_segment_tree.md) | [运行示例](../../examples/data_structures/lazy_segment_tree.cpp) |
| [data_structures/sparse_table.hpp](../../data_structures/sparse_table.hpp) | [静态区间查询 SparseTable](data_structures/sparse_table.md) | [运行示例](../../examples/data_structures/sparse_table.cpp) |
| [data_structures/xor_basis.hpp](../../data_structures/xor_basis.hpp) | [异或线性基 XorBasis](data_structures/xor_basis.md) | [运行示例](../../examples/data_structures/xor_basis.cpp) |
| [data_structures/binary_trie.hpp](../../data_structures/binary_trie.hpp) | [01 Trie：单个元素的最大异或](data_structures/binary_trie.md) | [运行示例](../../examples/data_structures/binary_trie.cpp) |

## 图与树

| 模板源码 | 使用方法 | 完整示例 |
| --- | --- | --- |
| [graph/shortest_path.hpp](../../graph/shortest_path.hpp) | [BFS、01 BFS、Dijkstra 与 Floyd](graph/shortest_path.md) | [运行示例](../../examples/graph/shortest_path.cpp) |
| [graph/mst.hpp](../../graph/mst.hpp) | [Kruskal 最小生成树 / 森林](graph/mst.md) | [运行示例](../../examples/graph/mst.cpp) |
| [graph/topological_sort.hpp](../../graph/topological_sort.hpp) | [拓扑排序与有向环判断](graph/topological_sort.md) | [运行示例](../../examples/graph/topological_sort.cpp) |
| [graph/scc.hpp](../../graph/scc.hpp) | [强连通分量与缩点](graph/scc.md) | [运行示例](../../examples/graph/scc.cpp) |
| [graph/two_sat.hpp](../../graph/two_sat.hpp) | [2-SAT 布尔约束](graph/two_sat.md) | [运行示例](../../examples/graph/two_sat.cpp) |
| [graph/lca.hpp](../../graph/lca.hpp) | [LCA、树上距离与第 k 个祖先](graph/lca.md) | [运行示例](../../examples/graph/lca.cpp) |
| [graph/tree_diameter.hpp](../../graph/tree_diameter.hpp) | [无权树直径路径](graph/tree_diameter.md) | [运行示例](../../examples/graph/tree_diameter.cpp) |
| [graph/hld.hpp](../../graph/hld.hpp) | [树链剖分：点权路径与子树操作](graph/hld.md) | [运行示例](../../examples/graph/hld.cpp) |
| [graph/dinic.hpp](../../graph/dinic.hpp) | [Dinic 最大流与最小割](graph/dinic.md) | [运行示例](../../examples/graph/dinic.cpp) |

## 字符串

| 模板源码 | 使用方法 | 完整示例 |
| --- | --- | --- |
| [string/kmp.hpp](../../string/kmp.hpp) | [KMP 与前缀函数](string/kmp.md) | [运行示例](../../examples/string/kmp.cpp) |
| [string/z_function.hpp](../../string/z_function.hpp) | [Z 函数与前缀匹配](string/z_function.md) | [运行示例](../../examples/string/z_function.cpp) |
| [string/manacher.hpp](../../string/manacher.hpp) | [Manacher 回文半径与最长回文](string/manacher.md) | [运行示例](../../examples/string/manacher.cpp) |
| [string/trie.hpp](../../string/trie.hpp) | [字典树：单词与前缀计数](string/trie.md) | [运行示例](../../examples/string/trie.cpp) |
| [string/aho_corasick.hpp](../../string/aho_corasick.hpp) | [AC 自动机：多个模式的出现次数](string/aho_corasick.md) | [运行示例](../../examples/string/aho_corasick.cpp) |
| [string/rolling_hash.hpp](../../string/rolling_hash.hpp) | [双模子串哈希 RollingHash](string/rolling_hash.md) | [运行示例](../../examples/string/rolling_hash.cpp) |

## 数学

| 模板源码 | 使用方法 | 完整示例 |
| --- | --- | --- |
| [math/number_theory.hpp](../../math/number_theory.hpp) | [快速幂、逆元、素数筛与分解](math/number_theory.md) | [运行示例](../../examples/math/number_theory.cpp) |
| [math/modint.hpp](../../math/modint.hpp) | [模整数 ModInt](math/modint.md) | [运行示例](../../examples/math/modint.cpp) |
| [math/combinatorics.hpp](../../math/combinatorics.hpp) | [阶乘、组合数与排列数](math/combinatorics.md) | [运行示例](../../examples/math/combinatorics.cpp) |
| [math/matrix.hpp](../../math/matrix.hpp) | [矩阵乘法与快速幂](math/matrix.md) | [运行示例](../../examples/math/matrix.cpp) |

## 动态规划与序列

| 模板源码 | 使用方法 | 完整示例 |
| --- | --- | --- |
| [dp/sequence.hpp](../../dp/sequence.hpp) | [LIS、逆序对与滑动窗口最小值](dp/sequence.md) | [运行示例](../../examples/dp/sequence.cpp) |
| [dp/knapsack.hpp](../../dp/knapsack.hpp) | [01、完全与多重背包](dp/knapsack.md) | [运行示例](../../examples/dp/knapsack.cpp) |

## 几何

| 模板源码 | 使用方法 | 完整示例 |
| --- | --- | --- |
| [geometry/convex_hull.hpp](../../geometry/convex_hull.hpp) | [整数几何叉积与凸包](geometry/convex_hull.md) | [运行示例](../../examples/geometry/convex_hull.cpp) |

## 辅助工具

| 模板源码 | 使用方法 | 完整示例 |
| --- | --- | --- |
| [util/debug.hpp](../../util/debug.hpp) | [LOCAL 调试输出 debug](util/debug.md) | [运行示例](../../examples/util/debug.cpp) |
| [util/random.hpp](../../util/random.hpp) | [随机数据与可重现对拍](util/random.md) | [运行示例](../../examples/util/random.cpp) |
| [util/hash.hpp](../../util/hash.hpp) | [unordered_map / unordered_set 随机盐哈希](util/hash.md) | [运行示例](../../examples/util/hash.cpp) |
| [util/graphviz.hpp](../../util/graphviz.hpp) | [把图输出为 Graphviz DOT](util/graphviz.md) | [运行示例](../../examples/util/graphviz.cpp) |

## 验证文档和示例

在仓库根目录执行（Python 3.10+；日常编译运行示例不依赖 Python）：

```powershell
python tools/test_cp_stl.py
```

脚本逐个检查头文件对应的使用说明、示例源码、文档内代码、输出和相对链接，再以 GNU C++17 编译运行全部 39 个示例；额外验证不定义 `LOCAL` 时调试输出关闭。默认使用 PATH 中的 g++；在 VS Developer PowerShell 中设置 `$env:CP_STL_CXX = "cl"` 可改用 MSVC。
