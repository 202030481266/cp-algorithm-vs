// Deterministic property tests for the offline / optimization templates: persistent DSU, persistent range add,
// sqrt decomposition, tree of trees, parallel binary search, CDQ, fractional programming, convex hull trick,
// monotone minima, Knuth optimization, residue shortest paths, Catalan numbers and the perfect shuffle.
// Each section compares a template with a small brute-force oracle on random data.
#ifdef NDEBUG
#error Tests require assertions
#endif
#include <algorithm>
#include <array>
#include <cassert>
#include <climits>
#include <cmath>
#include <iostream>
#include <numeric>
#include <random>
#include <string>
#include <utility>
#include <vector>
#include "basic/cdq.hpp"
#include "basic/parallel_binary_search.hpp"
#include "data_structures/dynamic_range_kth.hpp"
#include "data_structures/fenwick.hpp"
#include "data_structures/persistent_dsu.hpp"
#include "data_structures/persistent_segment_tree.hpp"
#include "data_structures/sqrt_decomposition.hpp"
#include "dp/convex_hull_trick.hpp"
#include "dp/monotone.hpp"
#include "dp/sequence.hpp"
#include "graph/residue_shortest_path.hpp"
#include "math/combinatorics.hpp"
#include "math/fractional_programming.hpp"
#include "math/permutation.hpp"

using namespace std;
using namespace cp;
mt19937_64 rng(20261125);
long long random_ll(long long l, long long r) { return uniform_int_distribution<long long>(l, r)(rng); }
int random_int(int l, int r) { return int(random_ll(l, r)); }

void persistent_dsu_checks() {
    for (int round = 0; round < 400; ++round) {
        int n = random_int(1, 12);
        PersistentDSU dsu(n);
        vector<int> versions{dsu.initial()};
        vector<vector<int>> labels{vector<int>(n)}; // labels[v][x]：版本 v 中 x 所在连通块的编号
        iota(labels[0].begin(), labels[0].end(), 0);
        for (int step = 0; step < 60; ++step) {
            int v = random_int(0, int(versions.size()) - 1), a = random_int(0, n - 1), b = random_int(0, n - 1);
            const vector<int>& label = labels[v];
            assert(dsu.same(versions[v], a, b) == (label[a] == label[b]));
            assert(dsu.size(versions[v], a) == count(label.begin(), label.end(), label[a]));
            assert(label[dsu.find(versions[v], a)] == label[a]);
            assert(dsu.find(versions[v], a) == dsu.find(versions[v], dsu.find(versions[v], a)));
            if (step % 2) continue;
            int next = dsu.merge(versions[v], a, b);
            assert((next == versions[v]) == (label[a] == label[b]));
            vector<int> merged = label;
            for (int& x : merged) if (x == label[b]) x = label[a];
            versions.push_back(next);
            labels.push_back(merged);
        }
    }
    const int N = 200000;
    PersistentDSU chain(N, N);
    int version = chain.initial(), half = 0;
    for (int i = 0; i + 1 < N; ++i) {
        version = chain.merge(version, i, i + 1);
        if (i + 1 == N / 2) half = version;
    }
    assert(chain.size(version, 0) == N && chain.same(version, 0, N - 1));
    assert(chain.size(half, 0) == N / 2 + 1 && !chain.same(half, 0, N - 1) && chain.size(chain.initial(), 5) == 1);
}

void persistent_range_add_checks() {
    for (int round = 0; round < 500; ++round) {
        int n = random_int(1, 20);
        PersistentRangeAddSum<long long> tree(n);
        vector<int> roots{0};
        vector<vector<long long>> arrays{vector<long long>(n)};
        if (round % 2) {
            vector<long long> a(n);
            for (auto& x : a) x = random_ll(-100, 100);
            roots.push_back(tree.build(a));
            arrays.push_back(a);
        }
        for (int step = 0; step < 80; ++step) {
            int v = random_int(0, int(roots.size()) - 1), l = random_int(0, n), r = random_int(l, n);
            if (step % 3 == 0) {
                long long delta = random_ll(-1000, 1000);
                vector<long long> a = arrays[v];
                for (int i = l; i < r; ++i) a[i] += delta;
                roots.push_back(tree.add(roots[v], l, r, delta));
                arrays.push_back(a);
            } else {
                assert(tree.sum(roots[v], l, r) == accumulate(arrays[v].begin() + l, arrays[v].begin() + r, 0LL));
                int p = random_int(0, n - 1);
                assert(tree.get(roots[v], p) == arrays[v][p]);
            }
        }
    }
}

