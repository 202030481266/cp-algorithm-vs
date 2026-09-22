// Deterministic property tests: compare templates with small brute-force oracles.
#ifdef NDEBUG
#error Tests require assertions
#endif
#include <algorithm>
#include <array>
#include <bitset>
#include <cassert>
#include <climits>
#include <cmath>
#include <cstdint>
#include <functional>
#include <iostream>
#include <limits>
#include <numeric>
#include <random>
#include <set>
#include <sstream>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <unordered_map>
#include <vector>
#include "basic/search.hpp"
#include "basic/compress.hpp"
#include "basic/sort.hpp"
#include "data_structures/dsu.hpp"
#include "data_structures/fenwick.hpp"
#include "data_structures/segment_tree.hpp"
#include "data_structures/lazy_segment_tree.hpp"
#include "data_structures/sparse_table.hpp"
#include "data_structures/xor_basis.hpp"
#include "data_structures/binary_trie.hpp"
#include "graph/shortest_path.hpp"
#include "graph/mst.hpp"
#include "graph/topological_sort.hpp"
#include "graph/scc.hpp"
#include "graph/two_sat.hpp"
#include "graph/lca.hpp"
#include "graph/tree_diameter.hpp"
#include "graph/hld.hpp"
#include "graph/dinic.hpp"
#include "string/kmp.hpp"
#include "string/z_function.hpp"
#include "string/manacher.hpp"
#include "string/trie.hpp"
#include "string/aho_corasick.hpp"
#include "string/rolling_hash.hpp"
#include "math/number_theory.hpp"
#include "math/modint.hpp"
#include "math/combinatorics.hpp"
#include "math/matrix.hpp"
#include "dp/sequence.hpp"
#include "dp/knapsack.hpp"
#include "geometry/convex_hull.hpp"
#include "util/debug.hpp"
#include "util/hash.hpp"
#include "util/random.hpp"
#include "util/graphviz.hpp"

using namespace std;
using namespace cp;
mt19937 rng(20260919);
int random_int(int l, int r) { return uniform_int_distribution<int>(l, r)(rng); }
string random_string(int n) {
    string s(n, 'a');
    for (char& c : s) c += random_int(0, 2);
    return s;
}

template<class T>
void check_integer_sorting(mt19937_64& sort_rng) {
    constexpr T low = numeric_limits<T>::min(), high = numeric_limits<T>::max();
    auto check = [](const vector<T>& input, bool counting = true) {
        const auto original = input;
        vector<size_t> expected_ids(input.size());
        iota(expected_ids.begin(), expected_ids.end(), size_t{0});
        stable_sort(expected_ids.begin(), expected_ids.end(),
                    [&](size_t l, size_t r) { return input[l] < input[r]; });
        assert(radix_sort_ids(input) == expected_ids);
        if (counting) assert(counting_sort_ids(input) == expected_ids);
        assert(input == original);
        auto expected = input;
        sort(expected.begin(), expected.end());
        auto actual = input;
        radix_sort(actual);
        assert(actual == expected);
        if (counting) {
            actual = input;
            counting_sort(actual);
            assert(actual == expected);
        }
    };
    check({});
    check({low});
    check({high});
    check(vector<T>(40, high));
    check({low, T(low + 3), T(low + 1), low, T(low + 2)});
    check({high, T(high - 3), T(high - 1), high, T(high - 2)});
    vector<T> edges{high, low, 0, 1, high, low, T(high - 1), T(low + 1)};
    if constexpr (is_signed_v<T>) edges.push_back(-1);
    check(edges, sizeof(T) <= 2);
    sort(edges.begin(), edges.end());
    check(edges, false);
    reverse(edges.begin(), edges.end());
    check(edges, false);
    if constexpr (numeric_limits<make_unsigned_t<T>>::digits == 8) {
        vector<T> all_values;
        for (int value = int(low); value <= int(high); ++value)
            all_values.push_back(static_cast<T>(value));
        shuffle(all_values.begin(), all_values.end(), sort_rng);
        check(all_values);
    }
    for (int trial = 0; trial < 100; ++trial) {
        vector<T> values(trial);
        for (T& x : values) {
            if constexpr (is_signed_v<T>)
                x = static_cast<T>(uniform_int_distribution<long long>(low, high)(sort_rng));
            else
                x = static_cast<T>(uniform_int_distribution<unsigned long long>(low, high)(sort_rng));
        }
        check(values, false);
        T middle = 0;
        if constexpr (is_signed_v<T>) middle = -31;
        for (T base : {low, T(high - 63), middle}) {
            for (T& x : values) x = static_cast<T>(base + uniform_int_distribution<int>(0, 63)(sort_rng));
            check(values);
        }
    }
}

