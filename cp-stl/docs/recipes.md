# 常用写法与进阶入口

需要从头照着运行时，先看 [逐个模板的使用手册](usage/README.md) 和 [全部完整示例](../examples/README.md)。

以下是组合用法片段，假设使用 workspace 的 [templates/main.cpp](../../templates/main.cpp) 并按需添加 <climits> 等标准头文件，已包含对应算法头文件、声明题目变量，并把操作语句放进 `solve()`。数组从 0 开始，数组与字符串区间左闭右开。

## 树状数组与线段树怎么选

- 单点加、区间求和：`Fenwick<long long>`。
- 单点赋值、区间最值或自定义合并：`SegmentTree<T,Op>`。
- 区间加、区间和：`RangeAddSum`。
- 数组不变，查询 min/max/gcd/and/or：`SparseTable`。求和不能直接用重叠 ST 查询。

```cpp
#include "data_structures/segment_tree.hpp"
#include "data_structures/sparse_table.hpp"
#include "data_structures/lazy_segment_tree.hpp"

std::vector<long long> a{1, 2, 3};
cp::SegmentTree<long long, std::plus<long long>> sum(a, 0);
cp::SegmentTree<long long, cp::Min> minimum(a, LLONG_MAX);
sum.set(1, 10);          // 单点赋值
auto s = sum.prod(0, 3); // 14
auto r = sum.max_right(0, [](long long x) { return x <= 11; }); // 2
cp::RangeAddSum lazy(a);
lazy.add(0, 2, 5);
auto t = lazy.sum(0, 3); // 16
```

`max_right` 的谓词必须随区间扩展单调，且对单位元为真。例子中的“和不超过上限”仅在数组非负时满足这个条件。

Fenwick 的 `lower_bound(k)` 返回第 k 个元素所在的 **0-based 下标**，要求频次数组非负；k 超过总频次时返回 n。k<=0 返回 0。

## 最短路与建图

```cpp
#include "graph/shortest_path.hpp"

cp::WeightedGraph g(n);
g[u].push_back({v, w});
g[v].push_back({u, w}); // 只有无向图才加这句
auto dist = cp::dijkstra(g, 0);
// dist[v] == cp::INF64 表示不可达；Dijkstra 要求非负边权。
// 只有 0/1 边权时可替换成 cp::zero_one_bfs(g, 0)。
```

无权图用 `vector<vector<int>>` 和 `cp::bfs(g, source)`，不可达标记为 -1。Floyd 允许负边但不允许负环；矩阵对角线先置 0，其他置 INF64，重边取最小值。

## 树链剖分接区间结构

```cpp
#include "graph/hld.hpp"
#include "data_structures/lazy_segment_tree.hpp"

cp::HLD hld(g, 0);
std::vector<long long> linear(n);
for (int u = 0; u < n; ++u) linear[hld.pos[u]] = value[u];
cp::RangeAddSum seg(linear);

hld.for_each_path(u, v, [&](int l, int r) { seg.add(l, r, delta); });
long long path_sum = 0;
hld.for_each_path(u, v, [&](int l, int r) { path_sum += seg.sum(l, r); });
auto [l, r] = hld.subtree(u);
long long subtree_sum = seg.sum(l, r);
```

这里维护**点权**，LCA 只计一次。路径回调不保证方向，适用于求和、min/max 等可交换运算；字符串拼接、矩阵连乘、边权问题需要按题意修改。

## 线性基和 01 Trie 的区别

- `XorBasis`：可以任选多个数异或，查询所有子集的异或结果。
- `BinaryTrie`：从集合选一个元素 y，使 x^y 最大；支持删除。

```cpp
#include "data_structures/xor_basis.hpp"
cp::XorBasis basis;
basis.insert(3); basis.insert(5);
// 去重后的结果为 0, 3, 5, 6。空子集的 0 总是存在。
auto maximum = basis.max_xor(); // 6
auto second = basis.kth(1);     // optional，值为 3；k 从 0 开始
auto missing = basis.kth(4);    // nullopt
```

