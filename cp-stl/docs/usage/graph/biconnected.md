# 割点、桥、点双、边双与圆方树

[模板源码](../../../graph/biconnected.hpp) · [完整示例](../../../examples/graph/biconnected.cpp) · [使用手册索引](../README.md)

无向图的连通性分析，一次非递归 Tarjan 同时求出：割点、桥（割边）、点双连通分量（块）、
边双连通分量，以及由点双构造的圆方树。允许重边和自环：重边不是桥，自环不影响结果。

## 接口和使用顺序

| 接口 | 含义 |
| --- | --- |
| `Biconnected(n, edges)` | edges 为 vector<pair<int,int>>，边编号就是下标，O(n + m)。 |
| `is_cut[v] / cut_vertices()` | 割点标记 / 升序割点列表。 |
| `is_bridge[id] / bridges()` | 桥标记 / 升序的桥的边编号。 |
| `two_edge_id[v] / two_edge_count` | 边双连通分量编号 / 个数；`two_edge_components()` 返回每个分量的点集。 |
| `blocks` | 点双连通分量，每个是一个点集；割点出现在多个块中，孤立点自成一块。 |
| `block_cut_tree()` | 圆方树邻接表：点 0..n-1 是原图点，n+i 是第 i 个块。 |

## 完整例子

下面是可以直接编译的完整程序，数据写在代码里，**不需要输入**。在 workspace 根目录执行文末的 `python tools/cp.py example ...` 命令即可运行并核对输出。

```cpp
#include <algorithm>
#include <iostream>
#include <utility>
#include <vector>
#include "graph/biconnected.hpp"

int main() {
    // 两个三角形 {0,1,2}、{3,4,5} 由桥 2-3 相连，5-6 也是桥，点 7 孤立。
    std::vector<std::pair<int, int>> edges{{0, 1}, {1, 2}, {2, 0}, {2, 3}, {3, 4}, {4, 5}, {5, 3}, {5, 6}};
    cp::Biconnected bc(8, edges);

    std::cout << "cut vertices:";
    for (int v : bc.cut_vertices()) std::cout << ' ' << v;
    std::cout << "\nbridges:";
    for (int id : bc.bridges()) std::cout << ' ' << edges[id].first << '-' << edges[id].second;
    std::cout << "\n2-edge components=" << bc.two_edge_count << ", id:";
    for (int id : bc.two_edge_id) std::cout << ' ' << id;
    std::cout << '\n';

    auto blocks = bc.blocks;
    for (auto& block : blocks) std::sort(block.begin(), block.end());
    std::sort(blocks.begin(), blocks.end());
    std::cout << "blocks:";
    for (auto& block : blocks) {
        std::cout << " {";
        for (int i = 0; i < int(block.size()); ++i) std::cout << (i ? "," : "") << block[i];
        std::cout << '}';
    }
    std::cout << "\nblock-cut tree nodes=" << bc.block_cut_tree().size() << '\n';
}
```

### 预期标准输出

```text
cut vertices: 2 3 5
bridges: 2-3 5-6
2-edge components=4, id: 0 0 0 1 1 1 2 3
blocks: {0,1,2} {2,3} {3,4,5} {5,6} {7}
block-cut tree nodes=13
```

## 注意事项

- 边双 = 删去所有桥后的连通块；把边双缩成点得到一棵树（桥树），常用于“加几条边使图无桥”等问题。
- 点双中两点之间至少有两条点不相交的路径；只有两个点、一条边的块对应一座桥。
- 圆方树中，原图两点之间的所有简单路径必经的点 = 圆方树路径上的圆点；配合 LCA、树上差分处理路径问题。
- 实现使用显式栈，链长 10⁶ 也不会爆栈；块按发现顺序存储，需要固定输出顺序时自行排序。

## 对应课程与练习

- 课程：[讲解191 割边、边双连通分量和缩点](https://github.com/algorithmzuo/algorithm-journey/tree/main/src/class191)、[讲解192 边双连通分量缩点的更多题目](https://github.com/algorithmzuo/algorithm-journey/tree/main/src/class192)、[讲解193 割点和点双连通分量](https://github.com/algorithmzuo/algorithm-journey/tree/main/src/class193)、[讲解194 圆方树的原理和相关题目](https://github.com/algorithmzuo/algorithm-journey/tree/main/src/class194)
- 练习：
  - [P3388 割点（割顶）](https://www.luogu.com.cn/problem/P3388)：`cut_vertices()`
  - [P8436 边双连通分量](https://www.luogu.com.cn/problem/P8436)：`two_edge_components()`
  - [P8435 点双连通分量](https://www.luogu.com.cn/problem/P8435)：`blocks`，含孤立点
  - [LeetCode 1192 查找集群内的关键连接](https://leetcode.cn/problems/critical-connections-in-a-network/)：`bridges()`
  - [P4630 铁人两项](https://www.luogu.com.cn/problem/P4630)：圆方树计数

也可以在仓库根目录执行：

```powershell
python tools/cp.py example "graph/biconnected"
```