void integer_sorting() {
    mt19937_64 sort_rng(20260922);
    check_integer_sorting<signed char>(sort_rng);
    check_integer_sorting<unsigned char>(sort_rng);
    check_integer_sorting<char>(sort_rng);
    check_integer_sorting<short>(sort_rng);
    check_integer_sorting<unsigned short>(sort_rng);
    check_integer_sorting<int>(sort_rng);
    check_integer_sorting<unsigned int>(sort_rng);
    check_integer_sorting<long>(sort_rng);
    check_integer_sorting<unsigned long>(sort_rng);
    check_integer_sorting<long long>(sort_rng);
    check_integer_sorting<unsigned long long>(sort_rng);
    check_integer_sorting<wchar_t>(sort_rng);
    check_integer_sorting<char16_t>(sort_rng);
    check_integer_sorting<char32_t>(sort_rng);

    auto reject_range = [](auto values, size_t limit) {
        auto original = values;
        bool caught = false;
        try { counting_sort(values, limit); }
        catch (const length_error&) { caught = true; }
        assert(caught && values == original);
        caught = false;
        try { counting_sort_ids(values, limit); }
        catch (const length_error&) { caught = true; }
        assert(caught && values == original);
    };
    reject_range(vector<int>{5, -2, 0}, 7);
    vector<int> exact{5, -2, 0};
    assert(counting_sort_ids(exact, 8) == vector<size_t>({1, 2, 0}));
    assert(exact == vector<int>({5, -2, 0}));
    counting_sort(exact, 8);
    assert(exact == vector<int>({-2, 0, 5}));
    reject_range(vector<int>{4, 4}, 0);
    vector<int> equal{4, 4};
    assert(counting_sort_ids(equal, 1) == vector<size_t>({0, 1}));
    counting_sort(equal, 1);
    assert(equal == vector<int>({4, 4}));
    vector<int> trivial;
    counting_sort(trivial, 0);
    assert(trivial.empty());
    assert(counting_sort_ids(trivial, 0).empty());
    trivial.push_back(7);
    counting_sort(trivial, 0);
    assert(trivial == vector<int>({7}));
    assert(counting_sort_ids(trivial, 0) == vector<size_t>({0}));
    vector<int> larger_range{1'000'000, 0};
    bool caught = false;
    try { counting_sort(larger_range); }
    catch (const length_error&) { caught = true; }
    assert(caught && larger_range == vector<int>({1'000'000, 0}));
    caught = false;
    try { counting_sort_ids(larger_range); }
    catch (const length_error&) { caught = true; }
    assert(caught && larger_range == vector<int>({1'000'000, 0}));
    assert(counting_sort_ids(larger_range, 1'000'001) == vector<size_t>({1, 0}));
    counting_sort(larger_range, 1'000'001);
    assert(larger_range == vector<int>({0, 1'000'000}));
    reject_range(vector<long long>{LLONG_MAX, LLONG_MIN}, numeric_limits<size_t>::max());
    reject_range(vector<unsigned long long>{ULLONG_MAX, 0}, numeric_limits<size_t>::max());
    reject_range(vector<unsigned long long>{ULLONG_MAX - 1, 0}, numeric_limits<size_t>::max());

    vector<unsigned long long> large(200000);
    for (auto& x : large) x = sort_rng();
    const auto original = large;
    vector<size_t> expected_ids(large.size());
    iota(expected_ids.begin(), expected_ids.end(), size_t{0});
    stable_sort(expected_ids.begin(), expected_ids.end(),
                [&](size_t l, size_t r) { return large[l] < large[r]; });
    assert(radix_sort_ids(large) == expected_ids);
    assert(large == original);
    auto expected = large;
    sort(expected.begin(), expected.end());
    radix_sort(large);
    assert(large == expected);
}

