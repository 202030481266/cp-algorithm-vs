#pragma once
// 使用说明：cp-stl/docs/usage/dp/knapsack.md
// 完整示例：cp-stl/examples/dp/knapsack.cpp
#include <algorithm>
#include <cassert>
#include <vector>

namespace cp {
// dp[c] 表示容量不超过 c 的最大价值，初始化为 0。
// 若要求恰好装满：初始化为负无穷并单独处理不可达状态，不直接用这些函数。
// 01 单物品 O(capacity)；允许 weight=0（该物品只使用一次）。
inline void knapsack01(std::vector<long long>& dp, int weight, long long value) {
    assert(weight >= 0);
    for (int c = int(dp.size()) - 1; c >= weight; --c)
        dp[c] = std::max(dp[c], dp[c - weight] + value);
}
// 完全背包单物品 O(capacity)，weight 必须正。
inline void knapsack_unbounded(std::vector<long long>& dp, int weight, long long value) {
    assert(weight > 0);
    for (int c = weight; c < int(dp.size()); ++c)
        dp[c] = std::max(dp[c], dp[c - weight] + value);
}
// 多重背包二进制拆分，O(capacity * log(count+1))。中间价值须能放入 long long。
inline void knapsack_bounded(std::vector<long long>& dp, int weight, long long value, int count) {
    assert(weight > 0 && count >= 0);
    count = std::min(count, int(dp.size()) / weight);
    for (long long block = 1; count > 0; block <<= 1) {
        int take = int(std::min<long long>(block, count));
        knapsack01(dp, weight * take, value * take);
        count -= take;
    }
}
} // namespace cp
