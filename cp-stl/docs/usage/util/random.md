# 随机数据与可重现对拍

[模板源码](../../../util/random.hpp) · [完整示例](../../../examples/util/random.cpp) · [使用手册索引](../README.md)

写对拍生成器（gen）用：一个 `cp::Random` 对象依次生成数、数组、排列、字符串、树和图。
同一 seed、同一调用顺序得到的数据**在 MSVC、GCC 和不同电脑上完全相同**：模板只使用 mt19937_64 的原始输出（其序列由标准规定），
均匀分布和打乱都是自己实现的，不依赖各标准库实现不同的 `uniform_int_distribution` / `std::shuffle`。

## 接口和使用顺序

数值范围 `[lo,hi]` 两端都包含。树和图的点编号是 `0..n-1`，返回边表 `vector<pair<int,int>>`，边的顺序和每条边两个端点的先后都已随机打乱。

| 接口 | 含义 |
| --- | --- |
| `Random(seed) / Random()` | 指定 uint64_t 种子 / 使用时钟种子。底层生成器 `engine` 为 mt19937_64，可传给其他标准库组件。 |
| `integer(lo,hi)` | `[lo,hi]` 内均匀的 long long，任意 `lo≤hi`（包括整个 long long 范围）都无偏。 |
| `real(lo,hi)` | `[lo,hi)` 内均匀的 double。 |
| `chance(p)` | 以概率 p 返回 true。 |
| `choice(a)` | 等概率返回容器中的一个元素：`choice(vec)`、`choice(str)`、`choice({1, 2, 100})`。 |
| `array(n,lo,hi)` | n 个 `[lo,hi]` 内的数，可重复，返回 `vector<long long>`。 |
| `distinct_array(n,lo,hi)` | n 个互不相同的数，顺序随机；要求 `n ≤ hi-lo+1`，O(n log n)，与值域大小无关。 |
| `permutation(n, first=0)` | `first..first+n-1` 的随机排列，`vector<int>`；1-based 写 `permutation(n, 1)`。 |
| `partition(total, parts, minimum=0)` | 和为 total 的 parts 个数（有序），每个至少 minimum，所有拆法等概率。 |
| `interval(lo,hi)` | 满足 `lo≤l≤r≤hi` 的随机区间 `pair{l,r}`。 |
| `string(n, lo='a', hi='z')` | 每个字符在 `[lo,hi]` 内的随机串，例如 `string(n, 'a', 'c')`。 |
| `string(n, alphabet)` | 字符取自 alphabet 的随机串，例如 `string(n, "01")`、`string(n, ".#")`。 |
| `shuffle(a)` | 原地打乱 vector、string 等，所有排列等概率，O(n)。 |
| `tree(n)` | 等概率的 n 点有标号树，n-1 条边；深度约 √n。 |
| `parents(n)` | 父亲数组：`p[0]=-1`，其余 `p[i]<i`；适合输入格式为 `p_2..p_n` 且要求父亲编号更小的题。树形与 `tree(n)` 同分布。 |
| `path_tree(n) / star_tree(n)` | 链 / 菊花，点的顺序随机；用于卡深度、卡度数。 |
| `graph(n, m, connected=false)` | m 条边的简单无向图（无自环、无重边），要求 `m≤n(n-1)/2`；connected 时要求 `m≥n-1`。 |

## 对拍生成器怎么写

`python tools/cp.py stress` 每轮把 seed、seed+1、……作为命令行参数传给 gen。用这个参数构造 `cp::Random`，失败的 seed 就能重现同一份输入。
下面是“数组 + 区间询问”题的 gen；值域在极小值和大值之间随机选择，这样更容易造出全相等、重复值等边界情况。

```cpp
// data/gen.cpp
#include <iostream>
#include <string>
#include "util/random.hpp"

int main(int argc, char** argv) {
    if (argc != 2) return 1;
    cp::Random rnd(std::stoull(argv[1]));
    int n = int(rnd.integer(1, 8)), q = int(rnd.integer(1, 5));
    long long limit = rnd.choice({1LL, 3LL, 1000000000LL});
    std::cout << n << ' ' << q << '\n';
    for (long long x : rnd.array(n, -limit, limit)) std::cout << x << ' ';
    std::cout << '\n';
    for (int i = 0; i < q; ++i) {
        auto [l, r] = rnd.interval(1, n);
        std::cout << l << ' ' << r << '\n';
    }
}
```

其他常见输入格式：

