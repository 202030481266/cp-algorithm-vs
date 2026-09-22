#pragma once
// 使用说明：cp-stl/docs/usage/graph/hld.md
// 完整示例：cp-stl/examples/graph/hld.cpp
#include <algorithm>
#include <cassert>
#include <utility>
#include <vector>

namespace cp {
// 非空无向树，迭代建树 O(n)，一次路径拆成 O(log n) 个 [l,r)。
// 对点权使用；for_each_path 不保证方向，仅适合和/min/max 等可交换运算。
struct HLD {
    std::vector<int> parent, depth, size, heavy, head, pos, vertex;
    explicit HLD(const std::vector<std::vector<int>>& g, int root = 0)
        : parent(g.size(), -2), depth(g.size()), size(g.size(), 1),
          heavy(g.size(), -1), head(g.size()), pos(g.size()), vertex(g.size()) {
        int n = int(g.size());
        assert(0 <= root && root < n);
        std::vector<int> order{root};
        parent[root] = -1;
        for (std::size_t i = 0; i < order.size(); ++i) {
            int u = order[i];
            for (int v : g[u]) if (v != parent[u]) {
                assert(parent[v] == -2); // 输入必须是树。
                parent[v] = u;
                depth[v] = depth[u] + 1;
                order.push_back(v);
            }
        }
        assert(int(order.size()) == n);
        for (int i = n - 1; i > 0; --i) {
            int u = order[i], p = parent[u];
            size[p] += size[u];
            if (heavy[p] == -1 || size[u] > size[heavy[p]]) heavy[p] = u;
        }
        int timer = 0;
        std::vector<std::pair<int, int>> stack{{root, root}};
        while (!stack.empty()) {
            auto [start, top] = stack.back(); stack.pop_back();
            for (int u = start; u != -1; u = heavy[u]) {
                head[u] = top;
                vertex[timer] = u;
                pos[u] = timer++;
                for (int v : g[u]) if (parent[v] == u && v != heavy[u]) stack.emplace_back(v, v);
            }
        }
    }
    template<class F>
    void for_each_path(int u, int v, F visit) const {
        while (head[u] != head[v]) {
            if (depth[head[u]] < depth[head[v]]) std::swap(u, v);
            visit(pos[head[u]], pos[u] + 1);
            u = parent[head[u]];
        }
        if (depth[u] > depth[v]) std::swap(u, v);
        visit(pos[u], pos[v] + 1);
    }
    std::pair<int, int> subtree(int u) const { return {pos[u], pos[u] + size[u]}; }
};
} // namespace cp