void sqrt_decomposition_checks() {
    for (int n = 0; n <= 30; ++n)
        for (int block = 0; block <= n + 2; ++block) {
            SqrtBlocks blocks(n, block);
            assert(blocks.block_size >= 1 && (n == 0 ? blocks.block_count == 0 : blocks.end(blocks.block_count - 1) == n));
            for (int l = 0; l <= n; ++l)
                for (int r = l; r <= n; ++r) {
                    vector<int> covered(n);
                    int parts = 0;
                    blocks.for_each(l, r,
                        [&](int b, int lo, int hi) {
                            assert(lo < hi && blocks.begin(b) <= lo && hi <= blocks.end(b));
                            assert(hi - lo < blocks.end(b) - blocks.begin(b));
                            ++parts;
                            for (int i = lo; i < hi; ++i) ++covered[i];
                        },
                        [&](int b) { for (int i = blocks.begin(b); i < blocks.end(b); ++i) ++covered[i]; });
                    assert(parts <= 2);
                    for (int i = 0; i < n; ++i) assert(covered[i] == (l <= i && i < r));
                }
        }
    for (int round = 0; round < 600; ++round) {
        int n = random_int(0, 40);
        vector<long long> a(n);
        for (auto& x : a) x = random_ll(-20, 20);
        RangeAddCountLess<long long> blocks(a, random_int(0, n + 1));
        for (int step = 0; step < 80; ++step) {
            int l = random_int(0, n), r = random_int(l, n);
            if (step % 2) {
                long long delta = random_ll(-10, 10);
                blocks.add(l, r, delta);
                for (int i = l; i < r; ++i) a[i] += delta;
            } else {
                long long x = random_ll(-40, 40);
                assert(blocks.count_less(l, r, x) == count_if(a.begin() + l, a.begin() + r, [&](long long y) { return y < x; }));
                if (n) { int p = random_int(0, n - 1); assert(blocks.get(p) == a[p]); }
            }
        }
    }
}

void dynamic_range_kth_checks() {
    for (int round = 0; round < 800; ++round) {
        int n = random_int(1, 30), limit = random_int(1, round % 2 ? 6 : 40);
        vector<int> a(n);
        for (int& x : a) x = random_int(0, limit - 1);
        DynamicRangeKth tree(a, limit);
        for (int step = 0; step < 80; ++step) {
            int l = random_int(0, n), r = random_int(l, n);
            if (step % 3 == 0) {
                int p = random_int(0, n - 1), value = random_int(0, limit - 1);
                tree.set(p, value);
                a[p] = value;
                assert(tree.get(p) == value);
                continue;
            }
            int x = random_int(-2, limit + 2), y = random_int(-2, limit + 2);
            assert(tree.count_less(l, r, x) == count_if(a.begin() + l, a.begin() + r, [&](int v) { return v < x; }));
            assert(tree.count(l, r, x, y) == count_if(a.begin() + l, a.begin() + r, [&](int v) { return x <= v && v < y; }));
            if (l < r) {
                vector<int> sorted(a.begin() + l, a.begin() + r);
                sort(sorted.begin(), sorted.end());
                int k = random_int(0, r - l - 1);
                assert(tree.kth(l, r, k) == sorted[k]);
            }
        }
    }
    const int N = 20000;
    vector<int> a(N);
    for (int& x : a) x = random_int(0, N - 1);
    DynamicRangeKth big(a, N);
    for (int step = 0; step < N; ++step) {
        int p = random_int(0, N - 1);
        big.set(p, a[p] = random_int(0, N - 1));
        int l = random_int(0, N - 1), r = random_int(l + 1, N), k = random_int(0, r - l - 1), value = big.kth(l, r, k);
        assert(big.count_less(l, r, value) <= k && k < big.count_less(l, r, value + 1));
    }
    assert(big.count_less(0, N, N) == N && big.count(0, N, 0, N) == N);
}