```cpp
// 带边权的树，1-based
for (auto [u, v] : rnd.tree(n)) std::cout << u + 1 << ' ' << v + 1 << ' ' << rnd.integer(1, 10) << '\n';
// 多组数据，n 的总和恰好为 20、每组 n ≥ 1
auto sizes = rnd.partition(20, t, 1);
// 有向图：把无向边随机定向
auto edges = rnd.graph(n, m);
for (auto& [u, v] : edges) if (rnd.chance(0.5)) std::swap(u, v);
// DAG：按随机排列 rank 的先后定向，保证无环
auto rank = rnd.permutation(n);
for (auto& [u, v] : edges) if (rank[u] > rank[v]) std::swap(u, v);
// 网格
for (int i = 0; i < rows; ++i) std::cout << rnd.string(cols, ".#") << '\n';
```

## 完整例子

下面是可以直接编译的完整程序，数据写在代码里，**不需要输入**。在 workspace 根目录执行文末的 `python tools/cp.py example ...` 命令即可运行并核对输出。

```cpp
#include <iomanip>
#include <iostream>
#include <vector>
#include "util/random.hpp"

template<class T> void print(const char* name, const std::vector<T>& a) {
    std::cout << name << ':';
    for (const auto& x : a) std::cout << ' ' << x;
    std::cout << '\n';
}

int main() {
    // 对拍生成器中写 cp::Random rnd(std::stoull(argv[1]))；同一种子在 MSVC 和 GCC 上生成相同的数据。
    cp::Random rnd(42);
    int n = int(rnd.integer(5, 8));
    std::cout << "n: " << n << '\n';
    print("array", rnd.array(n, -9, 9));                // 可重复，范围 [-9,9]
    print("distinct", rnd.distinct_array(n, 1, 20));    // 互不相同
    print("permutation", rnd.permutation(n, 1));        // 1..n 的排列
    print("partition", rnd.partition(20, 4, 1));        // 4 个正整数，和为 20
    auto [l, r] = rnd.interval(1, n);                    // 1 <= l <= r <= n
    std::cout << "interval: " << l << ' ' << r << '\n';
    std::cout << "strings: " << rnd.string(n, 'a', 'c') << ' ' << rnd.string(n, "01") << '\n';
    std::cout << "choice: " << rnd.choice({1, 10, 100}) << '\n';
    std::cout << "real: " << std::fixed << std::setprecision(3) << rnd.real(0, 1) << '\n';
    std::cout << "chance: " << std::boolalpha << rnd.chance(0.5) << '\n';
    std::vector<int> cards{1, 2, 3, 4, 5};
    rnd.shuffle(cards);
    print("shuffle", cards);

    // 树和图的点编号是 0..n-1，下面按 1-based 输出。
    std::cout << "tree:";
    for (auto [u, v] : rnd.tree(n)) std::cout << ' ' << u + 1 << '-' << v + 1;
    std::cout << "\nparents:";
    auto parent = rnd.parents(n);
    for (int v = 1; v < n; ++v) std::cout << ' ' << parent[v] + 1; // p_2 .. p_n，均小于自身编号
    std::cout << "\ngraph:";
    for (auto [u, v] : rnd.graph(n, n + 1, true)) std::cout << ' ' << u + 1 << '-' << v + 1;
    std::cout << '\n';

    cp::Random replay(42);
    std::cout << "replay: " << (replay.integer(5, 8) == n) << '\n';
}
```

### 预期标准输出

```text
n: 7
array: -4 2 4 1 4 -3 -9
distinct: 10 19 5 2 12 1 8
permutation: 1 3 6 2 7 4 5
partition: 7 1 3 9
interval: 2 5
strings: ccbbbca 0110101
choice: 10
real: 0.416
chance: true
shuffle: 3 2 4 5 1
tree: 7-2 6-1 6-4 4-5 2-3 2-4
parents: 1 2 2 4 5 5
graph: 6-2 7-2 7-3 1-5 1-4 7-4 4-3 7-5
replay: true
```

## 注意事项

- lo、hi 表示数值范围，两端都包含，与数组查询的左闭右开区间不同。
- 要保证重现，调用顺序必须固定：不要把两次随机调用写进同一个函数调用的参数里，例如 `f(rnd.integer(1, 5), rnd.integer(1, 5))`，参数的求值顺序由编译器决定。`std::cout << a << b` 的链式输出和一条声明里的多个初始化 `int x = ..., y = ...;` 都按从左到右的顺序求值，可以放心写。
- 不要在循环里不断用同一个种子重建对象，那样会不断重复随机序列的开头。
- `graph(n, m, true)` 是随机树加随机的额外边，不保证所有连通图等概率；不带 connected 时，所有 m 条边的简单图等概率。需要重边或自环时直接用 `integer` 生成端点。
- `tree`、`parents` 期望 O(n log n)，`graph` 期望 O(m log m)；n、m 为 2×10⁵ 时每次调用约 0.1 秒以内，造大数据也够用。
- 发现反例后保存完整输入（stress 工具会自动保存），这样即使以后修改了生成器也能重现问题。

也可以在仓库根目录执行：

```powershell
python tools/cp.py example "util/random"
```
