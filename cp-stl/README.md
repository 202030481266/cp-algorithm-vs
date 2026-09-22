# CP-STL：Visual Studio workspace 适配版

由本机 D:/CP-Templates-and-Solutions/cp-stl 导入。包含全部 35 个头文件、使用手册、38 个可运行示例及原算法对拍；MSVC 与 GCC 均可使用。完整接入说明见 [Visual Studio 使用说明](../docs/CP_STL.md)。

- **开始写题**：执行 `python tools/cp.py new`，使用 workspace 的 [templates/main.cpp](../templates/main.cpp)。
- **找算法**：[35 个模板的完整使用手册](docs/usage/README.md)。
- **运行例子**：`python tools/cp.py example data_structures/fenwick`；[全部示例](examples/README.md)。
- **组合写法**：[常用写法与进阶入口](docs/recipes.md)。
- **轻量起手式**：[template.cpp](template.cpp) · [说明](docs/usage/template.md)。

## 先记住这 8 个入口

| 需求 | 文件 | 最常用接口 |
| --- | --- | --- |
| 二分答案、负数整除 | [basic/search.hpp](basic/search.hpp) | `first_true(lo, hi, check)`、`floor_div`、`ceil_div` |
| 连通性 | [data_structures/dsu.hpp](data_structures/dsu.hpp) | `DSU(n)`、`merge(u,v)`、`same(u,v)` |
| 单点加、区间和、第 k 小 | [data_structures/fenwick.hpp](data_structures/fenwick.hpp) | `add(p,x)`、`sum(l,r)`、`lower_bound(k)` |
| 单点修改、区间合并 | [data_structures/segment_tree.hpp](data_structures/segment_tree.hpp) | `set(p,x)`、`prod(l,r)`、`max_right(l,pred)` |
| 静态区间最值 / gcd | [data_structures/sparse_table.hpp](data_structures/sparse_table.hpp) | `SparseTable<T,Min/Max/Gcd>`、`query(l,r)` |
| BFS / 01 BFS / Dijkstra | [graph/shortest_path.hpp](graph/shortest_path.hpp) | `bfs`、`zero_one_bfs`、`dijkstra` |
| LCA、树上距离 | [graph/lca.hpp](graph/lca.hpp) | `lca(u,v)`、`distance(u,v)` |
| 模运算、组合数 | [math/combinatorics.hpp](math/combinatorics.hpp) | `Mint`、`Combinations<Mint>(n).choose(n,k)` |

## 一致的使用约定

1. **下标默认从 0 开始，数组与字符串区间为左闭右开 `[l,r)`**。题目给 1-based 闭区间 `[L,R]` 时，转换成 `[L-1,R)`。
2. 算法函数和类型位于 `namespace cp`，例如 `cp::DSU dsu(n);`；DOT 输出工具位于 `namespace graphviz`，`debug(...)` 是宏。
3. 头文件支持 **MSVC / GNU C++17 及以上**；workspace 主模板需要 C++20。宽整数兼容方式和性能差异见 [适配说明](../docs/CP_STL.md)。
4. 每个头文件都包含自己所需的头文件；有内部依赖时使用相对 `#include`，不需要预编译头、模块或 `import std`。
5. 图的邻接表只存真实的 `n` 个点。无向边要插入两次；Kruskal 的边列表每条无向边只存一次。
6. 和、距离、容量默认使用 `long long`。结果仍须在类型范围内；每个模板的适用条件和复杂度写在文件开头。
7. `.hpp` 是可复用组件，没有 `main()`。请运行自己的解答或 [示例目录](examples/README.md)中的 `.cpp`。

本地写法：

```cpp
#include <iostream>
#include <vector>
#include "data_structures/fenwick.hpp"
using namespace std;

int main() {
    vector<long long> a{1, 2, 3};
    cp::Fenwick<long long> bit(a);
    bit.add(1, 5);
    cout << bit.sum(0, 3) << '\n'; // 11
}
```

Visual Studio 和 tools/cp.py 已配置头文件搜索路径。**提交普通 OJ 前**执行 `python tools/cp.py export`，提交生成的 `build/submission.cpp`，其中已展开本地头文件及其依赖。

## 全部模板索引

### 基础与数据结构

