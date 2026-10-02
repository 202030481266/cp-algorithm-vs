// Deterministic property tests for the graph templates added after the original import.
// Each section compares a template with a small brute-force oracle on random data.
#ifdef NDEBUG
#error Tests require assertions
#endif
#include <algorithm>
#include <cassert>
#include <climits>
#include <functional>
#include <iostream>
#include <map>
#include <numeric>
#include <optional>
#include <random>
#include <set>
#include <string>
#include <utility>
#include <vector>
#include "graph/spfa.hpp"
#include "graph/euler_path.hpp"
#include "graph/biconnected.hpp"
#include "graph/kruskal_tree.hpp"
#include "graph/fast_lca.hpp"
#include "graph/lca.hpp"
#include "graph/virtual_tree.hpp"
#include "graph/centroid.hpp"
#include "graph/dsu_on_tree.hpp"
#include "graph/rerooting.hpp"
#include "graph/segment_tree_graph.hpp"

using namespace std;
using namespace cp;
mt19937_64 rng(20261003);
long long random_ll(long long l, long long r) { return uniform_int_distribution<long long>(l, r)(rng); }
int random_int(int l, int r) { return int(random_ll(l, r)); }

vector<vector<int>> random_tree(int n) {
    vector<vector<int>> g(n);
    vector<int> label(n);
    iota(label.begin(), label.end(), 0);
    shuffle(label.begin(), label.end(), rng);
    for (int i = 1; i < n; ++i) {
        int p = random_int(0, 2) ? random_int(max(0, i - 3), i - 1) : random_int(0, i - 1); // 混合长链与随机树
        g[label[i]].push_back(label[p]);
        g[label[p]].push_back(label[i]);
    }
    for (auto& adj : g) shuffle(adj.begin(), adj.end(), rng);
    return g;
}

// Floyd 判负环与全源最短路，作为 SPFA / Johnson / 差分约束的独立对照。
struct FloydResult { vector<vector<long long>> dist; bool negative_cycle; };
FloydResult floyd(const WeightedGraph& g) {
    int n = int(g.size());
    const long long INF = LLONG_MAX / 4;
    vector<vector<long long>> d(n, vector<long long>(n, INF));
    for (int i = 0; i < n; ++i) d[i][i] = 0;
    for (int u = 0; u < n; ++u) for (auto [v, w] : g[u]) d[u][v] = min(d[u][v], w);
    for (int k = 0; k < n; ++k)
        for (int i = 0; i < n; ++i) if (d[i][k] < INF)
            for (int j = 0; j < n; ++j) if (d[k][j] < INF) d[i][j] = min(d[i][j], d[i][k] + d[k][j]);
    bool negative = false;
    for (int i = 0; i < n; ++i) negative |= d[i][i] < 0;
    return {d, negative};
}

void spfa_checks() {
    for (int round = 0; round < 500; ++round) {
        int n = random_int(1, 8), m = random_int(0, 16);
        WeightedGraph g(n);
        bool allow_negative_cycle = random_int(0, 1);
        for (int i = 0; i < m; ++i) {
            int u = random_int(0, n - 1), v = random_int(0, n - 1);
            long long w = allow_negative_cycle ? random_ll(-5, 10) : random_ll(0, 10);
            g[u].push_back({v, w});
        }
        if (!allow_negative_cycle) // 加入少量负边但保持无负环：用势能构造 w = w' + h[u] - h[v]
            for (int u = 0; u < n; ++u) for (auto& e : g[u]) e.weight += (u * 3 % 7) - (e.to * 3 % 7);
        auto expected = floyd(g);
        assert(has_negative_cycle(g) == expected.negative_cycle);
        int s = random_int(0, n - 1);
        bool reachable_cycle = false;
        for (int v = 0; v < n; ++v)
            if (expected.dist[s][v] < LLONG_MAX / 8 && expected.dist[v][v] < 0) reachable_cycle = true;
        auto dist = spfa(g, s);
        assert(dist.has_value() == !reachable_cycle);
        if (dist)
            for (int v = 0; v < n; ++v)
                assert((*dist)[v] == (expected.dist[s][v] >= LLONG_MAX / 8 ? INF64 : expected.dist[s][v]));
        auto all = johnson(g);
        assert(all.has_value() == !expected.negative_cycle);
        if (all)
            for (int u = 0; u < n; ++u) for (int v = 0; v < n; ++v)
                assert((*all)[u][v] == (expected.dist[u][v] >= LLONG_MAX / 8 ? INF64 : expected.dist[u][v]));
        // 差分约束：把同一张图看成约束 x[v] - x[u] <= w。
        DifferenceConstraints dc(n);
        for (int u = 0; u < n; ++u) for (auto [v, w] : g[u]) dc.add_less_equal(u, v, w);
        auto x = dc.solve();
        assert(x.has_value() == !expected.negative_cycle);
        if (x) {
            for (int u = 0; u < n; ++u) for (auto [v, w] : g[u]) assert((*x)[v] - (*x)[u] <= w);
            for (long long value : *x) assert(value <= 0);
        }
    }
    DifferenceConstraints mixed(3);
    mixed.add_equal(0, 1, 5);          // x1 - x0 = 5
    mixed.add_greater_equal(1, 2, 2);  // x2 - x1 >= 2
    mixed.add_less_equal(0, 2, 7);     // x2 - x0 <= 7
    auto x = mixed.solve();
    assert(x && (*x)[1] - (*x)[0] == 5 && (*x)[2] - (*x)[1] >= 2 && (*x)[2] - (*x)[0] <= 7);
    mixed.add_less_equal(0, 2, 6);     // 与上面矛盾
    assert(!mixed.solve());
}

