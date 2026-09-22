# LCA、树上距离与第 k 个祖先

[模板源码](../../../graph/lca.hpp) · [完整示例](../../../examples/graph/lca.cpp) · [使用手册索引](../README.md)

对一棵固定的无权无向树，先选定根预处理，再回答最近公共祖先、两点间边数和祖先跳跃查询。

## 接口和使用顺序

| 接口 | 含义 |
| --- | --- |
| `LCA(g, root=0)` | 对非空连通树预处理 O(n log n)，空间 O(n log n)。 |
| `lca(u,v)` | 返回在选定根下的最近公共祖先，O(log n)。 |
| `distance(u,v)` | 返回路径的边数，O(log n)。 |
| `kth_ancestor(u,k)` | k=0 返回 u；超出根或 k<0 返回 -1，O(log n)。 |
| `depth[u]` | u 的深度，根深度为 0。 |

## 完整例子

下面是可以直接编译的完整程序，数据写在代码里，**不需要输入**。在 workspace 根目录执行文末的 `python tools/cp.py example ...` 命令即可运行并核对输出。

```cpp
#include <iostream>
#include <vector>
#include "graph/lca.hpp"

int main() {
    // 边：0-1、0-2、1-3、1-4、2-5。
    std::vector<std::vector<int>> g{{1, 2}, {0, 3, 4}, {0, 5}, {1}, {1}, {2}};
    cp::LCA tree(g, 0);
    std::cout << "lca(3,4)=" << tree.lca(3, 4) << '\n';
    std::cout << "lca(4,5)=" << tree.lca(4, 5) << '\n';
    std::cout << "distance(4,5)=" << tree.distance(4, 5) << '\n';
    std::cout << "depth(4)=" << tree.depth[4] << '\n';
    std::cout << "ancestors:";
    for (int k = 0; k <= 3; ++k) std::cout << ' ' << tree.kth_ancestor(4, k);
    std::cout << '\n';
}
```

### 预期标准输出

```text
lca(3,4)=1
lca(4,5)=0
distance(4,5)=4
depth(4)=2
ancestors: 4 1 0 -1
```

## 注意事项

- 邻接表中每条树边要加两个方向；输入必须是树，不是一般图或森林。
- 选择不同根会改变 lca 和祖先关系，但不改变两点间距离。
- 当前 distance 是无权边数；带权距离需另存根到节点的权值和。

也可以在仓库根目录执行：

```powershell
python tools/cp.py example "graph/lca"
```
