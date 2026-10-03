# 可运行的模板示例

每个模板都有一个按相同路径命名的 `.cpp`。例如 `data_structures/fenwick.hpp` 对应本目录的 `data_structures/fenwick.cpp`，详细解释见 [使用手册](../docs/usage/README.md)。

下面 65 个模板示例都内置数据：在 workspace 根目录执行 `python tools/cp.py example 类别/名称`，自动编译运行并核对答案，不需要手工输入。期望的标准输出统一放在 [expected/](expected/) 中，按相同的相对路径命名（如 `expected/data_structures/fenwick.ans`），平时浏览例子时不用理会；`util/debug.cpp` 还会向标准错误输出内容，对照 [expected/util/debug.err](expected/util/debug.err)。

## 需要输入的入门示例

| 示例 | 输入 | 输出 | 说明 |
| --- | --- | --- | --- |
| [起手式求和](template.cpp) | [template.in](template.in) | [template.ans](expected/template.ans) | [从 template.cpp 开始写题](../docs/usage/template.md) |
| [树状数组区间查询](fenwick.cpp) | [fenwick.in](fenwick.in) | [fenwick.ans](expected/fenwick.ans) | 读入式示例；接口说明见 [Fenwick](../docs/usage/data_structures/fenwick.md)。 |
| [中文路径求和](<中文 路径/求和.cpp>) | [求和.in](<中文 路径/求和.in>) | [求和.ans](<expected/中文 路径/求和.ans>) | 验证中文和空格路径下的运行。 |

这三个示例有配套输入文件，example 命令会自动读取：

```powershell
python tools/cp.py example "template"
```

## 按模板查找

### 基础

| 使用方法 | 示例程序 | 预期输出 |
| --- | --- | --- |
| [二分答案与有符号整除](../docs/usage/basic/search.md) | [basic/search.cpp](basic/search.cpp) | [.ans](expected/basic/search.ans) |
| [离散化 Compressor](../docs/usage/basic/compress.md) | [basic/compress.cpp](basic/compress.cpp) | [.ans](expected/basic/compress.ans) |
| [计数排序与基数排序](../docs/usage/basic/sort.md) | [basic/sort.cpp](basic/sort.cpp) | [.ans](expected/basic/sort.ans) |
| [二维前缀和、二维差分与等差数列差分](../docs/usage/basic/prefix_sum.md) | [basic/prefix_sum.cpp](basic/prefix_sum.cpp) | [.ans](expected/basic/prefix_sum.ans) |
| [单调栈与笛卡尔树](../docs/usage/basic/monotonic_stack.md) | [basic/monotonic_stack.cpp](basic/monotonic_stack.cpp) | [.ans](expected/basic/monotonic_stack.ans) |

### 数据结构

