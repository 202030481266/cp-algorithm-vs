// Deterministic property tests for util/random.hpp: value ranges, distinctness, tree / graph structure,
// and frequency checks that every outcome of a small case is equally likely.
#ifdef NDEBUG
#error Tests require assertions
#endif
#include <algorithm>
#include <cassert>
#include <climits>
#include <cmath>
#include <cstdint>
#include <iostream>
#include <map>
#include <numeric>
#include <set>
#include <string>
#include <utility>
#include <vector>
#include "util/random.hpp"

using namespace std;
using namespace cp;
using Edges = vector<pair<int, int>>;

// 每种结果约出现 samples / outcomes 次；允许 6 个标准差，种子固定，结果确定。
template<class Key> void assert_uniform(const map<Key, int>& count, int outcomes, int samples) {
    assert(int(count.size()) == outcomes);
    const double expected = double(samples) / outcomes;
    for (auto& [key, times] : count) assert(fabs(times - expected) <= 6 * sqrt(expected));
}

int find_root(vector<int>& parent, int v) {
    while (parent[v] != v) v = parent[v] = parent[parent[v]];
    return v;
}

// 点在 [0,n)、无自环、无重边，返回连通块个数。
int check_simple_graph(int n, const Edges& edges) {
    vector<int> parent(n);
    iota(parent.begin(), parent.end(), 0);
    set<pair<int, int>> seen;
    int components = n;
    for (auto [u, v] : edges) {
        assert(0 <= u && u < n && 0 <= v && v < n && u != v);
        assert(seen.insert(minmax(u, v)).second);
        int a = find_root(parent, u), b = find_root(parent, v);
        if (a != b) parent[a] = b, --components;
    }
    return components;
}

void check_tree(int n, const Edges& edges) {
    assert(int(edges.size()) == n - 1 && check_simple_graph(n, edges) == 1);
}

Edges normalized(Edges edges) {
    for (auto& [u, v] : edges) if (u > v) swap(u, v);
    sort(edges.begin(), edges.end());
    return edges;
}

void number_checks() {
    Random random(20261006);
    for (auto [lo, hi] : vector<pair<long long, long long>>{
             {LLONG_MIN, LLONG_MAX}, {LLONG_MIN, LLONG_MIN}, {LLONG_MAX, LLONG_MAX},
             {LLONG_MAX - 1, LLONG_MAX}, {LLONG_MIN, 0}, {-3, 4}, {0, 0}}) {
        for (int i = 0; i < 1000; ++i) {
            long long x = random.integer(lo, hi);
            assert(lo <= x && x <= hi);
        }
    }
    map<long long, int> small;
    for (int i = 0; i < 60000; ++i) ++small[random.integer(-2, 3)];
    assert_uniform(small, 6, 60000);

    // 值域大小约为 2^64 的 2/3 时，直接取模会让前一半出现概率变成 2/3；拒绝采样后应为 1/2。
    const unsigned long long bound = ULLONG_MAX / 3 * 2;
    const long long lo = LLONG_MIN, hi = static_cast<long long>(std::uint64_t(lo) + (bound - 1));
    const long long middle = static_cast<long long>(std::uint64_t(lo) + bound / 2);
    int lower_half = 0, samples = 40000;
    for (int i = 0; i < samples; ++i) lower_half += random.integer(lo, hi) < middle;
    assert(fabs(lower_half - samples / 2.0) <= 6 * sqrt(samples / 4.0));

    double sum = 0;
    for (int i = 0; i < 100000; ++i) {
        double x = random.real(-1, 3);
        assert(-1 <= x && x < 3);
        sum += x;
    }
    assert(fabs(sum / 100000 - 1) < 0.02);
    int heads = 0;
    for (int i = 0; i < 100000; ++i) {
        assert(!random.chance(0) && random.chance(1));
        heads += random.chance(0.3);
    }
    assert(fabs(heads - 30000) <= 6 * sqrt(100000 * 0.3 * 0.7));

    map<int, int> picked;
    vector<int> values{5, 7, 7, 9};
    for (int i = 0; i < 40000; ++i) ++picked[random.choice(values)];
    assert(picked.size() == 3 && fabs(picked[7] - 20000) <= 6 * sqrt(10000.0));
    for (int i = 0; i < 1000; ++i) {
        int x = random.choice({1, 2, 100});
        assert(x == 1 || x == 2 || x == 100);
        char c = random.choice(string("xyz"));
        assert(c == 'x' || c == 'y' || c == 'z');
    }

    // 同一种子、同一调用顺序得到同样的数据。
    Random a(42), b(42);
    assert(a.array(100, -3, 3) == b.array(100, -3, 3));
    assert(a.distinct_array(50, 1, 1000) == b.distinct_array(50, 1, 1000));
    assert(a.string(30, "ab") == b.string(30, "ab"));
    assert(a.graph(20, 40, true) == b.graph(20, 40, true));
    assert(a.parents(50) == b.parents(50));
}

