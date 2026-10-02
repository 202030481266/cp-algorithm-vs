#pragma once
// 使用说明：cp-stl/docs/usage/graph/euler_path.md
// 完整示例：cp-stl/examples/graph/euler_path.cpp
#include <algorithm>
#include <cassert>
#include <optional>
#include <utility>
#include <vector>

namespace cp {
struct EulerTrail {
    std::vector<int> vertices; // 经过的点，长度为 边数+1
    std::vector<int> edges;    // 依次经过的边编号（add_edge 的返回值）
};

// 欧拉路径 / 欧拉回路（Hierholzer，非递归），有向图与无向图都支持，允许重边和自环。O(n + m)，
// smallest=true 时先给邻接表排序，得到字典序最小的点序列，O(n + m log m)。
class EulerPath {
    int n_;
    bool directed_;
    std::vector<std::pair<int, int>> edges_;
public:
    EulerPath(int n, bool directed) : n_(n), directed_(directed) {}
    int add_edge(int u, int v) {
        assert(0 <= u && u < n_ && 0 <= v && v < n_);
        edges_.emplace_back(u, v);
        return int(edges_.size()) - 1;
    }
    int edge_count() const { return int(edges_.size()); }
    // 找一条经过每条边恰好一次的路径。start = -1 时自动选择起点：
    // 有向图选出度比入度大 1 的点，无向图选编号较小的奇度点；都没有时选有边的最小编号点。
    // 不存在（度数条件不满足、指定起点不合法或边不连通）时返回 nullopt。没有边时返回只含起点的路径。
    std::optional<EulerTrail> find(int start = -1, bool smallest = false) const {
        int m = int(edges_.size());
        assert(-1 <= start && start < n_);
        if (m == 0) {
            if (n_ == 0) return std::nullopt;
            return EulerTrail{{start == -1 ? 0 : start}, {}};
        }
        std::vector<int> balance(n_), degree(n_);
        for (auto [u, v] : edges_) {
            ++degree[u]; ++degree[v];
            if (directed_) { ++balance[u]; --balance[v]; }
        }
        int forced = -1, odd = 0;
        for (int v = 0; v < n_; ++v) {
            if (directed_) {
                if (balance[v] == 1) { if (forced != -1) return std::nullopt; forced = v; }
                else if (balance[v] != 0 && balance[v] != -1) return std::nullopt;
            } else if (degree[v] & 1) {
                if (forced == -1) forced = v;
                ++odd;
            }
        }
        // 有向图的出入度差之和为 0：恰有一个 +1 时必然恰有一个 -1。
        if (!directed_ && odd != 0 && odd != 2) return std::nullopt;
        if (forced != -1) {
            if (start == -1) start = forced;
            else if (directed_ ? start != forced : !(degree[start] & 1)) return std::nullopt;
        } else if (start == -1) {
            start = 0;
            while (!degree[start]) ++start;
        } else if (!degree[start]) return std::nullopt;

        // CSR 邻接表：每条边存 (终点, 编号)；无向边在两端各存一份。
        std::vector<int> offset(n_ + 1);
        for (auto [u, v] : edges_) { ++offset[u + 1]; if (!directed_) ++offset[v + 1]; }
        for (int v = 0; v < n_; ++v) offset[v + 1] += offset[v];
        std::vector<std::pair<int, int>> adjacency(offset[n_]);
        std::vector<int> fill(offset.begin(), offset.end() - 1);
        for (int id = 0; id < m; ++id) {
            auto [u, v] = edges_[id];
            adjacency[fill[u]++] = {v, id};
            if (!directed_) adjacency[fill[v]++] = {u, id};
        }
        if (smallest)
            for (int v = 0; v < n_; ++v) std::sort(adjacency.begin() + offset[v], adjacency.begin() + offset[v + 1]);

        std::vector<int> next(offset.begin(), offset.end() - 1);
        std::vector<char> used(m);
        std::vector<std::pair<int, int>> stack{{start, -1}}; // (点, 到达它的边)
        EulerTrail trail;
        trail.vertices.reserve(m + 1);
        trail.edges.reserve(m + 1);
        while (!stack.empty()) {
            int u = stack.back().first;
            int& i = next[u];
            while (i < offset[u + 1] && used[adjacency[i].second]) ++i;
            if (i == offset[u + 1]) {
                trail.vertices.push_back(u);
                trail.edges.push_back(stack.back().second);
                stack.pop_back();
            } else {
                auto [v, id] = adjacency[i++];
                used[id] = 1;
                stack.emplace_back(v, id);
            }
        }
        if (int(trail.vertices.size()) != m + 1) return std::nullopt; // 有边不在起点所在的连通块中
        std::reverse(trail.vertices.begin(), trail.vertices.end());
        std::reverse(trail.edges.begin(), trail.edges.end());
        trail.edges.erase(trail.edges.begin()); // 去掉起点的占位 -1
        return trail;
    }
};
} // namespace cp