void data_structures() {
    Fenwick<long long> empty;
    assert(empty.sum(0, 0) == 0 && empty.lower_bound(1) == 0);
    RangeAddSum empty_lazy;
    assert(empty_lazy.sum(0, 0) == 0);
    empty_lazy.add(0, 0, 10);
    assert(SparseTable<int>().size() == 0);
    SegmentTree<long long, plus<long long>> empty_seg(0, 0);
    assert(empty_seg.prod(0, 0) == 0);
    for (int trial = 0; trial < 150; ++trial) {
        int n = random_int(1, 35);
        vector<long long> a(n);
        for (auto& x : a) x = random_int(0, 30);
        Fenwick<long long> bit(a);
        RangeAddSum lazy(a);
        SegmentTree<long long, plus<long long>> seg(a, 0);
        SparseTable<long long> st(a);
        SparseTable<long long, Gcd> gcd_st(a);
        for (int l = 0; l < n; ++l) for (int r = l + 1; r <= n; ++r) {
            assert(st.query(l, r) == *min_element(a.begin() + l, a.begin() + r));
            long long g = 0;
            for (int i = l; i < r; ++i) g = gcd(g, a[i]);
            assert(gcd_st.query(l, r) == g);
        }
        for (int step = 0; step < 100; ++step) {
            int l = random_int(0, n), r = random_int(l, n);
            if (random_int(0, 1)) {
                int delta = random_int(0, 5);
                lazy.add(l, r, delta);
                for (int i = l; i < r; ++i) { a[i] += delta; bit.add(i, delta); seg.set(i, a[i]); }
            }
            long long expected = accumulate(a.begin() + l, a.begin() + r, 0LL);
            assert(bit.sum(l, r) == expected && lazy.sum(l, r) == expected && seg.prod(l, r) == expected);
            long long target = random_int(1, int(bit.prefix(n)) + 2), sum = 0;
            int p = 0;
            while (p < n && sum + a[p] < target) sum += a[p++];
            assert(bit.lower_bound(target) == p);
            long long limit = random_int(0, 200);
            p = l; sum = 0;
            while (p < n && sum + a[p] <= limit) sum += a[p++];
            assert(seg.max_right(l, [&](long long value) { return value <= limit; }) == p);
        }
        lazy.add(0, n, -7);
        for (auto& x : a) x -= 7;
        assert(lazy.sum(0, n) == accumulate(a.begin(), a.end(), 0LL));
    }
    vector<long long> original{3, -2, 7, 0, 1, 6, -4, 5};
    RangeAddSum cancelling(original);
    cancelling.add(0, 8, 9);
    cancelling.add(0, 8, -9);
    cancelling.add(1, 7, 0);
    for (int l = 0; l <= 8; ++l) for (int r = l; r <= 8; ++r)
        assert(cancelling.sum(l, r) == accumulate(original.begin() + l, original.begin() + r, 0LL));

    vector<string> words{"a", "bc", "d", "ef"};
    SegmentTree<string, plus<string>> concatenation(words, "");
    assert(concatenation.prod(1, 4) == "bcdef");
    concatenation.set(2, "X");
    assert(concatenation.prod(0, 4) == "abcXef");

    for (int trial = 0; trial < 100; ++trial) {
        DSU dsu(12);
        vector<vector<int>> graph(12);
        for (int step = 0; step < 30; ++step) {
            int u = random_int(0, 11), v = random_int(0, 11);
            graph[u].push_back(v); graph[v].push_back(u); dsu.merge(u, v);
            auto distances = bfs(graph, u);
            assert(dsu.size(u) == count_if(distances.begin(), distances.end(), [](int d) { return d >= 0; }));
            for (int w = 0; w < 12; ++w) assert(dsu.same(u, w) == (distances[w] >= 0));
        }
    }
    for (int trial = 0; trial < 150; ++trial) {
        XorBasis basis;
        set<uint64_t> achievable{0};
        int n = random_int(0, 9);
        for (int i = 0; i < n; ++i) {
            uint64_t x = uint64_t(random_int(0, 1023));
            basis.insert(x);
            auto previous = achievable;
            for (auto value : previous) achievable.insert(value ^ x);
            // Interleave insertions and queries so the cached basis must refresh.
            uint64_t index = 0;
            const XorBasis& view = basis;
            for (auto value : achievable) assert(view.kth(index++).value() == value);
        }
        auto copied = basis;
        assert(copied.insert(uint64_t{1} << 63));
        assert(copied.kth(achievable.size()).value() == (uint64_t{1} << 63));
        assert(!basis.contains(uint64_t{1} << 63));
        uint64_t k = 0;
        for (auto value : achievable) {
            assert(basis.contains(value));
            assert(basis.kth(k++).value() == value);
        }
        assert(!basis.kth(k));
        assert(basis.max_xor() == *achievable.rbegin());
        for (int x = 0; x < 1024; ++x) assert(basis.contains(x) == (achievable.count(x) != 0));
    }
    XorBasis full;
    assert(full.kth(0).value() == 0 && !full.kth(1));
    for (int i = 0; i < 64; ++i) {
        full.insert(uint64_t{1} << i);
        assert(full.kth(uint64_t{1} << i).value() == (uint64_t{1} << i));
        assert(!full.insert(uint64_t{1} << i));
        assert(full.kth(0).value() == 0);
    }
    assert(full.rank() == 64 && full.kth(UINT64_MAX).value() == UINT64_MAX);
    BinaryTrie trie;
    vector<uint64_t> values;
    assert(!trie.max_xor(0));
    for (int i = 0; i < 100; ++i) { uint64_t x = uint64_t(rng()) << 32 | rng(); values.push_back(x); trie.insert(x); }
    trie.insert(values[0]); assert(trie.count(values[0]) == 2); assert(trie.erase(values[0]));
    for (int i = 0; i < 100; ++i) {
        uint64_t x = uint64_t(rng()) << 32 | rng(), best = 0;
        for (auto v : values) best = max(best, x ^ v);
        assert(trie.max_xor(x).value() == best);
    }
    for (auto x : values) assert(trie.erase(x));
    assert(trie.size() == 0 && !trie.erase(values[0]));
}

