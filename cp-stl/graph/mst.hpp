#pragma once
// 使用说明：cp-stl/docs/usage/graph/mst.md
// 完整示例：cp-stl/examples/graph/mst.cpp
#include <algorithm>
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
} // namespace cp
