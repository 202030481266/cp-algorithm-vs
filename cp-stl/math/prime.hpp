#pragma once
// 使用说明：cp-stl/docs/usage/math/prime.md
// 完整示例：cp-stl/examples/math/prime.cpp
#include <algorithm>
#include <cassert>
#include <cstdint>
#include <numeric>
#include <utility>
#include <vector>
#if defined(_MSC_VER) && !defined(__clang__)
#include <intrin.h>
#endif

namespace cp {
namespace prime_detail {
using u64 = std::uint64_t;
// 64x64 位乘积的高 64 位：GCC/Clang 用 128 位整数，MSVC x64 用 __umulh，其余平台拆成 32 位计算。
inline u64 mul_high(u64 a, u64 b) {
#if defined(__SIZEOF_INT128__)
    return u64((unsigned __int128)a * b >> 64);
#elif defined(_MSC_VER) && (defined(_M_X64) || defined(_M_ARM64))
    return __umulh(a, b);
#else
    u64 a_lo = a & 0xffffffffu, a_hi = a >> 32, b_lo = b & 0xffffffffu, b_hi = b >> 32;
    u64 lo_lo = a_lo * b_lo, hi_lo = a_hi * b_lo, lo_hi = a_lo * b_hi, hi_hi = a_hi * b_hi;
    u64 cross = (lo_lo >> 32) + (hi_lo & 0xffffffffu) + lo_hi;
    return hi_hi + (hi_lo >> 32) + (cross >> 32);
#endif
}
// 奇数模数 n 的 Montgomery 乘法：数 x 表示为 x*2^64 mod n，乘法只需两次 64 位乘高位，不做除法。
struct Montgomery {
    u64 mod, inv, r2; // inv = mod^{-1} mod 2^64，r2 = 2^128 mod mod
    explicit Montgomery(u64 n) : mod(n), inv(n), r2(0) {
        for (int i = 0; i < 5; ++i) inv *= 2 - n * inv; // 牛顿迭代，每次有效位数翻倍
        u64 r = (0 - n) % n;                             // 2^64 mod n
        r2 = r;
        for (int i = 0; i < 64; ++i) r2 = r2 >= n - r2 ? r2 - (n - r2) : r2 + r2;
    }
    // (hi*2^64 + lo) * 2^-64 mod n，要求输入小于 n*2^64。
    u64 reduce(u64 hi, u64 lo) const {
        u64 q = lo * inv, t = mul_high(q, mod);
        return hi >= t ? hi - t : hi - t + mod;
    }
    u64 mul(u64 a, u64 b) const { return reduce(mul_high(a, b), a * b); }
    u64 to(u64 a) const { return mul(a % mod, r2); }
    u64 from(u64 a) const { return reduce(0, a); }
    u64 pow(u64 base, u64 e) const {
        u64 result = to(1);
        for (; e; e >>= 1, base = mul(base, base)) if (e & 1) result = mul(result, base);
        return result;
    }
};
inline constexpr std::uint32_t kSmallPrimes[] = {2, 3, 5, 7, 11, 13, 17, 19, 23, 29, 31, 37};

// Brent 改进的 Pollard-Rho：n 为奇合数，返回 n 的一个非平凡因子（不一定是质数）。
inline u64 pollard_rho(u64 n) {
    Montgomery mont(n);
    const u64 one = mont.to(1);
    for (u64 c0 = 1;; ++c0) {
        const u64 c = mont.to(c0);
        auto f = [&](u64 x) {
            u64 y = mont.mul(x, x), z = y + c;
            if (z < y || z >= n) z -= n; // y、c 都小于 n，和可能超过 2^64
            return z;
        };
        const int batch = 128; // 每 128 步才做一次 gcd，把差的乘积攒起来
        u64 x = 0, y = mont.to(c0 + 1), saved = 0, product = one, g = 1;
        for (int r = 1; g == 1; r <<= 1) {
            x = y;
            for (int i = 0; i < r; ++i) y = f(y);
            for (int k = 0; k < r && g == 1; k += batch) {
                saved = y;
                for (int i = 0; i < batch && i < r - k; ++i) {
                    y = f(y);
                    product = mont.mul(product, x > y ? x - y : y - x);
                }
                g = std::gcd(product, n); // Montgomery 表示乘 2^64，与奇数 n 的 gcd 不变
            }
        }
        if (g == n) { // 一批里跳过了因子：从这一批开头逐步重算
            do {
                saved = f(saved);
                g = std::gcd(x > saved ? x - saved : saved - x, n);
            } while (g == 1);
        }
        if (g != n) return g;
    }
}
} // namespace prime_detail

// 确定性 Miller-Rabin，适用于全部 64 位无符号整数，O(log n) 次 Montgomery 乘法。
inline bool is_prime(std::uint64_t n) {
    using prime_detail::u64;
    if (n < 2) return false;
    for (u64 p : prime_detail::kSmallPrimes) if (n % p == 0) return n == p;
    if (n < 37 * 37) return true;
    prime_detail::Montgomery mont(n);
    u64 d = n - 1;
    int s = 0;
    while (!(d & 1)) { d >>= 1; ++s; }
    const u64 one = mont.to(1), minus_one = mont.to(n - 1);
    for (u64 a : {2ULL, 325ULL, 9375ULL, 28178ULL, 450775ULL, 9780504ULL, 1795265022ULL}) {
        if (a % n == 0) continue;
        u64 x = mont.pow(mont.to(a), d);
        if (x == one || x == minus_one) continue;
        bool composite = true;
        for (int i = 1; i < s && composite; ++i) {
            x = mont.mul(x, x);
            if (x == minus_one) composite = false;
        }
        if (composite) return false;
    }
    return true;
}

// 质因数分解（Pollard-Rho），返回升序的质因子，重复因子重复出现，例如 12 -> {2,2,3}。
// 期望 O(n^{1/4}) 次乘法；n <= 1 时返回空。
inline std::vector<std::uint64_t> factorize(std::uint64_t n) {
    std::vector<std::uint64_t> result;
    if (n <= 1) return result; // 0 能被任何数整除，必须先排除，否则下面的试除不会结束
    for (std::uint64_t p : prime_detail::kSmallPrimes)
        while (n % p == 0) { result.push_back(p); n /= p; }
    std::vector<std::uint64_t> stack;
    if (n > 1) stack.push_back(n);
    while (!stack.empty()) {
        std::uint64_t x = stack.back();
        stack.pop_back();
        if (is_prime(x)) { result.push_back(x); continue; }
        std::uint64_t d = prime_detail::pollard_rho(x);
        stack.push_back(d);
        stack.push_back(x / d);
    }
    std::sort(result.begin(), result.end());
    return result;
}

// 质因数分解的 (质数, 指数) 形式，例如 12 -> {(2,2), (3,1)}。
inline std::vector<std::pair<std::uint64_t, int>> prime_factors(std::uint64_t n) {
    std::vector<std::pair<std::uint64_t, int>> result;
    for (std::uint64_t p : factorize(n)) {
        if (result.empty() || result.back().first != p) result.emplace_back(p, 0);
        ++result.back().second;
    }
    return result;
}

// 全部正因数，升序。n >= 1；10^18 以内的数因数个数不超过 103680。
inline std::vector<std::uint64_t> divisors(std::uint64_t n) {
    assert(n >= 1);
    std::vector<std::uint64_t> result{1};
    for (auto [p, e] : prime_factors(n)) {
        std::size_t count = result.size();
        std::uint64_t power = 1;
        for (int i = 0; i < e; ++i) {
            power *= p;
            for (std::size_t j = 0; j < count; ++j) result.push_back(result[j] * power);
        }
    }
    std::sort(result.begin(), result.end());
    return result;
}

// 欧拉函数 phi(n)：[1,n] 中与 n 互质的数的个数，phi(1) = 1。
inline std::uint64_t euler_phi(std::uint64_t n) {
    assert(n >= 1);
    std::uint64_t result = n;
    for (auto [p, e] : prime_factors(n)) { (void)e; result = result / p * (p - 1); }
    return result;
}
} // namespace cp
