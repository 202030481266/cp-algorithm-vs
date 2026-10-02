#pragma once
// 使用说明：cp-stl/docs/usage/data_structures/lazy_segment_tree.md
// 完整示例：cp-stl/examples/data_structures/lazy_segment_tree.cpp
#include <algorithm>
#include <cassert>
#include <limits>
#include <vector>

namespace cp {
// 区间加、区间求和，O(log n)。所有区间为 [l,r)，注意 sum 与 delta*长度溢出。
// 赋值、仿射等其他标记用下面的通用 LazySegmentTree<Info,Tag>。
class RangeAddSum {
    int n_;
    std::vector<long long> sum_, lazy_;
    void build(int p, int l, int r, const std::vector<long long>& a) {
        if (r - l == 1) { sum_[p] = a[l]; return; }
        int m = l + (r - l) / 2;
        build(p * 2, l, m, a);
        build(p * 2 + 1, m, r, a);
        pull(p);
    }
    void pull(int p) { sum_[p] = sum_[p * 2] + sum_[p * 2 + 1]; }
    void apply(int p, int length, long long value) {
        sum_[p] += value * length;
        lazy_[p] += value;
    }
    void push(int p, int l, int r) {
        if (lazy_[p] == 0) return;
        int m = l + (r - l) / 2;
        apply(p * 2, m - l, lazy_[p]);
        apply(p * 2 + 1, r - m, lazy_[p]);
        lazy_[p] = 0;
    }
    // 调用方只进入与 [ql,qr) 相交的子节点，避免空分支的递归和边界检查。
    void add(int p, int l, int r, int ql, int qr, long long value) {
        if (ql <= l && r <= qr) { apply(p, r - l, value); return; }
        push(p, l, r);
        int m = l + (r - l) / 2;
        if (ql < m) add(p * 2, l, m, ql, qr, value);
        if (m < qr) add(p * 2 + 1, m, r, ql, qr, value);
        pull(p);
    }
    long long query(int p, int l, int r, int ql, int qr) {
        if (ql <= l && r <= qr) return sum_[p];
        push(p, l, r);
        int m = l + (r - l) / 2;
        if (qr <= m) return query(p * 2, l, m, ql, qr);
        if (ql >= m) return query(p * 2 + 1, m, r, ql, qr);
        return query(p * 2, l, m, ql, qr) + query(p * 2 + 1, m, r, ql, qr);
    }
public:
    explicit RangeAddSum(int n = 0) : n_(n), sum_(4 * n + 4), lazy_(4 * n + 4) {}
    explicit RangeAddSum(const std::vector<long long>& a) : RangeAddSum(int(a.size())) {
        if (n_) build(1, 0, n_, a);
    }
    void add(int l, int r, long long value) {
        assert(0 <= l && l <= r && r <= n_);
        if (l < r) add(1, 0, n_, l, r, value);
    }
    long long sum(int l, int r) {
        assert(0 <= l && l <= r && r <= n_);
        return l == r ? 0 : query(1, 0, n_, l, r);
    }
};

// 通用懒标记线段树（AtCoder Library 的非递归写法），0-based，区间 [l,r)。
// Info：默认构造是单位元；Info + Info 满足结合律（可不交换）；info.apply(tag) 作用一个标记。
// Tag：默认构造是空标记；old.apply(newer) 把较新的标记复合到旧标记之后。
// 修改、查询、二分均为 O(log n) 次 Info/Tag 运算。长度相关的信息（如区间和）把长度存进 Info。
template<class Info, class Tag>
class LazySegmentTree {
    int n_ = 0, log_ = 0, size_ = 1;
    std::vector<Info> info_;
    std::vector<Tag> tag_;
    void pull(int p) { info_[p] = info_[2 * p] + info_[2 * p + 1]; }
    void apply_at(int p, const Tag& t) {
        info_[p].apply(t);
        if (p < size_) tag_[p].apply(t);
    }
    void push(int p) {
        apply_at(2 * p, tag_[p]);
        apply_at(2 * p + 1, tag_[p]);
        tag_[p] = Tag();
    }
    // 把 [l,r) 两端所在路径上的标记从上往下推开。
    void push_bounds(int l, int r) {
        for (int i = log_; i >= 1; --i) {
            if (((l >> i) << i) != l) push(l >> i);
            if (((r >> i) << i) != r) push((r - 1) >> i);
        }
    }
public:
    LazySegmentTree() : LazySegmentTree(std::vector<Info>()) {}
    // 每个叶子都必须是真实元素（例如长度为 1），不能用单位元充当初值。
    LazySegmentTree(int n, const Info& value) : LazySegmentTree(std::vector<Info>(n, value)) {}
    explicit LazySegmentTree(const std::vector<Info>& a) : n_(int(a.size())) {
        while ((1 << log_) < n_) ++log_;
        size_ = 1 << log_;
        info_.assign(2 * size_, Info());
        tag_.assign(size_, Tag());
        std::copy(a.begin(), a.end(), info_.begin() + size_);
        for (int i = size_ - 1; i >= 1; --i) pull(i);
    }
    int size() const { return n_; }
    void set(int p, const Info& value) {
        assert(0 <= p && p < n_);
        p += size_;
        for (int i = log_; i >= 1; --i) push(p >> i);
        info_[p] = value;
        for (int i = 1; i <= log_; ++i) pull(p >> i);
    }
    Info get(int p) {
        assert(0 <= p && p < n_);
        p += size_;
        for (int i = log_; i >= 1; --i) push(p >> i);
        return info_[p];
    }
    Info prod(int l, int r) {
        assert(0 <= l && l <= r && r <= n_);
        if (l == r) return Info();
        l += size_; r += size_;
        push_bounds(l, r);
        Info left, right;
        for (; l < r; l >>= 1, r >>= 1) {
            if (l & 1) left = left + info_[l++];
            if (r & 1) right = info_[--r] + right;
        }
        return left + right;
    }
    const Info& all_prod() const { return info_[1]; }
    void apply(int p, const Tag& t) {
        assert(0 <= p && p < n_);
        p += size_;
        for (int i = log_; i >= 1; --i) push(p >> i);
        info_[p].apply(t);
        for (int i = 1; i <= log_; ++i) pull(p >> i);
    }
    void apply(int l, int r, const Tag& t) {
        assert(0 <= l && l <= r && r <= n_);
        if (l == r) return;
        l += size_; r += size_;
        push_bounds(l, r);
        for (int a = l, b = r; a < b; a >>= 1, b >>= 1) {
            if (a & 1) apply_at(a++, t);
            if (b & 1) apply_at(--b, t);
        }
        for (int i = 1; i <= log_; ++i) {
            if (((l >> i) << i) != l) pull(l >> i);
            if (((r >> i) << i) != r) pull((r - 1) >> i);
        }
    }
    // pred(Info()) 为真，且向右扩展时真假只能从真变假；返回最大的 r 使 pred(prod(l,r)) 为真。
    template<class Predicate>
    int max_right(int l, Predicate pred) {
        assert(0 <= l && l <= n_ && pred(Info()));
        if (l == n_) return n_;
        l += size_;
        for (int i = log_; i >= 1; --i) push(l >> i);
        Info current;
        do {
            while (!(l & 1)) l >>= 1;
            if (!pred(current + info_[l])) {
                while (l < size_) {
                    push(l);
                    l <<= 1;
                    if (pred(current + info_[l])) current = current + info_[l++];
                }
                return l - size_;
            }
            current = current + info_[l++];
        } while ((l & -l) != l);
        return n_;
    }
    // pred(Info()) 为真，且向左扩展时真假只能从真变假；返回最小的 l 使 pred(prod(l,r)) 为真。
    template<class Predicate>
    int min_left(int r, Predicate pred) {
        assert(0 <= r && r <= n_ && pred(Info()));
        if (r == 0) return 0;
        r += size_;
        for (int i = log_; i >= 1; --i) push((r - 1) >> i);
        Info current;
        do {
            --r;
            while (r > 1 && (r & 1)) r >>= 1;
            if (!pred(info_[r] + current)) {
                while (r < size_) {
                    push(r);
                    r = 2 * r + 1;
                    if (pred(info_[r] + current)) current = info_[r--] + current;
                }
                return r + 1 - size_;
            }
            current = info_[r] + current;
        } while ((r & -r) != r);
        return 0;
    }
};

// 预置标记：先把区间赋值为 value（assigned 为真时），再整体加 delta。
struct AssignAdd {
    long long value = 0, delta = 0;
    bool assigned = false;
    static AssignAdd assign(long long x) { return {x, 0, true}; }
    static AssignAdd add(long long x) { return {0, x, false}; }
    void apply(const AssignAdd& newer) {
        if (newer.assigned) *this = newer;
        else delta += newer.delta;
    }
};
// 预置信息：区间和、最小值、最大值与长度。可由 long long 隐式构造出长度为 1 的叶子。
struct SumMinMax {
    long long sum = 0;
    long long minimum = std::numeric_limits<long long>::max();
    long long maximum = std::numeric_limits<long long>::min();
    int length = 0;
    SumMinMax() = default;
    SumMinMax(long long x) : sum(x), minimum(x), maximum(x), length(1) {}
    void apply(const AssignAdd& t) {
        if (!length) return; // 单位元保持不变，避免哨兵值溢出。
        if (t.assigned) { sum = t.value * length; minimum = maximum = t.value; }
        sum += t.delta * length;
        minimum += t.delta;
        maximum += t.delta;
    }
    friend SumMinMax operator+(const SumMinMax& a, const SumMinMax& b) {
        SumMinMax c;
        c.sum = a.sum + b.sum;
        c.minimum = std::min(a.minimum, b.minimum);
        c.maximum = std::max(a.maximum, b.maximum);
        c.length = a.length + b.length;
        return c;
    }
};
} // namespace cp