void parallel_binary_search_checks() {
    for (int round = 0; round < 2000; ++round) {
        int n = random_int(1, 10), m = random_int(0, 20), q = random_int(0, 20);
        vector<pair<int, long long>> operations(m);
        for (auto& [pos, value] : operations) { pos = random_int(0, n - 1); value = random_ll(0, 5); }
        struct Query { int l, r; long long need; };
        vector<Query> queries(q);
        for (auto& query : queries) {
            query.l = random_int(0, n - 1);
            query.r = random_int(query.l + 1, n);
            query.need = random_ll(0, 25);
        }
        Fenwick<long long> bit(n);
        int resets = 0, applied = 0;
        vector<int> answer = parallel_binary_search(m, q,
            [&] { bit = Fenwick<long long>(n); ++resets; applied = 0; },
            [&](int t) { assert(t == applied++); bit.add(operations[t].first, operations[t].second); },
            [&](int i) { return bit.sum(queries[i].l, queries[i].r) >= queries[i].need; });
        int rounds = 0;
        while ((1 << rounds) < m + 2) ++rounds;
        assert(resets <= rounds);
        for (int i = 0; i < q; ++i) {
            vector<long long> a(n);
            int expected = m + 1;
            for (int t = 0; t <= m && expected > m; ++t) {
                if (accumulate(a.begin() + queries[i].l, a.begin() + queries[i].r, 0LL) >= queries[i].need) expected = t;
                if (t < m) a[operations[t].first] += operations[t].second;
            }
            assert(answer[i] == expected);
        }
    }
}

void cdq_checks() {
    for (int round = 0; round < 3000; ++round) {
        int n = random_int(0, 40), range = random_int(1, round % 2 ? 3 : 1000000000);
        vector<array<int, 3>> points(n);
        for (auto& p : points) for (int& x : p) x = random_int(-range, range);
        vector<int> actual = dominance_count_3d(points);
        for (int i = 0; i < n; ++i) {
            int expected = 0;
            for (int j = 0; j < n; ++j)
                expected += i != j && points[j][0] <= points[i][0] && points[j][1] <= points[i][1] && points[j][2] <= points[i][2];
            assert(actual[i] == expected);
        }
    }
    vector<array<int, 3>> big(200000);
    for (auto& p : big) for (int& x : p) x = random_int(0, 1000);
    vector<int> counts = dominance_count_3d(big);
    for (int probe = 0; probe < 20; ++probe) {
        int i = random_int(0, int(big.size()) - 1), expected = -1;
        for (const auto& p : big) expected += p[0] <= big[i][0] && p[1] <= big[i][1] && p[2] <= big[i][2];
        assert(counts[i] == expected);
    }
    // 骨架：用 CDQ 做“前面的 dp 推后面”的最长严格上升子序列，cross 时左半必须已经是最终值。
    for (int round = 0; round < 2000; ++round) {
        int n = random_int(0, 40);
        vector<int> a(n), dp(n, 1);
        for (int& x : a) x = random_int(0, 15);
        long long pairs = 0;
        cdq_divide(0, n, [&](int l, int mid, int r) {
            assert(l < mid && mid < r);
            pairs += 1LL * (mid - l) * (r - mid);
            for (int j = mid; j < r; ++j)
                for (int i = l; i < mid; ++i) if (a[i] < a[j]) dp[j] = max(dp[j], dp[i] + 1);
        });
        assert(pairs == 1LL * n * (n - 1) / 2);
        assert((n ? *max_element(dp.begin(), dp.end()) : 0) == lis_length(a));
    }
}

void fractional_programming_checks() {
    assert(fabs(bisect_last_true(0, 10, [](double x) { return x * x <= 2; }) - sqrt(2.0)) < 1e-12);
    for (int round = 0; round < 3000; ++round) {
        int n = random_int(1, 10), k = random_int(1, n);
        vector<long long> a(n), b(n);
        for (int i = 0; i < n; ++i) { a[i] = random_ll(-20, 20); b[i] = random_ll(1, 20); }
        double expected = -1e18;
        for (int mask = 1; mask < (1 << n); ++mask) {
            long long sum_a = 0, sum_b = 0;
            int chosen = 0;
            for (int i = 0; i < n; ++i) if (mask >> i & 1) { sum_a += a[i]; sum_b += b[i]; ++chosen; }
            if (chosen == k) expected = max(expected, double(sum_a) / double(sum_b));
        }
        assert(fabs(max_ratio_choose_k(a, b, k) - expected) < 1e-9);
        // 判定式二分与任意初值的 Dinkelbach 得到同一个答案。
        auto best = [&](double x) {
            vector<double> gain(n);
            for (int i = 0; i < n; ++i) gain[i] = double(a[i]) - x * double(b[i]);
            vector<int> ids(n);
            iota(ids.begin(), ids.end(), 0);
            sort(ids.begin(), ids.end(), [&](int i, int j) { return gain[i] > gain[j]; });
            double sum_a = 0, sum_b = 0, total = 0;
            for (int t = 0; t < k; ++t) { sum_a += double(a[ids[t]]); sum_b += double(b[ids[t]]); total += gain[ids[t]]; }
            return array<double, 3>{sum_a, sum_b, total};
        };
        double by_bisect = bisect_last_true(-25, 25, [&](double x) { return best(x)[2] >= 0; });
        assert(fabs(by_bisect - expected) < 1e-9);
        double start = double(random_int(-50, 50));
        double by_dinkelbach = dinkelbach([&](double x) { auto r = best(x); return pair<double, double>(r[0], r[1]); }, start);
        assert(fabs(by_dinkelbach - expected) < 1e-9);
    }
}

