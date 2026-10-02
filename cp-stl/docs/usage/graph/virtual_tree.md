# 虚树

[模板源码](../../../graph/virtual_tree.hpp) · [完整示例](../../../examples/graph/virtual_tree.cpp) · [使用手册索引](../README.md)

多次询问、每次只涉及少量关键点的树上问题，可以把关键点和它们两两的 LCA 取出来，
按原树的祖先关系连成一棵小树（虚树），在虚树上做 DP，每次询问的复杂度只与关键点数有关。

## 接口和使用顺序

| 接口 | 含义 |
| --- | --- |
| `virtual_tree(lca, keys)` | lca 为 [FastLCA](fast_lca.md)；keys 可乱序、可重复，O(k log k)。 |
| `VirtualTree::vertices` | 虚树中的点（原树编号），按 DFS 序排列，`vertices[0]` 是根。 |
| `VirtualTree::parent` | `parent[i]` 是 `vertices[i]` 的父亲在 vertices 中的下标，根为 -1。 |

## 完整例子

下面是可以直接编译的完整程序，数据写在代码里，**不需要输入**。在 workspace 根目录执行文末的 `python tools/cp.py example ...` 命令即可运行并核对输出。

```cpp
#include <iostream>
#include <vector>
#include "graph/virtual_tree.hpp"

int main() {
    // 边：0-1, 0-2, 1-3, 1-4, 4-5, 4-6, 2-7，根为 0。
    std::vector<std::vector<int>> g{{1, 2}, {0, 3, 4}, {0, 7}, {1}, {1, 5, 6}, {4}, {4}, {2}};
    cp::FastLCA tree(g, 0);
    auto vt = cp::virtual_tree(tree, {5, 6, 3, 7}); // 关键点可以乱序

    std::cout << "vertices:";
    for (int v : vt.vertices) std::cout << ' ' << v;
    std::cout << "\nedges:";
    for (int i = 1; i < int(vt.vertices.size()); ++i) {
        int child = vt.vertices[i], parent = vt.vertices[vt.parent[i]];
        std::cout << ' ' << parent << '-' << child << "(len " << tree.depth[child] - tree.depth[parent] << ')';
    }
    std::cout << '\n';
}
```

### 预期标准输出

```text
vertices: 0 1 3 4 5 6 7
edges: 0-1(len 1) 1-3(len 1) 1-4(len 1) 4-5(len 1) 4-6(len 1) 0-7(len 2)
```

## 注意事项

- 虚树的点数不超过 2k−1；边 (parent, child) 对应原树中的一条祖先链，长度为 `depth[child] - depth[parent]`。
- 因为 vertices 按 DFS 序排列，倒序遍历即可保证先处理儿子再处理父亲，不需要再建邻接表。
- 关键点之外被加入的 LCA 不是关键点，DP 时注意区分；常用一个 `is_key` 数组标记。
- 边上需要原树信息（如路径最小边权）时，用倍增或树剖在原树上查询祖先链。

## 对应课程与练习

- 课程：[讲解180 虚树的原理和相关题目](https://github.com/algorithmzuo/algorithm-journey/tree/main/src/class180)
- 练习：
  - [P2495 消耗战](https://www.luogu.com.cn/problem/P2495)：虚树 DP，边权为原树祖先链上的最小边
  - [P4103 大工程](https://www.luogu.com.cn/problem/P4103)：虚树上统计路径和、最短、最长
  - [CF613D Kingdom and its Cities](https://www.luogu.com.cn/problem/CF613D)：虚树 + 贪心

也可以在仓库根目录执行：

```powershell
python tools/cp.py example "graph/virtual_tree"
```
