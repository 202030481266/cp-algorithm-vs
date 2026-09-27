# Kruskal 与稠密图 Prim 最小生成树 / 森林

[模板源码](../../../graph/mst.hpp) · [完整示例](../../../examples/graph/mst.cpp) · [使用手册索引](../README.md)

给出无向图，选择总权值最小的连接方案。边列表使用 Kruskal；稠密图或完全图使用朴素 Prim，逐轮扫描顶点，无需排序全部边。两个算法都返回 `MSTResult`；若原图不连通，返回各连通块的最小生成树组成的森林，必须检查 `connected`。

## 如何选择

| 图的表示 / 场景 | 接口 | 时间 | 额外空间 |
| --- | --- | --- | --- |
| 边列表，尤其是稀疏图 | `kruskal(n, edges)` | O(m log m) | O(n + m)，含边列表副本与结果 |
| 对称邻接矩阵，稠密图 | `prim_dense(matrix)` | O(n²) | O(n)，输入矩阵另占 O(n²) |
| 完全图，边权可由点对直接计算 | `prim_dense(n, weight)` | O(n²)，假设每次查询边权 O(1) | O(n)，无需存储边列表或矩阵 |

完全图有 n(n-1)/2 条边，Kruskal 的边排序需要 O(n² log n) 时间；按需计算边权的 Prim 同时省去了 O(n²) 的边存储。稀疏图仍适合使用边列表和 Kruskal。

## 接口和使用顺序

| 接口 | 含义 |
| --- | --- |
| `UndirectedEdge{u,v,weight}` | 一条无向边，端点 0-based，权值 long long。 |
| `kruskal(n, edges)` | O(m log m)，参数按值传入，调用者的边列表不会被排序。 |
| `prim_dense(matrix, no_edge=LLONG_MAX)` | 接收 `const vector<vector<long long>>&`，矩阵必须为对称的 n×n 方阵，不修改输入，对角线忽略。 |
| `prim_dense(n, weight, no_edge=LLONG_MAX)` | n≥0，`weight(u,v)` 返回对称的 `long long` 边权；仅查询不同顶点，每个无序点对查询一次，共 n(n-1)/2 次。 |
| `no_edge` | Prim 的缺边标记，默认 `std::numeric_limits<long long>::max()`；仅相等时表示缺边，真实边权不能与它相等。 |
| `result.connected` | 是否存在覆盖全部顶点的生成树；空图约定为 true。 |
| `result.weight / result.edges` | 选中边的总权值 / 边列表；不连通时是最小生成森林。 |

矩阵建图时先用 `no_edge` 填充，设置 `matrix[u][v] = matrix[v][u] = w`。有重边时保存其中最小的权值；若自定义的缺边标记小于合法边权，先判断原位置是否缺边，再取最小值。使用最短路模块的 `cp::INF64` 作为缺边标记时，须显式传入 `prim_dense(matrix, cp::INF64)`。

按需计算的边权函数应在调用期间保持图不变，并满足 `weight(u,v) == weight(v,u)`。完全图的不同顶点间直接返回实际边权；一般图可返回 `no_edge` 表示缺边。若单次查询耗时 T，总时间为 O(n²·(1+T))。空图和单点图不会调用边权函数。

## 完整例子

下面是可以直接编译的完整程序，数据写在代码里，**不需要输入**。在 workspace 根目录执行文末的 `python tools/cp.py example ...` 命令即可运行并核对输出。

```cpp
#include <iostream>
#include <cstdlib>
#include <limits>
#include <vector>
#include "graph/mst.hpp"

int main() {
    std::vector<cp::UndirectedEdge> edges{
        {0, 1, 4}, {0, 2, 1}, {1, 2, 2}, {1, 3, 1}, {2, 3, 5}
    };
    auto tree = cp::kruskal(4, edges);
    std::cout << std::boolalpha;
    std::cout << "connected=" << tree.connected << '\n';
    std::cout << "weight=" << tree.weight << " edges=" << tree.edges.size() << '\n';

    // 加入没有任何边的点 4，结果变成生成森林。
    auto forest = cp::kruskal(5, edges);
    std::cout << "forest_connected=" << forest.connected << '\n';
    std::cout << "forest_weight=" << forest.weight << '\n';

    // 稠密图：对称邻接矩阵，缺边使用指定标记。
    const long long no_edge = std::numeric_limits<long long>::max();
    std::vector<std::vector<long long>> matrix{
        {0, 4, 1, no_edge}, {4, 0, 2, 1},
        {1, 2, 0, 5}, {no_edge, 1, 5, 0}
    };
    auto dense_tree = cp::prim_dense(matrix);
    std::cout << "dense_connected=" << dense_tree.connected << '\n';
    std::cout << "dense_weight=" << dense_tree.weight
              << " edges=" << dense_tree.edges.size() << '\n';

    for (auto& row : matrix) row.push_back(no_edge);
    matrix.emplace_back(5, no_edge); // 新增孤立点，对角线会被忽略。
    auto dense_forest = cp::prim_dense(matrix);
    std::cout << "dense_forest_connected=" << dense_forest.connected << '\n';
    std::cout << "dense_forest_weight=" << dense_forest.weight << '\n';

    // 完全图：边权为直线上两点的距离，按需计算，无需保存所有边。
    std::vector<long long> x{0, 2, 5, 9};
    auto complete_tree = cp::prim_dense(int(x.size()), [&](int u, int v) {
        return std::abs(x[u] - x[v]);
    });
    std::cout << "complete_connected=" << complete_tree.connected << '\n';
    std::cout << "complete_weight=" << complete_tree.weight
              << " edges=" << complete_tree.edges.size() << '\n';
}
```

### 预期标准输出

```text
connected=true
weight=4 edges=3
forest_connected=false
forest_weight=4
dense_connected=true
dense_weight=4 edges=3
dense_forest_connected=false
dense_forest_weight=4
complete_connected=true
complete_weight=9 edges=3
```

## 注意事项

- Kruskal 的每条无向边只需传一次；Prim 的矩阵和边权函数须对称。
- 允许负边和零权边；Kruskal 直接接收重边，Prim 建图时合并重边。自环不会被选中。
- Prim 的默认缺边标记为 `LLONG_MAX`；如果合法边权包含此值，传入不会作为真实边权出现的其他标记。边权可以大于自定义标记。
- 连通且 n>0 时选中 n-1 条边；有 c 个连通块时选中 n-c 条边，Prim 会处理所有连通块。空图和单点图的 `connected=true`、权值为 0、边列表为空。
- 每次权值累加及最终总权值须在 `long long` 范围内，边权函数自身的计算也不能溢出。
- 有多棵最优生成树时，Prim 与 Kruskal 可能返回不同边集或顺序；返回边的端点顺序不保证 u<v。
- Kruskal 内部依赖 DSU；引用头文件或使用单文件导出时会自动包含它，手动复制时不要漏掉。

也可以在仓库根目录执行：

```powershell
python tools/cp.py example "graph/mst"
```
