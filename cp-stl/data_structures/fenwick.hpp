#pragma once
// 使用说明：cp-stl/docs/usage/data_structures/fenwick.md
// 完整示例：cp-stl/examples/data_structures/fenwick.cpp
#include <cassert>
#include <vector>

namespace cp {
// 0-based；prefix(r) = [0,r)，sum(l,r) = [l,r)，修改/查询 O(log n)。
template<class T = long long>
struct Fenwick {
    int n;
    std::vector<T> bit;
    explicit Fenwick(int size = 0) : n(size), bit(n + 1, T{}) {}
    explicit Fenwick(const std::vector<T>& a) : Fenwick(int(a.size())) {
        for (int i = 1; i <= n; ++i) {
            bit[i] += a[i - 1];
            int j = i + (i & -i);
            if (j <= n) bit[j] += bit[i];
        }
    }
    void add(int p, T delta) {
        assert(0 <= p && p < n);
        for (++p; p <= n; p += p & -p) bit[p] += delta;
    }
    T prefix(int r) const {
        assert(0 <= r && r <= n);
        T result{};
        for (; r > 0; r -= r & -r) result += bit[r];
        return result;
    }
    T sum(int l, int r) const {
        assert(0 <= l && l <= r && r <= n);
        return prefix(r) - prefix(l);
    }
    // 所有元素非负时：返回最小的 p 使 prefix(p+1) >= target。
    // target <= 0 返回 0；不存在返回 n；可用于频次数组找第 k 小。
    int lower_bound(T target) const {
        if (target <= T{}) return 0;
        int p = 0, step = 1;
        T accumulated{};
        while (step <= n / 2) step <<= 1;
        for (; step; step >>= 1) {
            if (p + step <= n && accumulated + bit[p + step] < target) {
                accumulated += bit[p + step];
                p += step;
            }
        }
        return p;
    }
};
} // namespace cp