void graphs() {
    for (int trial = 0; trial < 120; ++trial) {
        int n = random_int(2, 9);
        WeightedGraph g(n), binary(n);
        vector<vector<long long>> dist(n, vector<long long>(n, INF64));
        vector<vector<int>> plain(n);
        for (int u = 0; u < n; ++u) {
            dist[u][u] = 0;
            for (int v = 0; v < n; ++v) if (random_int(0, 3) == 0) {
                int weight = random_int(0, 10);
                g[u].push_back({v, weight});
                binary[u].push_back({v, weight % 2});
                plain[u].push_back(v);
                dist[u][v] = min(dist[u][v], 1LL * weight);
            }
        }
        floyd_warshall(dist);
        for (int u = 0; u < n; ++u) {
            assert(dijkstra(g, u) == dist[u]);
            assert(zero_one_bfs(binary, u) == dijkstra(binary, u));
        }
        auto scc = strongly_connected_components(plain);
        for (int u = 0; u < n; ++u) {
            auto reach = bfs(plain, u);
            for (int v = 0; v < n; ++v) {
                bool together = reach[v] != -1 && bfs(plain, v)[u] != -1;
                assert((scc.id[u] == scc.id[v]) == together);
            }
            for (int v : plain[u]) assert(scc.id[u] <= scc.id[v]);
        }
    }
    assert(topological_sort({{1}, {2}, {}}) == vector<int>({0, 1, 2}));
    assert(topological_sort({{1}, {0}}).empty());
    assert(strongly_connected_components({}).groups.empty());
    for (int trial = 0; trial < 100; ++trial) {
        int n = random_int(2, 5), m = random_int(0, 9);
        vector<UndirectedEdge> edges;
        vector<vector<int>> graph(n);
        for (int i = 0; i < m; ++i) {
            int u = random_int(0, n - 1), v = random_int(0, n - 1);
            edges.push_back({u, v, random_int(-5, 10)});
        }
        long long best = LLONG_MAX;
        for (int mask = 0; mask < (1 << m); ++mask) {
            if (int(bitset<32>(unsigned(mask)).count()) != n - 1) continue;
            vector<vector<int>> candidate(n);
            long long weight = 0;
            for (int i = 0; i < m; ++i) if (mask >> i & 1) {
                auto e = edges[i];
                candidate[e.u].push_back(e.v); candidate[e.v].push_back(e.u); weight += e.weight;
            }
            auto reach = bfs(candidate, 0);
            if (count(reach.begin(), reach.end(), -1) == 0) best = min(best, weight);
        }
        auto result = kruskal(n, edges);
        assert(result.connected == (best != LLONG_MAX));
        if (result.connected) assert(result.weight == best);
    }
    for (int trial = 0; trial < 150; ++trial) {
        int n = random_int(1, 6);
        TwoSAT sat(n);
        vector<tuple<int, bool, int, bool>> clauses;
        for (int i = 0, m = random_int(0, 15); i < m; ++i) {
            int u = random_int(0, n - 1), v = random_int(0, n - 1);
            bool a = random_int(0, 1), b = random_int(0, 1);
            sat.add_clause(u, a, v, b); clauses.emplace_back(u, a, v, b);
        }
        bool feasible = false;
        for (int mask = 0; mask < (1 << n); ++mask) {
            bool valid = true;
            for (auto [u, a, v, b] : clauses) valid &= (bool(mask >> u & 1) == a || bool(mask >> v & 1) == b);
            feasible |= valid;
        }
        auto answer = sat.solve();
        assert(answer.has_value() == feasible);
        if (answer) for (auto [u, a, v, b] : clauses) assert((*answer)[u] == a || (*answer)[v] == b);
    }
    for (int trial = 0; trial < 100; ++trial) {
        int n = random_int(2, 7);
        Dinic flow(n);
        vector<UndirectedEdge> edges;
        for (int i = 0; i < 20; ++i) {
            int u = random_int(0, n - 1), v = random_int(0, n - 1), capacity = random_int(0, 10);
            flow.add_edge(u, v, capacity); edges.push_back({u, v, capacity});
        }
        long long expected = LLONG_MAX;
        for (int mask = 1; mask < (1 << n); ++mask) if ((mask & 1) && !(mask >> (n - 1) & 1)) {
            long long capacity = 0;
            for (auto e : edges) if ((mask >> e.u & 1) && !(mask >> e.v & 1)) capacity += e.weight;
            expected = min(expected, capacity);
        }
        long long first = flow.flow(0, n - 1, 3);
        assert(first + flow.flow(0, n - 1) == expected);
        auto cut = flow.min_cut(0);
        long long capacity = 0;
        for (auto e : edges) if (cut[e.u] && !cut[e.v]) capacity += e.weight;
        assert(capacity == expected);
    }
}

