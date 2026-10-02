#pragma once
// 使用说明：cp-stl/docs/usage/math/crt.md
// 完整示例：cp-stl/examples/math/crt.cpp
#include <cassert>
#include <limits>
#include <optional>
#include <utility>
#include <vector>
#include "number_theory.hpp"

namespace cp {
struct ExtGcd { long long g, x, y; };
// 扩展欧几里得：返回 g = gcd(a,b) >= 0 以及一组 x、y，使 a*x + b*y = g。
// 迭代实现；a、b 不同时为 0 时 |x| <= |b|/g、|y| <= |a|/g，不会溢出。要求 a、b 都不是 LLONG_MIN。
inline ExtGcd ext_gcd(long long a, long long b) {
    long long x0 = 1, y0 = 0, x1 = 0, y1 = 1;
    while (b != 0) {
        long long q = a / b, t;
        t = a - q * b; a = b; b = t;
        t = x0 - q * x1; x0 = x1; x1 = t;
        t = y0 - q * y1; y0 = y1; y1 = t;
    }
    if (a < 0) { a = -a; x0 = -x0; y0 = -y0; }
    return {a, x0, y0};
}

// 线性同余方程 a*x ≡ b (mod m)，m >= 1。有解时返回 (最小非负解 x, 周期 m/g)，全部解为 x + k*(m/g)；
// 无解（b 不是 gcd(a,m) 的倍数）返回 nullopt。中间乘法不会溢出。
inline std::optional<std::pair<long long, long long>> solve_congruence(long long a, long long b, long long m) {
    assert(m >= 1);
    a %= m; if (a < 0) a += m;
    b %= m; if (b < 0) b += m;
    ExtGcd e = ext_gcd(a, m);
    if (b % e.g != 0) return std::nullopt;
    long long period = m / e.g, x = e.x % period;
    if (x < 0) x += period;
    return std::make_pair(number_theory_detail::multiply_mod(x, (b / e.g) % period, period), period);
}

// 中国剩余定理（模数可以不互质，即扩展 CRT）：x ≡ remainders[i] (mod moduli[i])，moduli[i] >= 1。
// 有解时返回 (x, lcm)，0 <= x < lcm，全部解为 x + k*lcm；无解返回 nullopt。要求最终的 lcm 不超过 LLONG_MAX。
inline std::optional<std::pair<long long, long long>> crt(const std::vector<long long>& remainders,
                                                          const std::vector<long long>& moduli) {
    assert(remainders.size() == moduli.size());
    long long x = 0, lcm = 1; // 当前解为 x (mod lcm)
    for (std::size_t i = 0; i < moduli.size(); ++i) {
        long long m = moduli[i], r = remainders[i] % m;
        assert(m >= 1);
        if (r < 0) r += m;
        // 求 t 使 x + lcm*t ≡ r (mod m)，即 lcm*t ≡ r - x (mod m)。
        long long diff = (r - x % m) % m;
        if (diff < 0) diff += m;
        auto solution = solve_congruence(lcm % m, diff, m);
        if (!solution) return std::nullopt;
        auto [t, period] = *solution;
        assert(lcm <= std::numeric_limits<long long>::max() / period); // 新的 lcm 必须在 long long 内
        x += lcm * t; // x < lcm 且 t < period，结果小于新的 lcm，不会溢出
        lcm *= period;
    }
    return std::make_pair(x, lcm);
}

// 二元一次不定方程 a*x + b*y = c（a、b 都不为 0）的全部整数解：x = x0 + k*dx，y = y0 - k*dy，k 为任意整数。
// x0 是最小的非负 x，dx = |b|/g > 0；无解返回 nullopt。要求 |a*b/g| + |c| 在 long long 范围内。
struct Diophantine { long long x0, y0, dx, dy; };
inline std::optional<Diophantine> solve_diophantine(long long a, long long b, long long c) {
    assert(a != 0 && b != 0);
    ExtGcd e = ext_gcd(a, b);
    if (c % e.g != 0) return std::nullopt;
    long long dx = (b < 0 ? -b : b) / e.g, dy = a / e.g * (b < 0 ? -1 : 1);
    long long x = e.x % dx, k = (c / e.g) % dx;
    if (x < 0) x += dx;
    if (k < 0) k += dx;
    long long x0 = number_theory_detail::multiply_mod(x, k, dx); // x0 ≡ e.x * c/g (mod dx)
    long long y0 = (c - a * x0) / b;
    return Diophantine{x0, y0, dx, dy};
}
} // namespace cp
