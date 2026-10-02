// Deterministic property tests for the data structures added after the original import.
// Each section compares a template with a small brute-force oracle on random data.
#ifdef NDEBUG
#error Tests require assertions
#endif
#include <algorithm>
#include <array>
#include <cassert>
#include <climits>
#include <cstdint>
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
#include "data_structures/lazy_segment_tree.hpp"
#include "data_structures/range_fenwick.hpp"
#include "data_structures/segment_tree_beats.hpp"
#include "data_structures/persistent_segment_tree.hpp"
#include "data_structures/persistent_binary_trie.hpp"
#include "data_structures/segment_tree_merge.hpp"
#include "data_structures/dynamic_segment_tree.hpp"
#include "data_structures/treap.hpp"
#include "data_structures/leftist_heap.hpp"
#include "data_structures/weighted_dsu.hpp"
#include "data_structures/rollback_dsu.hpp"
#include "data_structures/xor_basis.hpp"
#include "data_structures/link_cut_tree.hpp"
#include "data_structures/mos_algorithm.hpp"

using namespace std;
using namespace cp;
mt19937_64 rng(20261002);
long long random_ll(long long l, long long r) { return uniform_int_distribution<long long>(l, r)(rng); }
int random_int(int l, int r) { return int(random_ll(l, r)); }
pair<int, int> random_range(int n) { // 允许空区间 [l,r)。
    int l = random_int(0, n), r = random_int(0, n);
    if (l > r) swap(l, r);
    return {l, r};
}

// 局部类不能定义友元 operator+，所以测试用的 Info/Tag 放在命名空间作用域。
struct Affine {
    long long mul = 1, add = 0; // x -> mul*x + add (mod 998244353)
    void apply(const Affine& t) { mul = mul * t.mul % 998244353; add = (add * t.mul + t.add) % 998244353; }
};
struct Poly { // 区间 [l,r) 编码为 sum a[i]*B^(i-l)，非交换；同时维护长度、普通和与 B^长度。
    long long hash = 0, sum = 0, power = 1, length = 0, power_sum = 0; // power_sum = sum B^k
    void apply(const Affine& t) {
        hash = (hash * t.mul + t.add * power_sum) % 998244353;
        sum = (sum * t.mul + t.add * length) % 998244353;
    }
    friend Poly operator+(const Poly& a, const Poly& b) {
        Poly c;
        c.hash = (a.hash + b.hash * a.power) % 998244353;
        c.sum = (a.sum + b.sum) % 998244353;
        c.power = a.power * b.power % 998244353;
        c.length = a.length + b.length;
        c.power_sum = (a.power_sum + b.power_sum * a.power) % 998244353;
        return c;
    }
};
void lazy_segment_tree_checks() {
    for (int round = 0; round < 300; ++round) {
        int n = random_int(1, 40);
        vector<long long> a(n);
        for (auto& x : a) x = random_ll(-50, 50);
        LazySegmentTree<SumMinMax, AssignAdd> seg(vector<SumMinMax>(a.begin(), a.end()));
        for (int step = 0; step < 200; ++step) {
            int type = random_int(0, 6);
            auto [l, r] = random_range(n);
            long long x = random_ll(-50, 50);
            if (type == 0) {
                seg.apply(l, r, AssignAdd::assign(x));
                for (int i = l; i < r; ++i) a[i] = x;
            } else if (type == 1) {
                seg.apply(l, r, AssignAdd::add(x));
                for (int i = l; i < r; ++i) a[i] += x;
            } else if (type == 2) {
                int p = random_int(0, n - 1);
                seg.set(p, x);
                a[p] = x;
            } else if (type == 3) {
                int p = random_int(0, n - 1);
                seg.apply(p, AssignAdd::add(x));
                a[p] += x;
                assert(seg.get(p).sum == a[p]);
            } else if (type == 4) {
                auto info = seg.prod(l, r);
                long long sum = 0, mn = LLONG_MAX, mx = LLONG_MIN;
                for (int i = l; i < r; ++i) { sum += a[i]; mn = min(mn, a[i]); mx = max(mx, a[i]); }
                assert(info.sum == sum && info.minimum == mn && info.maximum == mx && info.length == r - l);
            } else if (type == 5) { // max_right: 区间最大值 < x 的最长前缀。
                int got = seg.max_right(l, [&](const SumMinMax& s) { return s.maximum < x; });
                int expected = l;
                while (expected < n && a[expected] < x) ++expected;
                assert(got == expected);
            } else { // min_left: 区间最小值 > x 的最长后缀。
                int got = seg.min_left(r, [&](const SumMinMax& s) { return s.minimum > x; });
                int expected = r;
                while (expected > 0 && a[expected - 1] > x) --expected;
                assert(got == expected);
            }
            assert(seg.all_prod().sum == accumulate(a.begin(), a.end(), 0LL));
        }
    }
    // 仿射标记 + 非交换信息：验证 Tag 复合顺序与 Info 拼接顺序。
    const long long B = 131, MOD = 998244353;
    for (int round = 0; round < 100; ++round) {
        int n = random_int(1, 30);
        vector<long long> a(n);
        vector<Poly> init(n);
        for (int i = 0; i < n; ++i) {
            a[i] = random_ll(0, MOD - 1);
            init[i] = {a[i], a[i], B, 1, 1};
        }
        LazySegmentTree<Poly, Affine> seg(init);
        for (int step = 0; step < 100; ++step) {
            auto [l, r] = random_range(n);
            if (random_int(0, 1)) {
                Affine t{random_ll(0, MOD - 1), random_ll(0, MOD - 1)};
                seg.apply(l, r, t);
                for (int i = l; i < r; ++i) a[i] = (a[i] * t.mul + t.add) % MOD;
            } else {
                Poly got = seg.prod(l, r);
                long long hash = 0, power = 1, sum = 0;
                for (int i = l; i < r; ++i) { hash = (hash + a[i] * power) % MOD; power = power * B % MOD; sum = (sum + a[i]) % MOD; }
                assert(got.hash == hash && got.sum == sum && got.length == r - l);
            }
        }
    }
    LazySegmentTree<SumMinMax, AssignAdd> empty;
    assert(empty.size() == 0 && empty.all_prod().length == 0);
    // RangeAddSum 保留原接口。
    RangeAddSum old(vector<long long>{1, 2, 3});
    old.add(0, 2, 5);
    assert(old.sum(0, 3) == 16);
}