// 暴力枚举所有欧拉路径（边数很少），返回字典序最小的点序列；不存在时为空。
optional<vector<int>> brute_euler(int n, const vector<pair<int, int>>& edges, bool directed, int start) {
    int m = int(edges.size());
    optional<vector<int>> best;
    vector<char> used(m);
    vector<int> path;
    function<void(int)> dfs = [&](int u) {
        if (int(path.size()) == m + 1) {
            if (!best || path < *best) best = path;
            return;
        }
        for (int id = 0; id < m; ++id) {
            if (used[id]) continue;
            auto [a, b] = edges[id];
            int v = a == u ? b : (!directed && b == u ? a : -1);
            if (v == -1) continue;
            used[id] = 1;
            path.push_back(v);
            dfs(v);
            path.pop_back();
            used[id] = 0;
        }
    };
    for (int s = 0; s < n; ++s) {
        if (start != -1 && s != start) continue;
        path = {s};
        dfs(s);
    }
    return best;
}

void euler_path_checks() {
    for (int round = 0; round < 3000; ++round) {
        int n = random_int(1, 5), m = random_int(0, 6);
        bool directed = random_int(0, 1);
        EulerPath euler(n, directed);
        vector<pair<int, int>> edges;
        for (int i = 0; i < m; ++i) {
            int u = random_int(0, n - 1), v = random_int(0, n - 1);
            edges.emplace_back(u, v);
            assert(euler.add_edge(u, v) == i);
        }
        int start = random_int(0, 3) ? -1 : random_int(0, n - 1);
        auto got = euler.find(start, true);
        if (m == 0) {
            assert(got && got->vertices == vector<int>{start == -1 ? 0 : start});
            continue;
        }
        auto expected = brute_euler(n, edges, directed, start);
        assert(got.has_value() == expected.has_value());
        if (!got) continue;
        assert(got->vertices == *expected);
        // 边序列与点序列一致，且每条边恰好一次。
        assert(int(got->edges.size()) == m);
        vector<int> seen(m);
        for (int i = 0; i < m; ++i) {
            int id = got->edges[i], a = got->vertices[i], b = got->vertices[i + 1];
            ++seen[id];
            auto [u, v] = edges[id];
            assert((u == a && v == b) || (!directed && u == b && v == a));
        }
        assert(count(seen.begin(), seen.end(), 1) == m);
        auto any = euler.find(start, false); // 不排序时也必须是合法路径
        assert(any && int(any->vertices.size()) == m + 1);
    }
}

// 删除某些点/边后的连通块数（暴力）。
int components_without(int n, const vector<pair<int, int>>& edges, int removed_vertex, int removed_edge) {
    vector<int> leader(n);
    iota(leader.begin(), leader.end(), 0);
    function<int(int)> find = [&](int x) { return leader[x] == x ? x : leader[x] = find(leader[x]); };
    for (int id = 0; id < int(edges.size()); ++id) {
        auto [u, v] = edges[id];
        if (id == removed_edge || u == removed_vertex || v == removed_vertex) continue;
        leader[find(u)] = find(v);
    }
    int count = 0;
    for (int v = 0; v < n; ++v) if (v != removed_vertex && find(v) == v) ++count;
    return count;
}

