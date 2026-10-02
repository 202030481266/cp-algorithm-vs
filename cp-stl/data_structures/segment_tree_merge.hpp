#pragma once
// 使用说明：cp-stl/docs/usage/data_structures/segment_tree_merge.md
// 完整示例：cp-stl/examples/data_structures/segment_tree_merge.cpp
#include <algorithm>
#include <cassert>
#include <limits>
#include <vector>

namespace cp {
// 值域 [0,n) 上的一组动态开点计数线段树，支持线段树合并与分裂。
// 每棵树用根编号表示，0 是空树；修改/查询 O(log n)，n 次插入后全部合并的总代价 O(n log n)。
// 叶子存计数 cnt[pos]；sum 为计数和，max_count/max_pos 只考虑已经创建的叶子。
class MergeableSegmentTrees {
    static constexpr long long NONE = std::numeric_limits<long long>::min();
    struct Node { int left = 0, right = 0; long long sum = 0, max = NONE; };
    int n_;
    std::vector<Node> nodes_{Node()};
    int new_node() { nodes_.emplace_back(); return int(nodes_.size()) - 1; }
    void pull(int p) {
        const Node &a = nodes_[nodes_[p].left], &b = nodes_[nodes_[p].right];
        nodes_[p].sum = a.sum + b.sum;
        nodes_[p].max = std::max(a.max, b.max); // 0 号节点的 max 为 NONE，不影响结果。
    }
    int add(int p, int l, int r, int pos, long long delta) {
        if (!p) p = new_node();
        if (r - l == 1) {
            nodes_[p].sum += delta;
            nodes_[p].max = nodes_[p].sum;
            return p;
        }
        int m = l + (r - l) / 2;
        if (pos < m) { int child = add(nodes_[p].left, l, m, pos, delta); nodes_[p].left = child; }
        else { int child = add(nodes_[p].right, m, r, pos, delta); nodes_[p].right = child; }
        pull(p);
        return p;
    }
    long long sum(int p, int l, int r, int ql, int qr) const {
        if (!p) return 0;
        if (ql <= l && r <= qr) return nodes_[p].sum;
        int m = l + (r - l) / 2;
        long long result = 0;
        if (ql < m) result += sum(nodes_[p].left, l, m, ql, qr);
        if (m < qr) result += sum(nodes_[p].right, m, r, ql, qr);
        return result;
    }
    int merge(int a, int b, int l, int r) {
        if (!a || !b) return a ^ b;
        if (r - l == 1) {
            nodes_[a].sum += nodes_[b].sum;
            nodes_[a].max = nodes_[a].sum;
            return a;
        }
        int m = l + (r - l) / 2;
        int left = merge(nodes_[a].left, nodes_[b].left, l, m);
        int right = merge(nodes_[a].right, nodes_[b].right, m, r);
        nodes_[a].left = left;
        nodes_[a].right = right;
        pull(a);
        return a;
    }
    // 把 p 拆成位置 < k 的 x 与位置 >= k 的 y。
    void split(int p, int l, int r, int k, int& x, int& y) {
        if (!p) { x = y = 0; return; }
        if (r <= k) { x = p; y = 0; return; }
        if (k <= l) { x = 0; y = p; return; }
        int m = l + (r - l) / 2, q = new_node();
        if (k <= m) {
            int a, b;
            split(nodes_[p].left, l, m, k, a, b);
            nodes_[q].left = b;
            nodes_[q].right = nodes_[p].right;
            nodes_[p].left = a;
            nodes_[p].right = 0;
        } else {
            int a, b;
            split(nodes_[p].right, m, r, k, a, b);
            nodes_[q].right = b;
            nodes_[p].right = a;
        }
        pull(p);
        pull(q);
        x = p; y = q;
    }
public:
    // reserve_nodes 可预留节点数，例如 m 次 add 至多新建 m*(ceil(log2 n)+1) 个。
    explicit MergeableSegmentTrees(int n, int reserve_nodes = 0) : n_(n) {
        assert(n > 0);
        nodes_.reserve(std::max(reserve_nodes, 1));
    }
    int size() const { return n_; }
    int node_count() const { return int(nodes_.size()); }
    // cnt[pos] += delta；root 为 0 时会新建一棵树并写回 root。
    void add(int& root, int pos, long long delta) {
        assert(0 <= pos && pos < n_);
        root = add(root, 0, n_, pos, delta);
    }
    long long sum(int root, int l, int r) const {
        assert(0 <= l && l <= r && r <= n_);
        return l == r ? 0 : sum(root, 0, n_, l, r);
    }
    long long total(int root) const { return nodes_[root].sum; }
    // 把 b 合并进 a，返回新根；合并后 a、b 的旧根都不能再单独使用。
    int merge(int a, int b) { return merge(a, b, 0, n_); }
    // 从 root 中取出位置在 [l,r) 的部分作为新树返回，root 保留其余部分。
    int split(int& root, int l, int r) {
        assert(0 <= l && l <= r && r <= n_);
        int a, b, c, d;
        split(root, 0, n_, l, a, b);
        split(b, 0, n_, r, c, d);
        root = merge(a, d);
        return c;
    }
    // 计数非负时，返回第 k 小元素的位置（k 从 0 开始）；k >= total 时返回 -1。
    int kth(int root, long long k) const {
        assert(k >= 0);
        if (k >= nodes_[root].sum) return -1;
        int l = 0, r = n_;
        while (r - l > 1) {
            int m = l + (r - l) / 2;
            long long left = nodes_[nodes_[root].left].sum;
            if (k < left) { root = nodes_[root].left; r = m; }
            else { k -= left; root = nodes_[root].right; l = m; }
        }
        return l;
    }
    // 已创建叶子中的最大计数；空树返回 LLONG_MIN。
    long long max_count(int root) const { return nodes_[root].max; }
    // 计数最大的已创建叶子中位置最小者；空树返回 -1。
    int max_pos(int root) const {
        if (!root || nodes_[root].max == NONE) return -1;
        int l = 0, r = n_;
        while (r - l > 1) {
            int m = l + (r - l) / 2, left = nodes_[root].left;
            if (left && nodes_[left].max == nodes_[root].max) { root = left; r = m; }
            else { root = nodes_[root].right; l = m; }
        }
        return l;
    }
};
} // namespace cp
