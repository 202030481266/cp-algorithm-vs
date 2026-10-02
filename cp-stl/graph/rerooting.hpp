#pragma once
// 使用说明：cp-stl/docs/usage/graph/rerooting.md
// 完整示例：cp-stl/examples/graph/rerooting.cpp
#include <cassert>
#include <utility>
#include <vector>

namespace cp {
// 换根 DP（全方位树 DP）：对每个点 v，求以 v 为整棵树的根时的 DP 值。树或森林由边表给出，点 0..n-1。
// 用户提供三个运算：
//   merge(a, b)                       合并两个儿子方向的贡献，需结合、交换，identity 为单位元；
//   add_edge(dp, child, parent, id)   以 child 为根的子树（DP 值为 dp）经过边 id 贡献给 parent 的值；
//   add_vertex(merged, v)             v 的所有儿子贡献合并为 merged 后，加上 v 本身得到以 v 为根的子树 DP 值。
// 两遍非递归扫描，O(n) 次运算。返回 answer[v] = 以 v 为根时整棵树的 DP 值。
template<class T, class Merge, class AddEdge, class AddVertex>
std::vector<T> rerooting(int n, const std::vector<std::pair<int, int>>& edges, const T& identity,
                         Merge merge, AddEdge add_edge, AddVertex add_vertex) {
    std::vector<int> offset(n + 1);
    for (auto [u, v] : edges) {
        assert(0 <= u && u < n && 0 <= v && v < n && u != v);
        ++offset[u + 1];
        ++offset[v + 1];
    }
    for (int v = 0; v < n; ++v) offset[v + 1] += offset[v];
    std::vector<std::pair<int, int>> adj(offset[n]); // (邻点, 边编号)
    {
        std::vector<int> fill(offset.begin(), offset.end() - 1);
        for (int id = 0; id < int(edges.size()); ++id) {
            auto [u, v] = edges[id];
            adj[fill[u]++] = {v, id};
            adj[fill[v]++] = {u, id};
        }
    }
    // BFS 序；parent_edge[v] 为连向父亲的边，根为 -1。
    std::vector<int> order, parent(n, -1), parent_edge(n, -1);
    std::vector<char> seen(n);
    order.reserve(n);
    int components = 0;
    for (int s = 0; s < n; ++s) {
        if (seen[s]) continue;
        ++components;
        seen[s] = 1;
        order.push_back(s);
        for (std::size_t i = order.size() - 1; i < order.size(); ++i) {
            int u = order[i];
            for (int k = offset[u]; k < offset[u + 1]; ++k) {
                auto [v, id] = adj[k];
                if (seen[v]) continue;
                seen[v] = 1;
                parent[v] = u;
                parent_edge[v] = id;
                order.push_back(v);
            }
        }
    }
    assert(int(edges.size()) == n - components); // 必须是森林：每条边都是 BFS 树边
    (void)components;
    // 自底向上：down[v] 是 v 子树的 DP 值，to_parent[v] 是它经过父边贡献给父亲的值。
    std::vector<T> down(n, identity), to_parent(n, identity), from_parent(n, identity), answer(n, identity);
    for (int i = n - 1; i >= 0; --i) {
        int v = order[i];
        T merged = identity;
        for (int k = offset[v]; k < offset[v + 1]; ++k) {
            int c = adj[k].first;
            if (adj[k].second != parent_edge[v]) merged = merge(merged, to_parent[c]);
        }
        down[v] = add_vertex(merged, v);
        if (parent[v] != -1) to_parent[v] = add_edge(down[v], v, parent[v], parent_edge[v]);
    }
    // 自顶向下：from_parent[v] = 以 v 为根时，父亲那一侧作为 v 的一个儿子提供的贡献。
    std::vector<T> prefix, suffix;
    for (int v : order) {
        int degree = offset[v + 1] - offset[v];
        prefix.assign(degree + 1, identity);
        suffix.assign(degree + 1, identity);
        for (int k = 0; k < degree; ++k) {
            auto [c, id] = adj[offset[v] + k];
            prefix[k + 1] = merge(prefix[k], id == parent_edge[v] ? from_parent[v] : to_parent[c]);
        }
        for (int k = degree - 1; k >= 0; --k) {
            auto [c, id] = adj[offset[v] + k];
            suffix[k] = merge(id == parent_edge[v] ? from_parent[v] : to_parent[c], suffix[k + 1]);
        }
        answer[v] = add_vertex(prefix[degree], v);
        for (int k = 0; k < degree; ++k) {
            auto [c, id] = adj[offset[v] + k];
            if (id == parent_edge[v]) continue;
            T without_c = add_vertex(merge(prefix[k], suffix[k + 1]), v);
            from_parent[c] = add_edge(without_c, v, c, id);
        }
    }
    return answer;
}
} // namespace cp
