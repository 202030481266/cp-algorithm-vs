#pragma once
// 使用说明：cp-stl/docs/usage/graph/tree_diameter.md
// 完整示例：cp-stl/examples/graph/tree_diameter.cpp
#include <algorithm>
#include <cassert>
#include <utility>
#include <vector>

namespace cp {
// 无权无向树，0-based；返回直径上的点序列，长度为 path.size()-1。
// O(n)。空树返回空序列；不能直接用于一般图或有负边的带权树。
inline std::vector<int> tree_diameter(const std::vector<std::vector<int>>& g) {
    if (g.empty()) return {};
    auto farthest = [&](int source) {
        std::vector<int> parent(g.size(), -2), queue{source};
        parent[source] = -1;
        for (std::size_t i = 0; i < queue.size(); ++i)
            for (int v : g[queue[i]]) if (parent[v] == -2) {
                parent[v] = queue[i];
                queue.push_back(v);
            }
        assert(queue.size() == g.size());
        return std::make_pair(queue.back(), parent);
    };
    int source = farthest(0).first;
    auto [target, parent] = farthest(source);
    std::vector<int> path;
    for (int u = target; u != -1; u = parent[u]) path.push_back(u);
    std::reverse(path.begin(), path.end());
    return path;
}
} // namespace cp
