#pragma once
// 使用说明：cp-stl/docs/usage/graph/centroid.md
// 完整示例：cp-stl/examples/graph/centroid.cpp
#include <algorithm>
#include <cassert>
#include <vector>

namespace cp {
// 树的重心：删去后最大连通块最小的点，1 个或 2 个，升序返回。非递归，O(n)。
inline std::vector<int> tree_centroids(const std::vector<std::vector<int>>& g) {
    int n = int(g.size());
    if (n == 0) return {};
    std::vector<int> parent(n, -1), order{0}, size(n, 1);
    parent[0] = 0;
    for (int i = 0; i < int(order.size()); ++i)
        for (int v : g[order[i]]) if (parent[v] == -1) { parent[v] = order[i]; order.push_back(v); }
    assert(int(order.size()) == n); // 必须连通
    std::vector<int> heaviest(n, 0); // 删去该点后最大的连通块
    for (int i = n - 1; i > 0; --i) {
        int u = order[i];
        size[parent[u]] += size[u];
        heaviest[parent[u]] = std::max(heaviest[parent[u]], size[u]);
    }
    std::vector<int> result;
    for (int u = 0; u < n; ++u)
        if (std::max(heaviest[u], n - size[u]) <= n / 2) result.push_back(u);
    return result;
}

// 点分治（重心分解）：反复取当前连通块的重心并删去，得到深度 O(log n) 的点分树。非递归，O(n log n)。
// level[c] 是 c 被选为重心时的层数（根为 0），parent[c] 是点分树上的父亲（根为 -1），
// order 是重心被选出的顺序（父亲总在儿子之前）。
// c 负责的连通块 = 从 c 出发、只经过 level 大于 level[c] 的点能到达的点；遍历时用 can_visit(c, v) 判断。
class CentroidDecomposition {
public:
    std::vector<int> parent, level, order;
    explicit CentroidDecomposition(const std::vector<std::vector<int>>& g)
        : parent(g.size(), -1), level(g.size(), -1) {
        int n = int(g.size());
        order.reserve(n);
        std::vector<int> size(n), from(n), component;
        struct Task { int start, parent, depth; };
        std::vector<Task> tasks;
        for (int s = 0; s < n; ++s) {
            if (level[s] != -1) continue;
            tasks.push_back({s, -1, 0});
            while (!tasks.empty()) {
                Task task = tasks.back();
                tasks.pop_back();
                // BFS 收集当前连通块（未被删除的点），再倒序求子树大小。
                component.assign(1, task.start);
                from[task.start] = -1;
                for (int i = 0; i < int(component.size()); ++i) {
                    int u = component[i];
                    for (int v : g[u]) if (v != from[u] && level[v] == -1) { from[v] = u; component.push_back(v); }
                }
                int total = int(component.size()), centroid = task.start;
                for (int i = total - 1; i >= 0; --i) {
                    int u = component[i], heaviest = 0;
                    size[u] = 1;
                    for (int v : g[u]) if (v != from[u] && level[v] == -1) {
                        size[u] += size[v];
                        heaviest = std::max(heaviest, size[v]);
                    }
                    if (std::max(heaviest, total - size[u]) * 2 <= total) centroid = u;
                }
                level[centroid] = task.depth;
                parent[centroid] = task.parent;
                order.push_back(centroid);
                for (int v : g[centroid]) if (level[v] == -1) tasks.push_back({v, centroid, task.depth + 1});
            }
        }
    }
    // 处理重心 c 时，邻点 v 是否仍在 c 的连通块里（c 本身视为已删除）。
    bool can_visit(int c, int v) const { return level[v] > level[c]; }
};
} // namespace cp
