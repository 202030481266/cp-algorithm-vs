# 可运行的模板示例

每个模板都有一个按相同路径命名的 `.cpp`。例如 `data_structures/fenwick.hpp` 对应本目录的 `data_structures/fenwick.cpp`，详细解释见 [使用手册](../docs/usage/README.md)。

下面 36 个模板示例都内置数据：在 workspace 根目录执行 `python tools/cp.py example 类别/名称`，自动编译运行并核对答案，不需要手工输入。对应 `.ans` 文件记录标准输出；`util/debug.cpp` 还会向标准错误输出内容，对照 [debug.err](util/debug.err)。

## 需要输入的入门示例

| 示例 | 输入 | 输出 | 说明 |
| --- | --- | --- | --- |
| [起手式求和](template.cpp) | [template.in](template.in) | [template.ans](template.ans) | [从 template.cpp 开始写题](../docs/usage/template.md) |
| [树状数组区间查询](fenwick.cpp) | [fenwick.in](fenwick.in) | [fenwick.ans](fenwick.ans) | 读入式示例；接口说明见 [Fenwick](../docs/usage/data_structures/fenwick.md)。 |
| [中文路径求和](<中文 路径/求和.cpp>) | [求和.in](<中文 路径/求和.in>) | [求和.ans](<中文 路径/求和.ans>) | 验证中文和空格路径下的运行。 |

这三个示例有配套输入文件，example 命令会自动读取：

```powershell
python tools/cp.py example "template"
```

## 按模板查找

### 基础

| 使用方法 | 示例程序 | 预期输出 |
| --- | --- | --- |
| [二分答案与有符号整除](../docs/usage/basic/search.md) | [basic/search.cpp](basic/search.cpp) | [.ans](basic/search.ans) |
| [离散化 Compressor](../docs/usage/basic/compress.md) | [basic/compress.cpp](basic/compress.cpp) | [.ans](basic/compress.ans) |
| [计数排序与基数排序](../docs/usage/basic/sort.md) | [basic/sort.cpp](basic/sort.cpp) | [.ans](basic/sort.ans) |

### 数据结构

| 使用方法 | 示例程序 | 预期输出 |
| --- | --- | --- |
| [并查集 DSU](../docs/usage/data_structures/dsu.md) | [data_structures/dsu.cpp](data_structures/dsu.cpp) | [.ans](data_structures/dsu.ans) |
| [树状数组 Fenwick](../docs/usage/data_structures/fenwick.md) | [data_structures/fenwick.cpp](data_structures/fenwick.cpp) | [.ans](data_structures/fenwick.ans) |
| [泛型线段树 SegmentTree](../docs/usage/data_structures/segment_tree.md) | [data_structures/segment_tree.cpp](data_structures/segment_tree.cpp) | [.ans](data_structures/segment_tree.ans) |
| [区间加与区间和 RangeAddSum](../docs/usage/data_structures/lazy_segment_tree.md) | [data_structures/lazy_segment_tree.cpp](data_structures/lazy_segment_tree.cpp) | [.ans](data_structures/lazy_segment_tree.ans) |
| [静态区间查询 SparseTable](../docs/usage/data_structures/sparse_table.md) | [data_structures/sparse_table.cpp](data_structures/sparse_table.cpp) | [.ans](data_structures/sparse_table.ans) |
| [异或线性基 XorBasis](../docs/usage/data_structures/xor_basis.md) | [data_structures/xor_basis.cpp](data_structures/xor_basis.cpp) | [.ans](data_structures/xor_basis.ans) |
| [01 Trie：单个元素的最大异或](../docs/usage/data_structures/binary_trie.md) | [data_structures/binary_trie.cpp](data_structures/binary_trie.cpp) | [.ans](data_structures/binary_trie.ans) |

### 图与树

| 使用方法 | 示例程序 | 预期输出 |
| --- | --- | --- |
| [BFS、01 BFS、Dijkstra 与 Floyd](../docs/usage/graph/shortest_path.md) | [graph/shortest_path.cpp](graph/shortest_path.cpp) | [.ans](graph/shortest_path.ans) |
| [Kruskal 最小生成树 / 森林](../docs/usage/graph/mst.md) | [graph/mst.cpp](graph/mst.cpp) | [.ans](graph/mst.ans) |
| [拓扑排序与有向环判断](../docs/usage/graph/topological_sort.md) | [graph/topological_sort.cpp](graph/topological_sort.cpp) | [.ans](graph/topological_sort.ans) |
| [强连通分量与缩点](../docs/usage/graph/scc.md) | [graph/scc.cpp](graph/scc.cpp) | [.ans](graph/scc.ans) |
| [2-SAT 布尔约束](../docs/usage/graph/two_sat.md) | [graph/two_sat.cpp](graph/two_sat.cpp) | [.ans](graph/two_sat.ans) |
| [LCA、树上距离与第 k 个祖先](../docs/usage/graph/lca.md) | [graph/lca.cpp](graph/lca.cpp) | [.ans](graph/lca.ans) |
| [无权树直径路径](../docs/usage/graph/tree_diameter.md) | [graph/tree_diameter.cpp](graph/tree_diameter.cpp) | [.ans](graph/tree_diameter.ans) |
| [树链剖分：点权路径与子树操作](../docs/usage/graph/hld.md) | [graph/hld.cpp](graph/hld.cpp) | [.ans](graph/hld.ans) |
| [Dinic 最大流与最小割](../docs/usage/graph/dinic.md) | [graph/dinic.cpp](graph/dinic.cpp) | [.ans](graph/dinic.ans) |