如果题目要求非空子集，需要额外记录是否存在非空子集异或为 0；不能直接把上述顺序当作“非空子集第 k 小”。

## 模整数、组合数与矩阵

```cpp
#include "math/combinatorics.hpp"
#include "math/matrix.hpp"

using M = cp::ModInt<1000000007>;
cp::Combinations<M> comb(200000);
M ways = comb.choose(n, k);
M probability = M(3) / M(7); // 分母必须与模数互质

cp::Matrix<M> fib(2);
fib[0][0] = fib[0][1] = fib[1][0] = 1;
M fn = fib.pow(n)[0][1];
```

组合数预处理要求质数模数且预处理上界小于模数。n 更大时不能直接扩阶乘表，需要 Lucas 或其他数论方法。`PrimeSieve(n).factorize(x)` 只接受 x<=n；不能用小筛直接分解任意 64 位数。

## DP、前缀和与常见循环

背包 `dp[c]` 默认表示容量**不超过** c 的最大价值，初始化为 0。01 背包倒序，完全背包正序；恰好装满时应使用不可达标记，跳过不可达转移。

普通前缀和（下标约定和模板相同）：

```cpp
std::vector<long long> pref(n + 1);
for (int i = 0; i < n; ++i) pref[i + 1] = pref[i] + a[i];
auto sum = pref[r] - pref[l];
```

子集枚举（mask 非负，且能放进所用无符号类型）：

```cpp
for (unsigned sub = mask; ; sub = (sub - 1) & mask) {
    // 使用 sub；包含空集 0。
    if (sub == 0) break;
}
```

SOS DP：令数组长度为 2^k，经过以下循环，f[mask] 为原数组所有 sub⊆mask 的值之和。

```cpp
for (int bit = 0; bit < k; ++bit)
    for (int mask = 0; mask < (1 << k); ++mask)
        if (mask >> bit & 1) f[mask] += f[mask ^ (1 << bit)];
```

数位 DP 通常需要 `pos/state/tight/started`；只记忆化不受上界约束的状态，并先定义前导零和数字 0 是否计入。状态依题意变化，不把某一道题的状态硬编码成通用模板。

## 二分图匹配与 2-SAT

二分图匹配可用 Dinic：源点→左部、左部→右部、右部→汇点的容量都设为 1，最大流就是最大匹配数。左、右部点编号不要重叠。需要更专门的复杂度时再使用 Hopcroft–Karp。

```cpp
#include "graph/two_sat.hpp"
cp::TwoSAT sat(n);
sat.add_clause(0, true, 1, false); // x0 OR (!x1)
sat.add_clause(0, true, 0, true);  // 强制 x0 = true
auto answer = sat.solve();        // nullopt 表示无解
```

## 调试、随机与图输出

```cpp
#include "util/debug.hpp"
#include "util/random.hpp"
#include "util/graphviz.hpp"

cp::Random random(42);            // 固定种子便于重现
auto a = random.array(10, -5, 5);
debug(a);                         // LOCAL 下输出到 stderr
std::string dot = graphviz::to_dot(3, {{0, 1, 7}, {1, 2, {}}});
```

Graphviz 工具只输出 DOT，不要求安装 Graphviz；需要渲染时再用 DOT 查看器。默认点编号是 0..n-1；兼容旧的 1-based 数据时设置 `graphviz::Options opt; opt.index_base = 1;`。

## 更复杂的算法：使用现有 ACL

本 workspace 保留原 VS 配置中的外部 ACL 路径；cp-stl 的 35 个头文件本身不依赖 ACL。

需要 ACL 时，VS 按已有包含目录查找。命令行运行和导出须显式增加其目录，例如：

```powershell
python tools/cp.py run -I D:/CP-Templates-and-Solutions/ac-library
python tools/cp.py export -I D:/CP-Templates-and-Solutions/ac-library
```

可按需使用 atcoder/lazysegtree、mincostflow、string、convolution、math；前置条件及用法参见本机 ACL 自带文档。
