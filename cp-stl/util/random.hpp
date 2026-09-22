#pragma once
// 使用说明：cp-stl/docs/usage/util/random.md
// 完整示例：cp-stl/examples/util/random.cpp
#include <algorithm>
#include <cassert>
#include <chrono>
#include <cstdint>
#include <random>
#include <vector>

namespace cp {
// 对拍时传固定 seed，失败用同一 seed 重现；正常使用默认随机种子。
struct Random {
    std::mt19937_64 engine;
    explicit Random(std::uint64_t seed = std::chrono::steady_clock::now().time_since_epoch().count())
        : engine(seed) {}
    long long integer(long long lo, long long hi) {
        assert(lo <= hi);
        return std::uniform_int_distribution<long long>(lo, hi)(engine);
    }
    std::vector<long long> array(int n, long long lo, long long hi) {
        std::vector<long long> a(n);
        for (auto& x : a) x = integer(lo, hi);
        return a;
    }
    template<class T> void shuffle(std::vector<T>& a) { std::shuffle(a.begin(), a.end(), engine); }
};
} // namespace cp
