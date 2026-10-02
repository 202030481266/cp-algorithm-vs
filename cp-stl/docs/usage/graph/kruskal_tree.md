# Kruskal 重构树

[模板源码](../../../graph/kruskal_tree.hpp) · [完整示例](../../../examples/graph/kruskal_tree.cpp) · [使用手册索引](../README.md)

按 Kruskal 的顺序加边，每次合并两个连通块时新建一个点作为它们的父亲，点权为这条边的边权。
得到的树（森林）有两个关键性质：

- 两点 LCA 的点权 = 两点间所有路径中“最大边权”的最小值（最小瓶颈路）。
- 从点 u 往上，点权单调；“点权 ≤ limit 的最高祖先”的子树叶子，就是只走边权 ≤ limit 的边从 u 能到达的点。

后者把“限制边权的可达性”变成子树问题，配合 DFS 序上的数据结构（如可持久化线段树）可以在线回答。

## 接口和使用顺序

| 接口 | 含义 |
| --- | --- |
| `KruskalTree(n, edges, better=less)` | edges 为 vector<UndirectedEdge>；`std::greater<long long>()` 时按边权从大到小。 |
| `bottleneck(u, v)` | 最小瓶颈值（better=greater 时为“最大化最小边”）；不连通或 u == v 为 nullopt。 |
| `highest(u, pred)` | 从 u 往上满足 `pred(点权)` 的最高祖先，pred 须沿祖先链由真变假。 |
| `leaf_order / leaf_begin / leaf_end` | 叶子的 DFS 序；节点 x 子树中的原图点是 `leaf_order[leaf_begin[x], leaf_end[x])`。 |
| `parent / weight / children / depth / lca(u, v)` | 重构树本身：点数 `size()`，根的 parent 为 -1，不连通时 lca 为 -1。 |

## 完整例子

下面是可以直接编译的完整程序，数据写在代码里，**不需要输入**。在 workspace 根目录执行文末的 `python tools/cp.py example ...` 命令即可运行并核对输出。

```cpp
#include <functional>
#include <iostream>
#include <vector>
#include "graph/kruskal_tree.hpp"

int main() {
    std::vector<cp::UndirectedEdge> edges{{0, 1, 5}, {1, 2, 3}, {2, 3, 4}, {0, 3, 1}, {1, 3, 6}};
    cp::KruskalTree tree(4, edges); // 边权从小到大：最小瓶颈

    std::cout << "bottleneck(0,3)=" << *tree.bottleneck(0, 3) << " bottleneck(0,1)=" << *tree.bottleneck(0, 1)
              << " bottleneck(1,2)=" << *tree.bottleneck(1, 2) << '\n';

    // 从 0 出发只走边权 <= 3 的边能到达哪些点：最高的“点权 <= 3”的祖先的子树叶子。
    int top = tree.highest(0, [](long long w) { return w <= 3; });
    std::cout << "reach from 0 with w<=3:";
    for (int i = tree.leaf_begin[top]; i < tree.leaf_end[top]; ++i) std::cout << ' ' << tree.leaf_order[i];
    std::cout << '\n';

    cp::KruskalTree widest(4, edges, std::greater<long long>()); // 边权从大到小：最大化路径上的最小边
    std::cout << "widest(0,2)=" << *widest.bottleneck(0, 2) << " nodes=" << widest.size() << '\n';
}
```

### 预期标准输出

```text
bottleneck(0,3)=1 bottleneck(0,1)=4 bottleneck(1,2)=3
reach from 0 with w<=3: 0 3
widest(0,2)=4 nodes=7
```

## 注意事项

- 图不连通时得到森林；新点数 = n − 连通块数，总点数不超过 2n−1。
- 倍增表占 (2n) × ⌈log₂(2n)⌉ 个 int；n = 5×10⁵ 时约 80MB，注意内存限制。
- 边权相同的边按输入顺序加入（稳定排序），不影响答案。
- 边权越大越好走时（如 P4768 归程：只能走海拔高于水位线 p 的边）用 `std::greater<long long>()` 建树，pred 返回 `w > p`（w 为点权）。

## 对应课程与练习

- 课程：[讲解164 Kruskal重构树的原理和相关题目](https://github.com/algorithmzuo/algorithm-journey/tree/main/src/class164)
- 练习：
  - [P2245 星际导航](https://www.luogu.com.cn/problem/P2245)：`bottleneck`
  - [P1967 货车运输](https://www.luogu.com.cn/problem/P1967)：最大化最小边：`std::greater`
  - [P4768 归程](https://www.luogu.com.cn/problem/P4768)：`highest` + 子树最小值
  - [P7834 Peaks 加强版](https://www.luogu.com.cn/problem/P7834)：`highest` + leaf_order 上的主席树求第 k 大

也可以在仓库根目录执行：

```powershell
python tools/cp.py example "graph/kruskal_tree"
```
