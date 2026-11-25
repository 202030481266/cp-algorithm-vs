#pragma once
// 使用说明：cp-stl/docs/usage/dp/convex_hull_trick.md
// 完整示例：cp-stl/examples/dp/convex_hull_trick.cpp
#include <algorithm>
#include <cassert>
#include <limits>
#include <utility>
#include <vector>

namespace cp {
// 斜率优化（单调栈维护下凸壳）：维护若干直线 y = k·x + b，查询整数 x 处的最小值。
// add 的斜率必须单调不增。query 用二分，O(log n)；query_increasing 要求每次的 x 不降，均摊 O(1)。
// 求最大值：加入 (−k, −b)，结果取负，此时原斜率要单调不降。
// 只用整数除法比较交点，不会像交叉相乘那样溢出；要求 k·x + b 以及两条直线的 b 之差不溢出 long long。
class MonotoneCHT {
    struct Line { long long k, b, from; }; // from：这条直线开始成为最优的最小整数 x
    std::vector<Line> lines_;
    std::size_t head_ = 0;
public:
    bool empty() const { return lines_.empty(); }
    int size() const { return int(lines_.size()); }
    void add(long long k, long long b) {
        assert(lines_.empty() || k <= lines_.back().k);
        long long from = std::numeric_limits<long long>::min();
        while (!lines_.empty()) {
            const Line& top = lines_.back();
            if (top.k == k) {
                if (top.b <= b) return; // 平行且不更低，没有用
            } else {
                long long num = b - top.b, den = top.k - k; // 新直线在 x >= num/den 时不比 top 差
                from = num / den + (num % den > 0);
                if (from > top.from) break;
            }
            lines_.pop_back(); // top 没有成为最优的机会
            from = std::numeric_limits<long long>::min();
        }
        lines_.push_back({k, b, from});
        head_ = std::min(head_, lines_.size() - 1);
    }
    long long query(long long x) const {
        assert(!lines_.empty());
        auto it = std::upper_bound(lines_.begin(), lines_.end(), x,
                                   [](long long value, const Line& line) { return value < line.from; });
        --it;
        return it->k * x + it->b;
    }
    long long query_increasing(long long x) {
        assert(!lines_.empty());
        while (head_ + 1 < lines_.size() && lines_[head_ + 1].from <= x) ++head_;
        return lines_[head_].k * x + lines_[head_].b;
    }
};

// 李超线段树：直线 / 线段可以按任意顺序加入，查询某个横坐标处的最小值，每次 O(log n)（线段 O(log² n)）。
// 构造时给出所有会查询的横坐标（内部排序去重），坐标可以很大或为负数，也可以是 double。
// 求最大值：加入 (−k, −b)，结果取负。没有直线覆盖时返回 INF。
template<class T = long long>
class LiChaoTree {
    struct Line {
        T k, b;
        T at(T x) const { return k * x + b; }
    };
    int n_, size_ = 1;
    std::vector<T> xs_;
    std::vector<Line> tree_;
    // 节点 p 管辖下标 [l,r)。节点上保留在区间中点更低的那条，另一条只可能在一侧更优，继续下放。
    void insert(int p, int l, int r, Line line) {
        while (true) {
            int mid = (l + r) / 2;
            bool left = line.at(xs_[l]) < tree_[p].at(xs_[l]);
            bool middle = line.at(xs_[mid]) < tree_[p].at(xs_[mid]);
            if (middle) std::swap(tree_[p], line);
            if (r - l == 1) return;
            if (left != middle) { p = 2 * p; r = mid; }
            else { p = 2 * p + 1; l = mid; }
        }
    }
    void insert(int p, int l, int r, int ql, int qr, const Line& line) {
        if (qr <= l || r <= ql) return;
        if (ql <= l && r <= qr) { insert(p, l, r, line); return; }
        int mid = (l + r) / 2;
        insert(2 * p, l, mid, ql, qr, line);
        insert(2 * p + 1, mid, r, ql, qr, line);
    }
public:
    static constexpr T INF = std::numeric_limits<T>::max();
    explicit LiChaoTree(std::vector<T> xs) : xs_(std::move(xs)) {
        std::sort(xs_.begin(), xs_.end());
        xs_.erase(std::unique(xs_.begin(), xs_.end()), xs_.end());
        assert(!xs_.empty());
        n_ = int(xs_.size());
        while (size_ < n_) size_ *= 2;
        xs_.resize(size_, xs_.back());
        tree_.assign(2 * size_, Line{T(), INF});
    }
    void add_line(T k, T b) { insert(1, 0, size_, Line{k, b}); }
    // 线段只在横坐标闭区间 [xl, xr] 上生效。
    void add_segment(T k, T b, T xl, T xr) {
        int ql = int(std::lower_bound(xs_.begin(), xs_.begin() + n_, xl) - xs_.begin());
        int qr = int(std::upper_bound(xs_.begin(), xs_.begin() + n_, xr) - xs_.begin());
        if (ql < qr) insert(1, 0, size_, ql, qr, Line{k, b});
    }
    // x 必须是构造时给出的横坐标之一。
    T query(T x) const {
        int i = int(std::lower_bound(xs_.begin(), xs_.begin() + n_, x) - xs_.begin());
        assert(i < n_ && xs_[i] == x);
        T result = INF;
        for (int p = i + size_; p >= 1; p /= 2) result = std::min(result, tree_[p].at(x));
        return result;
    }
};
} // namespace cp