### 字符串

| 使用方法 | 示例程序 | 预期输出 |
| --- | --- | --- |
| [KMP 与前缀函数](../docs/usage/string/kmp.md) | [string/kmp.cpp](string/kmp.cpp) | [.ans](string/kmp.ans) |
| [Z 函数与前缀匹配](../docs/usage/string/z_function.md) | [string/z_function.cpp](string/z_function.cpp) | [.ans](string/z_function.ans) |
| [Manacher 回文半径与最长回文](../docs/usage/string/manacher.md) | [string/manacher.cpp](string/manacher.cpp) | [.ans](string/manacher.ans) |
| [字典树：单词与前缀计数](../docs/usage/string/trie.md) | [string/trie.cpp](string/trie.cpp) | [.ans](string/trie.ans) |
| [AC 自动机：多个模式的出现次数](../docs/usage/string/aho_corasick.md) | [string/aho_corasick.cpp](string/aho_corasick.cpp) | [.ans](string/aho_corasick.ans) |
| [双模子串哈希 RollingHash](../docs/usage/string/rolling_hash.md) | [string/rolling_hash.cpp](string/rolling_hash.cpp) | [.ans](string/rolling_hash.ans) |

### 数学

| 使用方法 | 示例程序 | 预期输出 |
| --- | --- | --- |
| [快速幂、逆元、素数筛与分解](../docs/usage/math/number_theory.md) | [math/number_theory.cpp](math/number_theory.cpp) | [.ans](math/number_theory.ans) |
| [模整数 ModInt](../docs/usage/math/modint.md) | [math/modint.cpp](math/modint.cpp) | [.ans](math/modint.ans) |
| [阶乘、组合数与排列数](../docs/usage/math/combinatorics.md) | [math/combinatorics.cpp](math/combinatorics.cpp) | [.ans](math/combinatorics.ans) |
| [矩阵乘法与快速幂](../docs/usage/math/matrix.md) | [math/matrix.cpp](math/matrix.cpp) | [.ans](math/matrix.ans) |

### 动态规划与序列

| 使用方法 | 示例程序 | 预期输出 |
| --- | --- | --- |
| [LIS、逆序对与滑动窗口最小值](../docs/usage/dp/sequence.md) | [dp/sequence.cpp](dp/sequence.cpp) | [.ans](dp/sequence.ans) |
| [01、完全与多重背包](../docs/usage/dp/knapsack.md) | [dp/knapsack.cpp](dp/knapsack.cpp) | [.ans](dp/knapsack.ans) |

### 几何

| 使用方法 | 示例程序 | 预期输出 |
| --- | --- | --- |
| [整数几何叉积与凸包](../docs/usage/geometry/convex_hull.md) | [geometry/convex_hull.cpp](geometry/convex_hull.cpp) | [.ans](geometry/convex_hull.ans) |

### 辅助工具

| 使用方法 | 示例程序 | 预期输出 |
| --- | --- | --- |
| [LOCAL 调试输出 debug](../docs/usage/util/debug.md) | [util/debug.cpp](util/debug.cpp) | [.ans](util/debug.ans) |
| [随机数据与可重现对拍](../docs/usage/util/random.md) | [util/random.cpp](util/random.cpp) | [.ans](util/random.ans) |
| [unordered_map / unordered_set 随机盐哈希](../docs/usage/util/hash.md) | [util/hash.cpp](util/hash.cpp) | [.ans](util/hash.ans) |
| [把图输出为 Graphviz DOT](../docs/usage/util/graphviz.md) | [util/graphviz.cpp](util/graphviz.cpp) | [.ans](util/graphviz.ans) |

## 批量验证

```powershell
python tools/test_cp_stl.py
```

在仓库根目录执行，将编译运行全部 39 个示例、核对输出，并检查所有模板都有使用文档且文档里的代码与示例一致。依赖 Python 3.10+，默认编译器为 PATH 中的 g++；在 VS Developer PowerShell 设置 `$env:CP_STL_CXX = "cl"` 可使用 MSVC。
