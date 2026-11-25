#pragma once
// 使用说明：cp-stl/docs/usage/data_structures/dynamic_range_kth.md
// 完整示例：cp-stl/examples/data_structures/dynamic_range_kth.cpp
#include <cassert>
#include <vector>

namespace cp {
// 树套树（树状数组套动态开点权值线段树）：带单点修改的区间第 k 小与区间排名。
// 下标 [0,n)，值必须在 [0, value_limit) 内；值域大时把初值和所有修改值一起离散化。
// set / count_less / kth 都是 O(log n · log value_limit)。
// 节点数不超过 (n + 2·修改次数) · ⌈log₂ n⌉ · (⌈log₂ value_limit⌉ + 1)，每个节点 12 字节。
class DynamicRangeKth {
    struct Node { int left = 0, right = 0, count = 0; };
    int n_, limit_;
    std::vector<int> value_, root_;
    std::vector<Node> nodes_ = std::vector<Node>(1); // 0 号是空节点
    void update(int pos, int value, int delta) {
        for (int i = pos + 1; i <= n_; i += i & -i) {
            if (!root_[i]) { root_[i] = int(nodes_.size()); nodes_.emplace_back(); }
            int p = root_[i], lo = 0, hi = limit_;
            while (true) {
                nodes_[p].count += delta;
                if (hi - lo == 1) break;
                int mid = lo + (hi - lo) / 2;
                bool go_left = value < mid;
                int child = go_left ? nodes_[p].left : nodes_[p].right;
                if (!child) {
                    child = int(nodes_.size());
                    nodes_.emplace_back();
                    (go_left ? nodes_[p].left : nodes_[p].right) = child;
                }
                p = child;
                (go_left ? hi : lo) = mid;
            }
        }
    }
    // 前缀 [0,r) 在树状数组上对应的内层根。
    int collect(int r, int* roots) const {
        int k = 0;
        for (; r > 0; r -= r & -r) roots[k++] = root_[r];
        return k;
    }
public:
    DynamicRangeKth(const std::vector<int>& a, int value_limit)
        : n_(int(a.size())), limit_(value_limit), value_(a), root_(a.size() + 1) {
        assert(value_limit > 0);
        for (int i = 0; i < n_; ++i) {
            assert(0 <= a[i] && a[i] < limit_);
            update(i, a[i], 1);
        }
    }
    void reserve(int nodes) { nodes_.reserve(nodes); }
    int size() const { return n_; }
    int node_count() const { return int(nodes_.size()); }
    int get(int pos) const { return value_[pos]; }
    void set(int pos, int value) {
        assert(0 <= pos && pos < n_ && 0 <= value && value < limit_);
        if (value_[pos] == value) return;
        update(pos, value_[pos], -1);
        update(pos, value_[pos] = value, 1);
    }
    // [l,r) 中严格小于 x 的元素个数。
    int count_less(int l, int r, int x) const {
        assert(0 <= l && l <= r && r <= n_);
        if (x <= 0) return 0;
        if (x >= limit_) return r - l;
        int plus[32], minus[32], np = collect(r, plus), nm = collect(l, minus);
        int lo = 0, hi = limit_, result = 0; // 始终有 lo < x < hi
        while (true) {
            int mid = lo + (hi - lo) / 2;
            if (x > mid) {
                for (int i = 0; i < np; ++i) { result += nodes_[nodes_[plus[i]].left].count; plus[i] = nodes_[plus[i]].right; }
                for (int i = 0; i < nm; ++i) { result -= nodes_[nodes_[minus[i]].left].count; minus[i] = nodes_[minus[i]].right; }
                lo = mid;
            } else {
                for (int i = 0; i < np; ++i) plus[i] = nodes_[plus[i]].left;
                for (int i = 0; i < nm; ++i) minus[i] = nodes_[minus[i]].left;
                hi = mid;
                if (x == mid) { // 当前节点的值域 [lo,mid) 全部小于 x
                    for (int i = 0; i < np; ++i) result += nodes_[plus[i]].count;
                    for (int i = 0; i < nm; ++i) result -= nodes_[minus[i]].count;
                    return result;
                }
            }
        }
    }
    // [l,r) 中值在 [lo,hi) 内的元素个数。
    int count(int l, int r, int lo, int hi) const { return lo < hi ? count_less(l, r, hi) - count_less(l, r, lo) : 0; }
    // [l,r) 中第 k 小的值，k 从 0 开始，要求 0 <= k < r-l。
    int kth(int l, int r, int k) const {
        assert(0 <= l && l <= r && r <= n_ && 0 <= k && k < r - l);
        int plus[32], minus[32], np = collect(r, plus), nm = collect(l, minus);
        int lo = 0, hi = limit_;
        while (hi - lo > 1) {
            int mid = lo + (hi - lo) / 2, left_count = 0;
            for (int i = 0; i < np; ++i) left_count += nodes_[nodes_[plus[i]].left].count;
            for (int i = 0; i < nm; ++i) left_count -= nodes_[nodes_[minus[i]].left].count;
            bool go_left = k < left_count;
            if (!go_left) k -= left_count;
            for (int i = 0; i < np; ++i) plus[i] = go_left ? nodes_[plus[i]].left : nodes_[plus[i]].right;
            for (int i = 0; i < nm; ++i) minus[i] = go_left ? nodes_[minus[i]].left : nodes_[minus[i]].right;
            (go_left ? hi : lo) = mid;
        }
        return lo;
    }
};
} // namespace cp