| 使用方法 | 示例程序 | 预期输出 |
| --- | --- | --- |
| [并查集 DSU](../docs/usage/data_structures/dsu.md) | [data_structures/dsu.cpp](data_structures/dsu.cpp) | [.ans](expected/data_structures/dsu.ans) |
| [树状数组 Fenwick](../docs/usage/data_structures/fenwick.md) | [data_structures/fenwick.cpp](data_structures/fenwick.cpp) | [.ans](expected/data_structures/fenwick.ans) |
| [泛型线段树 SegmentTree](../docs/usage/data_structures/segment_tree.md) | [data_structures/segment_tree.cpp](data_structures/segment_tree.cpp) | [.ans](expected/data_structures/segment_tree.ans) |
| [懒标记线段树：RangeAddSum 与通用 LazySegmentTree](../docs/usage/data_structures/lazy_segment_tree.md) | [data_structures/lazy_segment_tree.cpp](data_structures/lazy_segment_tree.cpp) | [.ans](expected/data_structures/lazy_segment_tree.ans) |
| [静态区间查询 SparseTable](../docs/usage/data_structures/sparse_table.md) | [data_structures/sparse_table.cpp](data_structures/sparse_table.cpp) | [.ans](expected/data_structures/sparse_table.ans) |
| [异或线性基 XorBasis 与前缀线性基](../docs/usage/data_structures/xor_basis.md) | [data_structures/xor_basis.cpp](data_structures/xor_basis.cpp) | [.ans](expected/data_structures/xor_basis.ans) |
| [01 Trie：单个元素的最大异或](../docs/usage/data_structures/binary_trie.md) | [data_structures/binary_trie.cpp](data_structures/binary_trie.cpp) | [.ans](expected/data_structures/binary_trie.ans) |
| [树状数组扩展：区间加区间和、二维树状数组](../docs/usage/data_structures/range_fenwick.md) | [data_structures/range_fenwick.cpp](data_structures/range_fenwick.cpp) | [.ans](expected/data_structures/range_fenwick.ans) |
| [吉司机线段树：区间取最值与区间加](../docs/usage/data_structures/segment_tree_beats.md) | [data_structures/segment_tree_beats.cpp](data_structures/segment_tree_beats.cpp) | [.ans](expected/data_structures/segment_tree_beats.ans) |
| [可持久化线段树（主席树）与区间第 k 小](../docs/usage/data_structures/persistent_segment_tree.md) | [data_structures/persistent_segment_tree.cpp](data_structures/persistent_segment_tree.cpp) | [.ans](expected/data_structures/persistent_segment_tree.ans) |
| [可持久化 01 Trie：区间内的最大 / 最小异或](../docs/usage/data_structures/persistent_binary_trie.md) | [data_structures/persistent_binary_trie.cpp](data_structures/persistent_binary_trie.cpp) | [.ans](expected/data_structures/persistent_binary_trie.ans) |
| [线段树合并与分裂](../docs/usage/data_structures/segment_tree_merge.md) | [data_structures/segment_tree_merge.cpp](data_structures/segment_tree_merge.cpp) | [.ans](expected/data_structures/segment_tree_merge.ans) |
| [动态开点线段树：超大下标的区间加、区间和、区间最大值](../docs/usage/data_structures/dynamic_segment_tree.md) | [data_structures/dynamic_segment_tree.cpp](data_structures/dynamic_segment_tree.cpp) | [.ans](expected/data_structures/dynamic_segment_tree.ans) |
| [FHQ Treap：有序多重集合与序列平衡树](../docs/usage/data_structures/treap.md) | [data_structures/treap.cpp](data_structures/treap.cpp) | [.ans](expected/data_structures/treap.ans) |
| [左偏树：可合并堆](../docs/usage/data_structures/leftist_heap.md) | [data_structures/leftist_heap.cpp](data_structures/leftist_heap.cpp) | [.ans](expected/data_structures/leftist_heap.ans) |
| [带权并查集：维护势能差](../docs/usage/data_structures/weighted_dsu.md) | [data_structures/weighted_dsu.cpp](data_structures/weighted_dsu.cpp) | [.ans](expected/data_structures/weighted_dsu.ans) |
| [可撤销并查集与线段树分治](../docs/usage/data_structures/rollback_dsu.md) | [data_structures/rollback_dsu.cpp](data_structures/rollback_dsu.cpp) | [.ans](expected/data_structures/rollback_dsu.ans) |
| [Link-Cut Tree：动态树](../docs/usage/data_structures/link_cut_tree.md) | [data_structures/link_cut_tree.cpp](data_structures/link_cut_tree.cpp) | [.ans](expected/data_structures/link_cut_tree.ans) |
| [莫队算法 MosAlgorithm：普通莫队、回滚莫队、带修莫队](../docs/usage/data_structures/mos_algorithm.md) | [data_structures/mos_algorithm.cpp](data_structures/mos_algorithm.cpp) | [.ans](expected/data_structures/mos_algorithm.ans) |

### 图与树