void range_fenwick_checks() {
    for (int round = 0; round < 300; ++round) {
        int n = random_int(0, 30);
        vector<long long> a(n);
        for (auto& x : a) x = random_ll(-1000, 1000);
        RangeFenwick<long long> bit(a);
        RangeFenwick<long long> zero(n);
        vector<long long> b(n);
        for (int step = 0; step < 100; ++step) {
            auto [l, r] = random_range(n);
            long long x = random_ll(-1000, 1000);
            if (random_int(0, 1)) {
                bit.add(l, r, x);
                zero.add(l, r, x);
                for (int i = l; i < r; ++i) { a[i] += x; b[i] += x; }
            } else {
                assert(bit.sum(l, r) == accumulate(a.begin() + l, a.begin() + r, 0LL));
                assert(zero.sum(l, r) == accumulate(b.begin() + l, b.begin() + r, 0LL));
                if (l < n) assert(bit.get(l) == a[l]);
            }
        }
    }
    for (int round = 0; round < 100; ++round) {
        int rows = random_int(1, 8), cols = random_int(1, 8);
        Fenwick2D<long long> point(rows, cols);
        RangeFenwick2D<long long> range(rows, cols);
        vector<vector<long long>> a(rows, vector<long long>(cols)), b = a;
        for (int step = 0; step < 100; ++step) {
            auto [r1, r2] = random_range(rows);
            auto [c1, c2] = random_range(cols);
            long long x = random_ll(-100, 100);
            int type = random_int(0, 2);
            if (type == 0) {
                int r = random_int(0, rows - 1), c = random_int(0, cols - 1);
                point.add(r, c, x);
                a[r][c] += x;
            } else if (type == 1) {
                range.add(r1, c1, r2, c2, x);
                for (int i = r1; i < r2; ++i) for (int j = c1; j < c2; ++j) b[i][j] += x;
            } else {
                long long sa = 0, sb = 0;
                for (int i = r1; i < r2; ++i) for (int j = c1; j < c2; ++j) { sa += a[i][j]; sb += b[i][j]; }
                assert(point.sum(r1, c1, r2, c2) == sa);
                assert(range.sum(r1, c1, r2, c2) == sb);
            }
        }
    }
}

void segment_tree_beats_checks() {
    for (int round = 0; round < 300; ++round) {
        int n = random_int(1, 40);
        vector<long long> a(n);
        long long span = random_int(0, 1) ? 5 : 1000;
        for (auto& x : a) x = random_ll(-span, span);
        SegmentTreeBeats seg(a);
        for (int step = 0; step < 200; ++step) {
            int type = random_int(0, 5);
            auto [l, r] = random_range(n);
            long long x = random_ll(-span, span);
            if (type == 0) { seg.chmin(l, r, x); for (int i = l; i < r; ++i) a[i] = min(a[i], x); }
            else if (type == 1) { seg.chmax(l, r, x); for (int i = l; i < r; ++i) a[i] = max(a[i], x); }
            else if (type == 2) { seg.add(l, r, x); for (int i = l; i < r; ++i) a[i] += x; }
            else if (type == 3) assert(seg.sum(l, r) == accumulate(a.begin() + l, a.begin() + r, 0LL));
            else if (l < r) {
                assert(seg.min(l, r) == *min_element(a.begin() + l, a.begin() + r));
                assert(seg.max(l, r) == *max_element(a.begin() + l, a.begin() + r));
            }
        }
    }
}