void biconnected_checks() {
    for (int round = 0; round < 1000; ++round) {
        int n = random_int(1, 9), m = random_int(0, 12);
        vector<pair<int, int>> edges;
        for (int i = 0; i < m; ++i) edges.emplace_back(random_int(0, n - 1), random_int(0, n - 1));
        Biconnected bc(n, edges);
        int base = components_without(n, edges, -1, -1);
        for (int id = 0; id < m; ++id)
            assert(bool(bc.is_bridge[id]) == (components_without(n, edges, -1, id) > base));
        // 割点：删去 v 后，原来 v 所在连通块分裂（删 v 后块数 > 原块数 - [v 是孤立点]）。
        for (int v = 0; v < n; ++v) {
            bool isolated = true;
            for (auto [a, b] : edges) if ((a == v) != (b == v)) isolated = false;
            int after = components_without(n, edges, v, -1);
            assert(bool(bc.is_cut[v]) == (after > base - (isolated ? 1 : 0)));
        }
        // 边双：u、v 在同一分量当且仅当删去所有桥后连通。
        vector<pair<int, int>> kept;
        for (int id = 0; id < m; ++id) if (!bc.is_bridge[id]) kept.push_back(edges[id]);
        for (int u = 0; u < n; ++u) for (int v = 0; v < n; ++v) {
            vector<int> leader(n);
            iota(leader.begin(), leader.end(), 0);
            function<int(int)> find = [&](int x) { return leader[x] == x ? x : leader[x] = find(leader[x]); };
            for (auto [a, b] : kept) leader[find(a)] = find(b);
            assert((bc.two_edge_id[u] == bc.two_edge_id[v]) == (find(u) == find(v)));
        }
        // 点双：u != v 同在某个块中，当且仅当相邻（非自环边），或连通且删去任意一个其他点都不能把它们分开。
        vector<vector<char>> together(n, vector<char>(n));
        vector<int> block_count(n);
        for (auto& block : bc.blocks) {
            set<int> unique(block.begin(), block.end());
            assert(unique.size() == block.size());
            for (int a : block) { ++block_count[a]; for (int b : block) together[a][b] = 1; }
        }
        auto connected_without = [&](int u, int v, int removed) {
            vector<int> leader(n);
            iota(leader.begin(), leader.end(), 0);
            function<int(int)> find = [&](int x) { return leader[x] == x ? x : leader[x] = find(leader[x]); };
            for (auto [a, b] : edges) if (a != removed && b != removed) leader[find(a)] = find(b);
            return find(u) == find(v);
        };
        for (int u = 0; u < n; ++u) {
            assert(block_count[u] >= 1);
            bool has_neighbor = false;
            for (auto [a, b] : edges) if ((a == u) != (b == u)) has_neighbor = true;
            if (has_neighbor) assert((block_count[u] > 1) == bool(bc.is_cut[u]));
            else assert(block_count[u] == 1);
            for (int v = u + 1; v < n; ++v) {
                bool adjacent = false;
                for (auto [a, b] : edges) if ((a == u && b == v) || (a == v && b == u)) adjacent = true;
                bool expected = adjacent;
                if (!expected && connected_without(u, v, -1)) {
                    expected = true;
                    for (int w = 0; w < n; ++w) if (w != u && w != v && !connected_without(u, v, w)) expected = false;
                }
                assert(bool(together[u][v]) == expected);
            }
        }
        // 圆方树：n + 块数 个点，边数等于所有块大小之和，且是森林。
        auto tree = bc.block_cut_tree();
        assert(int(tree.size()) == n + int(bc.blocks.size()));
        assert(bc.cut_vertices().size() == size_t(count(bc.is_cut.begin(), bc.is_cut.end(), 1)));
        assert(bc.bridges().size() == size_t(count(bc.is_bridge.begin(), bc.is_bridge.end(), 1)));
        auto groups = bc.two_edge_components();
        assert(int(groups.size()) == bc.two_edge_count);
    }
    // 20 万点长链：确认非递归实现不会爆栈。
    int n = 200000;
    vector<pair<int, int>> chain;
    for (int i = 0; i + 1 < n; ++i) chain.emplace_back(i, i + 1);
    Biconnected big(n, chain);
    assert(int(big.bridges().size()) == n - 1 && int(big.cut_vertices().size()) == n - 2);
    assert(int(big.blocks.size()) == n - 1 && big.two_edge_count == n);
}

