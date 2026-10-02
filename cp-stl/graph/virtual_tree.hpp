#pragma once
// 使用说明：cp-stl/docs/usage/graph/virtual_tree.md
// 完整示例：cp-stl/examples/graph/virtual_tree.cpp
#include <algorithm>
#include <cassert>
#include <vector>
#include "fast_lca.hpp"

namespace cp {
struct VirtualTree {
    std::vector<int> vertices; // 虚树中的原树点，按 DFS 序排列，vertices[0] 是虚树的根
    std::vector<int> parent;   // parent[i] 是 vertices[i] 在虚树中的父亲在 vertices 中的下标，根为 -1
};

// 虚树：只保留关键点及它们两两的 LCA，祖先关系与原树一致。keys 可以乱序、可以重复。
// O(k log k)（k 为关键点数），配合 FastLCA 的 O(1) LCA。原树中的边长可用 depth 之差计算。
inline VirtualTree virtual_tree(const FastLCA& tree, std::vector<int> keys) {
    auto by_tin = [&](int a, int b) { return tree.tin[a] < tree.tin[b]; };
    std::sort(keys.begin(), keys.end(), by_tin);
    keys.erase(std::unique(keys.begin(), keys.end()), keys.end());
    int k = int(keys.size());
    for (int i = 0; i + 1 < k; ++i) keys.push_back(tree.lca(keys[i], keys[i + 1]));
    std::sort(keys.begin(), keys.end(), by_tin);
    keys.erase(std::unique(keys.begin(), keys.end()), keys.end());
    VirtualTree result{keys, std::vector<int>(keys.size(), -1)};
    // 按 DFS 序相邻两点的 LCA 就是后一个点在虚树中的父亲。
    for (int i = 1; i < int(keys.size()); ++i) {
        int p = tree.lca(keys[i - 1], keys[i]);
        result.parent[i] = int(std::lower_bound(keys.begin(), keys.end(), p, by_tin) - keys.begin());
    }
    return result;
}
} // namespace cp
