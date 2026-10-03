# 逐个模板的使用手册

这里覆盖全部 **65 个算法/工具头文件**，另有 [起手式 template.cpp](template.md)。每篇说明包含适用场景、接口、完整程序、预期输出和使用限制；文档里的程序与对应 `.cpp` 保持一致。

## 如何使用

1. 从下面按类别找模板，先读“使用方法”。
2. 打开同一行的“完整示例”，在 workspace 终端执行文末的 example 命令。这 65 个示例都内置数据，不需要输入；`debug.cpp` 还会向标准错误输出调试信息。
3. 按题目修改示例数据或在自己的解答中引用头文件。数组与字符串区间通常为 `[l,r)`，图的点从 `0` 开始；具体约束以对应说明为准。
4. 提交到 OJ 前执行 `python tools/cp.py export`，展开引用的本地头文件。

预期标准输出集中放在 `examples/expected/` 中（如 `expected/data_structures/fenwick.ans`），`expected/util/debug.err` 是本地模式的预期标准错误输出。也可以在仓库根目录运行，例如：

```powershell
python tools/cp.py example "data_structures/fenwick"
```

[全部示例目录](../../examples/README.md) · [左程云课程对照表](../course-map.md) · [Visual Studio 使用说明](../../../docs/CP_STL.md) · [主索引](../../README.md)

## 基础

| 模板源码 | 使用方法 | 完整示例 |
| --- | --- | --- |
| [basic/search.hpp](../../basic/search.hpp) | [二分答案与有符号整除](basic/search.md) | [运行示例](../../examples/basic/search.cpp) |
| [basic/compress.hpp](../../basic/compress.hpp) | [离散化 Compressor](basic/compress.md) | [运行示例](../../examples/basic/compress.cpp) |
| [basic/sort.hpp](../../basic/sort.hpp) | [计数排序与基数排序](basic/sort.md) | [运行示例](../../examples/basic/sort.cpp) |
| [basic/prefix_sum.hpp](../../basic/prefix_sum.hpp) | [二维前缀和、二维差分与等差数列差分](basic/prefix_sum.md) | [运行示例](../../examples/basic/prefix_sum.cpp) |
| [basic/monotonic_stack.hpp](../../basic/monotonic_stack.hpp) | [单调栈与笛卡尔树](basic/monotonic_stack.md) | [运行示例](../../examples/basic/monotonic_stack.cpp) |

## 数据结构