void persistent_structure_checks() {
    // 可持久化数组：随机基于旧版本修改，逐版本对照。
    for (int round = 0; round < 100; ++round) {
        int n = random_int(1, 20);
        PersistentSegmentTree<long long> tree(n);
        vector<long long> init(n);
        for (auto& x : init) x = random_ll(-10, 10);
        bool built = random_int(0, 1);
        vector<int> roots{built ? tree.build(init) : 0};
        vector<vector<long long>> versions{built ? init : vector<long long>(n)};
        for (int step = 0; step < 100; ++step) {
            int v = random_int(0, int(roots.size()) - 1), pos = random_int(0, n - 1);
            long long x = random_ll(-10, 10);
            auto next = versions[v];
            if (random_int(0, 1)) { roots.push_back(tree.add(roots[v], pos, x)); next[pos] += x; }
            else { roots.push_back(tree.set(roots[v], pos, x)); next[pos] = x; }
            versions.push_back(next);
            int w = random_int(0, int(roots.size()) - 1);
            auto [l, r] = random_range(n);
            assert(tree.sum(roots[w], l, r) == accumulate(versions[w].begin() + l, versions[w].begin() + r, 0LL));
            int p = random_int(0, n - 1);
            assert(tree.get(roots[w], p) == versions[w][p]);
        }
    }
    // 区间第 k 小与计数。
    for (int round = 0; round < 200; ++round) {
        int n = random_int(1, 30);
        vector<int> a(n);
        for (auto& x : a) x = random_int(-5, 5);
        RangeKth<int> kth(a);
        for (int step = 0; step < 50; ++step) {
            auto [l, r] = random_range(n);
            vector<int> part(a.begin() + l, a.begin() + r);
            sort(part.begin(), part.end());
            if (l < r) {
                int k = random_int(0, r - l - 1);
                assert(kth.kth(l, r, k) == part[k]);
            }
            int x = random_int(-6, 6);
            assert(kth.count_less(l, r, x) == int(lower_bound(part.begin(), part.end(), x) - part.begin()));
        }
    }
    RangeKth<int> empty_kth(vector<int>{});
    assert(empty_kth.count_less(0, 0, 5) == 0);
    // 可持久化 01 Trie：前缀版本相减。
    for (int round = 0; round < 100; ++round) {
        int n = random_int(1, 40);
        PersistentBinaryTrie<6> trie(n);
        vector<uint64_t> a(n);
        vector<int> roots{0};
        for (auto& x : a) { x = random_int(0, 63); roots.push_back(trie.insert(roots.back(), x)); }
        for (int step = 0; step < 50; ++step) {
            auto [l, r] = random_range(n);
            if (l == r) continue;
            uint64_t x = random_int(0, 63), best = 0, worst = 64;
            for (int i = l; i < r; ++i) { best = max(best, x ^ a[i]); worst = min(worst, x ^ a[i]); }
            assert(trie.count(roots[l], roots[r]) == r - l);
            assert(trie.max_xor(roots[l], roots[r], x) == best);
            assert(trie.min_xor(roots[l], roots[r], x) == worst);
        }
    }
    PersistentBinaryTrie<64> wide;
    int root = wide.insert(0, UINT64_MAX);
    root = wide.insert(root, 1);
    assert(wide.max_xor(0, root, 0) == UINT64_MAX && wide.min_xor(0, root, 0) == 1);
}