| 模板 | 用途 / 复杂度 | 使用方法 | 完整例子 |
| --- | --- | --- | --- |
| [search.hpp](basic/search.hpp) | 二分第一个真值；有符号向上/下取整 | [说明](docs/usage/basic/search.md) | [示例](examples/basic/search.cpp) |
| [compress.hpp](basic/compress.hpp) | 离散化，预处理 O(n log n)，查询 O(log n) | [说明](docs/usage/basic/compress.md) | [示例](examples/basic/compress.cpp) |
| [dsu.hpp](data_structures/dsu.hpp) | 并查集，均摊 O(α(n)) | [说明](docs/usage/data_structures/dsu.md) | [示例](examples/data_structures/dsu.cpp) |
| [fenwick.hpp](data_structures/fenwick.hpp) | 树状数组，O(log n)；第 k 小要求所有频次非负 | [说明](docs/usage/data_structures/fenwick.md) | [示例](examples/data_structures/fenwick.cpp) |
| [segment_tree.hpp](data_structures/segment_tree.hpp) | 泛型线段树，O(log n)；结合律与单位元 | [说明](docs/usage/data_structures/segment_tree.md) | [示例](examples/data_structures/segment_tree.cpp) |
| [lazy_segment_tree.hpp](data_structures/lazy_segment_tree.hpp) | 区间加、区间和，O(log n) | [说明](docs/usage/data_structures/lazy_segment_tree.md) | [示例](examples/data_structures/lazy_segment_tree.cpp) |
| [sparse_table.hpp](data_structures/sparse_table.hpp) | ST 表，O(n log n) 建表，O(1) 查询；只用于幂等运算 | [说明](docs/usage/data_structures/sparse_table.md) | [示例](examples/data_structures/sparse_table.cpp) |
| [xor_basis.hpp](data_structures/xor_basis.hpp) | 64 位异或线性基，允许空子集；第 k 小从 0 开始 | [说明](docs/usage/data_structures/xor_basis.md) | [示例](examples/data_structures/xor_basis.cpp) |
| [binary_trie.hpp](data_structures/binary_trie.hpp) | 64 位 01 Trie，多重集合、最大异或，O(64) | [说明](docs/usage/data_structures/binary_trie.md) | [示例](examples/data_structures/binary_trie.cpp) |

### 图与树

| 模板 | 用途 / 复杂度 | 使用方法 | 完整例子 |
| --- | --- | --- | --- |
| [shortest_path.hpp](graph/shortest_path.hpp) | BFS、01 BFS、Dijkstra、Floyd；Dijkstra 禁止负边 | [说明](docs/usage/graph/shortest_path.md) | [示例](examples/graph/shortest_path.cpp) |
| [mst.hpp](graph/mst.hpp) | Kruskal 最小生成树/森林，O(m log m) | [说明](docs/usage/graph/mst.md) | [示例](examples/graph/mst.cpp) |
| [topological_sort.hpp](graph/topological_sort.hpp) | 拓扑排序，O(n+m)；长度不足 n 表示有环 | [说明](docs/usage/graph/topological_sort.md) | [示例](examples/graph/topological_sort.cpp) |
| [scc.hpp](graph/scc.hpp) | 强连通分量，迭代 Kosaraju，O(n+m)；编号按拓扑序 | [说明](docs/usage/graph/scc.md) | [示例](examples/graph/scc.cpp) |
| [two_sat.hpp](graph/two_sat.hpp) | 2-SAT，O(n+m)，无解返回 `nullopt` | [说明](docs/usage/graph/two_sat.md) | [示例](examples/graph/two_sat.cpp) |
| [lca.hpp](graph/lca.hpp) | 倍增 LCA / 距离 / 第 k 个祖先，查询 O(log n) | [说明](docs/usage/graph/lca.md) | [示例](examples/graph/lca.cpp) |
| [tree_diameter.hpp](graph/tree_diameter.hpp) | 无权树直径路径，O(n) | [说明](docs/usage/graph/tree_diameter.md) | [示例](examples/graph/tree_diameter.cpp) |
| [hld.hpp](graph/hld.hpp) | 迭代树链剖分，点权路径分成 O(log n) 段，子树连续 | [说明](docs/usage/graph/hld.md) | [示例](examples/graph/hld.cpp) |
| [dinic.hpp](graph/dinic.hpp) | 最大流/最小割，一般图 O(V²E)；可建模二分图匹配 | [说明](docs/usage/graph/dinic.md) | [示例](examples/graph/dinic.cpp) |

### 字符串

这些模板操作字节字符串，不把 UTF-8 汉字当成单个字符；Trie/AC 自动机只接受小写 `a-z`。