void kruskal_tree_checks() {
    for (int round = 0; round < 500; ++round) {
        int n = random_int(1, 9), m = random_int(0, 14);
        vector<UndirectedEdge> edges;
        for (int i = 0; i < m; ++i) edges.push_back({random_int(0, n - 1), random_int(0, n - 1), random_ll(0, 8)});
        bool maximum = random_int(0, 1);
        auto check = [&](const KruskalTree& tree) {
            auto reachable = [&](int s, long long limit) { // 只走 maximum ? w >= limit : w <= limit 的边
                vector<int> seen(n);
                vector<int> queue{s};
                seen[s] = 1;
                for (size_t i = 0; i < queue.size(); ++i)
                    for (auto e : edges) {
                        if (maximum ? e.weight < limit : e.weight > limit) continue;
                        int u = queue[i], v = e.u == u ? e.v : e.v == u ? e.u : -1;
                        if (v != -1 && !seen[v]) { seen[v] = 1; queue.push_back(v); }
                    }
                return seen;
            };
            for (int u = 0; u < n; ++u) for (int v = 0; v < n; ++v) {
                // 最小瓶颈：最小的 limit 使 u 能只用 w <= limit 的边到达 v（最大生成树时反过来）。
                optional<long long> expected;
                if (u != v)
                    for (long long limit = 0; limit <= 8 && !expected; ++limit) {
                        long long real = maximum ? 8 - limit : limit;
                        if (reachable(u, real)[v]) expected = real;
                    }
                assert(tree.bottleneck(u, v) == expected);
            }
            for (int u = 0; u < n; ++u) for (long long limit = -1; limit <= 9; ++limit) {
                int top = maximum ? tree.highest(u, [&](long long w) { return w >= limit; })
                                  : tree.highest(u, [&](long long w) { return w <= limit; });
                auto seen = reachable(u, limit);
                vector<int> leaves(tree.leaf_order.begin() + tree.leaf_begin[top],
                                   tree.leaf_order.begin() + tree.leaf_end[top]);
                sort(leaves.begin(), leaves.end());
                vector<int> expected;
                for (int v = 0; v < n; ++v) if (seen[v]) expected.push_back(v);
                assert(leaves == expected);
            }
        };
        if (maximum) check(KruskalTree(n, edges, greater<long long>()));
        else check(KruskalTree(n, edges));
    }
}

void lca_family_checks() {
    for (int round = 0; round < 300; ++round) {
        int n = random_int(1, 40), root = random_int(0, n - 1);
        auto g = random_tree(n);
        FastLCA fast(g, root);
        LCA slow(g, root);
        for (int step = 0; step < 100; ++step) {
            int u = random_int(0, n - 1), v = random_int(0, n - 1);
            assert(fast.lca(u, v) == slow.lca(u, v));
            assert(fast.distance(u, v) == slow.distance(u, v));
            assert(fast.is_ancestor(u, v) == (slow.lca(u, v) == u));
        }
        for (int v = 0; v < n; ++v) {
            assert(fast.order[fast.tin[v]] == v && fast.depth[v] == slow.depth[v]);
            assert(fast.parent[v] == (v == root ? -1 : slow.kth_ancestor(v, 1)));
        }
        // 虚树：点集 = 关键点及两两 LCA；父亲 = 点集中最深的真祖先。
        for (int step = 0; step < 20; ++step) {
            vector<int> keys(random_int(0, 6));
            for (int& x : keys) x = random_int(0, n - 1);
            auto vt = virtual_tree(fast, keys);
            set<int> expected(keys.begin(), keys.end());
            for (int a : keys) for (int b : keys) expected.insert(slow.lca(a, b));
            assert(set<int>(vt.vertices.begin(), vt.vertices.end()) == expected);
            assert(vt.vertices.size() == expected.size());
            for (int i = 0; i < int(vt.vertices.size()); ++i) {
                if (i) assert(fast.tin[vt.vertices[i - 1]] < fast.tin[vt.vertices[i]]);
                int x = vt.vertices[i], best = -1;
                for (int y : expected)
                    if (y != x && slow.lca(x, y) == y && (best == -1 || slow.depth[y] > slow.depth[best])) best = y;
                assert(vt.parent[i] == (best == -1 ? -1 : int(find(vt.vertices.begin(), vt.vertices.end(), best) - vt.vertices.begin())));
            }
        }
    }
    // 长链：非递归 DFS。
    int n = 200000;
    vector<vector<int>> chain(n);
    for (int i = 0; i + 1 < n; ++i) { chain[i].push_back(i + 1); chain[i + 1].push_back(i); }
    FastLCA big(chain, 0);
    assert(big.lca(n - 1, n / 2) == n / 2 && big.distance(0, n - 1) == n - 1);
}