void mergeable_segment_tree_checks() {
    // 分裂只移动叶子、合并只并起叶子，所以“已创建的叶子”集合可以精确跟踪。
    for (int round = 0; round < 300; ++round) {
        int n = random_int(1, 16), trees = random_int(1, 5);
        MergeableSegmentTrees pool(n);
        vector<int> roots(trees);
        vector<vector<long long>> cnt(trees, vector<long long>(n));
        vector<vector<char>> created(trees, vector<char>(n));
        auto move_range = [&](int from, int to, int l, int r) {
            for (int i = l; i < r; ++i) {
                cnt[to][i] += cnt[from][i]; cnt[from][i] = 0;
                created[to][i] |= created[from][i]; created[from][i] = 0;
            }
        };
        for (int step = 0; step < 100; ++step) {
            int type = random_int(0, 4), t = random_int(0, trees - 1), u = random_int(0, trees - 1);
            if (type == 0) {
                int pos = random_int(0, n - 1);
                long long d = random_ll(0, 5);
                pool.add(roots[t], pos, d);
                cnt[t][pos] += d;
                created[t][pos] = 1;
            } else if (type == 1 && u != t) {
                roots[t] = pool.merge(roots[t], roots[u]);
                roots[u] = 0;
                move_range(u, t, 0, n);
            } else if (type == 2 && u != t) {
                auto [l, r] = random_range(n);
                int part = pool.split(roots[t], l, r);
                roots[u] = pool.merge(roots[u], part);
                move_range(t, u, l, r);
            } else {
                auto [l, r] = random_range(n);
                assert(pool.sum(roots[t], l, r) == accumulate(cnt[t].begin() + l, cnt[t].begin() + r, 0LL));
                long long total = accumulate(cnt[t].begin(), cnt[t].end(), 0LL);
                assert(pool.total(roots[t]) == total);
                long long k = random_ll(0, total + 1);
                int expected = -1;
                long long seen = 0;
                for (int i = 0; i < n && expected == -1; ++i) if ((seen += cnt[t][i]) > k) expected = i;
                assert(pool.kth(roots[t], k) == expected);
                long long best = LLONG_MIN;
                int best_pos = -1;
                for (int i = 0; i < n; ++i) if (created[t][i] && cnt[t][i] > best) { best = cnt[t][i]; best_pos = i; }
                assert(pool.max_pos(roots[t]) == best_pos && pool.max_count(roots[t]) == best);
            }
        }
    }
}

void dynamic_segment_tree_checks() {
    for (int round = 0; round < 300; ++round) {
        long long lo = random_ll(-20, 5), hi = lo + random_ll(1, 30);
        DynamicSegmentTree tree(lo, hi);
        map<long long, long long> a;
        for (int step = 0; step < 100; ++step) {
            long long l = random_ll(lo, hi), r = random_ll(lo, hi);
            if (l > r) swap(l, r);
            if (random_int(0, 1)) {
                long long d = random_ll(-10, 10);
                tree.add(l, r, d);
                for (long long i = l; i < r; ++i) a[i] += d;
            } else {
                long long sum = 0, mx = LLONG_MIN;
                for (long long i = l; i < r; ++i) { sum += a[i]; mx = max(mx, a[i]); }
                assert(tree.sum(l, r) == sum);
                if (l < r) assert(tree.max(l, r) == mx);
                long long all = LLONG_MIN;
                for (long long i = lo; i < hi; ++i) all = max(all, a[i]);
                assert(tree.all_max() == all);
            }
        }
    }
    DynamicSegmentTree huge(0, 1000000000000000000LL);
    huge.add(5, 999999999999999999LL, 1);
    huge.add(100, 200, 2);
    assert(huge.sum(0, 1000000000000000000LL) == 999999999999999994LL + 200);
    assert(huge.max(0, 101) == 3 && huge.max(200, 300) == 1 && huge.max(0, 5) == 0);
    assert(huge.node_count() < 1000);
}

struct MaxSubarray { // 最大子段和：非交换、需要翻转。
    long long sum = 0, best = 0, prefix = 0, suffix = 0;
    int length = 0;
    MaxSubarray() = default;
    MaxSubarray(long long x) : sum(x), best(x), prefix(x), suffix(x), length(1) {}
    void reverse() { swap(prefix, suffix); }
    void apply(const AssignAdd& t) { // 只处理赋值标记：区间全部变成 value。
        if (!length || !t.assigned) return;
        sum = t.value * length;
        best = prefix = suffix = t.value > 0 ? sum : t.value;
    }
    friend MaxSubarray operator+(const MaxSubarray& a, const MaxSubarray& b) {
        if (!a.length) return b;
        if (!b.length) return a;
        MaxSubarray c;
        c.sum = a.sum + b.sum;
        c.best = max({a.best, b.best, a.suffix + b.prefix});
        c.prefix = max(a.prefix, a.sum + b.prefix);
        c.suffix = max(b.suffix, b.sum + a.suffix);
        c.length = a.length + b.length;
        return c;
    }
};