| 模板 | 用途 / 复杂度 | 使用方法 | 完整例子 |
| --- | --- | --- | --- |
| [kmp.hpp](string/kmp.hpp) | 前缀函数、所有匹配位置，O(n+m)，支持重叠与空模式 | [说明](docs/usage/string/kmp.md) | [示例](examples/string/kmp.cpp) |
| [z_function.hpp](string/z_function.hpp) | Z 函数，O(n)，`z[0]=n` | [说明](docs/usage/string/z_function.md) | [示例](examples/string/z_function.cpp) |
| [manacher.hpp](string/manacher.hpp) | 奇偶回文半径、最长回文区间，O(n) | [说明](docs/usage/string/manacher.md) | [示例](examples/string/manacher.cpp) |
| [trie.hpp](string/trie.hpp) | 字典树、前缀计数、重复插入/删除，O(字符串长度) | [说明](docs/usage/string/trie.md) | [示例](examples/string/trie.cpp) |
| [aho_corasick.hpp](string/aho_corasick.hpp) | AC 自动机、多模式出现次数；先插入再 build | [说明](docs/usage/string/aho_corasick.md) | [示例](examples/string/aho_corasick.cpp) |
| [rolling_hash.hpp](string/rolling_hash.hpp) | 双模子串哈希，O(1) 查询；概率算法，有碰撞可能 | [说明](docs/usage/string/rolling_hash.md) | [示例](examples/string/rolling_hash.cpp) |

### 数学、DP、几何与工具

| 模板 | 用途 | 使用方法 | 完整例子 |
| --- | --- | --- | --- |
| [number_theory.hpp](math/number_theory.hpp) | 安全模乘快速幂、一般模数逆元、线性筛、筛内质因数分解 | [说明](docs/usage/math/number_theory.md) | [示例](examples/math/number_theory.cpp) |
| [modint.hpp](math/modint.hpp) | 静态模整数；`Mint=998244353`，`Mint1000000007` | [说明](docs/usage/math/modint.md) | [示例](examples/math/modint.cpp) |
| [combinatorics.hpp](math/combinatorics.hpp) | 阶乘/逆阶乘、C(n,k)、A(n,k)；质数模数且 n < mod | [说明](docs/usage/math/combinatorics.md) | [示例](examples/math/combinatorics.cpp) |
| [matrix.hpp](math/matrix.hpp) | 方阵乘法与快速幂，推荐配合 `ModInt` | [说明](docs/usage/math/matrix.md) | [示例](examples/math/matrix.cpp) |
| [sequence.hpp](dp/sequence.hpp) | LIS/不下降子序列、严格逆序对、单调队列滑动最小值 | [说明](docs/usage/dp/sequence.md) | [示例](examples/dp/sequence.cpp) |
| [knapsack.hpp](dp/knapsack.hpp) | 01 / 完全 / 多重背包；容量不超过 c 的最大价值 | [说明](docs/usage/dp/knapsack.md) | [示例](examples/dp/knapsack.cpp) |
| [convex_hull.hpp](geometry/convex_hull.hpp) | 整数叉积、Andrew 凸包，去除中间共线点 | [说明](docs/usage/geometry/convex_hull.md) | [示例](examples/geometry/convex_hull.cpp) |
| [debug.hpp](util/debug.hpp) | `debug(a, b)`，容器/pair/tuple 输出到 stderr；仅 LOCAL 启用 | [说明](docs/usage/util/debug.md) | [示例](examples/util/debug.cpp) |
| [random.hpp](util/random.hpp) | 随机数据、打乱；支持固定种子重现对拍 | [说明](docs/usage/util/random.md) | [示例](examples/util/random.cpp) |
| [hash.hpp](util/hash.hpp) | unordered_map 的随机盐哈希 | [说明](docs/usage/util/hash.md) | [示例](examples/util/hash.cpp) |
| [graphviz.hpp](util/graphviz.hpp) | 保留原有图结构 DOT 输出工具，默认 0-based | [说明](docs/usage/util/graphviz.md) | [示例](examples/util/graphviz.cpp) |

## 验证

在 workspace 根目录运行 `python tools/test_cp_stl.py`，检查头文件独立编译、示例输出、使用手册、原算法对拍、Python 大整数边界和单文件导出。

默认编译器是 g++；在 Visual Studio Developer PowerShell 中设置 `$env:CP_STL_CXX = "cl"` 后可运行同一组 MSVC 检查。

`tests/algorithms.cpp` 保留固定种子的朴素算法对拍，包括 20 万节点长链；测试程序通过独立工具编译，不参与主项目生成。
