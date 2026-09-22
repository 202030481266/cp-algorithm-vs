#pragma once
// 使用说明：cp-stl/docs/usage/graph/dinic.md
// 完整示例：cp-stl/examples/graph/dinic.cpp
#include <algorithm>
#include <cassert>
#include <limits>
#include <vector>

namespace cp {
// Dinic 最大流 O(V^2 E)，容量非负。dfs 递归深度 O(V)，超深网络需注意栈空间。
// flow 可重复调用，返回这次新增流量；调用后保留残量网络。
class Dinic {
    struct Edge { int to, reverse; long long capacity; };
    std::vector<std::vector<Edge>> graph_;
    std::vector<int> level_, next_;
    long long dfs(int u, int sink, long long available) {
        if (u == sink) return available;
        for (int& i = next_[u]; i < int(graph_[u].size()); ++i) {
            Edge& e = graph_[u][i];
            if (e.capacity && level_[e.to] == level_[u] + 1) {
                long long sent = dfs(e.to, sink, std::min(available, e.capacity));
                if (sent) {
                    e.capacity -= sent;
                    graph_[e.to][e.reverse].capacity += sent;
                    return sent;
                }
            }
        }
        return 0;
    }
    bool bfs(int source, int sink) {
        std::fill(level_.begin(), level_.end(), -1);
        std::vector<int> queue{source};
        level_[source] = 0;
        for (std::size_t i = 0; i < queue.size(); ++i)
            for (auto e : graph_[queue[i]]) if (e.capacity && level_[e.to] == -1) {
                level_[e.to] = level_[queue[i]] + 1;
                queue.push_back(e.to);
            }
        return level_[sink] != -1;
    }
public:
    explicit Dinic(int n) : graph_(n), level_(n), next_(n) {}
    void add_edge(int from, int to, long long capacity) {
        int n = int(graph_.size());
        assert(0 <= from && from < n && 0 <= to && to < n && capacity >= 0);
        int a = int(graph_[from].size()), b = int(graph_[to].size());
        graph_[from].push_back({to, b + (from == to), capacity});
        graph_[to].push_back({from, a, 0});
    }
    long long flow(int source, int sink, long long limit = std::numeric_limits<long long>::max()) {
        assert(0 <= source && source < int(graph_.size()));
        assert(0 <= sink && sink < int(graph_.size()) && source != sink && limit >= 0);
        long long total = 0;
        while (total < limit && bfs(source, sink)) {
            std::fill(next_.begin(), next_.end(), 0);
            while (total < limit) {
                long long sent = dfs(source, sink, limit - total);
                if (!sent) break;
                total += sent;
            }
        }
        return total;
    }
    // 求完最大流（未被 limit 截断）后，返回最小割源点侧。
    std::vector<bool> min_cut(int source) const {
        assert(0 <= source && source < int(graph_.size()));
        std::vector<bool> seen(graph_.size());
        std::vector<int> queue{source};
        seen[source] = true;
        for (std::size_t i = 0; i < queue.size(); ++i)
            for (auto e : graph_[queue[i]]) if (e.capacity && !seen[e.to]) {
                seen[e.to] = true;
                queue.push_back(e.to);
            }
        return seen;
    }
};
} // namespace cp