void treap_checks() {
    for (int round = 0; round < 100; ++round) {
        OrderedMultiset<int> set;
        multiset<int> expected;
        for (int step = 0; step < 300; ++step) {
            int type = random_int(0, 5), x = random_int(-20, 20);
            if (type <= 1) { set.insert(x); expected.insert(x); }
            else if (type == 2) {
                auto it = expected.find(x);
                assert(set.erase(x) == (it != expected.end()));
                if (it != expected.end()) expected.erase(it);
            } else {
                assert(set.size() == int(expected.size()) && set.empty() == expected.empty());
                assert(set.count_less(x) == int(distance(expected.begin(), expected.lower_bound(x))));
                assert(set.count_less_equal(x) == int(distance(expected.begin(), expected.upper_bound(x))));
                assert(set.count(x) == int(expected.count(x)) && set.contains(x) == (expected.count(x) > 0));
                if (!expected.empty()) {
                    int k = random_int(0, int(expected.size()) - 1);
                    assert(set.kth(k) == *next(expected.begin(), k));
                }
                auto lower = expected.lower_bound(x);
                auto prev_value = set.prev(x);
                assert(prev_value.has_value() == (lower != expected.begin()));
                if (prev_value) assert(*prev_value == *prev(lower));
                auto upper = expected.upper_bound(x);
                auto next_value = set.next(x);
                assert(next_value.has_value() == (upper != expected.end()));
                if (next_value) assert(*next_value == *upper);
            }
        }
    }
    OrderedMultiset<int, greater<int>> descending;
    for (int x : {3, 1, 2}) descending.insert(x);
    assert(descending.kth(0) == 3 && descending.count_less(2) == 1);

    for (int round = 0; round < 200; ++round) {
        int n = random_int(0, 20);
        vector<long long> a(n);
        for (auto& x : a) x = random_ll(-10, 10);
        ImplicitTreap<MaxSubarray, AssignAdd> treap(vector<MaxSubarray>(a.begin(), a.end()));
        ImplicitTreap<long long> plain(a);
        for (int step = 0; step < 100; ++step) {
            int size = int(a.size()), type = random_int(0, 8);
            auto [l, r] = random_range(size);
            if (type == 0) {
                int pos = random_int(0, size);
                long long x = random_ll(-10, 10);
                treap.insert(pos, x); plain.insert(pos, x);
                a.insert(a.begin() + pos, x);
            } else if (type == 1) {
                treap.erase(l, r); plain.erase(l, r);
                a.erase(a.begin() + l, a.begin() + r);
            } else if (type == 2) {
                treap.reverse(l, r); plain.reverse(l, r);
                reverse(a.begin() + l, a.begin() + r);
            } else if (type == 3) {
                int m = random_int(l, r);
                treap.rotate(l, m, r); plain.rotate(l, m, r);
                rotate(a.begin() + l, a.begin() + m, a.begin() + r);
            } else if (type == 4) {
                long long x = random_ll(-10, 10);
                treap.apply(l, r, AssignAdd::assign(x));
                for (int i = l; i < r; ++i) a[i] = x;
                plain = ImplicitTreap<long long>(a);
            } else if (type == 5 && size) {
                int pos = random_int(0, size - 1);
                long long x = random_ll(-10, 10);
                treap.set(pos, x); plain.set(pos, x);
                a[pos] = x;
                assert(treap.get(pos).sum == x && plain.get(pos) == x);
            } else {
                auto info = treap.prod(l, r);
                assert(plain.prod(l, r) == accumulate(a.begin() + l, a.begin() + r, 0LL));
                assert(info.sum == accumulate(a.begin() + l, a.begin() + r, 0LL) && info.length == r - l);
                if (l < r) {
                    long long best = LLONG_MIN;
                    for (int i = l; i < r; ++i)
                        for (long long j = i, s = 0; j < r; ++j) { s += a[j]; best = max(best, s); }
                    assert(info.best == best);
                }
            }
            assert(treap.size() == int(a.size()) && plain.size() == int(a.size()));
            assert(plain.to_vector() == a);
            auto infos = treap.to_vector();
            for (int i = 0; i < int(a.size()); ++i) assert(infos[i].sum == a[i]);
        }
    }
}

void leftist_heap_checks() {
    for (int round = 0; round < 100; ++round) {
        int heaps = random_int(1, 6);
        LeftistHeap<int> pool;
        vector<int> roots(heaps, -1);
        vector<multiset<int>> expected(heaps);
        for (int step = 0; step < 200; ++step) {
            int type = random_int(0, 3), h = random_int(0, heaps - 1), g = random_int(0, heaps - 1);
            if (type <= 1) {
                int x = random_int(-50, 50);
                roots[h] = pool.push(roots[h], x);
                expected[h].insert(x);
            } else if (type == 2 && g != h) {
                roots[h] = pool.meld(roots[h], roots[g]);
                roots[g] = -1;
                expected[h].insert(expected[g].begin(), expected[g].end());
                expected[g].clear();
            } else if (type == 3 && roots[h] != -1) {
                assert(pool.top(roots[h]) == *expected[h].begin());
                roots[h] = pool.pop(roots[h]);
                expected[h].erase(expected[h].begin());
            }
            assert((roots[h] == -1) == expected[h].empty());
        }
    }
    LeftistHeap<int, greater<int>> max_heap;
    int root = -1;
    for (int x : {4, 9, 1}) root = max_heap.push(root, x);
    assert(max_heap.top(root) == 9 && max_heap.value(0) == 4);
}