| 使用方法 | 示例程序 | 预期输出 |
| --- | --- | --- |
| [BFS、01 BFS、Dijkstra 与 Floyd](../docs/usage/graph/shortest_path.md) | [graph/shortest_path.cpp](graph/shortest_path.cpp) | [.ans](expected/graph/shortest_path.ans) |
| [Kruskal 与稠密图 / 完全图 Prim（含最大生成树）](../docs/usage/graph/mst.md) | [graph/mst.cpp](graph/mst.cpp) | [.ans](expected/graph/mst.ans) |
| [拓扑排序与有向环判断](../docs/usage/graph/topological_sort.md) | [graph/topological_sort.cpp](graph/topological_sort.cpp) | [.ans](expected/graph/topological_sort.ans) |
| [强连通分量与缩点](../docs/usage/graph/scc.md) | [graph/scc.cpp](graph/scc.cpp) | [.ans](expected/graph/scc.ans) |
| [2-SAT 布尔约束](../docs/usage/graph/two_sat.md) | [graph/two_sat.cpp](graph/two_sat.cpp) | [.ans](expected/graph/two_sat.ans) |
| [LCA、树上距离与第 k 个祖先](../docs/usage/graph/lca.md) | [graph/lca.cpp](graph/lca.cpp) | [.ans](expected/graph/lca.ans) |
| [无权树直径路径](../docs/usage/graph/tree_diameter.md) | [graph/tree_diameter.cpp](graph/tree_diameter.cpp) | [.ans](expected/graph/tree_diameter.ans) |
| [树链剖分：点权路径与子树操作](../docs/usage/graph/hld.md) | [graph/hld.cpp](graph/hld.cpp) | [.ans](expected/graph/hld.ans) |
| [Dinic 最大流与最小割](../docs/usage/graph/dinic.md) | [graph/dinic.cpp](graph/dinic.cpp) | [.ans](expected/graph/dinic.ans) |
| [SPFA、负环、差分约束与 Johnson 全源最短路](../docs/usage/graph/spfa.md) | [graph/spfa.cpp](graph/spfa.cpp) | [.ans](expected/graph/spfa.ans) |
| [欧拉路径与欧拉回路](../docs/usage/graph/euler_path.md) | [graph/euler_path.cpp](graph/euler_path.cpp) | [.ans](expected/graph/euler_path.ans) |
| [割点、桥、点双、边双与圆方树](../docs/usage/graph/biconnected.md) | [graph/biconnected.cpp](graph/biconnected.cpp) | [.ans](expected/graph/biconnected.ans) |
| [Kruskal 重构树](../docs/usage/graph/kruskal_tree.md) | [graph/kruskal_tree.cpp](graph/kruskal_tree.cpp) | [.ans](expected/graph/kruskal_tree.ans) |
| [O(1) LCA：DFS 序 + ST 表](../docs/usage/graph/fast_lca.md) | [graph/fast_lca.cpp](graph/fast_lca.cpp) | [.ans](expected/graph/fast_lca.ans) |
| [虚树](../docs/usage/graph/virtual_tree.md) | [graph/virtual_tree.cpp](graph/virtual_tree.cpp) | [.ans](expected/graph/virtual_tree.ans) |
| [树的重心与点分治](../docs/usage/graph/centroid.md) | [graph/centroid.cpp](graph/centroid.cpp) | [.ans](expected/graph/centroid.ans) |
| [树上启发式合并（DSU on tree）](../docs/usage/graph/dsu_on_tree.md) | [graph/dsu_on_tree.cpp](graph/dsu_on_tree.cpp) | [.ans](expected/graph/dsu_on_tree.ans) |
| [换根 DP（全方位树 DP）](../docs/usage/graph/rerooting.md) | [graph/rerooting.cpp](graph/rerooting.cpp) | [.ans](expected/graph/rerooting.ans) |
| [线段树优化建图](../docs/usage/graph/segment_tree_graph.md) | [graph/segment_tree_graph.cpp](graph/segment_tree_graph.cpp) | [.ans](expected/graph/segment_tree_graph.ans) |

### 字符串

| 使用方法 | 示例程序 | 预期输出 |
| --- | --- | --- |
| [KMP 与前缀函数](../docs/usage/string/kmp.md) | [string/kmp.cpp](string/kmp.cpp) | [.ans](expected/string/kmp.ans) |
| [Z 函数与前缀匹配](../docs/usage/string/z_function.md) | [string/z_function.cpp](string/z_function.cpp) | [.ans](expected/string/z_function.ans) |
| [Manacher 回文半径与最长回文](../docs/usage/string/manacher.md) | [string/manacher.cpp](string/manacher.cpp) | [.ans](expected/string/manacher.ans) |
| [字典树：单词与前缀计数](../docs/usage/string/trie.md) | [string/trie.cpp](string/trie.cpp) | [.ans](expected/string/trie.ans) |
| [AC 自动机：多个模式的出现次数](../docs/usage/string/aho_corasick.md) | [string/aho_corasick.cpp](string/aho_corasick.cpp) | [.ans](expected/string/aho_corasick.ans) |
| [双模子串哈希 RollingHash](../docs/usage/string/rolling_hash.md) | [string/rolling_hash.cpp](string/rolling_hash.cpp) | [.ans](expected/string/rolling_hash.ans) |

