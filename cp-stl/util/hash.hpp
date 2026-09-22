#pragma once
// 使用说明：cp-stl/docs/usage/util/hash.md
// 完整示例：cp-stl/examples/util/hash.cpp
#include <chrono>
#include <cstddef>
#include <cstdint>

namespace cp {
// 给 unordered_map<long long,T,cp::SafeHash> 使用。
// 随机盐降低被构造碰撞的风险，并非“绝不可能被 hack”。
struct SafeHash {
    static std::uint64_t splitmix64(std::uint64_t x) {
        x += 0x9e3779b97f4a7c15ULL;
        x = (x ^ (x >> 30)) * 0xbf58476d1ce4e5b9ULL;
        x = (x ^ (x >> 27)) * 0x94d049bb133111ebULL;
        return x ^ (x >> 31);
    }
    std::size_t operator()(std::uint64_t x) const {
        static const std::uint64_t salt = std::chrono::steady_clock::now().time_since_epoch().count();
        return static_cast<std::size_t>(splitmix64(x + salt));
    }
};
} // namespace cp
