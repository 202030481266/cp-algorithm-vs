#pragma once
// 使用说明：cp-stl/docs/usage/graph/mst.md
// 完整示例：cp-stl/examples/graph/mst.cpp
#include <algorithm>
#include <cassert>
#include <limits>
#include <vector>
#include "../data_structures/dsu.hpp"

namespace cp {
struct UndirectedEdge { int u, v; long long weight; };
struct MSTResult {
    long long weight = 0;
    std::vector<UndirectedEdge> edges;
    bool connected = false;
};
// Kruskal O(m log m)；不连通时返回最小生成森林，connected=false。
// 总权值必须能放进 long long；允许负边、自环和重边。
inline MSTResult kruskal(int n, std::vector<UndirectedEdge> edges) {
    std::sort(edges.begin(), edges.end(), [](const auto& a, const auto& b) {
        return a.weight < b.weight;
    });
    DSU dsu(n);
    MSTResult result;
    for (auto e : edges) if (dsu.merge(e.u, e.v)) {
        result.weight += e.weight;
        result.edges.push_back(e);
    }
    result.connected = n == 0 || dsu.components == 1;
    return result;
}

// 稠密图 Prim：O(n^2) 次扫描和边权查询，额外空间 O(n)。
// weight(u,v) 为对称的 long long 边权，等于 no_edge 时表示无边；不查询自环。
// 不连通时返回最小生成森林；允许负边，每次权值累加须在 long long 范围内。
template<class Weight>
MSTResult prim_dense(int n, Weight weight,
                     long long no_edge = std::numeric_limits<long long>::max()) {
    assert(n >= 0);
    std::vector<long long> best(n);
    std::vector<int> parent(n, -1);
    std::vector<char> used(n, false);
    MSTResult result;
    int components = 0;
    for (int step = 0; step < n; ++step) {
        int u = -1;
        for (int v = 0; v < n; ++v)
            if (!used[v] && parent[v] != -1 && (u == -1 || best[v] < best[u])) u = v;
        if (u == -1) {
            // 当前连通块已完成，从尚未选入的点开始下一个连通块。
            for (int v = 0; v < n; ++v) if (!used[v]) { u = v; break; }
            ++components;
        } else {
            result.weight += best[u];
            result.edges.push_back({parent[u], u, best[u]});
        }
        used[u] = true;
        for (int v = 0; v < n; ++v) if (!used[v]) {
            long long w = weight(u, v);
            if (w != no_edge && (parent[v] == -1 || w < best[v])) {
                best[v] = w;
                parent[v] = u;
            }
        }
    }
    result.connected = components <= 1;
    return result;
}

// 对称 n*n 邻接矩阵；无边置 no_edge，重边取 min，对角线忽略。
// 时间 O(n^2)，除输入矩阵外额外空间 O(n)，不修改矩阵。
inline MSTResult prim_dense(const std::vector<std::vector<long long>>& graph,
                            long long no_edge = std::numeric_limits<long long>::max()) {
    int n = int(graph.size());
    for (int i = 0; i < n; ++i) assert(int(graph[i].size()) == n);
    return prim_dense(n, [&](int u, int v) { return graph[u][v]; }, no_edge);
}
} // namespace cp
