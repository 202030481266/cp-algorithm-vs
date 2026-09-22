#pragma once
// 使用说明：cp-stl/docs/usage/basic/sort.md
// 完整示例：cp-stl/examples/basic/sort.cpp
#include <algorithm>
#include <array>
#include <cstddef>
#include <limits>
#include <numeric>
#include <stdexcept>
#include <type_traits>
#include <vector>

namespace cp {
// 稳定升序计数排序，直接修改 a；T 为除 bool 外的整数类型。
// K = max(a) - min(a) + 1；时间 O(n + K)，额外空间 O(n + K)。
// 默认最多分配 100 万个计数桶；K 超过 max_range 时抛出 length_error，a 不变。
template<class T>
void counting_sort(std::vector<T>& a, std::size_t max_range = 1'000'000) {
    static_assert(std::is_integral_v<T> && !std::is_same_v<T, bool>,
                  "counting_sort requires a non-bool integer type");
    using U = std::make_unsigned_t<T>;
    if (a.size() < 2) return;
    auto bounds = std::minmax_element(a.begin(), a.end());
    T minimum = *bounds.first;
    // 无符号减法避免跨越正负边界时溢出；转回 U 兼容窄整数的整型提升。
    U span = static_cast<U>(static_cast<U>(*bounds.second) - static_cast<U>(minimum));
    std::vector<std::size_t> count;
    if (span >= max_range || span >= count.max_size())
        throw std::length_error("counting_sort range too large; use radix_sort or raise max_range");
    count.resize(static_cast<std::size_t>(span) + 1);
    auto index = [minimum](T x) {
        return static_cast<std::size_t>(static_cast<U>(static_cast<U>(x) - static_cast<U>(minimum)));
    };
    for (T x : a) ++count[index(x)];
    for (std::size_t i = 1; i < count.size(); ++i) count[i] += count[i - 1];
    std::vector<T> buffer(a.size());
    for (std::size_t i = a.size(); i > 0; --i) {
        T x = a[i - 1];
        buffer[--count[index(x)]] = x;
    }
    a.swap(buffer);
}

// 稳定升序 LSD 基数排序，直接修改 a；T 为除 bool 外的整数类型。
// 每轮处理 8 位，时间 O(ceil(w / 8) * (n + 256))，额外空间 O(n + 256)。
// w 为对应无符号类型的位数；支持负数和完整整数边界，不对最小负数取绝对值。
template<class T>
void radix_sort(std::vector<T>& a) {
    static_assert(std::is_integral_v<T> && !std::is_same_v<T, bool>,
                  "radix_sort requires a non-bool integer type");
    using U = std::make_unsigned_t<T>;
    constexpr int bits = std::numeric_limits<U>::digits;
    constexpr U sign_mask = std::is_signed_v<T> ? U{1} << (bits - 1) : U{0};
    if (a.size() < 2) return;
    std::vector<T> buffer(a.size());
    for (int shift = 0; shift < bits; shift += 8) {
        std::array<std::size_t, 256> count{};
        // 翻转符号位，使负数排在非负数前面；始终移动原值，避免转回有符号数。
        auto digit = [shift](T x) {
            U key = static_cast<U>(static_cast<U>(x) ^ sign_mask);
            return static_cast<std::size_t>((key >> shift) & U{255});
        };
        for (T x : a) ++count[digit(x)];
        for (std::size_t i = 1; i < count.size(); ++i) count[i] += count[i - 1];
        for (std::size_t i = a.size(); i > 0; --i) {
            T x = a[i - 1];
            buffer[--count[digit(x)]] = x;
        }
        a.swap(buffer);
    }
}

// 返回稳定升序排列的原下标 ids，a[ids[i]] 为第 i 个排序结果；不修改 a。
// 时间 O(n + K)，空间 O(n + K)；值域限制与 counting_sort 相同。
template<class T>
std::vector<std::size_t> counting_sort_ids(const std::vector<T>& a,
                                          std::size_t max_range = 1'000'000) {
    static_assert(std::is_integral_v<T> && !std::is_same_v<T, bool>,
                  "counting_sort_ids requires a non-bool integer type");
    using U = std::make_unsigned_t<T>;
    if (a.size() < 2) return std::vector<std::size_t>(a.size(), 0);
    auto bounds = std::minmax_element(a.begin(), a.end());
    T minimum = *bounds.first;
    U span = static_cast<U>(static_cast<U>(*bounds.second) - static_cast<U>(minimum));
    std::vector<std::size_t> count;
    if (span >= max_range || span >= count.max_size())
        throw std::length_error("counting_sort_ids range too large; use radix_sort_ids or raise max_range");
    count.resize(static_cast<std::size_t>(span) + 1);
    auto index = [minimum](T x) {
        return static_cast<std::size_t>(static_cast<U>(static_cast<U>(x) - static_cast<U>(minimum)));
    };
    for (T x : a) ++count[index(x)];
    for (std::size_t i = 1; i < count.size(); ++i) count[i] += count[i - 1];
    std::vector<std::size_t> ids(a.size());
    for (std::size_t i = a.size(); i > 0; --i)
        ids[--count[index(a[i - 1])]] = i - 1;
    return ids;
}

// 返回稳定升序排列的原下标 ids；相等元素按原下标升序排列，不修改 a。
// 每轮只重排下标，时间 O(ceil(w / 8) * (n + 256))，空间 O(n + 256)。
template<class T>
std::vector<std::size_t> radix_sort_ids(const std::vector<T>& a) {
    static_assert(std::is_integral_v<T> && !std::is_same_v<T, bool>,
                  "radix_sort_ids requires a non-bool integer type");
    using U = std::make_unsigned_t<T>;
    constexpr int bits = std::numeric_limits<U>::digits;
    constexpr U sign_mask = std::is_signed_v<T> ? U{1} << (bits - 1) : U{0};
    std::vector<std::size_t> ids(a.size());
    std::iota(ids.begin(), ids.end(), std::size_t{0});
    if (a.size() < 2) return ids;
    std::vector<std::size_t> buffer(a.size());
    for (int shift = 0; shift < bits; shift += 8) {
        std::array<std::size_t, 256> count{};
        auto digit = [shift](T x) {
            U key = static_cast<U>(static_cast<U>(x) ^ sign_mask);
            return static_cast<std::size_t>((key >> shift) & U{255});
        };
        // 桶频次与当前排列无关，顺序扫描原数组即可。
        for (T x : a) ++count[digit(x)];
        for (std::size_t i = 1; i < count.size(); ++i) count[i] += count[i - 1];
        for (std::size_t i = ids.size(); i > 0; --i) {
            std::size_t id = ids[i - 1];
            buffer[--count[digit(a[id])]] = id;
        }
        ids.swap(buffer);
    }
    return ids;
}
} // namespace cp
