# Kruskal 最小生成树 / 森林

[模板源码](../../../graph/mst.hpp) · [完整示例](../../../examples/graph/mst.cpp) · [使用手册索引](../README.md)

给出无向边列表，用 Kruskal 选择总权值最小的连接方案。若原图不连通，返回各连通块的最小生成树组成的森林，必须检查 connected。

## 接口和使用顺序

| 接口 | 含义 |
| --- | --- |
| `UndirectedEdge{u,v,weight}` | 一条无向边，端点 0-based，权值 long long。 |
| `kruskal(n, edges)` | O(m log m)，参数按值传入，调用者的边列表不会被排序。 |
| `result.connected` | 是否存在覆盖全部顶点的生成树；空图约定为 true。 |
| `result.weight / result.edges` | 选中边的总权值 / 边列表；不连通时是最小生成森林。 |

## 完整例子

下面是可以直接编译的完整程序，数据写在代码里，**不需要输入**。在 workspace 根目录执行文末的 `python tools/cp.py example ...` 命令即可运行并核对输出。

```cpp
#include <iostream>
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
}
```

### 预期标准输出

```text
connected=true
weight=4 edges=3
forest_connected=false
forest_weight=4
```

## 注意事项

- 每条无向边只需传一次，不要像邻接表那样主动添加反向边。
- 允许负边、重边和自环；自环不会被选中。
- 连通且 n>0 时选中 n-1 条边。总权值须放得进 long long。
- 内部依赖 DSU；引用头文件或使用单文件导出时会自动包含它，手动复制时不要漏掉。

也可以在仓库根目录执行：

```powershell
python tools/cp.py example "graph/mst"
```
