# Link-Cut Tree：动态树

[模板源码](../../../data_structures/link_cut_tree.hpp) · [完整示例](../../../examples/data_structures/link_cut_tree.cpp) · [使用手册索引](../README.md)

维护一个会不断连边、删边的森林，并在线查询路径信息。Link-Cut Tree 用 Splay 维护“实链”，
`access` 打通根到某点的路径，配合换根实现连边、删边、连通性、LCA 和路径聚合，全部均摊 O(log n)。
本实现完全非递归，长链也不会爆栈。

## 接口和使用顺序

| 接口 | 含义 |
| --- | --- |
| `LinkCutTree<T, Op>(values, identity)` | 点 0..n-1 带初始点权；也可 `(n, identity)` 全部为单位元。 |
| `link(u, v) / cut(u, v)` | 不连通时连边 / 边存在时删边；成功返回 true，否则什么也不做返回 false。 |
| `connected(u, v)` | 两点是否在同一棵树中。 |
| `set(u, value) / get(u)` | 修改 / 读取点权。 |
| `path_prod(u, v)` | u 到 v 路径上所有点权的聚合，要求连通；调用后 u 成为所在树的根。 |
| `make_root(u) / find_root(u) / lca(u, v)` | 换根 / 当前根 / 以当前根为准的 LCA（要求连通）。 |

## 完整例子

下面是可以直接编译的完整程序，数据写在代码里，**不需要输入**。在 workspace 根目录执行文末的 `python tools/cp.py example ...` 命令即可运行并核对输出。

```cpp
#include <functional>
#include <iostream>
#include <vector>
#include "data_structures/link_cut_tree.hpp"

int main() {
    // 5 个点，点权 1..5，维护路径点权和。
    cp::LinkCutTree<long long, std::plus<long long>> lct(std::vector<long long>{1, 2, 3, 4, 5}, 0);
    lct.link(0, 1);
    lct.link(1, 2);
    lct.link(1, 3); // 边：0-1, 1-2, 1-3
    std::cout << std::boolalpha;
    std::cout << "path(2,3)=" << lct.path_prod(2, 3) << " connected(0,4)=" << lct.connected(0, 4) << '\n';

    lct.cut(1, 3);
    lct.link(3, 4);
    std::cout << "link_again=" << lct.link(4, 3) << '\n'; // 已连通，不重复连边
    lct.link(4, 0); // 现在是一条链 2-1-0-4-3
    lct.set(1, 10);
    std::cout << "path(2,3)=" << lct.path_prod(2, 3) << '\n';
    lct.make_root(0);
    std::cout << "lca(2,3)=" << lct.lca(2, 3) << " root(3)=" << lct.find_root(3) << '\n';
}
```

### 预期标准输出

```text
path(2,3)=9 connected(0,4)=false
link_again=false
path(2,3)=23
lca(2,3)=0 root(3)=0
```

## 注意事项

- Op 必须结合且**交换**（加法、异或、min、max），因为换根会翻转路径方向；需要方向的聚合要额外维护反向信息。
- 边权问题把每条边变成一个新点（点权为边权），原来的点权设为单位元，再连两条边。
- `path_prod`、`lca` 会改变内部结构，但不改变森林本身；`lca` 以最近一次 `make_root`（或 `path_prod` 换出的根）为准。
- 常数比树链剖分大，静态树优先用 [HLD](../graph/hld.md)。

## 对应课程与练习

- 课程：[讲解201 LCT的原理和模版题目](https://github.com/algorithmzuo/algorithm-journey/tree/main/src/class201)、[讲解202 LCT维护路径](https://github.com/algorithmzuo/algorithm-journey/tree/main/src/class202)、[讲解203 LCT维护边权、生成树](https://github.com/algorithmzuo/algorithm-journey/tree/main/src/class203)、[讲解204 LCT维护子树、图结构](https://github.com/algorithmzuo/algorithm-journey/tree/main/src/class204)
- 练习：
  - [P3690 动态树（Link Cut Tree）](https://www.luogu.com.cn/problem/P3690)：异或聚合：`LinkCutTree<int, std::bit_xor<int>>`
  - [P2147 洞穴勘测](https://www.luogu.com.cn/problem/P2147)：只需 link/cut/connected
  - [P4172 水管局长](https://www.luogu.com.cn/problem/P4172)：边转点 + 维护路径最大边

也可以在仓库根目录执行：

```powershell
python tools/cp.py example "data_structures/link_cut_tree"
```
