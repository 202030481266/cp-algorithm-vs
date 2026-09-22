# 把图输出为 Graphviz DOT

[模板源码](../../../util/graphviz.hpp) · [完整示例](../../../examples/util/graphviz.cpp) · [使用手册索引](../README.md)

把点和边转成 DOT 文本，方便复制到图可视化工具里查看。这个头文件只负责输出文本，运行示例不需要安装 Graphviz；如需生成图片，再将 DOT 交给支持它的渲染工具。

## 接口和使用顺序

| 接口 | 含义 |
| --- | --- |
| `graphviz::Edge{u,v,w}` | 一条边；w 为 optional<long long>，不带权可用 std::nullopt。 |
| `graphviz::Options` | directed、index_base、left_to_right、show_edge_ids；默认无向、0-based、竖排、不显示边编号。 |
| `graphviz::to_dot(n,edges,opt={})` | 返回 DOT 字符串，适合写文件、日志或传给查看器。 |
| `graphviz::dump_graph(n,edges,out=cout,opt={})` | 直接写入输出流，不关闭或主动 flush 输出流。 |

## 完整例子

下面是可以直接编译的完整程序，数据写在代码里，**不需要输入**。在 workspace 根目录执行文末的 `python tools/cp.py example ...` 命令即可运行并核对输出。

```cpp
#include <iostream>
#include <optional>
#include <vector>
#include "util/graphviz.hpp"

int main() {
    std::vector<graphviz::Edge> edges{{0, 1, 7}, {1, 2, std::nullopt}};
    std::cout << "default:\n" << graphviz::to_dot(3, edges);

    graphviz::Options options;
    options.directed = true;
    options.index_base = 1;
    options.left_to_right = true;
    options.show_edge_ids = true;
    std::cout << "custom:\n";
    graphviz::dump_graph(2, {{1, 2, 9}}, std::cout, options);
}
```

### 预期标准输出

```text
default:
graph G {
  rankdir=TB;
  node [shape=circle];
  0;
  1;
  2;
  0 -- 1 [label="7"];
  1 -- 2;
}
custom:
digraph G {
  rankdir=LR;
  node [shape=circle];
  1;
  2;
  1 -> 2 [label="#0: 9"];
}
```

## 注意事项

- 该工具沿用 `graphviz` 命名空间，而不是 cp。
- 无向图中每条实际边只传一次，不要把邻接表的两个方向都复制进来。
- index_base 只支持 0 或 1；所有端点必须位于对应的 n 个点中，否则抛出异常。
- 会显式输出孤立点；边编号按 edges 中的顺序从 0 开始，与点的编号起点无关。
- 复杂度与顶点、边数及输出文本长度成正比。

也可以在仓库根目录执行：

```powershell
python tools/cp.py example "util/graphviz"
```