void trees() {
    assert(tree_diameter({}).empty());
    for (int trial = 0; trial < 80; ++trial) {
        int n = random_int(1, 50), root = random_int(0, n - 1);
        vector<vector<int>> g(n);
        for (int u = 1; u < n; ++u) {
            int v = random_int(0, u - 1);
            g[u].push_back(v); g[v].push_back(u);
        }
        LCA lca(g, root);
        HLD hld(g, root);
        int diameter = 0;
        for (int u = 0; u < n; ++u) {
            auto distances = bfs(g, u);
            for (int v = 0; v < n; ++v) {
                assert(lca.distance(u, v) == distances[v]);
                diameter = max(diameter, distances[v]);
                int length = 0;
                long long path_sum = 0;
                hld.for_each_path(u, v, [&](int l, int r) {
                    length += r - l;
                    for (int i = l; i < r; ++i) path_sum += hld.vertex[i];
                });
                assert(length == distances[v] + 1);
                int a = u, b = v;
                long long expected = 0;
                while (a != b) {
                    if (hld.depth[a] < hld.depth[b]) swap(a, b);
                    expected += a; a = hld.parent[a];
                }
                expected += a;
                assert(path_sum == expected);
            }
            auto [l, r] = hld.subtree(u);
            int descendants = 0;
            for (int v = 0; v < n; ++v) {
                bool descendant = lca.lca(u, v) == u;
                assert((l <= hld.pos[v] && hld.pos[v] < r) == descendant);
                descendants += descendant;
            }
            assert(descendants == hld.size[u]);
            assert(lca.kth_ancestor(u, lca.depth[u]) == root);
            assert(lca.kth_ancestor(u, lca.depth[u] + 1) == -1);
        }
        auto path = tree_diameter(g);
        assert(int(path.size()) - 1 == diameter);
        for (size_t i = 1; i < path.size(); ++i) assert(lca.distance(path[i - 1], path[i]) == 1);
    }
    // Long-chain regression for iterative traversals and dynamic lifting levels.
    int n = 200000;
    vector<vector<int>> chain(n);
    for (int i = 1; i < n; ++i) { chain[i - 1].push_back(i); chain[i].push_back(i - 1); }
    assert(LCA(chain).distance(0, n - 1) == n - 1);
    assert(HLD(chain).size[0] == n);
    assert(int(tree_diameter(chain).size()) == n);
    assert(strongly_connected_components(chain).groups.size() == 1);
}

