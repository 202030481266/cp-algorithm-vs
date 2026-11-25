#pragma once
// 使用说明：cp-stl/docs/usage/math/fractional_programming.md
// 完整示例：cp-stl/examples/math/fractional_programming.cpp
#include <algorithm>
#include <cassert>
#include <cmath>
#include <numeric>
#include <utility>
#include <vector>

namespace cp {
// 实数二分：feasible 在 [lo,hi] 上先真后假（feasible(lo) 为真），返回分界点。
// 01 分数规划的判定：比值能达到 x  <=>  max Σ(a_i − x·b_i) >= 0。
template<class Feasible>
double bisect_last_true(double lo, double hi, Feasible feasible, int iterations = 100) {
    for (int i = 0; i < iterations; ++i) {
        double mid = (lo + hi) / 2;
        (feasible(mid) ? lo : hi) = mid;
    }
    return lo;
}

// Dinkelbach 迭代：求 max Σa / Σb（要求 Σb > 0）。best(x) 返回使 Σ(a_i − x·b_i) 最大的方案的 {Σa, Σb}，
// 每轮把 x 换成这个方案的比值，直到不再变大。超线性收敛，通常不到 10 轮，比二分少调用很多次 best。
// 求最小比值时把 a 取负，结果再取负。
template<class Best>
double dinkelbach(Best best, double x = 0, double eps = 1e-12, int max_rounds = 100) {
    for (int round = 0; round < max_rounds; ++round) {
        auto [a, b] = best(x);
        double ratio = double(a) / double(b);
        // 第 0 轮只是把任意初值换成一个真实方案的比值，之后比值单调不降。
        if (round > 0 && ratio <= x + eps * std::max(1.0, std::abs(x))) return std::max(x, ratio);
        x = ratio;
    }
    return x;
}

// 从 n 个物品中恰好选 k 个，使 Σa / Σb 最大（b_i > 0，1 <= k <= n）。每轮 O(n)。
template<class T>
double max_ratio_choose_k(const std::vector<T>& a, const std::vector<T>& b, int k) {
    int n = int(a.size());
    assert(a.size() == b.size() && 1 <= k && k <= n);
    std::vector<int> ids(n);
    std::iota(ids.begin(), ids.end(), 0);
    return dinkelbach([&](double x) {
        std::nth_element(ids.begin(), ids.begin() + (k - 1), ids.end(),
                         [&](int i, int j) { return a[i] - x * b[i] > a[j] - x * b[j]; });
        double sum_a = 0, sum_b = 0;
        for (int t = 0; t < k; ++t) { sum_a += double(a[ids[t]]); sum_b += double(b[ids[t]]); }
        return std::pair<double, double>(sum_a, sum_b);
    });
}
} // namespace cp
