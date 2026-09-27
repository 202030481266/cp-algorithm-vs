# Kruskal 最小生成树与 Prim 最小 / 最大生成树

[模板源码](../../../graph/mst.hpp) · [完整示例](../../../examples/graph/mst.cpp) · [使用手册索引](../README.md)

给出无向图，选择总权值最小的连接方案。边列表使用 Kruskal；稠密图或完全图使用朴素 Prim，逐轮扫描顶点，无需排序全部边。Prim 还支持通过比较器选择最大生成树。两个算法都返回 `MSTResult`；若原图不连通，返回各连通块的最优生成树组成的森林，必须检查 `connected`。

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
| `prim_dense(matrix, no_edge=LLONG_MAX, better={})` | 接收 `const vector<vector<T>>&`，支持 `int`、`long long` 等取值范围能放进 `long long` 的整数类型。矩阵须为对称的 n×n 方阵，不修改输入，对角线忽略。 |
| `prim_dense(n, weight, no_edge=LLONG_MAX, better={})` | n≥0，`weight(u,v)` 返回对称的 `long long` 边权；仅查询不同顶点，每个无序点对查询一次，共 n(n-1)/2 次。 |
| `no_edge` | Prim 的缺边标记，默认 `std::numeric_limits<long long>::max()`；仅相等时表示缺边，真实边权不能与它相等。 |
| `better` | 默认 `std::less<long long>{}` 求最小生成树；传 `std::greater<long long>{}` 求最大生成树。比较器同时用于选点和更新候选边，缺边标记不随比较器改变。 |
| `result.connected` | 是否存在覆盖全部顶点的生成树；空图约定为 true。 |
| `result.weight / result.edges` | 选中边的总权值 / 边列表，均保留原始边权并使用 `long long`；不连通时是对应的最小 / 最大生成森林。 |

矩阵应按实际顶点数创建，如 `vector<vector<int>> matrix(n, vector<int>(n, -1))`，其中 -1 只适用于合法边权均非负的情况，并在调用时显式传入 `no_edge=-1`。算法把矩阵大小当成顶点数，因此不要用额外的 `MAXN` 行列代替实际大小。整数矩阵的默认缺边标记仍为 `LLONG_MAX`，不会自动改为 `INT_MAX`。

建图时设置 `matrix[u][v] = matrix[v][u] = w`。有重边时，最小生成树保存最小权值，最大生成树保存最大权值；先判断原位置是否缺边，再按比较器取最优值。使用最短路模块的 `cp::INF64` 作为缺边标记时，须显式传入 `prim_dense(matrix, cp::INF64)`。

按需计算的边权函数应在调用期间保持图不变，并满足 `weight(u,v) == weight(v,u)`。完全图的不同顶点间直接返回实际边权；一般图可返回 `no_edge` 表示缺边。若单次查询耗时 T，总时间为 O(n²·(1+T))。空图和单点图不会调用边权函数。

## 最大生成树与边权计算次数

对于列位集 `col`，以共同为 1 的位数作为边权，可以直接求最大生成树：

```cpp
auto tree = cp::prim_dense(n, [&](int u, int v) -> long long {
    return (col[u] & col[v]).count();
}, std::numeric_limits<long long>::max(), std::greater<long long>{});
// tree.weight 已是最大总权值，tree.edges 保存原始边权。
```

`no_edge` 只表示缺边；把它改成 `INT_MIN` 不会自动求最大生成树。使用比较器无需对边权取负，也适用于包含 `LLONG_MIN` 的合法边权。

每轮选入 u 后，只查询尚未入树的 v；当 v 后续入树时，u 已被跳过。因此每个无序点对恰好查询一次，共 (n-1)+(n-2)+…+1=n(n-1)/2 次，与预先建立上三角矩阵的计算次数相同。

按需计算省去矩阵的存储和读写；若同一图需要反复求解，缓存矩阵可避免重复计算边权。单次运行的实际速度仍取决于编译器、缓存和边权函数，不能仅凭复杂度保证最快。`bitset<B>` 的与运算和计数通常需要遍历约 ⌈B/W⌉ 个机器字，因此该场景的总时间约为 O(n²⌈B/W⌉)，W 为机器字的位数；固定长度位集的成本由 B 决定。

## 完整例子

下面是可以直接编译的完整程序，数据写在代码里，**不需要输入**。在 workspace 根目录执行文末的 `python tools/cp.py example ...` 命令即可运行并核对输出。

```cpp
#include <bitset>
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

    // 最大生成树：比较器同时控制选点和更新，返回的边权仍是原始正权。
    // int 邻接矩阵也可直接传入；这里用 -1 表示缺边。
    std::vector<std::vector<int>> int_matrix{
        {0, 4, 1, -1}, {4, 0, 2, 1}, {1, 2, 0, 5}, {-1, 1, 5, 0}
    };
    auto max_tree = cp::prim_dense(int_matrix, -1, std::greater<long long>{});
    std::cout << "max_weight=" << max_tree.weight
              << " edges=" << max_tree.edges.size() << '\n';

    // 完全图：边权为两列共同为 1 的位数，每个无序点对仅计算一次。
    std::vector<std::bitset<4>> columns{0b1111, 0b0111, 0b0011, 0b0001};
    int queries = 0;
    auto max_complete = cp::prim_dense(int(columns.size()), [&](int u, int v) -> long long {
        ++queries;
        return (columns[u] & columns[v]).count();
    }, no_edge, std::greater<long long>{});
    std::cout << "max_complete_weight=" << max_complete.weight
              << " queries=" << queries << '\n';
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
max_weight=11 edges=3
max_complete_weight=6 queries=6
```

## 注意事项

- Kruskal 的每条无向边只需传一次；Prim 的矩阵和边权函数须对称。
- 允许负边和零权边；Kruskal 直接接收重边，Prim 建图时合并重边。自环不会被选中。
- Prim 的默认缺边标记为 `LLONG_MAX`；如果合法边权包含此值，传入不会作为真实边权出现的其他标记。边权可以大于自定义标记。
- 连通且 n>0 时选中 n-1 条边；有 c 个连通块时选中 n-c 条边，Prim 会处理所有连通块。空图和单点图的 `connected=true`、权值为 0、边列表为空。
- 每次权值累加及最终总权值须在 `long long` 范围内，边权函数自身的计算也不能溢出。
- 有多棵最优生成树时，最小 Prim 与 Kruskal 可能返回不同边集或顺序；返回边的端点顺序不保证 u<v。
- Kruskal 内部依赖 DSU；引用头文件或使用单文件导出时会自动包含它，手动复制时不要漏掉。

也可以在仓库根目录执行：

```powershell
python tools/cp.py example "graph/mst"
```
