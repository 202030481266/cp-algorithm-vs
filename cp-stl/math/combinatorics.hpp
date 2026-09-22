#pragma once
// 使用说明：cp-stl/docs/usage/math/combinatorics.md
// 完整示例：cp-stl/examples/math/combinatorics.cpp
#include <cassert>
#include <vector>
#include "modint.hpp"

namespace cp {
// 模数必须为质数，且预处理 n < 模数；预处理 O(n+log Mod)，查询 O(1)。
template<class M = Mint>
struct Combinations {
    std::vector<M> fact, inv_fact;
    explicit Combinations(int n) : fact(n + 1, 1), inv_fact(n + 1, 1) {
        assert(0 <= n && n < M::mod());
        for (int i = 1; i <= n; ++i) fact[i] = fact[i - 1] * i;
        inv_fact[n] = fact[n].inv();
        for (int i = n; i > 0; --i) inv_fact[i - 1] = inv_fact[i] * i;
    }
    M choose(int n, int k) const {
        if (n < 0 || k < 0 || k > n) return 0;
        assert(n < int(fact.size()));
        return fact[n] * inv_fact[k] * inv_fact[n - k];
    }
    M permute(int n, int k) const {
        if (n < 0 || k < 0 || k > n) return 0;
        assert(n < int(fact.size()));
        return fact[n] * inv_fact[n - k];
    }
};
} // namespace cp