void strings() {
    assert(kmp_find("", "") == vector<int>{0});
    assert(z_function("").empty());
    // An independent polynomial evaluation catches narrow intermediate products
    // and signed-char errors after reducing RollingHash's storage to uint32_t.
    string bytes(513, '\0');
    for (int i = 0; i < int(bytes.size()); ++i) bytes[i] = char((i * 73) & 255);
    RollingHash byte_hash(bytes);
    constexpr array<long long, 2> moduli{1000000007, 1000000009};
    for (int trial = 0; trial < 1000; ++trial) {
        int l = random_int(0, int(bytes.size())), r = random_int(l, int(bytes.size()));
        array<long long, 2> expected{};
        for (int i = l; i < r; ++i) for (int j = 0; j < 2; ++j)
            expected[j] = static_cast<long long>(
                (expected[j] * 911382323 + static_cast<unsigned char>(bytes[i]) + 1) % moduli[j]);
        assert(byte_hash.get(l, r) == expected);
    }
    Trie trie;
    trie.insert(""); trie.insert("ab"); trie.insert("ab"); trie.insert("abc");
    assert(trie.count("") == 1 && trie.count_prefix("") == 4 && trie.count_prefix("ab") == 3);
    assert(trie.erase("ab") && trie.count("ab") == 1 && !trie.erase("b"));
    for (int trial = 0; trial < 200; ++trial) {
        string s = random_string(random_int(0, 30)), p = random_string(random_int(0, 8));
        vector<int> occurrences;
        for (int i = 0; i + int(p.size()) <= int(s.size()); ++i)
            if (s.compare(i, p.size(), p) == 0) occurrences.push_back(i);
        assert(kmp_find(s, p) == occurrences);
        auto z = z_function(s);
        for (int i = 0; i < int(s.size()); ++i) {
            int length = 0;
            while (i + length < int(s.size()) && s[length] == s[i + length]) ++length;
            assert(z[i] == length);
        }
        auto pal = manacher(s);
        int longest = 0;
        long long count_pal = 0;
        RollingHash hash(s);
        for (int l = 0; l <= int(s.size()); ++l) for (int r = l; r <= int(s.size()); ++r) {
            string part = s.substr(l, r - l), reversed = part;
            reverse(reversed.begin(), reversed.end());
            assert(hash.get(l, r) == RollingHash(part).get(0, r - l));
            if (l < r && part == reversed) { ++count_pal; longest = max(longest, r - l); }
        }
        assert(longest == pal.longest.second - pal.longest.first);
        assert(count_pal == accumulate(pal.odd.begin(), pal.odd.end(), 0LL) + accumulate(pal.even.begin(), pal.even.end(), 0LL));
        AhoCorasick ac;
        vector<string> patterns;
        for (int i = 0; i < 10; ++i) { patterns.push_back(random_string(random_int(1, 5))); ac.insert(patterns.back()); }
        ac.build();
        auto counts = ac.count(s);
        for (int i = 0; i < 10; ++i) assert(counts[i] == static_cast<long long>(kmp_find(s, patterns[i]).size()));
        assert(ac.count(s) == counts);
    }
}

