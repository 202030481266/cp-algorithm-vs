# O(1) LCA：DFS 序 + ST 表

[模板源码](../../../graph/fast_lca.hpp) · [完整示例](../../../examples/graph/fast_lca.cpp) · [使用手册索引](../README.md)

与倍增 [LCA](lca.md) 相比，`FastLCA` 预处理 O(n log n)，每次查询 O(1)，查询很多时明显更快。
原理是“DFS 序求 LCA”：设 tin[u] < tin[v] 且 u ≠ v，则 DFS 序区间 (tin[u], tin[v]] 内所有点的父亲中，
DFS 序最小的那个就是 LCA，用 ST 表求区间最小值即可。它同时给出 DFS 序，子树对应连续区间。

## 接口和使用顺序

| 接口 | 含义 |
| --- | --- |
| `FastLCA(g, root=0)` | 无向树的邻接表，非递归 DFS。 |
| `lca(u, v) / distance(u, v)` | 最近公共祖先 / 两点间边数，O(1)。 |
| `is_ancestor(u, v)` | u 是否是 v 的祖先（u == v 也算），O(1)。 |
| `parent / depth` | 父亲（根为 -1）/ 深度。 |
| `tin / tout / order` | 子树 u 是 DFS 序区间 `[tin[u], tout[u])`，`order[tin[u]] = u`。 |

## 完整例子

下面是可以直接编译的完整程序，数据写在代码里，**不需要输入**。在 workspace 根目录执行文末的 `python tools/cp.py example ...` 命令即可运行并核对输出。

```cpp
#include <iostream>
#include <vector>
#include "graph/fast_lca.hpp"

int main() {
    // 边：0-1, 0-2, 1-3, 1-4, 2-5，根为 0。
    std::vector<std::vector<int>> g{{1, 2}, {0, 3, 4}, {0, 5}, {1}, {1}, {2}};
    cp::FastLCA tree(g, 0);
    std::cout << std::boolalpha;
    std::cout << "lca(3,4)=" << tree.lca(3, 4) << " lca(3,5)=" << tree.lca(3, 5)
              << " distance(4,5)=" << tree.distance(4, 5) << '\n';
    std::cout << "is_ancestor(1,4)=" << tree.is_ancestor(1, 4) << " is_ancestor(2,4)=" << tree.is_ancestor(2, 4) << '\n';
    std::cout << "subtree(1):";
    for (int i = tree.tin[1]; i < tree.tout[1]; ++i) std::cout << ' ' << tree.order[i];
    std::cout << '\n';
}
```

### 预期标准输出

```text
lca(3,4)=1 lca(3,5)=0 distance(4,5)=4
is_ancestor(1,4)=true is_ancestor(2,4)=false
subtree(1): 1 3 4
```

## 注意事项

- 只处理 root 所在的连通块；森林可以加一个虚拟根连接各棵树的根。
- ST 表占 n × ⌈log₂n⌉ 个 int；n = 5×10⁵ 时约 40MB。
- 需要第 k 个祖先时仍用倍增 [LCA](lca.md)；带边权的距离可另开 dist 数组，`dist[u] + dist[v] - 2*dist[lca]`。
- [虚树](virtual_tree.md) 直接使用本模板。

## 对应课程与练习

- 课程：[讲解118 树上倍增和LCA-上](https://github.com/algorithmzuo/algorithm-journey/tree/main/src/class118)、[讲解186 欧拉序求LCA、dfn序求LCA](https://github.com/algorithmzuo/algorithm-journey/tree/main/src/class186)
- 练习：
  - [P3379 最近公共祖先（LCA）](https://www.luogu.com.cn/problem/P3379)：5×10⁵ 次查询
  - [P3128 Max Flow P](https://www.luogu.com.cn/problem/P3128)：树上点差分：`cnt[u]++, cnt[v]++, cnt[lca]--, cnt[parent[lca]]--`

也可以在仓库根目录执行：

```powershell
python tools/cp.py example "graph/fast_lca"
```
