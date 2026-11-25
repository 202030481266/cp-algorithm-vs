#pragma once
// 使用说明：cp-stl/docs/usage/dp/monotone.md
// 完整示例：cp-stl/examples/dp/monotone.cpp
#include <algorithm>
#include <cassert>
#include <vector>

namespace cp {
// 决策单调性分治：n 行 m 列的矩阵 cost(i, j)，要求每行最小值所在的（最左）列随行号单调不降，
// 返回每行的这个列下标。先算中间一行，再把列的范围分给上下两半，只调用 O((n + m) log n) 次 cost。
// 求最大值时把 cost 取负。
template<class Cost>
std::vector<int> monotone_minima(int n, int m, Cost cost) {
    assert(n >= 0 && (n == 0 || m >= 1));
    std::vector<int> best(n);
    // 行 [top,bottom) 的答案在列 [left,right] 内。
    auto solve = [&](auto&& self, int top, int bottom, int left, int right) -> void {
        if (top >= bottom) return;
        int mid = top + (bottom - top) / 2, arg = left;
        auto value = cost(mid, left);
        for (int j = left + 1; j <= right; ++j) {
            auto now = cost(mid, j);
            if (now < value) { value = now; arg = j; }
        }
        best[mid] = arg;
        self(self, top, mid, left, arg);
        self(self, mid + 1, bottom, arg, right);
    };
    solve(solve, 0, n, 0, m - 1);
    return best;
}

// 四边形不等式（Knuth）优化的区间 DP。区间 [l,r)（0 <= l < r <= n）：
//   dp[l][l+1] = 0，dp[l][r] = min{ dp[l][k] + dp[k][r] : l < k < r } + w(l, r)。
// w 满足四边形不等式且对区间包含单调时，最优分割点满足 opt[l][r-1] <= opt[l][r] <= opt[l+1][r]，
// 只在这个范围内枚举，总复杂度 O(n²)。返回 dp 表（只有 l < r 的位置有意义），答案是 dp[0][n]。
template<class T, class Weight>
std::vector<std::vector<T>> knuth_interval_dp(int n, Weight w) {
    assert(n >= 0);
    std::vector<std::vector<T>> dp(n + 1, std::vector<T>(n + 1, T()));
    std::vector<int> opt(std::size_t(n + 1) * (n + 1));
    auto at = [&](int l, int r) -> int& { return opt[std::size_t(l) * (n + 1) + r]; };
    for (int l = 0; l < n; ++l) at(l, l + 1) = l;
    for (int len = 2; len <= n; ++len)
        for (int l = 0; l + len <= n; ++l) {
            int r = l + len, from = std::max(l + 1, at(l, r - 1)), to = std::min(r - 1, at(l + 1, r));
            assert(from <= to);
            T value = dp[l][from] + dp[from][r];
            int arg = from;
            for (int k = from + 1; k <= to; ++k) {
                T now = dp[l][k] + dp[k][r];
                if (now < value) { value = now; arg = k; }
            }
            dp[l][r] = value + w(l, r);
            at(l, r) = arg;
        }
    return dp;
}
} // namespace cp