void centroid_checks() {
    for (int round = 0; round < 300; ++round) {
        int n = random_int(1, 40);
        auto g = random_tree(n);
        // 重心：暴力删点。
        vector<int> expected;
        int best = INT_MAX;
        vector<int> heaviest(n);
        for (int c = 0; c < n; ++c) {
            vector<int> seen(n);
            seen[c] = 1;
            int worst = 0;
            for (int s : g[c]) {
                vector<int> queue{s};
                seen[s] = 1;
                for (size_t i = 0; i < queue.size(); ++i)
                    for (int v : g[queue[i]]) if (!seen[v]) { seen[v] = 1; queue.push_back(v); }
                worst = max(worst, int(queue.size()));
            }
            heaviest[c] = worst;
            best = min(best, worst);
        }
        for (int c = 0; c < n; ++c) if (heaviest[c] == best) expected.push_back(c);
        assert(tree_centroids(g) == expected);
        // 点分树：每个重心的连通块按 can_visit 收集，c 必须是它的重心，层数与父亲一致。
        CentroidDecomposition cd(g);
        assert(int(cd.order.size()) == n);
        int max_level = 0;
        for (int c = 0; c < n; ++c) {
            vector<int> component{c};
            vector<int> seen(n);
            seen[c] = 1;
            for (size_t i = 0; i < component.size(); ++i)
                for (int v : g[component[i]]) if (!seen[v] && cd.can_visit(c, v)) { seen[v] = 1; component.push_back(v); }
            int size = int(component.size());
            for (int s : g[c]) if (seen[s] && s != c) {
                vector<int> part{s};
                vector<int> mark(n);
                mark[c] = mark[s] = 1;
                for (size_t i = 0; i < part.size(); ++i)
                    for (int v : g[part[i]]) if (!mark[v] && seen[v]) { mark[v] = 1; part.push_back(v); }
                assert(int(part.size()) * 2 <= size);
            }
            for (int v : component) if (v != c) {
                assert(cd.level[v] > cd.level[c]);
                // c 是 v 在点分树上的祖先。
                int x = v;
                while (x != -1 && x != c) x = cd.parent[x];
                assert(x == c);
            }
            if (cd.parent[c] != -1) assert(cd.level[cd.parent[c]] + 1 == cd.level[c]);
            max_level = max(max_level, cd.level[c]);
        }
        assert((1 << max_level) <= n);
    }
    int n = 200000;
    vector<vector<int>> chain(n);
    for (int i = 0; i + 1 < n; ++i) { chain[i].push_back(i + 1); chain[i + 1].push_back(i); }
    CentroidDecomposition big(chain);
    assert(*max_element(big.level.begin(), big.level.end()) <= 18);
}

void dsu_on_tree_checks() {
    for (int round = 0; round < 300; ++round) {
        int n = random_int(1, 40), root = random_int(0, n - 1);
        auto g = random_tree(n);
        vector<int> color(n);
        for (int& c : color) c = random_int(0, 4);
        vector<int> cnt(5), answer(n, -1), added(n);
        int distinct = 0;
        dsu_on_tree(g, root,
                    [&](int v) { assert(!added[v]); added[v] = 1; if (cnt[color[v]]++ == 0) ++distinct; },
                    [&](int v) { assert(added[v]); added[v] = 0; if (--cnt[color[v]] == 0) --distinct; },
                    [&](int v) { answer[v] = distinct; });
        assert(distinct == 0 && count(added.begin(), added.end(), 1) == 0);
        LCA lca(g, root);
        for (int v = 0; v < n; ++v) {
            set<int> colors;
            for (int u = 0; u < n; ++u) if (lca.lca(u, v) == v) colors.insert(color[u]);
            assert(answer[v] == int(colors.size()));
        }
    }
    int n = 200000;
    vector<vector<int>> chain(n);
    for (int i = 0; i + 1 < n; ++i) { chain[i].push_back(i + 1); chain[i + 1].push_back(i); }
    long long calls = 0;
    dsu_on_tree(chain, 0, [&](int) { ++calls; }, [&](int) { ++calls; }, [&](int) {});
    assert(calls == 2LL * n); // 长链上每个点只加入、清空各一次
}