void dsu_variant_checks() {
    for (int round = 0; round < 200; ++round) {
        int n = random_int(1, 12);
        WeightedDSU<long long> dsu(n);
        WeightedDSU<int, bit_xor<int>, bit_xor<int>> parity(n);
        vector<int> comp(n);
        iota(comp.begin(), comp.end(), 0);
        vector<long long> value(n);
        vector<int> bits(n);
        for (auto& x : value) x = random_ll(-100, 100);
        for (auto& x : bits) x = random_int(0, 7);
        for (int step = 0; step < 40; ++step) {
            int a = random_int(0, n - 1), b = random_int(0, n - 1);
            bool truthful = random_int(0, 3) != 0, same = comp[a] == comp[b];
            long long w = truthful ? value[b] - value[a] : random_ll(-200, 200);
            int x = truthful ? bits[a] ^ bits[b] : random_int(0, 7);
            assert(dsu.merge(a, b, w) == (!same || w == value[b] - value[a]));
            assert(parity.merge(a, b, x) == (!same || x == (bits[a] ^ bits[b])));
            if (!same) { // 平移 b 所在集合，让真实值满足新约束。
                long long shift = value[a] + w - value[b];
                int flip = bits[a] ^ x ^ bits[b], old = comp[b];
                for (int i = 0; i < n; ++i)
                    if (comp[i] == old) { comp[i] = comp[a]; value[i] += shift; bits[i] ^= flip; }
            }
            int u = random_int(0, n - 1), v = random_int(0, n - 1);
            auto d = dsu.diff(u, v);
            assert(d.has_value() == (comp[u] == comp[v]) && dsu.same(u, v) == d.has_value());
            if (d) assert(*d == value[v] - value[u]);
            auto p = parity.diff(u, v);
            assert(p.has_value() == d.has_value());
            if (p) assert(*p == (bits[u] ^ bits[v]));
            assert(dsu.size(u) == int(count(comp.begin(), comp.end(), comp[u])));
        }
    }
    // 可撤销并查集：随机快照与回滚，对照每个时刻的代表元。
    for (int round = 0; round < 200; ++round) {
        int n = random_int(1, 12);
        RollbackDSU dsu(n);
        vector<pair<int, vector<int>>> saved;
        auto labels = [&] { vector<int> l(n); for (int i = 0; i < n; ++i) l[i] = dsu.find(i); return l; };
        for (int step = 0; step < 60; ++step) {
            int type = random_int(0, 3);
            if (type <= 1) {
                int a = random_int(0, n - 1), b = random_int(0, n - 1);
                bool was_same = dsu.same(a, b);
                assert(dsu.merge(a, b) == !was_same && dsu.same(a, b));
            } else if (type == 2) saved.emplace_back(dsu.snapshot(), labels());
            else if (!saved.empty()) {
                int k = random_int(0, int(saved.size()) - 1);
                dsu.rollback(saved[k].first);
                assert(labels() == saved[k].second);
                saved.resize(k + 1);
            }
            set<int> roots;
            for (int i = 0; i < n; ++i) roots.insert(dsu.find(i));
            assert(dsu.components() == int(roots.size()));
            int x = random_int(0, n - 1), size = 0;
            for (int i = 0; i < n; ++i) size += dsu.same(i, x);
            assert(dsu.size(x) == size);
        }
    }
    // 线段树分治：离线动态连通性，对照每个时刻的暴力并查集。
    struct TimedEdge { int u, v; };
    for (int round = 0; round < 100; ++round) {
        int n = random_int(1, 8), times = random_int(1, 20);
        SegmentTreeDivide<TimedEdge> divide(times);
        vector<array<int, 4>> edges;
        for (int i = random_int(0, 15); i > 0; --i) {
            auto [l, r] = random_range(times);
            int u = random_int(0, n - 1), v = random_int(0, n - 1);
            divide.add(l, r, {u, v});
            edges.push_back({l, r, u, v});
        }
        RollbackDSU dsu(n);
        vector<int> got(times, -1);
        divide.run([&] { return dsu.snapshot(); }, [&](const TimedEdge& e) { dsu.merge(e.u, e.v); },
                   [&](int s) { dsu.rollback(s); }, [&](int t) { got[t] = dsu.components(); });
        for (int t = 0; t < times; ++t) {
            RollbackDSU brute(n);
            for (auto [l, r, u, v] : edges) if (l <= t && t < r) brute.merge(u, v);
            assert(got[t] == brute.components());
        }
        assert(dsu.components() == n);
    }
}

