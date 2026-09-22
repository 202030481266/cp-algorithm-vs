#pragma once
// 使用说明：cp-stl/docs/usage/data_structures/segment_tree.md
// 完整示例：cp-stl/examples/data_structures/segment_tree.cpp
#include <cassert>
#include <vector>

namespace cp {
// op 必须结合，identity 是双侧单位元；支持不满足交换律的运算。
// set/prod/max_right O(log n)，区间为 [l,r)。
template<class T, class Op>
class SegmentTree {
    int n_, size_ = 1;
    T identity_;
    Op op_;
    std::vector<T> tree_;
public:
    SegmentTree(int n, T identity, Op op = {})
        : n_(n), identity_(identity), op_(op) {
        while (size_ < n_) size_ <<= 1;
        tree_.assign(size_ * 2, identity_);
    }
    SegmentTree(const std::vector<T>& a, T identity, Op op = {})
        : SegmentTree(int(a.size()), identity, op) {
        for (int i = 0; i < n_; ++i) tree_[size_ + i] = a[i];
        for (int i = size_ - 1; i; --i) tree_[i] = op_(tree_[i * 2], tree_[i * 2 + 1]);
    }
    void set(int p, T value) {
        assert(0 <= p && p < n_);
        tree_[p += size_] = value;
        while (p >>= 1) tree_[p] = op_(tree_[p * 2], tree_[p * 2 + 1]);
    }
    T get(int p) const {
        assert(0 <= p && p < n_);
        return tree_[size_ + p];
    }
    T prod(int l, int r) const {
        assert(0 <= l && l <= r && r <= n_);
        T left = identity_, right = identity_;
        for (l += size_, r += size_; l < r; l >>= 1, r >>= 1) {
            if (l & 1) left = op_(left, tree_[l++]);
            if (r & 1) right = op_(tree_[--r], right);
        }
        return op_(left, right);
    }
    T all_prod() const { return tree_[1]; }
    // pred(identity) 为真，向右扩展后真假只能从真变假。
    // 返回最大的 r 使 pred(prod(l,r)) 为真。
    template<class Predicate>
    int max_right(int l, Predicate pred) const {
        assert(0 <= l && l <= n_ && pred(identity_));
        if (l == n_) return n_;
        l += size_;
        T current = identity_;
        do {
            while (!(l & 1)) l >>= 1;
            if (!pred(op_(current, tree_[l]))) {
                while (l < size_) {
                    l <<= 1;
                    if (pred(op_(current, tree_[l]))) current = op_(current, tree_[l++]);
                }
                return l - size_;
            }
            current = op_(current, tree_[l++]);
        } while ((l & -l) != l);
        return n_;
    }
};
} // namespace cp