template<int Mod>
void modint_boundaries() {
    using M = ModInt<Mod>;
    auto normalized = [](long long x) { return (x % Mod + Mod) % Mod; };
    vector<long long> values{LLONG_MIN, LLONG_MAX, -1LL * Mod, -1, 0, 1, Mod - 1LL, Mod};
    for (long long x : values) for (long long y : values) {
        long long a = normalized(x), b = normalized(y);
        assert((M(x) + M(y)).val() == (a + b) % Mod);
        assert((M(x) - M(y)).val() == (a - b + Mod) % Mod);
        assert((M(x) * M(y)).val() == a * b % Mod);
        long long expected = 1;
        for (int exponent = 0; exponent <= 12; ++exponent) {
            assert(M(x).pow(exponent).val() == expected);
            expected = expected * a % Mod;
        }
    }
    for (int trial = 0; trial < 500; ++trial) {
        long long a = rng() % Mod, b = rng() % Mod;
        assert((M(a) + M(b)).val() == (a + b) % Mod);
        auto bits = (uint64_t(rng()) << 32 | rng()) & uint64_t(LLONG_MAX);
        long long exponent = static_cast<long long>(bits);
        assert(M(a).pow(exponent).val() == pow_mod(a, exponent, Mod));
    }
}

void math_and_dp() {
    modint_boundaries<2>();
    modint_boundaries<12>();
    modint_boundaries<998244353>();
    modint_boundaries<1000000007>();
    modint_boundaries<2147483647>();
    PrimeSieve sieve(2000);
    for (int x = 1; x <= 2000; ++x) {
        bool prime = x >= 2;
        for (int d = 2; d * d <= x; ++d) if (x % d == 0) prime = false;
        assert(sieve.is_prime(x) == prime);
        int product = 1;
        for (auto [p, e] : sieve.factorize(x)) while (e--) product *= p;
        assert(product == x);
    }
    for (int mod = 2; mod < 100; ++mod) for (int a = -100; a <= 100; ++a) {
        auto inverse = inverse_mod(a, mod);
        assert(inverse.has_value() == (gcd(a, mod) == 1));
        if (inverse) assert(((a * *inverse) % mod + mod) % mod == 1);
        for (int exponent = 0; exponent <= 8; ++exponent) {
            long long expected = 1;
            for (int i = 0; i < exponent; ++i) expected = (expected * a % mod + mod) % mod;
            assert(pow_mod(a, exponent, mod) == expected);
        }
    }
    assert(pow_mod(LLONG_MAX - 1, 2, LLONG_MAX) == 1);
    assert(ModInt<12>(5).inv().val() == 5);
    assert((Mint(-1) + 2).val() == 1);
    assert((Mint(13) / Mint(7) * Mint(7)).val() == 13);
    using M = ModInt<101>;
    Combinations<M> comb(50);
    vector<vector<int>> pascal(51, vector<int>(51));
    for (int n = 0; n <= 50; ++n) {
        pascal[n][0] = 1;
        for (int k = 1; k <= n; ++k) pascal[n][k] = (pascal[n - 1][k - 1] + pascal[n - 1][k]) % 101;
        for (int k = 0; k <= n; ++k) assert(comb.choose(n, k).val() == pascal[n][k]);
    }
    assert(comb.choose(3, -1).val() == 0 && comb.choose(3, 4).val() == 0);
    Matrix<Mint> fib(2);
    fib[0][0] = fib[0][1] = fib[1][0] = 1;
    Mint a = 0, b = 1;
    for (int n = 0; n < 100; ++n) { assert(fib.pow(n)[0][1] == a); Mint next = a + b; a = b; b = next; }

    for (int trial = 0; trial < 100; ++trial) {
        int n = random_int(0, 10);
        vector<int> values(n);
        for (auto& x : values) x = random_int(-5, 5);
        long long inversions = 0;
        for (int i = 0; i < n; ++i) for (int j = i + 1; j < n; ++j) inversions += values[i] > values[j];
        assert(inversion_count(values) == inversions);
        for (bool strict : {false, true}) {
            int longest = 0;
            for (int mask = 0; mask < (1 << n); ++mask) {
                int last = INT_MIN, length = 0; bool valid = true;
                for (int i = 0; i < n; ++i) if (mask >> i & 1) {
                    valid &= strict ? values[i] > last : values[i] >= last;
                    last = values[i]; ++length;
                }
                if (valid) longest = max(longest, length);
            }
            assert(lis_length(values, strict) == longest);
        }
        for (int k = 1; k <= n + 1; ++k) {
            vector<int> expected;
            for (int i = 0; i + k <= n; ++i) expected.push_back(*min_element(values.begin() + i, values.begin() + i + k));
            assert(sliding_min(values, k) == expected);
        }
    }
    for (int trial = 0; trial < 100; ++trial) {
        vector<int> weight(4), value(4), counts(4);
        for (int i = 0; i < 4; ++i) { weight[i] = random_int(1, 6); value[i] = random_int(-3, 9); counts[i] = random_int(0, 4); }
        for (int mode = 0; mode < 3; ++mode) {
            vector<long long> dp(21), expected(21);
            for (int i = 0; i < 4; ++i) {
                if (mode == 0) knapsack01(dp, weight[i], value[i]);
                if (mode == 1) knapsack_bounded(dp, weight[i], value[i], counts[i]);
                if (mode == 2) knapsack_unbounded(dp, weight[i], value[i]);
            }
            function<void(int, int, long long)> enumerate = [&](int i, int cost, long long val) {
                if (i == 4) { for (int cap = cost; cap <= 20; ++cap) expected[cap] = max(expected[cap], val); return; }
                int limit = mode == 0 ? 1 : mode == 1 ? counts[i] : 20 / weight[i];
                for (int k = 0; k <= limit && cost + k * weight[i] <= 20; ++k)
                    enumerate(i + 1, cost + k * weight[i], val + k * value[i]);
            };
            enumerate(0, 0, 0);
            assert(dp == expected);
        }
    }
    assert(first_true(LLONG_MIN, LLONG_MAX, [](long long x) { return x >= 123; }) == 123);
    assert(first_true(-3, 4, [](long long) { return false; }) == 4);
    for (int x = -100; x <= 100; ++x) for (int y = -10; y <= 10; ++y) if (y) {
        assert(floor_div(x, y) == static_cast<long long>(floor(1.0 * x / y)));
        assert(ceil_div(x, y) == static_cast<long long>(ceil(1.0 * x / y)));
    }
}

