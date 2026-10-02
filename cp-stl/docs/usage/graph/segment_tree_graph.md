# 线段树优化建图

[模板源码](../../../graph/segment_tree_graph.hpp) · [完整示例](../../../examples/graph/segment_tree_graph.cpp) · [使用手册索引](../README.md)

“点向一个区间的所有点连边”“区间内所有点向某点连边”如果逐条建边会有 O(n²) 条边。
线段树优化建图额外建两棵线段树：出树的边从父亲指向儿子，入树的边从儿子指向父亲（边权都为 0），
一条区间边只需连到 O(log n) 个线段树节点上。建好后直接对 `graph()` 跑最短路。

## 接口和使用顺序

| 接口 | 含义 |
| --- | --- |
| `SegmentTreeGraph(n)` | 原图点 0..n-1，内部共 3n−2 个点。 |
| `add_edge(u, v, w)` | 普通边 u → v。 |
| `add_edge_to_range(u, l, r, w)` | u → [l,r) 中每个点。 |
| `add_edge_from_range(l, r, v, w)` | [l,r) 中每个点 → v。 |
| `add_range_to_range(l1, r1, l2, r2, w)` | [l1,r1) 中每个点 → [l2,r2) 中每个点，新建一个虚点。 |
| `graph() / node_count()` | 建好的 `WeightedGraph`；前 n 个点就是原图的点。 |

## 完整例子

下面是可以直接编译的完整程序，数据写在代码里，**不需要输入**。在 workspace 根目录执行文末的 `python tools/cp.py example ...` 命令即可运行并核对输出。

```cpp
#include <iostream>
#include "graph/segment_tree_graph.hpp"

int main() {
    cp::SegmentTreeGraph stg(5);
    stg.add_edge_to_range(0, 1, 3, 5);         // 0 -> {1,2}，花费 5
    stg.add_edge(0, 3, 20);                    // 0 -> 3，花费 20
    stg.add_edge_from_range(1, 3, 4, 2);       // {1,2} -> 4，花费 2
    stg.add_range_to_range(4, 5, 3, 4, 1);     // {4} -> {3}，花费 1
    auto dist = cp::dijkstra(stg.graph(), 0);  // 前 5 个就是原图点的距离
    std::cout << "dist:";
    for (int v = 0; v < stg.size(); ++v) std::cout << ' ' << dist[v];
    std::cout << "\nnodes=" << stg.node_count() << '\n';
}
```

### 预期标准输出

```text
dist: 0 5 5 8 7
nodes=14
```

## 注意事项

- 每条区间边 O(log n) 条实际边；最短路在 O(n + m log n) 个点和边上运行。
- 边权为 0/1 时可以改用 `zero_one_bfs(graph(), s)`。
- 同样的思想也用于 2-SAT 的前缀优化建图、树链剖分优化建图（把区间换成 DFS 序上的链）。
- graph() 返回常量引用；继续加边会改变它，之后要重新求最短路。

## 对应课程与练习

- 课程：[讲解195 优化建图的技巧和题目](https://github.com/algorithmzuo/algorithm-journey/tree/main/src/class195)、[讲解196 2-SAT与前缀优化建图](https://github.com/algorithmzuo/algorithm-journey/tree/main/src/class196)、[讲解198 树状数组、主席树、CDQ优化建图](https://github.com/algorithmzuo/algorithm-journey/tree/main/src/class198)
- 练习：
  - [CF786B Legacy](https://www.luogu.com.cn/problem/CF786B)：例子中的四种边
  - [P6348 Journeys](https://www.luogu.com.cn/problem/P6348)：区间到区间：`add_range_to_range`
  - [P5025 炸弹](https://www.luogu.com.cn/problem/P5025)：建图后缩点

也可以在仓库根目录执行：

```powershell
python tools/cp.py example "graph/segment_tree_graph"
```
