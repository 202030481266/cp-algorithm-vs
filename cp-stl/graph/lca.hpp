#pragma once
// 使用说明：cp-stl/docs/usage/graph/lca.md
// 完整示例：cp-stl/examples/graph/lca.cpp
#include <algorithm>
#include <cassert>
#include <vector>

namespace cp {
// 非空无权无向树，0-based。迭代预处理 O(n log n)，查询 O(log n)。
// 倍增层数由 n 计算；根的父亲为自身；不存在的第 k 个祖先返回 -1。
class LCA {
    std::vector<std::vector<int>> up_;
public:
    std::vector<int> depth;
    explicit LCA(const std::vector<std::vector<int>>& g, int root = 0) : depth(g.size(), -1) {
        int n = int(g.size()), levels = 1;
        assert(0 <= root && root < n);
        while ((1LL << levels) <= n) ++levels;
        up_.assign(levels, std::vector<int>(n, root));
        std::vector<int> queue{root};
        depth[root] = 0;
        for (std::size_t i = 0; i < queue.size(); ++i) {
            int u = queue[i];
            for (int v : g[u]) if (depth[v] == -1) {
                depth[v] = depth[u] + 1;
                up_[0][v] = u;
                queue.push_back(v);
            }
        }
        assert(int(queue.size()) == n);
        for (int k = 1; k < levels; ++k)
            for (int u = 0; u < n; ++u) up_[k][u] = up_[k - 1][up_[k - 1][u]];
    }
    int kth_ancestor(int u, int k) const {
        assert(0 <= u && u < int(depth.size()));
        if (k < 0 || k > depth[u]) return -1;
        for (int j = 0; k; ++j, k >>= 1) if (k & 1) u = up_[j][u];
        return u;
    }
    int lca(int u, int v) const {
        assert(0 <= u && u < int(depth.size()) && 0 <= v && v < int(depth.size()));
        if (depth[u] < depth[v]) std::swap(u, v);
        u = kth_ancestor(u, depth[u] - depth[v]);
        if (u == v) return u;
        for (int k = int(up_.size()) - 1; k >= 0; --k)
            if (up_[k][u] != up_[k][v]) { u = up_[k][u]; v = up_[k][v]; }
        return up_[0][u];
    }
    int distance(int u, int v) const { return depth[u] + depth[v] - 2 * depth[lca(u, v)]; }
};
} // namespace cp