| 模板源码 | 使用方法 | 完整示例 |
| --- | --- | --- |
| [data_structures/dsu.hpp](../../data_structures/dsu.hpp) | [并查集 DSU](data_structures/dsu.md) | [运行示例](../../examples/data_structures/dsu.cpp) |
| [data_structures/fenwick.hpp](../../data_structures/fenwick.hpp) | [树状数组 Fenwick](data_structures/fenwick.md) | [运行示例](../../examples/data_structures/fenwick.cpp) |
| [data_structures/segment_tree.hpp](../../data_structures/segment_tree.hpp) | [泛型线段树 SegmentTree](data_structures/segment_tree.md) | [运行示例](../../examples/data_structures/segment_tree.cpp) |
| [data_structures/lazy_segment_tree.hpp](../../data_structures/lazy_segment_tree.hpp) | [懒标记线段树：RangeAddSum 与通用 LazySegmentTree](data_structures/lazy_segment_tree.md) | [运行示例](../../examples/data_structures/lazy_segment_tree.cpp) |
| [data_structures/sparse_table.hpp](../../data_structures/sparse_table.hpp) | [静态区间查询 SparseTable](data_structures/sparse_table.md) | [运行示例](../../examples/data_structures/sparse_table.cpp) |
| [data_structures/xor_basis.hpp](../../data_structures/xor_basis.hpp) | [异或线性基 XorBasis 与前缀线性基](data_structures/xor_basis.md) | [运行示例](../../examples/data_structures/xor_basis.cpp) |
| [data_structures/binary_trie.hpp](../../data_structures/binary_trie.hpp) | [01 Trie：单个元素的最大异或](data_structures/binary_trie.md) | [运行示例](../../examples/data_structures/binary_trie.cpp) |
| [data_structures/range_fenwick.hpp](../../data_structures/range_fenwick.hpp) | [树状数组扩展：区间加区间和、二维树状数组](data_structures/range_fenwick.md) | [运行示例](../../examples/data_structures/range_fenwick.cpp) |
| [data_structures/segment_tree_beats.hpp](../../data_structures/segment_tree_beats.hpp) | [吉司机线段树：区间取最值与区间加](data_structures/segment_tree_beats.md) | [运行示例](../../examples/data_structures/segment_tree_beats.cpp) |
| [data_structures/persistent_segment_tree.hpp](../../data_structures/persistent_segment_tree.hpp) | [可持久化线段树（主席树）与区间第 k 小](data_structures/persistent_segment_tree.md) | [运行示例](../../examples/data_structures/persistent_segment_tree.cpp) |
| [data_structures/persistent_binary_trie.hpp](../../data_structures/persistent_binary_trie.hpp) | [可持久化 01 Trie：区间内的最大 / 最小异或](data_structures/persistent_binary_trie.md) | [运行示例](../../examples/data_structures/persistent_binary_trie.cpp) |
| [data_structures/segment_tree_merge.hpp](../../data_structures/segment_tree_merge.hpp) | [线段树合并与分裂](data_structures/segment_tree_merge.md) | [运行示例](../../examples/data_structures/segment_tree_merge.cpp) |
| [data_structures/dynamic_segment_tree.hpp](../../data_structures/dynamic_segment_tree.hpp) | [动态开点线段树：超大下标的区间加、区间和、区间最大值](data_structures/dynamic_segment_tree.md) | [运行示例](../../examples/data_structures/dynamic_segment_tree.cpp) |
| [data_structures/treap.hpp](../../data_structures/treap.hpp) | [FHQ Treap：有序多重集合与序列平衡树](data_structures/treap.md) | [运行示例](../../examples/data_structures/treap.cpp) |
| [data_structures/leftist_heap.hpp](../../data_structures/leftist_heap.hpp) | [左偏树：可合并堆](data_structures/leftist_heap.md) | [运行示例](../../examples/data_structures/leftist_heap.cpp) |
| [data_structures/weighted_dsu.hpp](../../data_structures/weighted_dsu.hpp) | [带权并查集：维护势能差](data_structures/weighted_dsu.md) | [运行示例](../../examples/data_structures/weighted_dsu.cpp) |
| [data_structures/rollback_dsu.hpp](../../data_structures/rollback_dsu.hpp) | [可撤销并查集与线段树分治](data_structures/rollback_dsu.md) | [运行示例](../../examples/data_structures/rollback_dsu.cpp) |
| [data_structures/link_cut_tree.hpp](../../data_structures/link_cut_tree.hpp) | [Link-Cut Tree：动态树](data_structures/link_cut_tree.md) | [运行示例](../../examples/data_structures/link_cut_tree.cpp) |
| [data_structures/mos_algorithm.hpp](../../data_structures/mos_algorithm.hpp) | [莫队算法 MosAlgorithm：普通莫队、回滚莫队、带修莫队](data_structures/mos_algorithm.md) | [运行示例](../../examples/data_structures/mos_algorithm.cpp) |

## 图与树

