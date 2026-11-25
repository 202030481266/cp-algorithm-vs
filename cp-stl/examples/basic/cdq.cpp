#include <algorithm>
#include <array>
#include <iostream>
#include <vector>
#include "basic/cdq.hpp"

int main() {
    // 三维偏序：每个点有多少个其他的点，三个坐标都不超过它。
    std::vector<std::array<int, 3>> points{{1, 1, 1}, {2, 2, 2}, {1, 2, 3}, {2, 2, 2}, {3, 1, 2}, {3, 3, 3}};
    std::cout << "dominated:";
    for (int count : cp::dominance_count_3d(points)) std::cout << ' ' << count;
    std::cout << '\n';

    // 骨架 cdq_divide：选出最长的点序列，要求下标递增且 x、y 都严格递增。
    // dp[j] = max(dp[i] + 1)，i < j、x_i < x_j、y_i < y_j；调用 cross 时左半的 dp 已经是最终值。
    std::vector<std::array<int, 2>> p{{1, 5}, {2, 2}, {3, 3}, {2, 4}, {4, 4}, {5, 6}};
    std::vector<int> dp(p.size(), 1);
    cp::cdq_divide(0, int(p.size()), [&](int l, int mid, int r) {
        for (int j = mid; j < r; ++j)
            for (int i = l; i < mid; ++i) // 示例直接枚举；实际题目把两半各自按 x 排序，用树状数组维护 y
                if (p[i][0] < p[j][0] && p[i][1] < p[j][1]) dp[j] = std::max(dp[j], dp[i] + 1);
    });
    std::cout << "dp:";
    for (int x : dp) std::cout << ' ' << x;
    std::cout << '\n';
}
