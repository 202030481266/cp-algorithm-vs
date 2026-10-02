# 换根 DP（全方位树 DP）

[模板源码](../../../graph/rerooting.hpp) · [完整示例](../../../examples/graph/rerooting.cpp) · [使用手册索引](../README.md)

很多树形 DP 需要“以每个点为根各算一次”。换根 DP 先自底向上算出每个子树的值，
再自顶向下把父亲一侧的信息传给儿子，总共 O(n)。本模板把这个过程封装成三个运算：

- `merge(a, b)`：合并两个儿子方向的贡献（结合、交换），`identity` 是单位元；
- `add_edge(dp, child, parent, id)`：以 child 为根的子树（DP 值为 dp）经过边 id 贡献给 parent 的值；
- `add_vertex(merged, v)`：合并完所有儿子后加上点 v 本身，得到以 v 为根的子树 DP 值。

## 接口和使用顺序

| 接口 | 含义 |
| --- | --- |
| `rerooting(n, edges, identity, merge, add_edge, add_vertex)` | edges 为 vector<pair<int,int>>（树或森林），返回每个点为根时的 DP 值。 |
| `add_edge(dp, child, parent, id)` | 可用 `edges[id]` 或自备的边权数组区分方向和权值。 |
| 返回值 `answer[v]` | `add_vertex(所有邻点方向贡献的 merge, v)`。 |

## 完整例子

下面是可以直接编译的完整程序，数据写在代码里，**不需要输入**。在 workspace 根目录执行文末的 `python tools/cp.py example ...` 命令即可运行并核对输出。

```cpp
#include <algorithm>
#include <iostream>
#include <utility>
#include <vector>
#include "graph/rerooting.hpp"

int main() {
    int n = 6;
    std::vector<std::pair<int, int>> edges{{0, 1}, {0, 2}, {2, 3}, {2, 4}, {2, 5}};

    // 每个点到其他所有点的距离和（LeetCode 834）：Info = (子树点数, 子树内各点到根的距离和)。
    struct Info { long long size, dist_sum; };
    auto sums = cp::rerooting(n, edges, Info{0, 0},
        [](Info a, Info b) { return Info{a.size + b.size, a.dist_sum + b.dist_sum}; },
        [](Info child, int, int, int) { return Info{child.size, child.dist_sum + child.size}; }, // 每个点多走一条边
        [](Info merged, int) { return Info{merged.size + 1, merged.dist_sum}; });               // 加上根自己
    std::cout << "sum of distances:";
    for (auto info : sums) std::cout << ' ' << info.dist_sum;

    // 每个点的最远距离：子树高度取最大值。
    auto farthest = cp::rerooting(n, edges, 0,
        [](int a, int b) { return std::max(a, b); },
        [](int height, int, int, int) { return height + 1; },
        [](int merged, int) { return merged; });
    std::cout << "\nfarthest:";
    for (int h : farthest) std::cout << ' ' << h;
    std::cout << '\n';
}
```

### 预期标准输出

```text
sum of distances: 8 12 6 10 10 10
farthest: 2 3 2 3 3 3
```

## 注意事项

- T 可以是任意可复制的类型，例如 `struct {子树大小, 距离和}`。局部结构体可以直接使用（不需要运算符重载）。
- 边上有方向信息时（如 CF219D 需要翻转的有向边），在 add_edge 中比较 `edges[id].first == child` 判断方向。
- 运算次数 O(n)，两遍均为非递归扫描；森林的每棵树分别处理。
- merge 需满足结合律和交换律；模板对每个点的邻接表做前缀/后缀合并，求“去掉某个儿子”的结果时不需要逆运算。

## 对应课程与练习

- 课程：[讲解123 换根dp](https://github.com/algorithmzuo/algorithm-journey/tree/main/src/class123)
- 练习：
  - [P3478 STA-Station](https://www.luogu.com.cn/problem/P3478)：深度和最大的根：例子中的距离和
  - [LeetCode 834 树中距离之和](https://leetcode.cn/problems/sum-of-distances-in-tree/)：例子原题
  - [CF219D Choosing Capital for Treeland](https://www.luogu.com.cn/problem/CF219D)：有向边的翻转数
  - [CF1187E Tree Painting](https://www.luogu.com.cn/problem/CF1187E)：子树大小之和

也可以在仓库根目录执行：

```powershell
python tools/cp.py example "graph/rerooting"
```