void prefix_xor_basis_checks() {
    for (int round = 0; round < 200; ++round) {
        int n = random_int(1, 12);
        vector<uint64_t> a(n);
        PrefixXorBasis basis;
        for (int r = 1; r <= n; ++r) {
            a[r - 1] = random_int(0, 1) ? rng() : rng() % 16;
            basis.push_back(a[r - 1]);
            for (int l = 0; l <= r; ++l) {
                XorBasis brute;
                for (int i = l; i < r; ++i) brute.insert(a[i]);
                assert(basis.max_xor(l) == brute.max_xor());
            }
        }
        assert(basis.size() == n);
    }
}

void link_cut_tree_checks() {
    for (int round = 0; round < 200; ++round) {
        int n = random_int(1, 12);
        vector<long long> value(n);
        for (auto& x : value) x = random_ll(0, 1000);
        LinkCutTree<long long, plus<long long>> lct(value, 0);
        LinkCutTree<long long, bit_xor<long long>> xor_lct(value, 0);
        set<pair<int, int>> edges;
        auto path = [&](int s, int t) { // 森林上的暴力路径 t -> s，不连通返回空。
            vector<int> parent(n, -2), queue{s};
            parent[s] = -1;
            for (size_t i = 0; i < queue.size(); ++i)
                for (auto [x, y] : edges) {
                    int u = queue[i], v = x == u ? y : y == u ? x : -1;
                    if (v != -1 && parent[v] == -2) { parent[v] = u; queue.push_back(v); }
                }
            vector<int> result;
            if (parent[t] == -2) return result;
            for (int v = t; v != -1; v = parent[v]) result.push_back(v);
            return result;
        };
        for (int step = 0; step < 100; ++step) {
            int type = random_int(0, 4), u = random_int(0, n - 1), v = random_int(0, n - 1);
            bool connected = !path(u, v).empty();
            if (type == 0) {
                bool linked = lct.link(u, v);
                assert(xor_lct.link(u, v) == linked && linked == !connected);
                if (linked) edges.insert(minmax(u, v));
            } else if (type == 1) {
                bool exists = edges.count(minmax(u, v)) > 0;
                assert(lct.cut(u, v) == exists && xor_lct.cut(u, v) == exists);
                if (exists) edges.erase(minmax(u, v));
            } else if (type == 2) {
                value[u] = random_ll(0, 1000);
                lct.set(u, value[u]);
                xor_lct.set(u, value[u]);
                assert(lct.get(u) == value[u]);
            } else if (type == 3) {
                assert(lct.connected(u, v) == connected);
                if (connected) {
                    long long sum = 0, x = 0;
                    for (int w : path(u, v)) { sum += value[w]; x ^= value[w]; }
                    assert(lct.path_prod(u, v) == sum && xor_lct.path_prod(v, u) == x);
                }
            } else if (connected) {
                int root = random_int(0, n - 1);
                if (path(root, u).empty()) continue;
                lct.make_root(root);
                assert(lct.find_root(u) == root && lct.find_root(v) == root);
                auto pu = path(root, u), pv = path(root, v); // 从 u / v 走到根。
                set<int> on_u(pu.begin(), pu.end());
                int expected = -1;
                for (int w : pv) if (on_u.count(w)) { expected = w; break; }
                assert(lct.lca(u, v) == expected);
            }
        }
    }
    // 20 万点长链：确认非递归实现不会爆栈。
    int n = 200000;
    LinkCutTree<long long, plus<long long>> chain(vector<long long>(n, 1), 0);
    for (int i = 0; i + 1 < n; ++i) assert(chain.link(i, i + 1));
    assert(chain.path_prod(0, n - 1) == n);
    assert(chain.cut(n / 2, n / 2 + 1) && !chain.connected(0, n - 1));
}

