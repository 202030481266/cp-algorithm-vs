# 无权树直径路径

[模板源码](../../../graph/tree_diameter.hpp) · [完整示例](../../../examples/graph/tree_diameter.cpp) · [使用手册索引](../README.md)

求一棵无权无向树中最长的简单路径。两次 BFS 得到直径端点，再还原路径；函数直接返回路径上的顶点序列。

## 接口和使用顺序

| 接口 | 含义 |
| --- | --- |
| `tree_diameter(g)` | 返回 vector<int> 路径，时间与空间 O(n)。 |
| `path.front() / path.back()` | 非空时为直径两端点。 |
| `int(path.size())-1` | 非空树的直径边数；路径点数比边数多 1。 |

## 完整例子

下面是可以直接编译的完整程序，数据写在代码里，**不需要输入**。在 workspace 根目录执行文末的 `python tools/cp.py example ...` 命令即可运行并核对输出。

```cpp
#include <iostream>
#include <vector>
#include "graph/tree_diameter.hpp"

int main() {
    std::vector<std::vector<int>> g{{1, 2}, {0, 3, 4}, {0}, {1}, {1}};
    auto path = cp::tree_diameter(g);
    if (!path.empty()) {
        std::cout << "length=" << int(path.size()) - 1 << '\n';
        std::cout << "path:";
        for (int u : path) std::cout << ' ' << u;
        std::cout << '\n';
    }
    std::vector<std::vector<int>> single(1);
    std::cout << "single_length=" << int(cp::tree_diameter(single).size()) - 1 << '\n';
    std::cout << "empty_size=" << cp::tree_diameter({}).size() << '\n';
}
```

### 预期标准输出

```text
length=3
path: 4 1 0 2
single_length=0
empty_size=0
```

## 注意事项

- 输入必须是无权无向树；不能直接用于一般图，也不能据此处理负权树。
- 空树返回空路径；先判断空再访问端点或计算长度，避免 size_t 下溢。
- 单点树返回一个点，直径长度为 0。
- 存在多条直径时返回其中一条，端点顺序不固定；示例给出当前邻接顺序下的输出。

也可以在仓库根目录执行：

```powershell
python tools/cp.py example "graph/tree_diameter"
```
