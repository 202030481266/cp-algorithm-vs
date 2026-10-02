#pragma once
// 使用说明：cp-stl/docs/usage/data_structures/persistent_segment_tree.md
// 完整示例：cp-stl/examples/data_structures/persistent_segment_tree.cpp
#include <algorithm>
#include <cassert>
#include <vector>

namespace cp {
// 可持久化线段树（主席树），下标 [0,n)。每个版本用一个根编号表示，根 0 是全零数组。
// add/set 返回新版本的根，旧版本保持不变；每次修改新建 O(log n) 个节点，查询 O(log n)。
template<class T = long long>
class PersistentSegmentTree {
    struct Node { int left = 0, right = 0; T sum = T(); };
    int n_;
    std::vector<Node> nodes_{Node()}; // 0 号是空节点：左右儿子都是自己，和为 0。
    int clone(int p) {
        Node copy = nodes_[p];
        nodes_.push_back(copy);
        return int(nodes_.size()) - 1;
    }
    int build(int l, int r, const std::vector<T>& a) {
        int p = clone(0);
        if (r - l == 1) { nodes_[p].sum = a[l]; return p; }
        int m = l + (r - l) / 2;
        int left = build(l, m, a), right = build(m, r, a);
        nodes_[p].left = left;
        nodes_[p].right = right;
        nodes_[p].sum = nodes_[left].sum + nodes_[right].sum;
        return p;
    }
    T sum(int p, int l, int r, int ql, int qr) const {
        if (!p) return T();
        if (ql <= l && r <= qr) return nodes_[p].sum;
        int m = l + (r - l) / 2;
        T result = T();
        if (ql < m) result += sum(nodes_[p].left, l, m, ql, qr);
        if (m < qr) result += sum(nodes_[p].right, m, r, ql, qr);
        return result;
    }
public:
    // reserve_nodes 可预留节点数，例如 n 次单点修改约需 n*(ceil(log2 n)+1)+1 个。
    explicit PersistentSegmentTree(int n, int reserve_nodes = 0) : n_(n) {
        assert(n > 0);
        nodes_.reserve(std::max(reserve_nodes, 1));
    }
    int size() const { return n_; }
    int node_count() const { return int(nodes_.size()); }
    // 用初始数组建出一个版本，新建 2n-1 个节点。
    int build(const std::vector<T>& a) {
        assert(int(a.size()) == n_);
        return build(0, n_, a);
    }
    int add(int root, int pos, T delta) {
        assert(0 <= root && root < node_count() && 0 <= pos && pos < n_);
        int new_root = clone(root), p = new_root, l = 0, r = n_;
        while (true) {
            nodes_[p].sum += delta;
            if (r - l == 1) return new_root;
            int m = l + (r - l) / 2;
            if (pos < m) { int child = clone(nodes_[p].left); nodes_[p].left = child; p = child; r = m; }
            else { int child = clone(nodes_[p].right); nodes_[p].right = child; p = child; l = m; }
        }
    }
    int set(int root, int pos, T value) { return add(root, pos, value - get(root, pos)); }
    T get(int root, int pos) const {
        assert(0 <= root && root < node_count() && 0 <= pos && pos < n_);
        int l = 0, r = n_;
        while (r - l > 1 && root) {
            int m = l + (r - l) / 2;
            if (pos < m) { root = nodes_[root].left; r = m; }
            else { root = nodes_[root].right; l = m; }
        }
        return nodes_[root].sum;
    }
    T sum(int root, int l, int r) const {
        assert(0 <= root && root < node_count() && 0 <= l && l <= r && r <= n_);
        return l == r ? T() : sum(root, 0, n_, l, r);
    }
    // 计数数组 hi - lo（按版本逐位相减）中，第 k 小元素所在的下标，k 从 0 开始。
    // 要求差值都非负且 0 <= k < 总数；常用于“前缀版本相减”求区间第 k 小。
    int kth(int lo_root, int hi_root, T k) const {
        assert(T() <= k && k < nodes_[hi_root].sum - nodes_[lo_root].sum);
        int l = 0, r = n_;
        while (r - l > 1) {
            int m = l + (r - l) / 2;
            T left_count = nodes_[nodes_[hi_root].left].sum - nodes_[nodes_[lo_root].left].sum;
            if (k < left_count) {
                lo_root = nodes_[lo_root].left; hi_root = nodes_[hi_root].left; r = m;
            } else {
                k -= left_count;
                lo_root = nodes_[lo_root].right; hi_root = nodes_[hi_root].right; l = m;
            }
        }
        return l;
    }
};

// 静态数组的区间第 k 小与区间计数，在线查询 O(log n)，预处理 O(n log n) 时间和空间。
template<class T>
class RangeKth {
    std::vector<T> values_; // 排序去重后的值。
    PersistentSegmentTree<int> tree_;
    std::vector<int> roots_; // roots_[i] 是前 i 个元素的计数树。
    static std::vector<T> sorted_unique(std::vector<T> a) {
        std::sort(a.begin(), a.end());
        a.erase(std::unique(a.begin(), a.end()), a.end());
        if (a.empty()) a.push_back(T());
        return a;
    }
    static int levels(int n) { int k = 1; while ((1 << (k - 1)) < n) ++k; return k; }
public:
    explicit RangeKth(const std::vector<T>& a)
        : values_(sorted_unique(a)),
          tree_(int(values_.size()), int(a.size()) * levels(int(values_.size())) + 1),
          roots_(a.size() + 1) {
        for (int i = 0; i < int(a.size()); ++i) {
            int index = int(std::lower_bound(values_.begin(), values_.end(), a[i]) - values_.begin());
            roots_[i + 1] = tree_.add(roots_[i], index, 1);
        }
    }
    // [l,r) 中第 k 小的值，k 从 0 开始，要求 0 <= k < r-l。
    T kth(int l, int r, int k) const {
        assert(0 <= l && l <= r && r < int(roots_.size()) && 0 <= k && k < r - l);
        return values_[tree_.kth(roots_[l], roots_[r], k)];
    }
    // [l,r) 中严格小于 x 的元素个数。
    int count_less(int l, int r, const T& x) const {
        assert(0 <= l && l <= r && r < int(roots_.size()));
        int index = int(std::lower_bound(values_.begin(), values_.end(), x) - values_.begin());
        return tree_.sum(roots_[r], 0, index) - tree_.sum(roots_[l], 0, index);
    }
};
} // namespace cp
