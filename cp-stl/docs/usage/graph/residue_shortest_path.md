# 同余最短路

[模板源码](../../../graph/residue_shortest_path.hpp) · [完整示例](../../../examples/graph/residue_shortest_path.cpp) · [使用手册索引](../README.md)

给定若干种步长，每种可以用任意多次，问一个很大的范围内有多少个数能被凑出来。
取最小的步长作为 base：如果 x 能凑出，那么 x + base、x + 2·base、…… 都能凑出，所以只需要知道每个余数 r 上最小的能凑出的数 dist[r]。
把 base 个余数看成点，“加上步长 a”看成从 r 到 (r + a) mod base、长度为 a 的边，dist 就是从 0 出发的最短路。
这张图的结构很特殊：每个步长把余数连成若干个环，沿每个环走两圈就完成了全部松弛，不需要 Dijkstra。

## 接口和使用顺序

| 接口 | 含义 |
| --- | --- |
| `residue_distances(steps)` | steps 是正整数步长。返回长度为 base = min(steps) 的数组，`dist[r]` 是模 base 余 r 的最小可凑出的数，凑不出时为 `LLONG_MAX`。O(步长个数 · base)。 |
| `count_representable(dist, limit)` | [0, limit] 中能凑出的数的个数，0 也算在内。O(base)。 |

## 完整例子

下面是可以直接编译的完整程序，数据写在代码里，**不需要输入**。在 workspace 根目录执行文末的 `python tools/cp.py example ...` 命令即可运行并核对输出。

```cpp
#include <iostream>
#include <vector>
#include "graph/residue_shortest_path.hpp"

int main() {
    // 只用 5、7、11 三种面值（数量不限）能恰好凑出哪些金额。
    std::vector<long long> dist = cp::residue_distances({5, 7, 11});
    std::cout << "dist:"; // dist[r]：模 5 余 r 的金额里最小的可凑出的那个
    for (long long d : dist) std::cout << ' ' << d;
    std::cout << '\n';
    for (long long x : {13, 23, 24}) std::cout << x << (x >= dist[x % 5] ? ": yes" : ": no") << '\n';

    // 1..100 中能凑出的个数（去掉 0）；范围到 1e18 也是 O(base)。
    std::cout << "count[1,100]=" << cp::count_representable(dist, 100) - 1 << '\n';
    std::cout << "count[1,1e18]=" << cp::count_representable(dist, 1000000000000000000LL) - 1 << '\n';
}
```

### 预期标准输出

```text
dist: 0 11 7 18 14
13: no
23: yes
24: yes
count[1,100]=92
count[1,1e18]=999999999999999992
```

## 注意事项

- 判断单个 x 能否凑出：`x >= dist[x % base]`。
- 统计 [L, R] 内的个数：`count_representable(dist, R) - count_representable(dist, L - 1)`（L ≥ 1）。
- base 是数组的长度，所以最小的步长不能太大（一般不超过 10⁶ 量级）；其他步长可以到 10¹⁸。
- 题目从 1 出发（如跳楼机从 1 楼开始）时，把范围整体减 1 再统计。
- 需要的不是“能否凑出”而是带限制的最短路（例如每种步长有不同的代价）时，自己按余数建图，用 [Dijkstra](shortest_path.md)。

## 对应课程与练习

- 课程：[讲解143（同余最短路）](https://github.com/algorithmzuo/algorithm-journey/tree/main/src/class143)
- 练习：
  - [P3403 跳楼机](https://www.luogu.com.cn/problem/P3403)：`count_representable(dist, h - 1)`
  - [P2371 [国家集训队] 墨墨的等式](https://www.luogu.com.cn/problem/P2371)：区间 [l, r] 内的个数，两次前缀相减
  - [P2662 牛场围栏](https://www.luogu.com.cn/problem/P2662)：求凑不出的最大的数：max(dist[r]) − base

也可以在仓库根目录执行：

```powershell
python tools/cp.py example "graph/residue_shortest_path"
```
