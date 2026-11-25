#pragma once
// 使用说明：cp-stl/docs/usage/data_structures/sqrt_decomposition.md
// 完整示例：cp-stl/examples/data_structures/sqrt_decomposition.cpp
#include <algorithm>
#include <cassert>
#include <cmath>
#include <vector>

namespace cp {
// 分块的下标划分：把 [0,n) 切成每块 block_size 个元素的若干块，最后一块可以不满。
// block_size 传 0 时取 ⌊√n⌋。
struct SqrtBlocks {
    int n, block_size, block_count;
    explicit SqrtBlocks(int size, int block = 0)
        : n(size), block_size(block > 0 ? block : std::max(1, int(std::sqrt(double(size))))),
          block_count((size + block_size - 1) / block_size) { assert(size >= 0); }
    int id(int i) const { return i / block_size; }
    int begin(int b) const { return b * block_size; }
    int end(int b) const { return std::min(n, (b + 1) * block_size); }
    // 把 [l,r) 拆开：被完整覆盖的块调用 whole(b)，两端的零散部分调用 part(b, lo, hi)（[lo,hi) 在块 b 内）。
    template<class Part, class Whole>
    void for_each(int l, int r, Part part, Whole whole) const {
        assert(0 <= l && l <= r && r <= n);
        for (int b = l < r ? id(l) : 0; l < r; ++b) {
            int stop = std::min(r, end(b));
            if (l == begin(b) && stop == end(b)) whole(b);
            else part(b, l, stop);
            l = stop;
        }
    }
};

// 区间加、区间内小于 x 的元素个数。每块保存一份排好序的副本和一个整块的加法标记。
// add 重排两端的块，O(B log B + n/B)；count_less 在整块上二分，O(B + (n/B) log B)。
template<class T = long long>
class RangeAddCountLess {
    SqrtBlocks blocks_;
    std::vector<T> a_, sorted_, lazy_;
    void rebuild(int b) {
        auto first = sorted_.begin() + blocks_.begin(b), last = sorted_.begin() + blocks_.end(b);
        std::copy(a_.begin() + blocks_.begin(b), a_.begin() + blocks_.end(b), first);
        std::sort(first, last);
    }
public:
    explicit RangeAddCountLess(const std::vector<T>& a, int block_size = 0)
        : blocks_(int(a.size()), block_size), a_(a), sorted_(a), lazy_(blocks_.block_count, T()) {
        for (int b = 0; b < blocks_.block_count; ++b) rebuild(b);
    }
    int size() const { return blocks_.n; }
    T get(int i) const { return a_[i] + lazy_[blocks_.id(i)]; }
    void add(int l, int r, T delta) {
        blocks_.for_each(l, r,
            [&](int b, int lo, int hi) { for (int i = lo; i < hi; ++i) a_[i] += delta; rebuild(b); },
            [&](int b) { lazy_[b] += delta; });
    }
    int count_less(int l, int r, T x) const {
        int total = 0;
        blocks_.for_each(l, r,
            [&](int b, int lo, int hi) { for (int i = lo; i < hi; ++i) total += a_[i] + lazy_[b] < x; },
            [&](int b) {
                auto first = sorted_.begin() + blocks_.begin(b), last = sorted_.begin() + blocks_.end(b);
                total += int(std::lower_bound(first, last, x - lazy_[b]) - first);
            });
        return total;
    }
};
} // namespace cp
