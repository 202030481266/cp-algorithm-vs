# 强连通分量与缩点

[模板源码](../../../graph/scc.hpp) · [完整示例](../../../examples/graph/scc.cpp) · [使用手册索引](../README.md)

有向图中互相可达的点属于同一个强连通分量。把每个分量缩成一个点，所得图是 DAG，可以继续做拓扑 DP。实现采用迭代 Kosaraju，避免深链递归。

## 接口和使用顺序

| 接口 | 含义 |
| --- | --- |
| `strongly_connected_components(g)` | 返回 SCCResult，时间与额外空间 O(n+m)。 |
| `result.id[u]` | 顶点 u 所属的分量编号，范围 `[0,groups.size())`。 |
| `result.groups[c]` | 分量 c 包含的原图顶点。 |

## 完整例子

下面是可以直接编译的完整程序，数据写在代码里，**不需要输入**。在 workspace 根目录执行文末的 `python tools/cp.py example ...` 命令即可运行并核对输出。

```cpp
#include <algorithm>
#include <iostream>
#include <vector>
#include "graph/scc.hpp"

int main() {
    // 0<->1，2<->3，另有 1->2、3->4。
    std::vector<std::vector<int>> g{{1}, {0, 2}, {3}, {2, 4}, {}};
    auto scc = cp::strongly_connected_components(g);
    std::cout << "components=" << scc.groups.size() << '\n';
    std::cout << "ids:";
    for (int id : scc.id) std::cout << ' ' << id;
    std::cout << '\n';

    std::vector<std::vector<int>> dag(scc.groups.size());
    for (int u = 0; u < int(g.size()); ++u)
        for (int v : g[u])
            if (scc.id[u] != scc.id[v]) dag[scc.id[u]].push_back(scc.id[v]);
    for (int u = 0; u < int(dag.size()); ++u) {
        auto& edges = dag[u];
        std::sort(edges.begin(), edges.end());
        edges.erase(std::unique(edges.begin(), edges.end()), edges.end());
        for (int v : edges) std::cout << "edge=" << u << "->" << v << '\n';
    }
}
```

### 预期标准输出

```text
components=3
ids: 0 0 1 1 2
edge=0->1
edge=1->2
```

## 注意事项

- 分量编号按拓扑序排列，跨分量边 u→v 满足 id[u]<id[v]。
- 分量内顶点顺序不保证有序；如要排序展示，单独排序拷贝。
- 缩点时丢弃同分量边，若题意不允许重边，再排序去重。
- 这是有向图的强连通性；普通无向连通性通常 DSU/BFS 更直接。

也可以在仓库根目录执行：

```powershell
python tools/cp.py example "graph/scc"
```
