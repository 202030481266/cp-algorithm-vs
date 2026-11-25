#pragma once
// 使用说明：cp-stl/docs/usage/data_structures/persistent_dsu.md
// 完整示例：cp-stl/examples/data_structures/persistent_dsu.cpp
#include <cassert>
#include <utility>
#include <vector>

namespace cp {
// 可持久化并查集：用可持久化数组保存“父亲 / 集合大小”，按大小合并、不做路径压缩。
// 版本用一个整数表示：initial() 是 n 个孤立点，merge 返回新版本，旧版本仍然可用。
// find / same / size O(log² n)；merge 另外新建 2·(⌈log₂ n⌉+1) 个节点（每个 8 字节）。
class PersistentDSU {
    struct Node { int left, right; }; // 叶子的 left 存值：负数是 -(集合大小)，否则是父亲
    int n_, height_ = 0;
    std::vector<Node> nodes_;
    int get(int root, int i) const {
        for (int h = height_ - 1; h >= 0; --h) root = (i >> h & 1) ? nodes_[root].right : nodes_[root].left;
        return nodes_[root].left;
    }
    int set(int root, int i, int value) {
        int result = int(nodes_.size());
        for (int h = height_ - 1; h >= 0; --h) {
            Node node = nodes_[root];
            int child = int(nodes_.size()) + 1;
            if (i >> h & 1) { root = node.right; node.right = child; }
            else { root = node.left; node.left = child; }
            nodes_.push_back(node);
        }
        nodes_.push_back({value, 0});
        return result;
    }
    std::pair<int, int> root_of(int version, int x) const { // {代表元, 集合大小}
        assert(0 <= version && version < int(nodes_.size()) && 0 <= x && x < n_);
        while (true) {
            int p = get(version, x);
            if (p < 0) return {x, -p};
            x = p;
        }
    }
public:
    // reserve_merges：预计的 merge 次数，用来预留节点。
    explicit PersistentDSU(int n, int reserve_merges = 0) : n_(n) {
        assert(n > 0);
        while ((1 << height_) < n) ++height_;
        nodes_.reserve(height_ + 1 + 2 * std::size_t(reserve_merges) * (height_ + 1));
        nodes_.push_back({-1, 0}); // 初始版本的每一层只有一个节点，左右儿子都指向下一层的同一个节点
        for (int h = 1; h <= height_; ++h) nodes_.push_back({h - 1, h - 1});
    }
    int initial() const { return height_; }
    int find(int version, int x) const { return root_of(version, x).first; }
    int size(int version, int x) const { return root_of(version, x).second; }
    bool same(int version, int a, int b) const { return find(version, a) == find(version, b); }
    // 在 version 的基础上合并 a、b 所在的集合，返回新版本；已经连通时返回 version 本身。
    int merge(int version, int a, int b) {
        auto [ra, size_a] = root_of(version, a);
        auto [rb, size_b] = root_of(version, b);
        if (ra == rb) return version;
        if (size_a < size_b) std::swap(ra, rb);
        return set(set(version, rb, ra), ra, -(size_a + size_b));
    }
    int node_count() const { return int(nodes_.size()); }
};
} // namespace cp