void rerooting_checks() {
    for (int round = 0; round < 300; ++round) {
        int n = random_int(1, 30);
        auto g = random_tree(n);
        vector<pair<int, int>> edges;
        vector<long long> weight;
        for (int u = 0; u < n; ++u) for (int v : g[u]) if (u < v) { edges.emplace_back(u, v); weight.push_back(random_ll(1, 9)); }
        if (random_int(0, 1) && n > 2) { // 偶尔删一条边，得到森林
            edges.pop_back();
            weight.pop_back();
        }
        // 1) 每个点到其他所有点的带权距离和；2) 最远带权距离。T = (点数, 距离和, 最远距离)。
        struct Info { long long count, sum, far; };
        auto answer = rerooting(n, edges, Info{0, 0, 0},
            [](Info a, Info b) { return Info{a.count + b.count, a.sum + b.sum, max(a.far, b.far)}; },
            [&](Info dp, int, int, int id) { return Info{dp.count, dp.sum + dp.count * weight[id], dp.far + weight[id]}; },
            [](Info merged, int) { return Info{merged.count + 1, merged.sum, merged.far}; });
        vector<vector<pair<int, long long>>> adj(n);
        for (int id = 0; id < int(edges.size()); ++id) {
            auto [u, v] = edges[id];
            adj[u].emplace_back(v, weight[id]);
            adj[v].emplace_back(u, weight[id]);
        }
        for (int s = 0; s < n; ++s) {
            vector<long long> dist(n, -1);
            vector<int> queue{s};
            dist[s] = 0;
            for (size_t i = 0; i < queue.size(); ++i)
                for (auto [v, w] : adj[queue[i]]) if (dist[v] == -1) { dist[v] = dist[queue[i]] + w; queue.push_back(v); }
            long long sum = 0, far = 0, count = 0;
            for (long long d : dist) if (d != -1) { sum += d; far = max(far, d); ++count; }
            assert(answer[s].count == count && answer[s].sum == sum && answer[s].far == far);
        }
    }
}

void segment_tree_graph_checks() {
    for (int round = 0; round < 500; ++round) {
        int n = random_int(1, 12);
        SegmentTreeGraph stg(n);
        // 暴力图：原图点 + 每条区间到区间边一个虚点。
        WeightedGraph brute(n);
        for (int i = random_int(0, 15); i > 0; --i) {
            int type = random_int(0, 3), l1 = random_int(0, n), r1 = random_int(0, n), l2 = random_int(0, n), r2 = random_int(0, n);
            if (l1 > r1) swap(l1, r1);
            if (l2 > r2) swap(l2, r2);
            int u = random_int(0, n - 1), v = random_int(0, n - 1);
            long long w = random_ll(0, 9);
            if (type == 0) { stg.add_edge(u, v, w); brute[u].push_back({v, w}); }
            else if (type == 1) { stg.add_edge_to_range(u, l1, r1, w); for (int x = l1; x < r1; ++x) brute[u].push_back({x, w}); }
            else if (type == 2) { stg.add_edge_from_range(l1, r1, v, w); for (int x = l1; x < r1; ++x) brute[x].push_back({v, w}); }
            else {
                stg.add_range_to_range(l1, r1, l2, r2, w);
                for (int x = l1; x < r1; ++x) for (int y = l2; y < r2; ++y) brute[x].push_back({y, w});
            }
        }
        int s = random_int(0, n - 1);
        auto got = dijkstra(stg.graph(), s);
        auto expected = dijkstra(brute, s);
        for (int v = 0; v < n; ++v) assert(got[v] == expected[v]);
        assert(stg.size() == n && stg.node_count() >= 3 * n - 2);
    }
}

int main() {
    spfa_checks();
    euler_path_checks();
    biconnected_checks();
    kruskal_tree_checks();
    lca_family_checks();
    centroid_checks();
    dsu_on_tree_checks();
    rerooting_checks();
    segment_tree_graph_checks();
    cout << "All graph extension tests passed\n";
}
