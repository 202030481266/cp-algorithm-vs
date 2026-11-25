# 斜率优化与李超线段树

[模板源码](../../../dp/convex_hull_trick.hpp) · [完整示例](../../../examples/dp/convex_hull_trick.cpp) · [使用手册索引](../README.md)

转移形如 dp[i] = min_j (k_j · x_i + b_j) + c_i 的 DP，每个决策 j 是一条直线，求 dp[i] 就是查询所有直线在 x_i 处的最小值。
斜率单调时用 `MonotoneCHT`，它用单调栈维护下凸壳，加入直线均摊 O(1)；
斜率没有规律、或者加入的是线段时用 `LiChaoTree`，每次 O(log n)，写法不依赖任何单调性。

## 接口和使用顺序

| 接口 | 含义 |
| --- | --- |
| `MonotoneCHT hull` | 求最小值，x 为整数。 |
| `hull.add(k, b)` | 加入直线 y = k·x + b，斜率必须单调不增。均摊 O(1)。 |
| `hull.query(x)` | 任意 x 处的最小值，二分，O(log n)。 |
| `hull.query_increasing(x)` | 要求每次查询的 x 不降，均摊 O(1)。 |
| `LiChaoTree<T> tree(xs)` | xs 是所有会查询的横坐标（内部排序去重）。T 默认 long long，也可以是 double。 |
| `tree.add_line(k, b)` | 加入直线，O(log n)。 |
| `tree.add_segment(k, b, xl, xr)` | 加入只在闭区间 [xl, xr] 上生效的线段，O(log² n)。 |
| `tree.query(x)` | x 处的最小值，x 必须在 xs 中；没有直线覆盖时返回 `LiChaoTree<T>::INF`。 |

## 完整例子

下面是可以直接编译的完整程序，数据写在代码里，**不需要输入**。在 workspace 根目录执行文末的 `python tools/cp.py example ...` 命令即可运行并核对输出。

```cpp
#include <iostream>
#include <vector>
#include "dp/convex_hull_trick.hpp"

int main() {
    // 把数组分成若干连续段，每段的代价是 (段内和)^2 + C，求最小总代价。
    // dp[i] = s_i^2 + C + min_j { (−2·s_j)·s_i + dp[j] + s_j^2 }：
    // 直线的斜率 −2·s_j 单调递减，查询点 s_i 单调递增。
    std::vector<long long> a{3, 1, 4, 1, 5, 9, 2, 6};
    const long long C = 20;
    int n = int(a.size());
    std::vector<long long> s(n + 1), dp(n + 1);
    for (int i = 0; i < n; ++i) s[i + 1] = s[i] + a[i];
    cp::MonotoneCHT hull;
    hull.add(0, 0); // j = 0
    for (int i = 1; i <= n; ++i) {
        dp[i] = s[i] * s[i] + C + hull.query_increasing(s[i]);
        hull.add(-2 * s[i], dp[i] + s[i] * s[i]);
    }
    std::cout << "monotone: " << dp[n] << '\n';

    // 李超线段树：直线可以按任意顺序加入，只要事先给出所有查询的横坐标。
    cp::LiChaoTree<long long> tree(s);
    std::vector<long long> f(n + 1);
    tree.add_line(0, 0);
    for (int i = 1; i <= n; ++i) {
        f[i] = s[i] * s[i] + C + tree.query(s[i]);
        tree.add_line(-2 * s[i], f[i] + s[i] * s[i]);
    }
    std::cout << "li chao: " << f[n] << '\n';

    // 线段只在自己的横坐标范围内参与取最小值。
    cp::LiChaoTree<long long> segments({0, 1, 2, 3, 4, 5});
    segments.add_segment(1, 0, 0, 5);  // y = x，x 在 [0,5]
    segments.add_segment(-1, 4, 1, 3); // y = 4 − x，x 在 [1,3]
    segments.add_line(0, 3);           // y = 3
    std::cout << "segments:";
    for (long long x = 0; x <= 5; ++x) std::cout << ' ' << segments.query(x);
    std::cout << '\n';
}
```

### 预期标准输出

```text
monotone: 307
li chao: 307
segments: 0 1 2 1 3 3
```

## 注意事项

- 求最大值：加入 (−k, −b)，查询结果取负。用 `MonotoneCHT` 时，这要求原来的斜率单调不降。
- 斜率是单调递增的最小值问题，可以把 x 取负：加入 (−k, b)、查询 −x，斜率就变成递减的了。
- `MonotoneCHT` 只用整数除法比较交点，不做交叉相乘，因此不需要 128 位整数；需要 k·x + b 以及任意两条直线的 b 之差不溢出 long long。
- `query` 和 `query_increasing` 不要在同一个对象上混用：后者会移动内部指针。
- 李超树只在给定的横坐标上保证正确，查询坐标要事先全部放进 xs；值域是连续整数 0..n−1 时传 iota 生成的数组即可。
- 查询点和斜率都不单调、但可以离线时，也可以用 [CDQ 分治](../basic/cdq.md)把它们排成单调的。

## 练习

- [P3195 [HNOI2008] 玩具装箱](https://www.luogu.com.cn/problem/P3195)：斜率和查询点都单调，`add` + `query_increasing`
- [P3628 [APIO2010] 特别行动队](https://www.luogu.com.cn/problem/P3628)：求最大值，取负后同上
- [P5785 [SDOI2012] 任务安排](https://www.luogu.com.cn/problem/P5785)：查询点不单调，用 `query`
- [P4097 【模板】李超线段树 / [HEOI2013] Segment](https://www.luogu.com.cn/problem/P4097)：`add_segment`，求最大值并记录编号
- [P4254 [JSOI2008] Blue Mary 开公司](https://www.luogu.com.cn/problem/P4254)：`add_line` 求最大值

也可以在仓库根目录执行：

```powershell
python tools/cp.py example "dp/convex_hull_trick"
```
