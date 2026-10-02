#pragma once
// 使用说明：cp-stl/docs/usage/graph/fast_lca.md
// 完整示例：cp-stl/examples/graph/fast_lca.cpp
#include <algorithm>
#include <cassert>
#include <utility>
#include <vector>

namespace cp {
// O(1) 查询的 LCA（DFS 序 + ST 表）。非递归 DFS，预处理 O(n log n) 时间和空间。
// 原理：tin[u] < tin[v] 且 u != v 时，DFS 序区间 (tin[u], tin[v]] 中各点的父亲里 DFS 序最小的就是 LCA。
// 同时给出 DFS 序：子树 u 对应区间 [tin[u], tout[u])，order[tin[u]] = u。只处理 root 所在的连通块。
class FastLCA {
    std::vector<std::vector<int>> table_; // table_[k][i]：位置 [i, i+2^k) 的父亲 DFS 序的最小值
    std::vector<int> log_;
public:
    std::vector<int> parent, depth, tin, tout, order;
    explicit FastLCA(const std::vector<std::vector<int>>& g, int root = 0)
        : parent(g.size(), -1), depth(g.size(), 0), tin(g.size(), -1), tout(g.size(), -1) {
        int n = int(g.size());
        assert(0 <= root && root < n);
        order.reserve(n);
        std::vector<std::pair<int, int>> stack{{root, 0}}; // (点, 下一个邻点下标)
        tin[root] = 0;
        order.push_back(root);
        while (!stack.empty()) {
            auto& [u, i] = stack.back();
            if (i < int(g[u].size())) {
                int v = g[u][i++];
                if (v == parent[u] || tin[v] != -1) continue; // 跳过父亲（无向边的反向）
                parent[v] = u;
                depth[v] = depth[u] + 1;
                tin[v] = int(order.size());
                order.push_back(v);
                stack.emplace_back(v, 0);
            } else {
                tout[u] = int(order.size());
                stack.pop_back();
            }
        }
        int m = int(order.size());
        log_.assign(m + 1, 0);
        for (int i = 2; i <= m; ++i) log_[i] = log_[i / 2] + 1;
        table_.assign(log_[std::max(m, 1)] + 1, std::vector<int>(m));
        for (int i = 1; i < m; ++i) table_[0][i] = tin[parent[order[i]]];
        for (int k = 1; k < int(table_.size()); ++k)
            for (int i = 0; i + (1 << k) <= m; ++i)
                table_[k][i] = std::min(table_[k - 1][i], table_[k - 1][i + (1 << (k - 1))]);
    }
    // u、v 都必须在 root 所在的连通块中。
    int lca(int u, int v) const {
        assert(tin[u] != -1 && tin[v] != -1);
        if (u == v) return u;
        int l = tin[u], r = tin[v];
        if (l > r) std::swap(l, r);
        ++l; // 查询闭区间 [l, r]
        int k = log_[r - l + 1];
        return order[std::min(table_[k][l], table_[k][r - (1 << k) + 1])];
    }
    int distance(int u, int v) const { return depth[u] + depth[v] - 2 * depth[lca(u, v)]; }
    // u 是否是 v 的祖先（u == v 也算）。
    bool is_ancestor(int u, int v) const { return tin[u] <= tin[v] && tin[v] < tout[u]; }
};
} // namespace cp
