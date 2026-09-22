#pragma once
// 使用说明：cp-stl/docs/usage/graph/shortest_path.md
// 完整示例：cp-stl/examples/graph/shortest_path.cpp
#include <cassert>
#include <deque>
#include <functional>
#include <limits>
#include <queue>
#include <utility>
#include <vector>

namespace cp {
inline constexpr long long INF64 = std::numeric_limits<long long>::max() / 4;
struct Edge { int to; long long weight; };
using WeightedGraph = std::vector<std::vector<Edge>>;

// 无权图 O(n+m)，不可达距离为 -1。
inline std::vector<int> bfs(const std::vector<std::vector<int>>& g, int source) {
    assert(0 <= source && source < int(g.size()));
    std::vector<int> dist(g.size(), -1), queue{source};
    dist[source] = 0;
    for (std::size_t i = 0; i < queue.size(); ++i) {
        int u = queue[i];
        for (int v : g[u]) if (dist[v] == -1) {
            dist[v] = dist[u] + 1;
            queue.push_back(v);
        }
    }
    return dist;
}

// 非负边权；有限距离必须 < INF64。O((n+m) log n)，不可达为 INF64。
inline std::vector<long long> dijkstra(const WeightedGraph& g, int source) {
    assert(0 <= source && source < int(g.size()));
    std::vector<long long> dist(g.size(), INF64);
    using State = std::pair<long long, int>;
    std::priority_queue<State, std::vector<State>, std::greater<State>> queue;
    queue.emplace(dist[source] = 0, source);
    while (!queue.empty()) {
        auto [d, u] = queue.top(); queue.pop();
        if (d != dist[u]) continue;
        for (auto [v, w] : g[u]) {
            assert(0 <= w && w < INF64);
            if (w < INF64 - d && d + w < dist[v]) {
                dist[v] = d + w;
                queue.emplace(dist[v], v);
            }
        }
    }
    return dist;
}

// 边权只能为 0 或 1。O(n+m)，不可达为 INF64。
inline std::vector<long long> zero_one_bfs(const WeightedGraph& g, int source) {
    assert(0 <= source && source < int(g.size()));
    std::vector<long long> dist(g.size(), INF64);
    std::deque<std::pair<long long, int>> queue;
    queue.emplace_back(dist[source] = 0, source);
    while (!queue.empty()) {
        auto [d, u] = queue.front(); queue.pop_front();
        if (d != dist[u]) continue;
        for (auto [v, w] : g[u]) {
            assert(w == 0 || w == 1);
            if (d + w < dist[v]) {
                dist[v] = d + w;
                if (w) queue.emplace_back(dist[v], v);
                else queue.emplace_front(dist[v], v);
            }
        }
    }
    return dist;
}

// 邻接矩阵：对角线置 0，重边取 min，不存在的边置 INF64。
// O(n^3)；可有负边，但不能有负环，有限路径的绝对值须 < INF64。
inline void floyd_warshall(std::vector<std::vector<long long>>& dist) {
    int n = int(dist.size());
    for (int k = 0; k < n; ++k)
        for (int i = 0; i < n; ++i) if (dist[i][k] != INF64)
            for (int j = 0; j < n; ++j) if (dist[k][j] != INF64)
                if (dist[i][k] + dist[k][j] < dist[i][j])
                    dist[i][j] = dist[i][k] + dist[k][j];
}
} // namespace cp
