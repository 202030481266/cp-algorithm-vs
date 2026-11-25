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
