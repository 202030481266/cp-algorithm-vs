# 树链剖分：点权路径与子树操作

[模板源码](../../../graph/hld.hpp) · [完整示例](../../../examples/graph/hld.cpp) · [使用手册索引](../README.md)

把树上的路径拆成若干数组区间，再交给线段树维护。本例组合 HLD 与 RangeAddSum，演示路径求和、路径加和子树求和。

## 接口和使用顺序

| 接口 | 含义 |
| --- | --- |
| `HLD(g, root=0)` | 非空无向树，迭代预处理 O(n)。 |
| `pos[u] / vertex[p]` | 顶点映射到数组位置 / 数组位置还原顶点。 |
| `for_each_path(u,v,visit)` | 把包含两端点的路径拆成 O(log n) 个 `[l,r)`，逐段调用 visit。 |
| `subtree(u)` | 返回子树对应的连续 `[l,r)`。 |
| `parent / depth / size / head / heavy` | 父节点（根为 -1）、深度、子树大小、链头、重儿子（无则 -1）。 |

## 完整例子

下面是可以直接编译的完整程序，数据写在代码里，**不需要输入**。在 workspace 根目录执行文末的 `python tools/cp.py example ...` 命令即可运行并核对输出。

```cpp
#include <iostream>
#include <vector>
#include "graph/hld.hpp"
#include "data_structures/lazy_segment_tree.hpp"

int main() {
    std::vector<std::vector<int>> g{{1, 2}, {0, 3, 4}, {0}, {1}, {1}};
    std::vector<long long> value{1, 2, 3, 4, 5};
    cp::HLD hld(g, 0);
    std::vector<long long> linear(value.size());
    for (int u = 0; u < int(value.size()); ++u) linear[hld.pos[u]] = value[u];
    cp::RangeAddSum seg(linear);

    auto path_sum = [&](int u, int v) {
        long long answer = 0;
        hld.for_each_path(u, v, [&](int l, int r) { answer += seg.sum(l, r); });
        return answer;
    };
    std::cout << "before=" << path_sum(3, 4) << '\n'; // 点 3、1、4
    hld.for_each_path(3, 2, [&](int l, int r) { seg.add(l, r, 10); });
    std::cout << "after=" << path_sum(3, 4) << '\n';
    auto [l, r] = hld.subtree(1);
    std::cout << "subtree(1)=" << seg.sum(l, r) << '\n';
}
```

### 预期标准输出

```text
before=11
after=31
subtree(1)=31
```

## 注意事项

- 本接口处理点权，路径上的 LCA 只访问一次。边权问题需要另行处理 LCA 的位置。
- 回调的区间不保证沿 u→v 的方向顺序；适用于求和、min/max 等可交换运算，不能直接做有方向的字符串拼接。
- 搭配 O(log n) 的线段树后，整条路径操作 O(log² n)，子树操作 O(log n)。
- 调用线段树前必须按 pos 重排原来的点权；不要直接把原编号的 value 当成链剖数组。

也可以在仓库根目录执行：

```powershell
python tools/cp.py example "graph/hld"
```
