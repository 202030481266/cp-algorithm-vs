# SPFA、负环、差分约束与 Johnson 全源最短路

[模板源码](../../../graph/spfa.hpp) · [完整示例](../../../examples/graph/spfa.cpp) · [使用手册索引](../README.md)

边权可以为负时 Dijkstra 不再正确，改用队列优化的 Bellman-Ford（SPFA）。本模板同时提供负环判断、
差分约束系统求解，以及 Johnson 全源最短路（先用 SPFA 求势能把边权变成非负，再从每个点跑 Dijkstra）。
图的类型与 [shortest_path.hpp](shortest_path.md) 相同：`cp::WeightedGraph g(n); g[u].push_back({v, w});`。

## 接口和使用顺序

| 接口 | 含义 |
| --- | --- |
| `spfa(g, source)` | 单源最短路，optional<vector<long long>>；从 source 可达负环时为 nullopt，不可达为 INF64。 |
| `has_negative_cycle(g)` | 整张图是否存在负环（不要求从某点可达）。 |
| `DifferenceConstraints(n)` | 差分约束，变量 x[0..n)。 |
| `add_less_equal(u, v, w)` | 约束 `x[v] - x[u] <= w`；另有 `add_greater_equal`（>= w）与 `add_equal`（== w）。 |
| `solve()` | 无解返回 nullopt；有解时返回所有 x ≤ 0 的解中逐个最大的那一个。 |
| `johnson(g)` | 全源最短路 optional<vector<vector<long long>>>；有负环为 nullopt，不可达为 INF64。O(nm log m)。 |

## 完整例子

下面是可以直接编译的完整程序，数据写在代码里，**不需要输入**。在 workspace 根目录执行文末的 `python tools/cp.py example ...` 命令即可运行并核对输出。

```cpp
#include <iostream>
#include <vector>
#include "graph/spfa.hpp"

int main() {
    cp::WeightedGraph g(4);
    g[0].push_back({1, 4});
    g[0].push_back({2, 5});
    g[1].push_back({2, -3}); // 负边
    g[2].push_back({3, 2});
    auto dist = cp::spfa(g, 0);
    std::cout << "dist:";
    for (long long d : *dist) std::cout << ' ' << d;
    std::cout << '\n';

    g[3].push_back({1, -1}); // 1->2->3->1 的总权值为 -2，形成负环
    std::cout << std::boolalpha << "reachable_negative_cycle=" << !cp::spfa(g, 0).has_value()
              << " any_negative_cycle=" << cp::has_negative_cycle(g) << '\n';

    // 差分约束：x1 - x0 <= 3，x2 - x1 <= -2，x0 - x2 <= 1。
    cp::DifferenceConstraints dc(3);
    dc.add_less_equal(0, 1, 3);
    dc.add_less_equal(1, 2, -2);
    dc.add_less_equal(2, 0, 1);
    auto x = dc.solve();
    std::cout << "x:";
    for (long long v : *x) std::cout << ' ' << v;
    std::cout << '\n';

    // Johnson 全源最短路：允许负边，不允许负环。
    cp::WeightedGraph h(3);
    h[0].push_back({1, -2});
    h[1].push_back({2, 3});
    h[2].push_back({0, 1});
    auto all = cp::johnson(h);
    std::cout << "johnson[2][1]=" << (*all)[2][1] << " johnson[0][2]=" << (*all)[0][2] << '\n';
}
```

### 预期标准输出

```text
dist: 0 4 1 3
reachable_negative_cycle=true any_negative_cycle=true
x: -1 0 -2
johnson[2][1]=-1 johnson[0][2]=1
```

## 注意事项

- SPFA 最坏 O(nm)，可能被专门构造的数据卡掉；没有负边时一律用 Dijkstra。
- 负环判断依据“最短路的边数达到 n”，比统计入队次数更早发现负环。
- 差分约束的解加上同一个常数仍是解。需要“所有变量 ≥ 0 的最小解”时，令 y = -x：把约束 `x[v]-x[u] <= w` 改写成 `y[u]-y[v] <= w` 求出 y 后取反。
- 有限距离的绝对值必须明显小于 INF64（约 2.3e18），否则加法可能溢出。
- Johnson 需要 O(n²) 的结果数组；n ≤ 3000 左右时可用，稠密小图直接用 Floyd 更简单。

## 对应课程与练习

- 课程：[讲解065 A星、Floyd、Bellman-Ford与SPFA](https://github.com/algorithmzuo/algorithm-journey/tree/main/src/class065)、[讲解142 负环和差分约束](https://github.com/algorithmzuo/algorithm-journey/tree/main/src/class142)、[讲解207 Johnson全源最短路、传递闭包](https://github.com/algorithmzuo/algorithm-journey/tree/main/src/class207)
- 练习：
  - [P3385 负环](https://www.luogu.com.cn/problem/P3385)：`spfa` 是否返回 nullopt
  - [P5960 差分约束](https://www.luogu.com.cn/problem/P5960)：`DifferenceConstraints`
  - [P1993 小 K 的农场](https://www.luogu.com.cn/problem/P1993)：三种约束混合
  - [P5905 Johnson 全源最短路](https://www.luogu.com.cn/problem/P5905)：`johnson`

也可以在仓库根目录执行：

```powershell
python tools/cp.py example "graph/spfa"
```