void convex_hull_trick_checks() {
    for (int round = 0; round < 3000; ++round) {
        int n = random_int(1, 30);
        long long scale = round % 3 ? 20 : 1000000000LL;
        vector<pair<long long, long long>> lines(n);
        for (auto& [k, b] : lines) { k = random_ll(-scale, scale); b = random_ll(-scale * scale, scale * scale); }
        sort(lines.begin(), lines.end(), [](auto& x, auto& y) { return x.first > y.first; });
        MonotoneCHT any, increasing;
        long long last_x = -scale;
        for (int i = 0; i < n; ++i) {
            any.add(lines[i].first, lines[i].second);
            increasing.add(lines[i].first, lines[i].second);
            for (int probe = 0; probe < 3; ++probe) {
                long long x = random_ll(-scale, scale), expected = LLONG_MAX, expected_inc = LLONG_MAX;
                last_x = random_ll(last_x, min(scale, last_x + scale / 8 + 1));
                for (int j = 0; j <= i; ++j) {
                    expected = min(expected, lines[j].first * x + lines[j].second);
                    expected_inc = min(expected_inc, lines[j].first * last_x + lines[j].second);
                }
                assert(any.query(x) == expected && increasing.query_increasing(last_x) == expected_inc);
            }
        }
        assert(any.size() <= n && !any.empty());
    }
    for (int round = 0; round < 3000; ++round) {
        int points = random_int(1, 20);
        long long scale = round % 3 ? 20 : 1000000000LL;
        vector<long long> xs(points);
        for (auto& x : xs) x = random_ll(-scale, scale);
        LiChaoTree<long long> tree(xs);
        struct Segment { long long k, b, l, r; };
        vector<Segment> segments;
        for (int step = 0; step < 40; ++step) {
            if (step % 2) {
                long long k = random_ll(-scale, scale), b = random_ll(-scale * scale, scale * scale);
                long long l = random_ll(-scale, scale), r = random_ll(l - scale / 4, scale);
                if (step % 4 == 1) { l = -scale; r = scale; tree.add_line(k, b); }
                else tree.add_segment(k, b, l, r);
                segments.push_back({k, b, l, r});
            } else {
                long long x = xs[random_int(0, points - 1)], expected = LiChaoTree<long long>::INF;
                for (const Segment& s : segments) if (s.l <= x && x <= s.r) expected = min(expected, s.k * x + s.b);
                assert(tree.query(x) == expected);
            }
        }
    }
    LiChaoTree<double> real({0.5, 1.5, 2.5});
    real.add_line(2.0, 1.0);
    real.add_line(-1.0, 4.0);
    assert(fabs(real.query(0.5) - 2.0) < 1e-12 && fabs(real.query(2.5) - 1.5) < 1e-12);
}

void monotone_checks() {
    for (int round = 0; round < 3000; ++round) {
        int n = random_int(0, 12), m = random_int(1, 12);
        // Monge 矩阵：A[i][j] = 行项 + 列项 − 非负密度的二维前缀和，每行最左的最小值位置单调不降。
        vector<vector<long long>> a(n, vector<long long>(m));
        vector<long long> row(n), column(m);
        for (auto& x : row) x = random_ll(-20, 20);
        for (auto& x : column) x = random_ll(-20, 20);
        vector<vector<long long>> prefix(n + 1, vector<long long>(m + 1));
        for (int i = 0; i < n; ++i)
            for (int j = 0; j < m; ++j) {
                prefix[i + 1][j + 1] = prefix[i][j + 1] + prefix[i + 1][j] - prefix[i][j] + random_ll(0, round % 2 ? 1 : 6);
                a[i][j] = row[i] + column[j] - prefix[i + 1][j + 1];
            }
        int calls = 0;
        vector<int> actual = monotone_minima(n, m, [&](int i, int j) { ++calls; return a[i][j]; });
        for (int i = 0; i < n; ++i) assert(actual[i] == min_element(a[i].begin(), a[i].end()) - a[i].begin());
        int levels = 1;
        while ((1 << levels) < n + 1) ++levels;
        assert(calls <= (n + m) * levels + n);
    }
    for (int round = 0; round < 1500; ++round) {
        int n = random_int(0, 14);
        vector<long long> sum(n + 1);
        for (int i = 0; i < n; ++i) sum[i + 1] = sum[i] + random_ll(0, 30);
        bool squared = round % 2;
        auto weight = [&](int l, int r) { long long s = sum[r] - sum[l]; return squared ? s * s : s + 3; };
        auto dp = knuth_interval_dp<long long>(n, weight);
        vector<vector<long long>> brute(n + 1, vector<long long>(n + 1));
        for (int len = 2; len <= n; ++len)
            for (int l = 0; l + len <= n; ++l) {
                int r = l + len;
                brute[l][r] = LLONG_MAX;
                for (int k = l + 1; k < r; ++k) brute[l][r] = min(brute[l][r], brute[l][k] + brute[k][r]);
                brute[l][r] += weight(l, r);
            }
        for (int l = 0; l <= n; ++l) for (int r = l + 1; r <= n; ++r) assert(dp[l][r] == brute[l][r]);
    }
}

