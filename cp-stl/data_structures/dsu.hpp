#pragma once
// 使用说明：cp-stl/docs/usage/data_structures/dsu.md
// 完整示例：cp-stl/examples/data_structures/dsu.cpp
#include <algorithm>
#include <cassert>
#include <vector>

namespace cp {
// 0-based；按大小合并 + 路径压缩，均摊 O(alpha(n))。
struct DSU {
    std::vector<int> parent; // 根存负的集合大小。
    int components;
    explicit DSU(int n) : parent(n, -1), components(n) {}
    int find(int x) {
        assert(0 <= x && x < int(parent.size()));
        int root = x;
        while (parent[root] >= 0) root = parent[root];
        while (x != root) {
            int next = parent[x];
            parent[x] = root;
            x = next;
        }
        return root;
    }
    bool merge(int a, int b) {
        a = find(a); b = find(b);
        if (a == b) return false;
        if (parent[a] > parent[b]) std::swap(a, b);
        parent[a] += parent[b];
        parent[b] = a;
        --components;
        return true;
    }
    bool same(int a, int b) { return find(a) == find(b); }
    int size(int x) { return -parent[find(x)]; }
};
} // namespace cp
