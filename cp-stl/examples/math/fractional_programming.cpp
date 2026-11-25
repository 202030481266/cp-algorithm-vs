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
