# 01 分数规划：二分与 Dinkelbach

[模板源码](../../../math/fractional_programming.hpp) · [完整示例](../../../examples/math/fractional_programming.cpp) · [使用手册索引](../README.md)

01 分数规划求的是一个比值的最值：在允许的方案中最大化 Σa / Σb（b 为正）。
关键的变形是：比值能达到 x，当且仅当存在方案使 Σ(a_i − x·b_i) ≥ 0。于是固定 x 之后，问题变成“每个元素的权值是 a_i − x·b_i，求最大权方案”，
这通常是一个普通的排序、背包、最短路或最小生成树问题。外层用二分找到分界点，或者用 Dinkelbach 迭代直接跳到当前最优方案的比值。

## 接口和使用顺序

| 接口 | 含义 |
| --- | --- |
| `bisect_last_true(lo, hi, feasible, iterations=100)` | 实数二分。feasible 在 [lo,hi] 上先真后假，返回分界点。 |
| `dinkelbach(best, x=0, eps=1e-12)` | `best(x)` 返回使 Σ(a_i − x·b_i) 最大的方案的 `{Σa, Σb}`；函数返回最大比值。通常不到 10 轮。 |
| `max_ratio_choose_k(a, b, k)` | n 个物品中恰好选 k 个，使 Σa / Σb 最大，每轮 O(n)。 |

## 完整例子

下面是可以直接编译的完整程序，数据写在代码里，**不需要输入**。在 workspace 根目录执行文末的 `python tools/cp.py example ...` 命令即可运行并核对输出。

```cpp
#include <algorithm>
#include <iomanip>
#include <iostream>
#include <utility>
#include <vector>
#include "math/fractional_programming.hpp"

int main() {
    // 3 次测验的得分 a 和满分 b，去掉 1 次（保留 k = 2 次），使 总得分 / 总满分 最大。
    std::vector<long long> a{5, 0, 2}, b{5, 1, 6};
    int k = 2;
    std::cout << std::fixed << std::setprecision(6);
    std::cout << "choose_k=" << cp::max_ratio_choose_k(a, b, k) << '\n';

    // 一般的写法：best(x) 求出使 Σ(a_i − x·b_i) 最大的方案，返回它的 {Σa, Σb}。
    auto best = [&](double x) {
        std::vector<int> ids{0, 1, 2};
        std::sort(ids.begin(), ids.end(), [&](int i, int j) { return a[i] - x * b[i] > a[j] - x * b[j]; });
        double sum_a = 0, sum_b = 0;
        for (int t = 0; t < k; ++t) { sum_a += double(a[ids[t]]); sum_b += double(b[ids[t]]); }
        return std::pair<double, double>(sum_a, sum_b);
    };
    // 二分：比值能达到 x  <=>  最优方案的 Σa − x·Σb >= 0。
    auto feasible = [&](double x) { auto [sum_a, sum_b] = best(x); return sum_a - x * sum_b >= 0; };
    std::cout << "bisect=" << cp::bisect_last_true(0, 1, feasible) << '\n';
    // Dinkelbach：直接跳到当前最优方案的比值。
    std::cout << "dinkelbach=" << cp::dinkelbach(best) << '\n';
}
```

### 预期标准输出

```text
choose_k=0.833333
bisect=0.833333
dinkelbach=0.833333
```

## 注意事项

- 求最小比值：把 a 全部取负，求出最大值后再取负。
- `dinkelbach` 的初值 x 可以任取，第一轮之后比值单调不降；它要求 best 返回的是真正的最优方案，而二分只需要知道最优值的符号。
- 内层问题只能判定、求不出具体方案（例如判负环）时用二分；最优比率环是“是否存在 Σ(a − x·b) > 0 的环”，用 [SPFA 判负环](../graph/spfa.md)。
- 二分固定迭代 100 次，区间长度缩小到 2⁻¹⁰⁰ 倍，不需要再设 eps；内层判定很慢时可以把次数降到 50 左右。
- 最优比率生成树的内层是以 a_i − x·b_i 为边权的[最小生成树](../graph/mst.md)，完全图用稠密图 Prim。

## 对应课程与练习

- 课程：[讲解138（01 分数规划）](https://github.com/algorithmzuo/algorithm-journey/tree/main/src/class138)
- 练习：
  - [POJ 2976 Dropping tests](http://poj.org/problem?id=2976)：`max_ratio_choose_k`，同示例
  - [P4377 [USACO18OPEN] Talent Show G](https://www.luogu.com.cn/problem/P4377)：内层是 01 背包
  - [P3199 [HNOI2009] 最小圈](https://www.luogu.com.cn/problem/P3199)：最小平均权环，二分后判负环
  - [P2868 [USACO07DEC] Sightseeing Cows G](https://www.luogu.com.cn/problem/P2868)：最优比率环

也可以在仓库根目录执行：

```powershell
python tools/cp.py example "math/fractional_programming"
```
