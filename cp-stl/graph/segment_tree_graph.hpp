#pragma once
// 使用说明：cp-stl/docs/usage/graph/segment_tree_graph.md
// 完整示例：cp-stl/examples/graph/segment_tree_graph.cpp
#include <cassert>
#include <vector>
#include "shortest_path.hpp"

namespace cp {
// 线段树优化建图：点 0..n-1 是原图的点，另建两棵线段树的内部点。
// “出树”边从父亲指向儿子（边权 0），用于 点 -> 区间；“入树”边从儿子指向父亲（边权 0），用于 区间 -> 点。
// 每条区间边只增加 O(log n) 条实际的边。建好后对 graph() 跑 dijkstra / zero_one_bfs，前 n 个结果就是原图的点。
class SegmentTreeGraph {
    int n_;
    WeightedGraph g_;
    // 自底向上的线段树：叶子 [n, 2n) 对应原图点 0..n-1；内部点 1..n-1 在两棵树中各有一个编号。
    int out_id(int i) const { return i >= n_ ? i - n_ : n_ + i - 1; }
    int in_id(int i) const { return i >= n_ ? i - n_ : 2 * n_ - 1 + i - 1; }
    template<class F>
    void cover(int l, int r, F f) const {
        for (l += n_, r += n_; l < r; l >>= 1, r >>= 1) {
            if (l & 1) f(l++);
            if (r & 1) f(--r);
        }
    }
public:
    explicit SegmentTreeGraph(int n) : n_(n), g_(n > 0 ? 3 * n - 2 : 0) {
        assert(n >= 0);
        for (int i = 1; i < n; ++i) {
            for (int c = 2 * i; c <= 2 * i + 1; ++c) {
                g_[out_id(i)].push_back({out_id(c), 0});
                g_[in_id(c)].push_back({in_id(i), 0});
            }
        }
    }
    int size() const { return n_; }
    int node_count() const { return int(g_.size()); }
    const WeightedGraph& graph() const { return g_; }
    void add_edge(int u, int v, long long w) {
        assert(0 <= u && u < n_ && 0 <= v && v < n_);
        g_[u].push_back({v, w});
    }
    // u -> [l,r) 中每个点，边权 w。
    void add_edge_to_range(int u, int l, int r, long long w) {
        assert(0 <= u && u < n_ && 0 <= l && l <= r && r <= n_);
        cover(l, r, [&](int x) { g_[u].push_back({out_id(x), w}); });
    }
    // [l,r) 中每个点 -> v，边权 w。
    void add_edge_from_range(int l, int r, int v, long long w) {
        assert(0 <= v && v < n_ && 0 <= l && l <= r && r <= n_);
        cover(l, r, [&](int x) { g_[in_id(x)].push_back({v, w}); });
    }
    // [l1,r1) 中每个点 -> [l2,r2) 中每个点，边权 w：新建一个虚点，O(log n) 条边。
    void add_range_to_range(int l1, int r1, int l2, int r2, long long w) {
        assert(0 <= l1 && l1 <= r1 && r1 <= n_ && 0 <= l2 && l2 <= r2 && r2 <= n_);
        int hub = int(g_.size());
        g_.emplace_back();
        cover(l1, r1, [&](int x) { g_[in_id(x)].push_back({hub, w}); });
        cover(l2, r2, [&](int x) { g_[hub].push_back({out_id(x), 0}); });
    }
};
} // namespace cp