void residue_shortest_path_checks() {
    for (int round = 0; round < 3000; ++round) {
        int k = random_int(1, 5);
        vector<long long> steps(k);
        for (auto& x : steps) x = random_ll(1, round % 2 ? 12 : 40);
        const int LIMIT = 2000;
        vector<char> reachable(LIMIT + 1);
        reachable[0] = 1;
        for (int x = 1; x <= LIMIT; ++x)
            for (long long step : steps) if (step <= x && reachable[x - step]) reachable[x] = 1;
        vector<long long> dist = residue_distances(steps);
        long long base = *min_element(steps.begin(), steps.end());
        assert(static_cast<long long>(dist.size()) == base && dist[0] == 0);
        for (int x = 0; x <= LIMIT; ++x) assert(reachable[x] == (x >= dist[x % base]));
        int limit = random_int(0, LIMIT);
        assert(count_representable(dist, limit) == count(reachable.begin(), reachable.begin() + limit + 1, 1));
    }
    vector<long long> dist = residue_distances({1000000, 1000003, 999999999989LL});
    assert(dist.size() == 1000000 && dist[3] == 1000003 && dist[9] == 3 * 1000003LL);
    assert(count_representable(residue_distances({4, 6}), 20) == 10); // 0 4 6 8 ... 20
}

void combinatorics_and_shuffle_checks() {
    const int N = 300;
    Combinations<Mint> comb(2 * N);
    vector<Mint> catalan(N + 1);
    catalan[0] = 1;
    for (int n = 0; n < N; ++n)
        for (int i = 0; i <= n; ++i) catalan[n + 1] += catalan[i] * catalan[n - i];
    for (int n = 0; n <= N; ++n) assert(comb.catalan(n) == catalan[n]);
    for (int half = 0; half <= 400; ++half) {
        vector<int> a(2 * half), expected(2 * half);
        iota(a.begin(), a.end(), 0);
        for (int i = 0; i < half; ++i) { expected[2 * i] = half + i; expected[2 * i + 1] = i; }
        perfect_shuffle(a);
        assert(a == expected);
    }
    for (int round = 0; round < 2000; ++round) {
        int n = random_int(0, 60), l = random_int(0, n), r = l + (random_int(0, n - l) / 2) * 2, half = (r - l) / 2;
        vector<string> a(n), expected;
        for (int i = 0; i < n; ++i) a[i] = to_string(random_int(0, 9)) + char('a' + i % 26);
        expected = a;
        for (int i = 0; i < half; ++i) { expected[l + 2 * i] = a[l + half + i]; expected[l + 2 * i + 1] = a[l + i]; }
        perfect_shuffle(a, l, r);
        assert(a == expected);
    }
    vector<int> big(2000000);
    iota(big.begin(), big.end(), 0);
    perfect_shuffle(big);
    for (int i = 0; i < 1000000; ++i) assert(big[2 * i] == 1000000 + i && big[2 * i + 1] == i);
}

int main() {
    persistent_dsu_checks();
    persistent_range_add_checks();
    sqrt_decomposition_checks();
    dynamic_range_kth_checks();
    parallel_binary_search_checks();
    cdq_checks();
    fractional_programming_checks();
    convex_hull_trick_checks();
    monotone_checks();
    residue_shortest_path_checks();
    combinatorics_and_shuffle_checks();
    cout << "All advanced extension tests passed\n";
}
