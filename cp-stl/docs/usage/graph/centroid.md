# 树的重心与点分治

[模板源码](../../../graph/centroid.hpp) · [完整示例](../../../examples/graph/centroid.cpp) · [使用手册索引](../README.md)

删去重心后，每个连通块的大小都不超过原树的一半。反复“取重心、删除、递归处理各连通块”，
得到深度 O(log n) 的点分树：每条路径恰好在“路径上 level 最小的点”处被统计一次，
所以统计所有路径的问题可以在 O(n log n)（或 O(n log² n)）内完成。本模板只负责分解；
统计时从每个重心出发，用 `can_visit` 限制在它负责的连通块内遍历。

## 接口和使用顺序

| 接口 | 含义 |
| --- | --- |
| `tree_centroids(g)` | 树的重心，1 个或 2 个，升序。 |
| `CentroidDecomposition(g)` | 非递归分解，O(n log n)；g 可以是森林。 |
| `order` | 重心被选出的顺序，点分树的父亲总在儿子之前。 |
| `parent[c] / level[c]` | 点分树上的父亲（根为 -1）/ 层数（根为 0），层数不超过 log₂n。 |
| `can_visit(c, v)` | 处理重心 c 时，邻点 v 是否仍属于 c 的连通块（即 `level[v] > level[c]`）。 |

## 完整例子

下面是可以直接编译的完整程序，数据写在代码里，**不需要输入**。在 workspace 根目录执行文末的 `python tools/cp.py example ...` 命令即可运行并核对输出。

```cpp
#include <algorithm>
#include <array>
#include <iostream>
#include <vector>
#include "graph/centroid.hpp"

// 无序对 (i,j) 中 d[i] + d[j] <= k 的个数：排序后双指针。
long long count_pairs(std::vector<int> d, int k) {
    std::sort(d.begin(), d.end());
    long long result = 0;
    for (int i = 0, j = int(d.size()) - 1; i < j;) {
        if (d[i] + d[j] <= k) { result += j - i; ++i; }
        else --j;
    }
    return result;
}

int main() {
    // 0-1-2-3-4 是一条链，5 挂在 2 上。
    std::vector<std::vector<int>> g{{1}, {0, 2}, {1, 3, 5}, {2, 4}, {3}, {2}};
    std::cout << "centroids:";
    for (int c : cp::tree_centroids(g)) std::cout << ' ' << c;
    cp::CentroidDecomposition cd(g);
    std::cout << "\ncentroid tree parent:";
    for (int p : cd.parent) std::cout << ' ' << p;
    std::cout << '\n';

    // 点分治：统计距离 <= K 的无序点对，每个重心只统计经过它的路径。
    const int K = 2;
    long long pairs = 0;
    for (int c : cd.order) {
        std::vector<int> all{0}; // 重心自己到自己的距离
        for (int s : g[c]) {
            if (!cd.can_visit(c, s)) continue;
            std::vector<int> branch;
            std::vector<std::array<int, 3>> queue{{s, c, 1}}; // (点, 父亲, 到重心的距离)
            for (std::size_t i = 0; i < queue.size(); ++i) {
                auto [u, p, d] = queue[i];
                branch.push_back(d);
                for (int v : g[u]) if (v != p && cd.can_visit(c, v)) queue.push_back({v, u, d + 1});
            }
            pairs -= count_pairs(branch, K); // 同一分支内的点对不经过 c，在更深层统计
            all.insert(all.end(), branch.begin(), branch.end());
        }
        pairs += count_pairs(all, K);
    }
    std::cout << "pairs with distance <= " << K << ": " << pairs << '\n';
}
```

### 预期标准输出

```text
centroids: 2
centroid tree parent: 1 2 -1 2 3 2
pairs with distance <= 2: 10
```

## 注意事项

- 标准写法见例子：对每个重心 c，从每个可访问的邻点出发 BFS 收集距离，先在全体中统计，再减去同一分支内部的点对。
- 点分树（动态点分治）：每个点到它所有点分树祖先的距离可以预处理（共 O(n log n) 个），修改与查询只沿点分树向上走 O(log n) 层。
- 所有遍历都只经过 c 负责的连通块，总访问量为 O(n log n)。

## 对应课程与练习

- 课程：[讲解120 树的重心](https://github.com/algorithmzuo/algorithm-journey/tree/main/src/class120)、[讲解183 静态点分治-上](https://github.com/algorithmzuo/algorithm-journey/tree/main/src/class183)、[讲解184 静态点分治-下](https://github.com/algorithmzuo/algorithm-journey/tree/main/src/class184)、[讲解185 点分树和动态点分治](https://github.com/algorithmzuo/algorithm-journey/tree/main/src/class185)
- 练习：
  - [P3806 点分治 1](https://www.luogu.com.cn/problem/P3806)：距离为 k 的点对是否存在
  - [P4178 Tree](https://www.luogu.com.cn/problem/P4178)：例子中的距离 ≤ k 计数
  - [P2634 聪聪可可](https://www.luogu.com.cn/problem/P2634)：距离模 3 的计数
  - [P6329 震波](https://www.luogu.com.cn/problem/P6329)：点分树：单点修改、距离 ≤ k 的点权和

也可以在仓库根目录执行：

```powershell
python tools/cp.py example "graph/centroid"
```
