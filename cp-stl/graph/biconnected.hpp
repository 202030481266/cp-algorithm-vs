#pragma once
// 使用说明：cp-stl/docs/usage/graph/biconnected.md
// 完整示例：cp-stl/examples/graph/biconnected.cpp
#include <algorithm>
#include <cassert>
#include <utility>
#include <vector>

namespace cp {
// 无向图的割点、桥、边双连通分量、点双连通分量与圆方树。允许重边和自环，O(n + m)。
// 非递归 Tarjan：用显式栈模拟 DFS，链状图也不会爆栈。边编号为 edges 中的下标。
class Biconnected {
    int n_;
    std::vector<int> offset_;               // CSR 邻接表
    std::vector<std::pair<int, int>> adj_;  // (邻点, 边编号)
public:
    std::vector<char> is_cut;               // 每个点是否为割点
    std::vector<char> is_bridge;            // 每条边是否为桥
    std::vector<int> two_edge_id;           // 每个点所在边双连通分量的编号
    int two_edge_count = 0;
    std::vector<std::vector<int>> blocks;   // 点双连通分量（点集）；孤立点自成一块

    Biconnected(int n, const std::vector<std::pair<int, int>>& edges)
        : n_(n), offset_(n + 1), is_cut(n), is_bridge(edges.size()), two_edge_id(n, -1) {
        int m = int(edges.size());
        for (auto [u, v] : edges) {
            assert(0 <= u && u < n && 0 <= v && v < n);
            ++offset_[u + 1];
            ++offset_[v + 1];
        }
        for (int v = 0; v < n; ++v) offset_[v + 1] += offset_[v];
        adj_.resize(offset_[n]);
        std::vector<int> fill(offset_.begin(), offset_.end() - 1);
        for (int id = 0; id < m; ++id) {
            auto [u, v] = edges[id];
            adj_[fill[u]++] = {v, id};
            adj_[fill[v]++] = {u, id};
        }

        std::vector<int> tin(n, -1), low(n), vertex_stack;
        struct Frame { int u, parent_edge, next; };
        std::vector<Frame> frames;
        int timer = 0;
        for (int root = 0; root < n; ++root) {
            if (tin[root] != -1) continue;
            tin[root] = low[root] = timer++;
            vertex_stack.push_back(root);
            frames.push_back({root, -1, offset_[root]});
            int root_children = 0;
            while (!frames.empty()) {
                Frame& f = frames.back();
                int u = f.u;
                if (f.next < offset_[u + 1]) {
                    auto [v, id] = adj_[f.next++];
                    if (id == f.parent_edge) continue; // 只跳过来时的那一条边，重边仍然有效
                    if (tin[v] == -1) {
                        tin[v] = low[v] = timer++;
                        vertex_stack.push_back(v);
                        frames.push_back({v, id, offset_[v]}); // f 在这之后失效
                    } else {
                        low[u] = std::min(low[u], tin[v]);
                    }
                    continue;
                }
                int parent_edge = f.parent_edge;
                frames.pop_back();
                if (frames.empty()) break;
                int p = frames.back().u;
                low[p] = std::min(low[p], low[u]);
                if (low[u] > tin[p]) is_bridge[parent_edge] = 1;
                if (low[u] >= tin[p]) { // p 把 u 所在的子树切开：弹出一个点双
                    if (p != root || ++root_children > 1) is_cut[p] = 1;
                    std::vector<int> block;
                    int x;
                    do {
                        x = vertex_stack.back();
                        vertex_stack.pop_back();
                        block.push_back(x);
                    } while (x != u);
                    block.push_back(p);
                    blocks.push_back(std::move(block));
                }
            }
            vertex_stack.pop_back(); // 栈底只剩 root
            if (root_children == 0) blocks.push_back({root}); // 没有其他邻点：孤立点
        }
        // 去掉桥之后的连通块就是边双连通分量。
        std::vector<int> queue;
        for (int s = 0; s < n; ++s) {
            if (two_edge_id[s] != -1) continue;
            two_edge_id[s] = two_edge_count;
            queue.assign(1, s);
            for (std::size_t i = 0; i < queue.size(); ++i) {
                int u = queue[i];
                for (int k = offset_[u]; k < offset_[u + 1]; ++k) {
                    auto [v, id] = adj_[k];
                    if (!is_bridge[id] && two_edge_id[v] == -1) {
                        two_edge_id[v] = two_edge_count;
                        queue.push_back(v);
                    }
                }
            }
            ++two_edge_count;
        }
    }
    std::vector<int> bridges() const {
        std::vector<int> result;
        for (int id = 0; id < int(is_bridge.size()); ++id) if (is_bridge[id]) result.push_back(id);
        return result;
    }
    std::vector<int> cut_vertices() const {
        std::vector<int> result;
        for (int v = 0; v < n_; ++v) if (is_cut[v]) result.push_back(v);
        return result;
    }
    // 每个边双连通分量的点集，按编号排列。
    std::vector<std::vector<int>> two_edge_components() const {
        std::vector<std::vector<int>> groups(two_edge_count);
        for (int v = 0; v < n_; ++v) groups[two_edge_id[v]].push_back(v);
        return groups;
    }
    // 圆方树：点 0..n-1 是原图的点（圆点），n+i 是第 i 个点双（方点），方点与块内每个点连边。
    std::vector<std::vector<int>> block_cut_tree() const {
        std::vector<std::vector<int>> tree(n_ + blocks.size());
        for (int i = 0; i < int(blocks.size()); ++i)
            for (int v : blocks[i]) {
                tree[n_ + i].push_back(v);
                tree[v].push_back(n_ + i);
            }
        return tree;
    }
};
} // namespace cp
