#pragma once
// 使用说明：cp-stl/docs/usage/graph/spfa.md
// 完整示例：cp-stl/examples/graph/spfa.cpp
#include <cassert>
#include <optional>
#include <vector>
#include "shortest_path.hpp"

namespace cp {
namespace spfa_detail {
// 队列优化的 Bellman-Ford。dist 已初始化，starts 是初始入队的点。
// length[v] 记录当前最短路的边数，达到 n 说明路径上有重复点，即存在负环，返回 false。
inline bool relax(const WeightedGraph& g, std::vector<long long>& dist, const std::vector<int>& starts) {
    int n = int(g.size());
    std::vector<int> length(n), queue(n);
    std::vector<char> in_queue(n);
    int head = 0, count = 0; // queue 是长度 n 的环形队列，每个点至多在队列中出现一次。
    for (int s : starts) { queue[(head + count++) % n] = s; in_queue[s] = 1; }
    while (count) {
        int u = queue[head];
        head = (head + 1) % n;
        --count;
        in_queue[u] = 0;
        for (auto [v, w] : g[u]) {
            if (dist[u] + w < dist[v]) {
                dist[v] = dist[u] + w;
                length[v] = length[u] + 1;
                if (length[v] >= n) return false;
                if (!in_queue[v]) { in_queue[v] = 1; queue[(head + count++) % n] = v; }
            }
        }
    }
    return true;
}
} // namespace spfa_detail

// 允许负边的单源最短路（SPFA），最坏 O(nm)。从 source 可达负环时返回 nullopt；不可达为 INF64。
// 所有有限距离的绝对值必须明显小于 INF64。
inline std::optional<std::vector<long long>> spfa(const WeightedGraph& g, int source) {
    assert(0 <= source && source < int(g.size()));
    std::vector<long long> dist(g.size(), INF64);
    dist[source] = 0;
    if (!spfa_detail::relax(g, dist, {source})) return std::nullopt;
    return dist;
}

// 整张图（不要求从某点可达）是否存在负环：相当于从虚拟源点向所有点连 0 边。
inline bool has_negative_cycle(const WeightedGraph& g) {
    std::vector<long long> dist(g.size(), 0);
    std::vector<int> all(g.size());
    for (int i = 0; i < int(g.size()); ++i) all[i] = i;
    return !spfa_detail::relax(g, dist, all);
}

// 差分约束：变量 x[0..n)，每条约束形如 x[v] - x[u] <= w。
// solve() 无解返回 nullopt；有解时返回所有 x <= 0 的解中逐个最大的那一个（相当于虚拟源点到各点的最短路）。
class DifferenceConstraints {
    WeightedGraph g_;
public:
    explicit DifferenceConstraints(int n) : g_(n) {}
    // x[v] - x[u] <= w
    void add_less_equal(int u, int v, long long w) {
        assert(0 <= u && u < int(g_.size()) && 0 <= v && v < int(g_.size()));
        g_[u].push_back({v, w});
    }
    // x[v] - x[u] >= w
    void add_greater_equal(int u, int v, long long w) { add_less_equal(v, u, -w); }
    // x[v] - x[u] == w
    void add_equal(int u, int v, long long w) { add_less_equal(u, v, w); add_less_equal(v, u, -w); }
    std::optional<std::vector<long long>> solve() const {
        std::vector<long long> x(g_.size(), 0);
        std::vector<int> all(g_.size());
        for (int i = 0; i < int(g_.size()); ++i) all[i] = i;
        if (!spfa_detail::relax(g_, x, all)) return std::nullopt;
        return x;
    }
};

// Johnson 全源最短路：允许负边，存在负环时返回 nullopt。O(nm log m)，结果 dist[u][v]，不可达为 INF64。
// 先用 SPFA 求势能 h，把边权改成 w + h[u] - h[v] >= 0，再从每个点跑一次 Dijkstra。
inline std::optional<std::vector<std::vector<long long>>> johnson(const WeightedGraph& g) {
    int n = int(g.size());
    std::vector<long long> h(n, 0);
    std::vector<int> all(n);
    for (int i = 0; i < n; ++i) all[i] = i;
    if (!spfa_detail::relax(g, h, all)) return std::nullopt;
    WeightedGraph reweighted(n);
    for (int u = 0; u < n; ++u)
        for (auto [v, w] : g[u]) reweighted[u].push_back({v, w + h[u] - h[v]});
    std::vector<std::vector<long long>> dist(n);
    for (int s = 0; s < n; ++s) {
        dist[s] = dijkstra(reweighted, s);
        for (int v = 0; v < n; ++v)
            if (dist[s][v] != INF64) dist[s][v] += h[v] - h[s];
    }
    return dist;
}
} // namespace cp
