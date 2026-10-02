#pragma once
// 使用说明：cp-stl/docs/usage/data_structures/segment_tree_beats.md
// 完整示例：cp-stl/examples/data_structures/segment_tree_beats.cpp
#include <algorithm>
#include <cassert>
#include <limits>
#include <vector>

namespace cp {
// 吉司机线段树（Segment Tree Beats）：区间 chmin / chmax / 加，查询区间和、最小值、最大值。
// 0-based，区间 [l,r)。均摊 O((n+q) log^2 n)；没有区间加时为 O((n+q) log n)。
// 元素与区间和都必须在 long long 范围内；long long 的最小/最大值保留为内部哨兵。
class SegmentTreeBeats {
    static constexpr long long INF = std::numeric_limits<long long>::max();
    static constexpr long long NEG = std::numeric_limits<long long>::min();
    struct Node {
        long long sum = 0, add = 0;
        long long max1 = NEG, max2 = NEG, min1 = INF, min2 = INF; // 最大/严格次大，最小/严格次小。
        int max_count = 0, min_count = 0;
    };
    int n_;
    std::vector<Node> t_;
    void pull(int p) {
        Node& x = t_[p];
        const Node &a = t_[2 * p], &b = t_[2 * p + 1];
        x.sum = a.sum + b.sum;
        if (a.max1 == b.max1) {
            x.max1 = a.max1; x.max_count = a.max_count + b.max_count;
            x.max2 = std::max(a.max2, b.max2);
        } else if (a.max1 > b.max1) {
            x.max1 = a.max1; x.max_count = a.max_count;
            x.max2 = std::max(a.max2, b.max1);
        } else {
            x.max1 = b.max1; x.max_count = b.max_count;
            x.max2 = std::max(a.max1, b.max2);
        }
        if (a.min1 == b.min1) {
            x.min1 = a.min1; x.min_count = a.min_count + b.min_count;
            x.min2 = std::min(a.min2, b.min2);
        } else if (a.min1 < b.min1) {
            x.min1 = a.min1; x.min_count = a.min_count;
            x.min2 = std::min(a.min2, b.min1);
        } else {
            x.min1 = b.min1; x.min_count = b.min_count;
            x.min2 = std::min(a.min1, b.min2);
        }
    }
    // 要求 max2 < value < max1：只有最大值们被改成 value。
    void apply_chmin(int p, long long value) {
        Node& x = t_[p];
        x.sum += (value - x.max1) * x.max_count;
        if (x.min1 == x.max1) x.min1 = value;      // 区间只有一种值。
        else if (x.min2 == x.max1) x.min2 = value; // 区间恰有两种值。
        x.max1 = value;
    }
    void apply_chmax(int p, long long value) {
        Node& x = t_[p];
        x.sum += (value - x.min1) * x.min_count;
        if (x.max1 == x.min1) x.max1 = value;
        else if (x.max2 == x.min1) x.max2 = value;
        x.min1 = value;
    }
    void apply_add(int p, int length, long long value) {
        Node& x = t_[p];
        x.sum += value * length;
        x.add += value;
        x.max1 += value; x.min1 += value;
        if (x.max2 != NEG) x.max2 += value;
        if (x.min2 != INF) x.min2 += value;
    }
    void push(int p, int l, int r) {
        int m = l + (r - l) / 2;
        if (t_[p].add) {
            apply_add(2 * p, m - l, t_[p].add);
            apply_add(2 * p + 1, r - m, t_[p].add);
            t_[p].add = 0;
        }
        for (int c = 2 * p; c <= 2 * p + 1; ++c) {
            if (t_[c].max1 > t_[p].max1) apply_chmin(c, t_[p].max1);
            if (t_[c].min1 < t_[p].min1) apply_chmax(c, t_[p].min1);
        }
    }
    void build(int p, int l, int r, const std::vector<long long>& a) {
        if (r - l == 1) {
            Node& x = t_[p];
            x.sum = x.max1 = x.min1 = a[l];
            x.max_count = x.min_count = 1;
            return;
        }
        int m = l + (r - l) / 2;
        build(2 * p, l, m, a);
        build(2 * p + 1, m, r, a);
        pull(p);
    }
    void chmin(int p, int l, int r, int ql, int qr, long long value) {
        if (t_[p].max1 <= value) return;
        if (ql <= l && r <= qr && t_[p].max2 < value) { apply_chmin(p, value); return; }
        push(p, l, r);
        int m = l + (r - l) / 2;
        if (ql < m) chmin(2 * p, l, m, ql, qr, value);
        if (m < qr) chmin(2 * p + 1, m, r, ql, qr, value);
        pull(p);
    }
    void chmax(int p, int l, int r, int ql, int qr, long long value) {
        if (t_[p].min1 >= value) return;
        if (ql <= l && r <= qr && t_[p].min2 > value) { apply_chmax(p, value); return; }
        push(p, l, r);
        int m = l + (r - l) / 2;
        if (ql < m) chmax(2 * p, l, m, ql, qr, value);
        if (m < qr) chmax(2 * p + 1, m, r, ql, qr, value);
        pull(p);
    }
    void add(int p, int l, int r, int ql, int qr, long long value) {
        if (ql <= l && r <= qr) { apply_add(p, r - l, value); return; }
        push(p, l, r);
        int m = l + (r - l) / 2;
        if (ql < m) add(2 * p, l, m, ql, qr, value);
        if (m < qr) add(2 * p + 1, m, r, ql, qr, value);
        pull(p);
    }
    // kind: 0 求和，1 最小值，2 最大值。
    long long query(int p, int l, int r, int ql, int qr, int kind) {
        if (ql <= l && r <= qr) return kind == 0 ? t_[p].sum : kind == 1 ? t_[p].min1 : t_[p].max1;
        push(p, l, r);
        int m = l + (r - l) / 2;
        if (qr <= m) return query(2 * p, l, m, ql, qr, kind);
        if (ql >= m) return query(2 * p + 1, m, r, ql, qr, kind);
        long long a = query(2 * p, l, m, ql, qr, kind), b = query(2 * p + 1, m, r, ql, qr, kind);
        return kind == 0 ? a + b : kind == 1 ? std::min(a, b) : std::max(a, b);
    }
public:
    explicit SegmentTreeBeats(const std::vector<long long>& a) : n_(int(a.size())), t_(4 * a.size() + 4) {
        if (n_) build(1, 0, n_, a);
    }
    int size() const { return n_; }
    // a[i] = min(a[i], value)，i in [l,r)。
    void chmin(int l, int r, long long value) {
        assert(0 <= l && l <= r && r <= n_ && NEG < value && value < INF);
        if (l < r) chmin(1, 0, n_, l, r, value);
    }
    // a[i] = max(a[i], value)，i in [l,r)。
    void chmax(int l, int r, long long value) {
        assert(0 <= l && l <= r && r <= n_ && NEG < value && value < INF);
        if (l < r) chmax(1, 0, n_, l, r, value);
    }
    void add(int l, int r, long long value) {
        assert(0 <= l && l <= r && r <= n_);
        if (l < r) add(1, 0, n_, l, r, value);
    }
    long long sum(int l, int r) {
        assert(0 <= l && l <= r && r <= n_);
        return l == r ? 0 : query(1, 0, n_, l, r, 0);
    }
    // 非空区间的最小值 / 最大值。
    long long min(int l, int r) {
        assert(0 <= l && l < r && r <= n_);
        return query(1, 0, n_, l, r, 1);
    }
    long long max(int l, int r) {
        assert(0 <= l && l < r && r <= n_);
        return query(1, 0, n_, l, r, 2);
    }
};
} // namespace cp
