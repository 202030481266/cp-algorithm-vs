#pragma once
// 使用说明：cp-stl/docs/usage/graph/topological_sort.md
// 完整示例：cp-stl/examples/graph/topological_sort.cpp
#include <vector>

namespace cp {
// Kahn O(n+m)。返回长度 < n 表示有环；空图返回空序列。
inline std::vector<int> topological_sort(const std::vector<std::vector<int>>& g) {
    std::vector<int> degree(g.size()), order;
    for (const auto& edges : g) for (int v : edges) ++degree[v];
    for (int u = 0; u < int(g.size()); ++u) if (!degree[u]) order.push_back(u);
    for (std::size_t i = 0; i < order.size(); ++i)
        for (int v : g[order[i]]) if (!--degree[v]) order.push_back(v);
    return order;
}
} // namespace cp
