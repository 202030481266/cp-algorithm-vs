#pragma once
// 使用说明：cp-stl/docs/usage/graph/kruskal_tree.md
// 完整示例：cp-stl/examples/graph/kruskal_tree.cpp
#include <algorithm>
#include <cassert>
#include <functional>
#include <numeric>
#include <optional>
#include <vector>
#include "mst.hpp"

namespace cp {
// Kruskal 重构树：按 better 的顺序加边（默认边权从小到大），每次成功合并两个连通块就新建一个点，
// 点权为这条边的边权，两个连通块的根成为它的儿子。点 0..n-1 是原图的点（叶子），n.. 是新点。
// 性质：任意两点 u、v 的 LCA 的点权就是“所有 u-v 路径中最大边权的最小值”（最小瓶颈）；
// 从 u 往上走，点权单调不降，满足“点权 <= limit”的最高祖先的子树叶子 = 只走边权 <= limit 的边能到达的点。
// 建树 O(m log m)，倍增预处理 O(n log n)，查询 O(log n)。图不连通时得到森林。
class KruskalTree {
    std::vector<std::vector<int>> up_; // up_[k][x]：x 的第 2^k 个祖先，根的祖先是自己
public:
    int n;                            // 原图点数
    std::vector<int> parent;          // 根为 -1
    std::vector<long long> weight;    // 新点的点权；原图点为 0
    std::vector<std::vector<int>> children;
    std::vector<int> depth;
    std::vector<int> leaf_order;      // 叶子（原图点）的 DFS 顺序
    std::vector<int> leaf_begin, leaf_end; // 子树中的叶子是 leaf_order[leaf_begin[x], leaf_end[x])

    template<class Compare = std::less<long long>>
    KruskalTree(int vertex_count, std::vector<UndirectedEdge> edges, Compare better = {})
        : n(vertex_count), parent(2 * vertex_count, -1), weight(2 * vertex_count, 0),
          children(2 * vertex_count) {
        std::stable_sort(edges.begin(), edges.end(),
                         [&](const UndirectedEdge& a, const UndirectedEdge& b) { return better(a.weight, b.weight); });
        std::vector<int> leader(2 * n); // 并查集：每个连通块指向它当前的树根
        std::iota(leader.begin(), leader.end(), 0);
        auto find = [&](int x) {
            int root = x;
            while (leader[root] != root) root = leader[root];
            while (leader[x] != root) { int next = leader[x]; leader[x] = root; x = next; }
            return root;
        };
        int count = n;
        for (const auto& e : edges) {
            assert(0 <= e.u && e.u < n && 0 <= e.v && e.v < n);
            int a = find(e.u), b = find(e.v);
            if (a == b) continue;
            int z = count++;
            weight[z] = e.weight;
            parent[a] = parent[b] = z;
            leader[a] = leader[b] = z;
            children[z] = {a, b};
        }
        parent.resize(count);
        weight.resize(count);
        children.resize(count);
        depth.assign(count, 0);
        leaf_begin.assign(count, 0);
        leaf_end.assign(count, 0);
        int levels = 1;
        while ((1 << levels) < count) ++levels;
        up_.assign(levels, std::vector<int>(count));
        // 从每个根出发做非递归 DFS：先序给深度和倍增表，后序给叶子区间。
        std::vector<std::pair<int, int>> stack; // (点, 下一个儿子下标)
        for (int r = count - 1; r >= 0; --r) {
            if (parent[r] != -1) continue;
            up_[0][r] = r;
            stack.emplace_back(r, 0);
            leaf_begin[r] = int(leaf_order.size());
            while (!stack.empty()) {
                auto& [x, i] = stack.back();
                if (x < n && i == 0) leaf_order.push_back(x);
                if (i < int(children[x].size())) {
                    int c = children[x][i++];
                    depth[c] = depth[x] + 1;
                    up_[0][c] = x;
                    leaf_begin[c] = int(leaf_order.size());
                    stack.emplace_back(c, 0);
                } else {
                    leaf_end[x] = int(leaf_order.size());
                    stack.pop_back();
                }
            }
        }
        for (int k = 1; k < levels; ++k)
            for (int x = 0; x < count; ++x) up_[k][x] = up_[k - 1][up_[k - 1][x]];
    }
    int size() const { return int(parent.size()); }
    // 从 x 往上，满足 pred(点权) 的最高祖先（只检查 x 的真祖先；没有满足的祖先时返回 x 本身）。
    // pred 必须沿祖先链单调：靠下为真、靠上为假，例如 [&](long long w) { return w <= limit; }。
    template<class Predicate>
    int highest(int x, Predicate pred) const {
        assert(0 <= x && x < size());
        for (int k = int(up_.size()) - 1; k >= 0; --k) {
            int y = up_[k][x];
            if (y != x && pred(weight[y])) x = y;
        }
        return x;
    }
    // 不连通返回 -1。
    int lca(int u, int v) const {
        assert(0 <= u && u < size() && 0 <= v && v < size());
        if (depth[u] < depth[v]) std::swap(u, v);
        for (int k = int(up_.size()) - 1; k >= 0; --k)
            if (depth[u] - (1 << k) >= depth[v]) u = up_[k][u];
        if (u == v) return u;
        for (int k = int(up_.size()) - 1; k >= 0; --k)
            if (up_[k][u] != up_[k][v]) { u = up_[k][u]; v = up_[k][v]; }
        return up_[0][u] == up_[0][v] && parent[u] != -1 ? up_[0][u] : -1;
    }
    // u、v（原图点）之间所有路径的最大边权的最小值；不连通或 u == v 时为 nullopt。
    std::optional<long long> bottleneck(int u, int v) const {
        assert(0 <= u && u < n && 0 <= v && v < n);
        int z = lca(u, v);
        if (z == -1 || u == v) return std::nullopt;
        return weight[z];
    }
};
} // namespace cp
