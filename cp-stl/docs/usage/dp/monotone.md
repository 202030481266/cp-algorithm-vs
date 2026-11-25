# 决策单调性分治与四边形不等式

[模板源码](../../../dp/monotone.hpp) · [完整示例](../../../examples/dp/monotone.cpp) · [使用手册索引](../README.md)

很多“分段”类 DP 的代价函数满足四边形不等式：w(a,c) + w(b,d) ≤ w(a,d) + w(b,c)（a ≤ b ≤ c ≤ d），直观地说就是“区间越长，再加长的代价增长越快”。
它带来两种可以直接套用的单调性：
一维的转移 f[i] = min_j g[j] + w(j, i) 的最优决策点随 i 单调不降，用 `monotone_minima` 分治，每层 O(n log n)；
区间 DP 的最优分割点夹在相邻两个区间的分割点之间，用 `knuth_interval_dp` 把 O(n³) 降到 O(n²)。

## 接口和使用顺序

| 接口 | 含义 |
| --- | --- |
| `monotone_minima(n, m, cost)` | n×m 的矩阵 `cost(i, j)`，要求每行最小值的最左位置随行号单调不降。返回每行的这个列下标，调用 cost O((n+m) log n) 次。 |
| `knuth_interval_dp<T>(n, w)` | 区间 [l,r)，`dp[l][l+1] = 0`，`dp[l][r] = min(dp[l][k] + dp[k][r]) + w(l, r)`。返回 (n+1)×(n+1) 的 dp 表，答案是 `dp[0][n]`，O(n²)。 |

## 完整例子

下面是可以直接编译的完整程序，数据写在代码里，**不需要输入**。在 workspace 根目录执行文末的 `python tools/cp.py example ...` 命令即可运行并核对输出。

```cpp
#include <iostream>
#include <vector>
#include "dp/monotone.hpp"

int main() {
    // 把数组分成恰好 k 段，每段的代价是 (段内和)^2，求最小总代价。按段数一层一层地推：
    // next[i] = min_{j<i} cur[j] + (s_i − s_j)^2，最优的 j 随 i 单调不降，每层用 monotone_minima 求。
    std::vector<long long> a{3, 1, 4, 1, 5, 9, 2, 6};
    int n = int(a.size()), k = 3;
    const long long INF = 1000000000000000000LL;
    std::vector<long long> s(n + 1), cur(n + 1, INF);
    for (int i = 0; i < n; ++i) s[i + 1] = s[i] + a[i];
    cur[0] = 0;
    for (int layer = 0; layer < k; ++layer) {
        auto cost = [&](int i, int j) {
            return j < i && cur[j] < INF ? cur[j] + (s[i] - s[j]) * (s[i] - s[j]) : INF;
        };
        std::vector<int> arg = cp::monotone_minima(n + 1, n + 1, cost);
        std::vector<long long> next(n + 1);
        for (int i = 0; i <= n; ++i) next[i] = cost(i, arg[i]);
        cur = next;
    }
    std::cout << "3 segments: " << cur[n] << '\n';

    // 石子合并（一排）：合并相邻两堆的代价是它们的总数，w(l, r) = 区间和，满足四边形不等式。
    std::vector<long long> stones{4, 1, 1, 4, 3};
    int m = int(stones.size());
    std::vector<long long> prefix(m + 1);
    for (int i = 0; i < m; ++i) prefix[i + 1] = prefix[i] + stones[i];
    auto dp = cp::knuth_interval_dp<long long>(m, [&](int l, int r) { return prefix[r] - prefix[l]; });
    std::cout << "merge stones: " << dp[0][m] << '\n';
}
```

### 预期标准输出

```text
3 segments: 341
merge stones: 28
```

## 注意事项

- `monotone_minima` 不检查单调性，条件不满足时结果是错的而不会报错。拿不准时先写一个 O(nm) 的暴力，对拍最优决策点是否单调。
- 不合法的决策（如 j ≥ i）让 cost 返回一个足够大的值即可，示例中用的是 10¹⁸；注意不要让它参与加法后溢出。
- 求最大值时把 cost 取负；此时要求的是每行最大值的位置单调。
- `monotone_minima` 只适用于“这一层的值只依赖上一层”的转移。f[i] 依赖同一层的 f[j]（j < i）时，要用单调队列加二分的做法，或者配合 [CDQ 分治](../basic/cdq.md)。
- `knuth_interval_dp` 要求 w 满足四边形不等式，并且对区间包含单调（大区间的 w 不小于它包含的小区间）。石子合并求最小值满足；求最大值不满足，最大值的最优分割点一定在两端。
- `knuth_interval_dp` 占用 (n+1)² 个 T 加 (n+1)² 个 int，n = 5000 时约 300MB，一般用于 n ≤ 3000。

## 练习

- [CF321E Ciel and Gondolas](https://www.luogu.com.cn/problem/CF321E)：分 k 段，每层 `monotone_minima`，同示例第一部分
- [P3515 [POI2011] Lightning Conductor](https://www.luogu.com.cn/problem/P3515)：求最大值的决策单调性，正反各做一次
- [P4767 [IOI2000] 邮局](https://www.luogu.com.cn/problem/P4767)：分 k 段，代价是区间到中位数的距离和
- [P1880 [NOI1995] 石子合并](https://www.luogu.com.cn/problem/P1880)：环形，复制一遍后用 `knuth_interval_dp` 求最小值

也可以在仓库根目录执行：

```powershell
python tools/cp.py example "dp/monotone"
```
