#pragma once
// 使用说明：cp-stl/docs/usage/math/number_theory.md
// 完整示例：cp-stl/examples/math/number_theory.cpp
#include <cassert>
#include <limits>
#include <optional>
#include <utility>
#include <vector>

namespace cp {
namespace number_theory_detail {
// 0 <= a,b < mod <= LLONG_MAX。GNU/Clang 使用原生宽整数；MSVC 使用精确整数运算。
inline long long multiply_mod(long long a, long long b, long long mod) {
#if defined(__SIZEOF_INT128__)
    return static_cast<long long>(static_cast<__int128>(a) * b % mod);
#else
    using U = unsigned long long;
    U x = static_cast<U>(a), y = static_cast<U>(b), m = static_cast<U>(mod);
    if (x == 0 || y <= std::numeric_limits<U>::max() / x)
        return static_cast<long long>(x * y % m);
    U result = 0;
    while (y) {
        if (y & 1) result = (result + x) % m;
        x = (x + x) % m; // 两个模值之和小于 2^64，不溢出。
        y >>= 1;
    }
    return static_cast<long long>(result);
#endif
}
} // namespace number_theory_detail

// mod > 0，exponent >= 0。MSVC 对大模数的溢出乘积用 O(log mod) 次加倍处理。
inline long long pow_mod(long long a, long long exponent, long long mod) {
    assert(mod > 0 && exponent >= 0);
    a %= mod;
    if (a < 0) a += mod;
    long long result = 1 % mod;
    while (exponent) {
        if (exponent & 1) result = number_theory_detail::multiply_mod(result, a, mod);
        a = number_theory_detail::multiply_mod(a, a, mod);
        exponent >>= 1;
    }
    return result;
}
// 逆元：支持非质数模数，gcd(a,mod)!=1 返回 nullopt，mod 必须 >1。
inline std::optional<long long> inverse_mod(long long a, long long mod) {
    assert(mod > 1);
    a %= mod;
    if (a < 0) a += mod;
    long long b = mod;
    long long x = 1, y = 0;
    while (b) {
        long long q = a / b, next = a % b;
        a = b; b = next;
        // 系数始终保存为 [0,mod) 的模值，避免中间乘积溢出。
        long long product = number_theory_detail::multiply_mod(q % mod, y, mod);
        long long z = x >= product ? x - product : mod - (product - x);
        x = y; y = z;
    }
    if (a != 1) return std::nullopt;
    return x;
}
// 线性筛 O(n)，factorize(x) 只接受 1<=x<=n。
struct PrimeSieve {
    std::vector<int> primes, spf;
    explicit PrimeSieve(int n) : spf(n + 1) {
        for (int i = 2; i <= n; ++i) {
            if (!spf[i]) { spf[i] = i; primes.push_back(i); }
            for (int p : primes) {
                if (p > spf[i] || 1LL * p * i > n) break;
                spf[p * i] = p;
            }
        }
    }
    bool is_prime(int x) const {
        assert(0 <= x && x < int(spf.size()));
        return x >= 2 && spf[x] == x;
    }
    std::vector<std::pair<int, int>> factorize(int x) const {
        assert(1 <= x && x < int(spf.size()));
        std::vector<std::pair<int, int>> result;
        while (x > 1) {
            int p = spf[x], count = 0;
            do { x /= p; ++count; } while (x % p == 0);
            result.emplace_back(p, count);
        }
        return result;
    }
};
} // namespace cp