void sequence_checks() {
    Random random(20261007);
    for (int round = 0; round < 3000; ++round) {
        int n = int(random.integer(0, 20));
        long long lo = random.integer(-30, 30), hi = lo + random.integer(max(n - 1, 0), 40);
        auto a = random.array(n, lo, hi);
        assert(int(a.size()) == n);
        for (long long x : a) assert(lo <= x && x <= hi);
        auto d = random.distinct_array(n, lo, hi);
        assert(int(d.size()) == n && int(set<long long>(d.begin(), d.end()).size()) == n);
        for (long long x : d) assert(lo <= x && x <= hi);
        auto [l, r] = random.interval(lo, hi);
        assert(lo <= l && l <= r && r <= hi);
    }
    assert(random.distinct_array(0, 5, 4).empty());
    auto all = random.distinct_array(6, -2, 3);
    sort(all.begin(), all.end());
    assert((all == vector<long long>{-2, -1, 0, 1, 2, 3}));
    auto top = random.distinct_array(5, LLONG_MAX - 4, LLONG_MAX);
    sort(top.begin(), top.end());
    for (int i = 0; i < 5; ++i) assert(top[i] == LLONG_MAX - 4 + i);
    auto wide = random.distinct_array(1000, LLONG_MIN, LLONG_MAX);
    assert(set<long long>(wide.begin(), wide.end()).size() == 1000);

    // distinct_array 的子集等概率、顺序也等概率：从 4 个数中取 2 个有序对，共 12 种。
    map<vector<long long>, int> ordered_pairs;
    for (int i = 0; i < 60000; ++i) ++ordered_pairs[random.distinct_array(2, 1, 4)];
    assert_uniform(ordered_pairs, 12, 60000);
    map<vector<int>, int> orders;
    for (int i = 0; i < 48000; ++i) ++orders[random.permutation(4)];
    assert_uniform(orders, 24, 48000);
    for (int n = 0; n <= 50; ++n) {
        auto p = random.permutation(n, 1);
        sort(p.begin(), p.end());
        for (int i = 0; i < n; ++i) assert(p[i] == i + 1);
    }

    for (int round = 0; round < 3000; ++round) {
        int parts = int(random.integer(1, 8));
        long long minimum = random.integer(-3, 3), total = minimum * parts + random.integer(0, 20);
        auto a = random.partition(total, parts, minimum);
        assert(int(a.size()) == parts && accumulate(a.begin(), a.end(), 0LL) == total);
        for (long long x : a) assert(x >= minimum);
    }
    assert((random.partition(5, 1) == vector<long long>{5}));
    assert((random.partition(0, 3) == vector<long long>{0, 0, 0}));
    // 把 7 拆成 3 个正整数共 C(6,2) = 15 种，应等概率。
    map<vector<long long>, int> compositions;
    for (int i = 0; i < 45000; ++i) ++compositions[random.partition(7, 3, 1)];
    assert_uniform(compositions, 15, 45000);

    for (int round = 0; round < 1000; ++round) {
        int n = int(random.integer(0, 30));
        string s = random.string(n, 'b', 'd'), t = random.string(n, ".#");
        assert(int(s.size()) == n && int(t.size()) == n);
        for (char c : s) assert('b' <= c && c <= 'd');
        for (char c : t) assert(c == '.' || c == '#');
        string shuffled = s;
        random.shuffle(shuffled);
        assert(is_permutation(s.begin(), s.end(), shuffled.begin()));
    }
    assert(random.string(0).empty());
}

