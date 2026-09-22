# 拓扑排序与有向环判断

[模板源码](../../../graph/topological_sort.hpp) · [完整示例](../../../examples/graph/topological_sort.cpp) · [使用手册索引](../README.md)

任务存在先后依赖关系时，把“u 必须先于 v”建成有向边 u→v，再做拓扑排序。也可据此按顺序进行 DAG DP。

## 接口和使用顺序

| 接口 | 含义 |
| --- | --- |
| `topological_sort(g)` | g 为有向邻接表；返回处理顺序 vector<int>，O(n+m)。 |
| `order.size() == g.size()` | 全部顶点被处理，说明无有向环；空图也满足。 |
| `order.size() < g.size()` | 存在有向环，返回的只是部分处理顺序。 |

## 完整例子

下面是可以直接编译的完整程序，数据写在代码里，**不需要输入**。在 workspace 根目录执行文末的 `python tools/cp.py example ...` 命令即可运行并核对输出。

```cpp
#include <iostream>
#include <vector>
#include "graph/topological_sort.hpp"

int main() {
    std::vector<std::vector<int>> dag{{1, 2}, {3}, {3}, {}};
    auto order = cp::topological_sort(dag);
    if (order.size() == dag.size()) {
        std::cout << "order:";
        for (int u : order) std::cout << ' ' << u;
        std::cout << '\n';
    }

    std::vector<std::vector<int>> cycle{{1}, {2}, {0}};
    std::cout << std::boolalpha;
    std::cout << "has_cycle=" << (cp::topological_sort(cycle).size() < cycle.size()) << '\n';
}
```

### 预期标准输出

```text
order: 0 1 2 3
has_cycle=true
```

## 注意事项

- 拓扑序通常不唯一，本模板不保证字典序最小。
- 必须比较返回长度与 n；不能只凭 order.empty() 判断有环，因为空图的合法顺序也是空的。
- 无向图不要直接把双向边送进这个接口判断环。

也可以在仓库根目录执行：

```powershell
python tools/cp.py example "graph/topological_sort"
```
