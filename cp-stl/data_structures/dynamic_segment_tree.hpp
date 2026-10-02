#pragma once
// 使用说明：cp-stl/docs/usage/data_structures/dynamic_segment_tree.md
// 完整示例：cp-stl/examples/data_structures/dynamic_segment_tree.cpp
#include <algorithm>
#include <cassert>
#include <limits>
#include <vector>

namespace cp {
// 动态开点线段树：下标范围 [lo,hi) 可达 1e18，初始全为 0；在线区间加、区间和、区间最大值。
// 采用标记永久化，修改只创建经过的节点；每次操作 O(log(hi-lo)) 时间并新建 O(log(hi-lo)) 个节点。
// 要求 hi-lo 在 long long 范围内；和与 delta*长度的中间值也必须在 long long 范围内。
class DynamicSegmentTree {
    struct Node {
        int left = 0, right = 0;
        long long sum = 0;  // 本子树的和，已包含本节点及以下的全部标记。
        long long max = 0;  // 本子树的最大值，不含祖先的永久标记。
        long long tag = 0;  // 整个子树都加过的值（永久标记，不下传）。
    };
    long long lo_, hi_;
    std::vector<Node> nodes_{Node(), Node()}; // 0 号为空节点，1 号为根。
    static long long overlap(long long l, long long r, long long ql, long long qr) {
        return std::min(r, qr) - std::max(l, ql);
    }
    long long child_max(int p) const { return p ? nodes_[p].max : 0; } // 空儿子对应全 0 区间。
    void add(int p, long long l, long long r, long long ql, long long qr, long long delta) {
        nodes_[p].sum += delta * overlap(l, r, ql, qr);
        if (ql <= l && r <= qr) {
            nodes_[p].tag += delta;
            nodes_[p].max += delta;
            return;
        }
        long long m = l + (r - l) / 2;
        if (ql < m) {
            if (!nodes_[p].left) { int c = int(nodes_.size()); nodes_.emplace_back(); nodes_[p].left = c; }
            add(nodes_[p].left, l, m, ql, qr, delta);
        }
        if (m < qr) {
            if (!nodes_[p].right) { int c = int(nodes_.size()); nodes_.emplace_back(); nodes_[p].right = c; }
            add(nodes_[p].right, m, r, ql, qr, delta);
        }
        nodes_[p].max = nodes_[p].tag + std::max(child_max(nodes_[p].left), child_max(nodes_[p].right));
    }
    long long sum(int p, long long l, long long r, long long ql, long long qr) const {
        if (ql <= l && r <= qr) return nodes_[p].sum;
        long long result = nodes_[p].tag * overlap(l, r, ql, qr), m = l + (r - l) / 2;
        if (ql < m && nodes_[p].left) result += sum(nodes_[p].left, l, m, ql, qr);
        if (m < qr && nodes_[p].right) result += sum(nodes_[p].right, m, r, ql, qr);
        return result;
    }
    long long max(int p, long long l, long long r, long long ql, long long qr) const {
        if (ql <= l && r <= qr) return nodes_[p].max;
        long long m = l + (r - l) / 2, best = std::numeric_limits<long long>::min();
        if (ql < m) best = std::max(best, nodes_[p].left ? max(nodes_[p].left, l, m, ql, qr) : 0);
        if (m < qr) best = std::max(best, nodes_[p].right ? max(nodes_[p].right, m, r, ql, qr) : 0);
        return nodes_[p].tag + best;
    }
public:
    // reserve_nodes 可预留节点数，单次区间加至多新建约 4*log2(hi-lo) 个节点。
    DynamicSegmentTree(long long lo, long long hi, int reserve_nodes = 0) : lo_(lo), hi_(hi) {
        assert(lo < hi);
        nodes_.reserve(std::max(reserve_nodes, 2));
    }
    // a[i] += delta，i in [l,r)。
    void add(long long l, long long r, long long delta) {
        assert(lo_ <= l && l <= r && r <= hi_);
        if (l < r) add(1, lo_, hi_, l, r, delta);
    }
    long long sum(long long l, long long r) const {
        assert(lo_ <= l && l <= r && r <= hi_);
        return l == r ? 0 : sum(1, lo_, hi_, l, r);
    }
    // 非空区间的最大值。
    long long max(long long l, long long r) const {
        assert(lo_ <= l && l < r && r <= hi_);
        return max(1, lo_, hi_, l, r);
    }
    long long all_max() const { return nodes_[1].max; }
    int node_count() const { return int(nodes_.size()); }
};
} // namespace cp