### 数学

| 使用方法 | 示例程序 | 预期输出 |
| --- | --- | --- |
| [快速幂、逆元、素数筛与分解](../docs/usage/math/number_theory.md) | [math/number_theory.cpp](math/number_theory.cpp) | [.ans](expected/math/number_theory.ans) |
| [模整数 ModInt](../docs/usage/math/modint.md) | [math/modint.cpp](math/modint.cpp) | [.ans](expected/math/modint.ans) |
| [阶乘、组合数与排列数](../docs/usage/math/combinatorics.md) | [math/combinatorics.cpp](math/combinatorics.cpp) | [.ans](expected/math/combinatorics.ans) |
| [矩阵乘法与快速幂](../docs/usage/math/matrix.md) | [math/matrix.cpp](math/matrix.cpp) | [.ans](expected/math/matrix.ans) |
| [Miller-Rabin 与 Pollard-Rho：64 位素性测试和质因数分解](../docs/usage/math/prime.md) | [math/prime.cpp](math/prime.cpp) | [.ans](expected/math/prime.ans) |
| [扩展欧几里得、线性同余与中国剩余定理](../docs/usage/math/crt.md) | [math/crt.cpp](math/crt.cpp) | [.ans](expected/math/crt.ans) |
| [高斯消元：实数、模质数与异或方程组](../docs/usage/math/gauss.md) | [math/gauss.cpp](math/gauss.cpp) | [.ans](expected/math/gauss.ans) |
| [康托展开与约瑟夫问题](../docs/usage/math/permutation.md) | [math/permutation.cpp](math/permutation.cpp) | [.ans](expected/math/permutation.ans) |

### 动态规划与序列

| 使用方法 | 示例程序 | 预期输出 |
| --- | --- | --- |
| [LIS、逆序对与滑动窗口最小值](../docs/usage/dp/sequence.md) | [dp/sequence.cpp](dp/sequence.cpp) | [.ans](expected/dp/sequence.ans) |
| [01、完全与多重背包](../docs/usage/dp/knapsack.md) | [dp/knapsack.cpp](dp/knapsack.cpp) | [.ans](expected/dp/knapsack.ans) |

### 几何

| 使用方法 | 示例程序 | 预期输出 |
| --- | --- | --- |
| [整数几何叉积与凸包](../docs/usage/geometry/convex_hull.md) | [geometry/convex_hull.cpp](geometry/convex_hull.cpp) | [.ans](expected/geometry/convex_hull.ans) |
| [矩形面积并与周长并（扫描线）](../docs/usage/geometry/rectangle_union.md) | [geometry/rectangle_union.cpp](geometry/rectangle_union.cpp) | [.ans](expected/geometry/rectangle_union.ans) |

### 辅助工具

| 使用方法 | 示例程序 | 预期输出 |
| --- | --- | --- |
| [LOCAL 调试输出 debug](../docs/usage/util/debug.md) | [util/debug.cpp](util/debug.cpp) | [.ans](expected/util/debug.ans) |
| [随机数据与可重现对拍](../docs/usage/util/random.md) | [util/random.cpp](util/random.cpp) | [.ans](expected/util/random.ans) |
| [unordered_map / unordered_set 随机盐哈希](../docs/usage/util/hash.md) | [util/hash.cpp](util/hash.cpp) | [.ans](expected/util/hash.ans) |
| [把图输出为 Graphviz DOT](../docs/usage/util/graphviz.md) | [util/graphviz.cpp](util/graphviz.cpp) | [.ans](expected/util/graphviz.ans) |

## 批量验证

```powershell
python tools/test_cp_stl.py
```

在仓库根目录执行，将编译运行全部 68 个示例、核对输出，并检查所有模板都有使用文档且文档里的代码与示例一致。依赖 Python 3.10+，默认编译器为 PATH 中的 g++；在 VS Developer PowerShell 设置 `$env:CP_STL_CXX = "cl"` 可使用 MSVC。
