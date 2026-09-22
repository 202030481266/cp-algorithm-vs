# BFS、01 BFS、Dijkstra 与 Floyd

[模板源码](../../../graph/shortest_path.hpp) · [完整示例](../../../examples/graph/shortest_path.cpp) · [使用手册索引](../README.md)

先按边权与查询类型选算法：无权单源用 BFS；边权只有 0/1 用 01 BFS；非负权单源用 Dijkstra；小规模图的全源最短路用 Floyd。图中顶点编号为 0..n-1。

## 接口和使用顺序

| 接口 | 含义 |
| --- | --- |
| `bfs(g, source)` | g 为 vector<vector<int>>；返回边数距离 vector<int>，不可达为 -1，O(n+m)。 |
| `WeightedGraph / Edge{to, weight}` | 带权邻接表 vector<vector<Edge>>，每条边存终点与 long long 权值。 |
| `zero_one_bfs(g, source)` | 0/1 边权；返回 vector<long long>，不可达为 INF64，O(n+m)。 |
| `dijkstra(g, source)` | 非负权；返回 vector<long long>，不可达为 INF64，O((n+m) log n)。 |
| `floyd_warshall(dist)` | 原地修改 n×n 距离矩阵，O(n³)；可有负边但不可有负环。 |

## 完整例子

下面是可以直接编译的完整程序，数据写在代码里，**不需要输入**。在 workspace 根目录执行文末的 `python tools/cp.py example ...` 命令即可运行并核对输出。

```cpp
#include <algorithm>
#include <iostream>
#include <vector>
#include "graph/shortest_path.hpp"

int main() {
    std::vector<std::vector<int>> unweighted{{1, 2}, {3}, {1, 3}, {}, {}};
    std::cout << "bfs:";
    for (int d : cp::bfs(unweighted, 0)) std::cout << ' ' << d;
    std::cout << '\n';

    cp::WeightedGraph g(5);
    g[0] = {{1, 4}, {2, 1}};
    g[1] = {{3, 1}};
    g[2] = {{1, 2}, {3, 5}};
    std::cout << "dijkstra:";
    for (auto d : cp::dijkstra(g, 0)) {
        if (d == cp::INF64) std::cout << " INF";
        else std::cout << ' ' << d;
    }
    std::cout << '\n';

    cp::WeightedGraph binary(4);
    binary[0] = {{1, 1}, {2, 0}};
    binary[2] = {{1, 0}, {3, 1}};
    binary[1] = {{3, 1}};
    std::cout << "01bfs:";
    for (auto d : cp::zero_one_bfs(binary, 0)) std::cout << ' ' << d;
    std::cout << '\n';

    std::vector<std::vector<long long>> dist(5, std::vector<long long>(5, cp::INF64));
    for (int u = 0; u < 5; ++u) {
        dist[u][u] = 0;
        for (auto [v, w] : g[u]) dist[u][v] = std::min(dist[u][v], w);
    }
    cp::floyd_warshall(dist);
    std::cout << "floyd(0,3)=" << dist[0][3] << '\n';
}
```

### 预期标准输出

```text
bfs: 0 1 1 2 -1
dijkstra: 0 3 1 4 INF
01bfs: 0 0 0 1
floyd(0,3)=4
```

## 注意事项

- 示例是有向图；无向边需要在邻接表中插入两个方向。
- Floyd 的矩阵先设 INF64，对角线设 0，重边取 min。
- Dijkstra/01 BFS 不可把 INF64 当普通距离继续做算术；先判断是否可达。有限距离要小于 INF64。
- 这些接口只返回距离；需要输出路径时另外维护前驱。
- 源点必须存在；不能在空图上调用单源算法。

也可以在仓库根目录执行：

```powershell
python tools/cp.py example "graph/shortest_path"
```