void mos_algorithm_checks() {
    for (int round = 0; round < 200; ++round) {
        int n = random_int(0, 30), q = random_int(0, 40);
        vector<int> a(n);
        for (auto& x : a) x = random_int(0, 5);
        vector<pair<int, int>> queries(q);
        for (auto& query : queries) query = random_range(n);
        MosAlgorithm mo(n, queries); // 构造时一次给出
        MosAlgorithm incremental(n); // 逐个登记，编号就是登记顺序
        for (int id = 0; id < q; ++id) assert(incremental.add_query(queries[id].first, queries[id].second) == id);
        assert(mo.query_count() == q && incremental.query_count() == q);
        // 普通莫队：区间不同值个数。
        vector<int> cnt(6), distinct_answer(q, -1);
        int distinct = 0;
        auto add = [&](int i) { if (cnt[a[i]]++ == 0) ++distinct; };
        auto remove = [&](int i) { if (--cnt[a[i]] == 0) --distinct; };
        mo.run(add, remove, [&](int id) { distinct_answer[id] = distinct; });
        // 区分方向的写法：每次回调的下标正好是端点，且任何时刻 cl <= cr。
        int cl = 0, cr = 0, answered = 0;
        incremental.run([&](int i) { assert(i == cl - 1 && cl <= cr); --cl; },
                        [&](int i) { assert(i == cr); ++cr; },
                        [&](int i) { assert(i == cl && cl < cr); ++cl; },
                        [&](int i) { assert(i == cr - 1 && cl < cr); --cr; },
                        [&](int id) { assert(cl == queries[id].first && cr == queries[id].second); ++answered; });
        assert(answered == q);
        // 回滚莫队：区间众数的出现次数，用操作栈撤销；结束后统计为空。
        vector<int> freq(6), mode_answer(q, -1), history, mode_history;
        int mode = 0;
        auto push = [&](int i) { history.push_back(a[i]); mode_history.push_back(mode); mode = max(mode, ++freq[a[i]]); };
        incremental.run_rollback(push, push, [&] { return int(history.size()); },
                                 [&](int s) {
                                     for (; int(history.size()) > s; history.pop_back(), mode_history.pop_back()) {
                                         --freq[history.back()];
                                         mode = mode_history.back();
                                     }
                                 },
                                 [&](int id) { mode_answer[id] = mode; });
        assert(history.empty() && mode == 0 && count(freq.begin(), freq.end(), 0) == 6);
        for (int id = 0; id < q; ++id) {
            auto [l, r] = queries[id];
            assert(distinct_answer[id] == int(set<int>(a.begin() + l, a.begin() + r).size()));
            int best = 0;
            for (int v = 0; v < 6; ++v) best = max(best, int(count(a.begin() + l, a.begin() + r, v)));
            assert(mode_answer[id] == best);
        }
    }
    // 带修莫队：单点修改 + 区间不同值个数，修改与询问按输入顺序交替登记。
    for (int round = 0; round < 300; ++round) {
        int n = random_int(1, 20), operations = random_int(0, 45);
        vector<int> a(n);
        for (auto& x : a) x = random_int(0, 4);
        const auto original = a;
        MosAlgorithmWithUpdates mo(n);
        vector<pair<int, int>> change;   // 第 k 次修改：(位置, 新值)，toggle 时与数组中的值交换
        vector<array<int, 3>> queries;   // (l, r, 已生效的修改数)，用于暴力对照
        for (int i = 0; i < operations; ++i) {
            if (random_int(0, 2) == 0) {
                assert(mo.add_update() == int(change.size()));
                change.emplace_back(random_int(0, n - 1), random_int(0, 4));
            } else {
                auto [l, r] = random_range(n);
                assert(mo.add_query(l, r) == int(queries.size()));
                queries.push_back({l, r, int(change.size())});
            }
        }
        assert(mo.update_count() == int(change.size()) && mo.query_count() == int(queries.size()));
        const auto original_change = change;
        vector<int> cnt(5), answer(queries.size(), -1);
        int distinct = 0;
        auto add = [&](int i) { if (cnt[a[i]]++ == 0) ++distinct; };
        auto remove = [&](int i) { if (--cnt[a[i]] == 0) --distinct; };
        auto toggle = [&](int k, int l, int r) {
            auto& [pos, value] = change[k];
            bool inside = l <= pos && pos < r;
            if (inside) remove(pos);
            swap(a[pos], value);
            if (inside) add(pos);
        };
        mo.run(add, remove, toggle, [&](int id) { answer[id] = distinct; });
        assert(a == original && change == original_change); // 结束前撤销全部修改
        for (int id = 0; id < int(queries.size()); ++id) {
            auto [l, r, t] = queries[id];
            vector<int> b = original;
            for (int k = 0; k < t; ++k) b[original_change[k].first] = original_change[k].second;
            assert(answer[id] == int(set<int>(b.begin() + l, b.begin() + r).size()));
        }
    }
}

int main() {
    lazy_segment_tree_checks();
    range_fenwick_checks();
    segment_tree_beats_checks();
    persistent_structure_checks();
    mergeable_segment_tree_checks();
    dynamic_segment_tree_checks();
    treap_checks();
    leftist_heap_checks();
    dsu_variant_checks();
    prefix_xor_basis_checks();
    link_cut_tree_checks();
    mos_algorithm_checks();
    cout << "All data structure extension tests passed\n";
}