| 模板源码 | 使用方法 | 完整示例 |
| --- | --- | --- |
| [graph/shortest_path.hpp](../../graph/shortest_path.hpp) | [BFS、01 BFS、Dijkstra 与 Floyd](graph/shortest_path.md) | [运行示例](../../examples/graph/shortest_path.cpp) |
| [graph/mst.hpp](../../graph/mst.hpp) | [Kruskal 与稠密图 Prim（含最大生成树）](graph/mst.md) | [运行示例](../../examples/graph/mst.cpp) |
| [graph/topological_sort.hpp](../../graph/topological_sort.hpp) | [拓扑排序与有向环判断](graph/topological_sort.md) | [运行示例](../../examples/graph/topological_sort.cpp) |
| [graph/scc.hpp](../../graph/scc.hpp) | [强连通分量与缩点](graph/scc.md) | [运行示例](../../examples/graph/scc.cpp) |
| [graph/two_sat.hpp](../../graph/two_sat.hpp) | [2-SAT 布尔约束](graph/two_sat.md) | [运行示例](../../examples/graph/two_sat.cpp) |
| [graph/lca.hpp](../../graph/lca.hpp) | [LCA、树上距离与第 k 个祖先](graph/lca.md) | [运行示例](../../examples/graph/lca.cpp) |
| [graph/tree_diameter.hpp](../../graph/tree_diameter.hpp) | [无权树直径路径](graph/tree_diameter.md) | [运行示例](../../examples/graph/tree_diameter.cpp) |
| [graph/hld.hpp](../../graph/hld.hpp) | [树链剖分：点权路径与子树操作](graph/hld.md) | [运行示例](../../examples/graph/hld.cpp) |
| [graph/dinic.hpp](../../graph/dinic.hpp) | [Dinic 最大流与最小割](graph/dinic.md) | [运行示例](../../examples/graph/dinic.cpp) |
| [graph/spfa.hpp](../../graph/spfa.hpp) | [SPFA、负环、差分约束与 Johnson 全源最短路](graph/spfa.md) | [运行示例](../../examples/graph/spfa.cpp) |
| [graph/euler_path.hpp](../../graph/euler_path.hpp) | [欧拉路径与欧拉回路](graph/euler_path.md) | [运行示例](../../examples/graph/euler_path.cpp) |
| [graph/biconnected.hpp](../../graph/biconnected.hpp) | [割点、桥、点双、边双与圆方树](graph/biconnected.md) | [运行示例](../../examples/graph/biconnected.cpp) |
| [graph/kruskal_tree.hpp](../../graph/kruskal_tree.hpp) | [Kruskal 重构树](graph/kruskal_tree.md) | [运行示例](../../examples/graph/kruskal_tree.cpp) |
| [graph/fast_lca.hpp](../../graph/fast_lca.hpp) | [O(1) LCA：DFS 序 + ST 表](graph/fast_lca.md) | [运行示例](../../examples/graph/fast_lca.cpp) |
| [graph/virtual_tree.hpp](../../graph/virtual_tree.hpp) | [虚树](graph/virtual_tree.md) | [运行示例](../../examples/graph/virtual_tree.cpp) |
| [graph/centroid.hpp](../../graph/centroid.hpp) | [树的重心与点分治](graph/centroid.md) | [运行示例](../../examples/graph/centroid.cpp) |
| [graph/dsu_on_tree.hpp](../../graph/dsu_on_tree.hpp) | [树上启发式合并（DSU on tree）](graph/dsu_on_tree.md) | [运行示例](../../examples/graph/dsu_on_tree.cpp) |
| [graph/rerooting.hpp](../../graph/rerooting.hpp) | [换根 DP（全方位树 DP）](graph/rerooting.md) | [运行示例](../../examples/graph/rerooting.cpp) |
| [graph/segment_tree_graph.hpp](../../graph/segment_tree_graph.hpp) | [线段树优化建图](graph/segment_tree_graph.md) | [运行示例](../../examples/graph/segment_tree_graph.cpp) |

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
| [math/prime.hpp](../../math/prime.hpp) | [Miller-Rabin 与 Pollard-Rho：64 位素性测试和质因数分解](math/prime.md) | [运行示例](../../examples/math/prime.cpp) |
| [math/crt.hpp](../../math/crt.hpp) | [扩展欧几里得、线性同余与中国剩余定理](math/crt.md) | [运行示例](../../examples/math/crt.cpp) |
| [math/gauss.hpp](../../math/gauss.hpp) | [高斯消元：实数、模质数与异或方程组](math/gauss.md) | [运行示例](../../examples/math/gauss.cpp) |
| [math/permutation.hpp](../../math/permutation.hpp) | [康托展开与约瑟夫问题](math/permutation.md) | [运行示例](../../examples/math/permutation.cpp) |

## 动态规划与序列

| 模板源码 | 使用方法 | 完整示例 |
| --- | --- | --- |
| [dp/sequence.hpp](../../dp/sequence.hpp) | [LIS、逆序对与滑动窗口最小值](dp/sequence.md) | [运行示例](../../examples/dp/sequence.cpp) |
| [dp/knapsack.hpp](../../dp/knapsack.hpp) | [01、完全与多重背包](dp/knapsack.md) | [运行示例](../../examples/dp/knapsack.cpp) |

## 几何

| 模板源码 | 使用方法 | 完整示例 |
| --- | --- | --- |
| [geometry/convex_hull.hpp](../../geometry/convex_hull.hpp) | [整数几何叉积与凸包](geometry/convex_hull.md) | [运行示例](../../examples/geometry/convex_hull.cpp) |
| [geometry/rectangle_union.hpp](../../geometry/rectangle_union.hpp) | [矩形面积并与周长并（扫描线）](geometry/rectangle_union.md) | [运行示例](../../examples/geometry/rectangle_union.cpp) |

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

脚本逐个检查头文件对应的使用说明、示例源码、文档内代码、输出和相对链接，再以 GNU C++17 编译运行全部 68 个示例；额外验证不定义 `LOCAL` 时调试输出关闭。默认使用 PATH 中的 g++；在 VS Developer PowerShell 中设置 `$env:CP_STL_CXX = "cl"` 可改用 MSVC。
