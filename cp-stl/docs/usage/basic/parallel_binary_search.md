# 整体二分

[模板源码](../../../basic/parallel_binary_search.hpp) · [完整示例](../../../examples/basic/parallel_binary_search.cpp) · [使用手册索引](../README.md)

每个询问单独二分答案，判定时都要把数据结构从头建一遍，太慢。整体二分把所有询问放在一起：
每一轮按顺序执行一遍操作，执行到某个询问当前的二分中点时顺便判定它，于是一轮就让所有询问的范围同时缩小一半。
适用条件是询问的判定结果随操作的执行单调（先假后真），并且可以离线。

## 接口和使用顺序

| 接口 | 含义 |
| --- | --- |
| `parallel_binary_search(m, q, reset, apply, check)` | m 个操作、q 个询问，返回长度为 q 的数组。 |
| `reset()` | 把数据结构清空到“一个操作都没执行”的状态，每轮调用一次。 |
| `apply(t)` | 执行第 t 个操作（0 ≤ t < m），每轮按 t 递增的顺序调用。 |
| `check(i)` | 在当前状态下询问 i 是否成立。 |
| 返回值 `answer[i]` | 最小的 t（0 ≤ t ≤ m），使执行完前 t 个操作后询问 i 成立；全部执行完仍不成立时是 m + 1。 |

## 完整例子

下面是可以直接编译的完整程序，数据写在代码里，**不需要输入**。在 workspace 根目录执行文末的 `python tools/cp.py example ...` 命令即可运行并核对输出。

```cpp
#include <algorithm>
#include <iostream>
#include <numeric>
#include <vector>
#include "basic/parallel_binary_search.hpp"
#include "data_structures/fenwick.hpp"

int main() {
    // 离线区间第 k 小：把位置按值从小到大依次“点亮”，
    // 询问 (l, r, k) 的答案就是 [l,r) 内点亮的个数第一次达到 k 时点亮的那个值。
    std::vector<int> a{5, 2, 6, 3, 7, 4};
    struct Query { int l, r, k; }; // [l,r) 中第 k 小，k 从 1 开始
    std::vector<Query> queries{{1, 5, 1}, {1, 5, 3}, {0, 6, 4}, {2, 3, 1}};

    int n = int(a.size());
    std::vector<int> order(n); // 第 t 个操作：点亮位置 order[t]
    std::iota(order.begin(), order.end(), 0);
    std::sort(order.begin(), order.end(), [&](int i, int j) { return a[i] < a[j]; });

    cp::Fenwick<int> lit(n);
    std::vector<int> answer = cp::parallel_binary_search(n, int(queries.size()),
        [&] { lit = cp::Fenwick<int>(n); },
        [&](int t) { lit.add(order[t], 1); },
        [&](int i) { return lit.sum(queries[i].l, queries[i].r) >= queries[i].k; });

    // answer[i] 是需要执行的操作个数，最后执行的是第 answer[i] - 1 个操作。
    for (int i = 0; i < int(queries.size()); ++i)
        std::cout << "kth(" << queries[i].l << ',' << queries[i].r << ',' << queries[i].k << ")="
                  << a[order[answer[i] - 1]] << '\n';
}
```

### 预期标准输出

```text
kth(1,5,1)=2
kth(1,5,3)=6
kth(0,6,4)=5
kth(2,3,1)=6
```

## 注意事项

- `answer[i] = t ≥ 1` 表示第 t−1 个操作（下标从 0 开始）让询问 i 第一次成立；`answer[i] = 0` 表示不需要任何操作。
- 共 ⌈log₂(m+2)⌉ 轮，每轮最多 m 次 apply、q 次 check，总复杂度 O((m + q) log m · 数据结构的单次复杂度)。
- “二分值域”的题目把值排序后当作操作：第 t 个操作是加入第 t 小的元素，示例就是这样做区间第 k 小的。
- reset 尽量便宜：树状数组直接重新构造即可。
- 操作带时间顺序、询问还限定只看某个时刻之前的操作时（如带修改的区间第 k 小），需要按值域递归并拆分操作序列的写法，不在这个模板的范围内，可以用[树套树](../data_structures/dynamic_range_kth.md)。

## 对应课程与练习

- 课程：[讲解168（整体二分）](https://github.com/algorithmzuo/algorithm-journey/tree/main/src/class168)–[169](https://github.com/algorithmzuo/algorithm-journey/tree/main/src/class169)
- 练习：
  - [P3834 【模板】可持久化线段树 2](https://www.luogu.com.cn/problem/P3834)：静态区间第 k 小，同示例
  - [P3527 [POI2011] MET-Meteors](https://www.luogu.com.cn/problem/P3527)：操作是陨石雨（环上的区间加），询问是每个国家何时收集够
  - [P1527 [国家集训队] 矩阵乘法](https://www.luogu.com.cn/problem/P1527)：矩形内第 k 小：按值排序后依次加入二维树状数组

也可以在仓库根目录执行：

```powershell
python tools/cp.py example "basic/parallel_binary_search"
```