void utilities_and_geometry() {
    assert(convex_hull({}).empty());
    assert(convex_hull({{0, 0}, {1, 1}, {2, 2}, {1, 1}}).size() == 2);
    for (int trial = 0; trial < 100; ++trial) {
        vector<Point> points;
        for (int i = 0; i < 30; ++i) points.push_back({random_int(-20, 20), random_int(-20, 20)});
        auto hull = convex_hull(points);
        for (size_t i = 0; i < hull.size(); ++i) {
            for (Point p : points) assert(cross(hull[i], hull[(i + 1) % hull.size()], p) >= 0);
            if (hull.size() >= 3) assert(cross(hull[i], hull[(i + 1) % hull.size()], hull[(i + 2) % hull.size()]) > 0);
        }
    }
    Random a(42), b(42);
    assert(a.array(100, -3, 3) == b.array(100, -3, 3));
    unordered_map<long long, int, SafeHash> table;
    table[-1] = 3; assert(table[-1] == 3);
    auto dot = graphviz::to_dot(2, {{0, 1, 7}});
    assert(dot.find("0 -- 1") != string::npos);
    bool caught = false;
    try { graphviz::to_dot(2, {{0, 2, {}}}); } catch (const out_of_range&) { caught = true; }
    assert(caught);
    ostringstream capture;
    auto* old = cerr.rdbuf(capture.rdbuf());
    debug_detail::print(make_tuple(vector<int>{1, 2}, make_pair(3, 4), string("x")));
    cerr.rdbuf(old);
    assert(capture.str() == "([1, 2], (3, 4), \"x\")");
}

int main() {
    integer_sorting(); data_structures(); graphs(); trees(); strings(); math_and_dp(); utilities_and_geometry();
    cout << "All template tests passed (seed 20260919; random brute-force checks + 200000-node chain).\n";
}
