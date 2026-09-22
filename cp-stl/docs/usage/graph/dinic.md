# Dinic 最大流与最小割

[模板源码](../../../graph/dinic.hpp) · [完整示例](../../../examples/graph/dinic.cpp) · [使用手册索引](../README.md)

将问题建成带非负容量的有向网络，指定不同的源点和汇点，求最大可通过流量。求满最大流后，残量网络中源点可达集合给出一个最小割。

## 接口和使用顺序

| 接口 | 含义 |
| --- | --- |
| `Dinic(n)` | 创建 n 个点的空网络。 |
| `add_edge(u,v,capacity)` | 添加一条有向边；反向残量边由模板自动建立。 |
| `flow(source,sink,limit=LLONG_MAX)` | 本次最多再增广 limit，返回本次新增流量；一般复杂度 O(V²E)。 |
| `min_cut(source)` | 返回 vector<bool>：是否在残量网络中从 source 可达，O(V+E)。 |

## 完整例子

下面是可以直接编译的完整程序，数据写在代码里，**不需要输入**。在 workspace 根目录执行文末的 `python tools/cp.py example ...` 命令即可运行并核对输出。

```cpp
#include <iostream>
#include "graph/dinic.hpp"

int main() {
    cp::Dinic network(4);
    network.add_edge(0, 1, 3);
    network.add_edge(0, 2, 2);
    network.add_edge(1, 2, 1);
    network.add_edge(1, 3, 2);
    network.add_edge(2, 3, 3);

    long long first = network.flow(0, 3, 2); // 本次最多送 2
    long long rest = network.flow(0, 3);     // 继续求到最大流
    std::cout << "first=" << first << " rest=" << rest << '\n';
    std::cout << "total=" << first + rest << '\n';
    std::cout << "additional=" << network.flow(0, 3) << '\n';
    auto source_side = network.min_cut(0);
    std::cout << "source_side:";
    for (int u = 0; u < 4; ++u) if (source_side[u]) std::cout << ' ' << u;
    std::cout << '\n';
}
```

### 预期标准输出

```text
first=2 rest=3
total=5
additional=0
source_side: 0
```

## 注意事项

- 不要为同一有向边手动添加残量反向边；模板已处理。题目确有另一条反向容量时才再次 add_edge。
- 多次 flow 调用共用残量网络，返回增量而不是历史总流量。
- 必须完成最大流、未被 limit 提前截断，才能把 min_cut 结果作为最小割。
- 容量和总流量必须能放进 long long；源点和汇点必须不同。
- DFS 递归深度可达 O(V)，非常深的网络要留意栈空间。

也可以在仓库根目录执行：

```powershell
python tools/cp.py example "graph/dinic"
```