void tree_checks() {
    Random random(20261008);
    for (int round = 0; round < 2000; ++round) {
        int n = int(random.integer(1, 30));
        check_tree(n, random.tree(n));
        auto path = random.path_tree(n), star = random.star_tree(n);
        check_tree(n, path);
        check_tree(n, star);
        vector<int> path_degree(n), star_degree(n);
        for (auto [u, v] : path) ++path_degree[u], ++path_degree[v];
        for (auto [u, v] : star) ++star_degree[u], ++star_degree[v];
        assert(*max_element(path_degree.begin(), path_degree.end()) <= 2);
        assert(*max_element(star_degree.begin(), star_degree.end()) == n - 1);
        auto parent = random.parents(n);
        assert(int(parent.size()) == n && parent[0] == -1);
        for (int v = 1; v < n; ++v) assert(0 <= parent[v] && parent[v] < v);
    }
    // 4 个点有 16 棵、5 个点有 125 棵有标号树，应等概率出现。
    for (auto [n, trees] : vector<pair<int, int>>{{4, 16}, {5, 125}}) {
        map<Edges, int> count;
        int samples = trees * 400;
        for (int i = 0; i < samples; ++i) ++count[normalized(random.tree(n))];
        assert_uniform(count, trees, samples);
    }
    // 3 个点的树都是链，根随机：根落在中点（p = {-1,0,0}）的概率为 1/3。
    int rooted_at_middle = 0, samples = 30000;
    for (int i = 0; i < samples; ++i) rooted_at_middle += random.parents(3) == vector<int>{-1, 0, 0};
    assert(fabs(rooted_at_middle - samples / 3.0) <= 6 * sqrt(samples * 2 / 9.0));
    // 随机树的深度约为 √n 量级：既不是 O(log n) 的浅树，也不是长链。
    const int n = 100000;
    auto parent = random.parents(n);
    vector<int> depth(n);
    for (int v = 1; v < n; ++v) depth[v] = depth[parent[v]] + 1;
    int height = *max_element(depth.begin(), depth.end());
    assert(200 < height && height < 5000);
    check_tree(n, random.tree(n));
    check_tree(n, random.path_tree(n));
    check_tree(n, random.star_tree(n));
}

void graph_checks() {
    Random random(20261009);
    for (int round = 0; round < 3000; ++round) {
        int n = int(random.integer(1, 12)), total = n * (n - 1) / 2;
        int m = int(random.integer(0, total));
        bool connected = m >= n - 1 && random.chance(0.5);
        auto edges = random.graph(n, m, connected);
        assert(int(edges.size()) == m);
        int components = check_simple_graph(n, edges);
        if (connected) assert(components == 1);
    }
    assert(int(normalized(random.graph(30, 435)).size()) == 435);
    auto big = random.graph(100000, 200000, true);
    assert(big.size() == 200000 && check_simple_graph(100000, big) == 1);
    auto dense = random.graph(1000, 400000);
    assert(dense.size() == 400000 && check_simple_graph(1000, dense) >= 1);
    // 稀疏（抽边去重）和稠密（枚举后打乱）两条路径都应让所有边集等概率：C(6,2) = C(6,4) = 15。
    for (int m : {2, 4}) {
        map<Edges, int> count;
        for (int i = 0; i < 45000; ++i) ++count[normalized(random.graph(4, m))];
        assert_uniform(count, 15, 45000);
    }
}

int main() {
    number_checks();
    sequence_checks();
    tree_checks();
    graph_checks();
    cout << "All util extension tests passed\n";
}
