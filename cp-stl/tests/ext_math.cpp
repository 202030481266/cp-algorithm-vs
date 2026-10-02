// Deterministic property tests for the math, basic and geometry templates added after the original import.
// Each section compares a template with a small brute-force oracle on random data.
#ifdef NDEBUG
#error Tests require assertions
#endif
#include <algorithm>
#include <cassert>
#include <climits>
#include <cstdint>
#include <functional>
#include <iostream>
#include <numeric>
#include <optional>
#include <random>
#include <set>
#include <utility>
#include <vector>
#include "math/prime.hpp"
#include "math/crt.hpp"
#include "math/gauss.hpp"
#include "math/permutation.hpp"
#include "math/modint.hpp"
#include "basic/prefix_sum.hpp"
#include "basic/monotonic_stack.hpp"
#include "geometry/rectangle_union.hpp"

using namespace std;
using namespace cp;
mt19937_64 rng(20261004);
long long random_ll(long long l, long long r) { return uniform_int_distribution<long long>(l, r)(rng); }
int random_int(int l, int r) { return int(random_ll(l, r)); }

void prime_checks() {
    const int LIMIT = 200000;
    vector<char> composite(LIMIT + 1);
    composite[0] = composite[1] = 1;
    for (int i = 2; i * i <= LIMIT; ++i) if (!composite[i]) for (int j = i * i; j <= LIMIT; j += i) composite[j] = 1;
    for (int n = 0; n <= LIMIT; ++n) assert(is_prime(n) == !composite[n]);
    // 已知的大质数、Carmichael 数与强伪素数。
    for (uint64_t p : {2305843009213693951ULL, 18446744073709551557ULL, 1000000007ULL, 998244353ULL, 4611686018427387847ULL})
        assert(is_prime(p));
    for (uint64_t c : {561ULL, 3215031751ULL, 3825123056546413051ULL, 18446744073709551615ULL,
                       1000000007ULL * 998244353ULL, 4294967291ULL * 4294967279ULL, 3ULL * 2305843009213693951ULL})
        assert(!is_prime(c));
    auto check_factorization = [](uint64_t n) {
        auto f = factorize(n);
        assert(is_sorted(f.begin(), f.end()));
        uint64_t product = 1;
        for (uint64_t p : f) { assert(is_prime(p)); product *= p; }
        assert(product == n || n <= 1);
        return f;
    };
    for (int round = 0; round < 3000; ++round) {
        uint64_t n = rng() >> random_int(0, 63);
        check_factorization(n);
    }
    for (uint64_t n : {18446744073709551615ULL, 4294967291ULL * 4294967279ULL, 999999999999999989ULL * 1ULL,
                       2305843009213693951ULL * 3ULL, 1000000007ULL * 1000000007ULL, 1ULL << 63, 6469693230ULL * 2147483647ULL})
        check_factorization(n);
    assert((factorize(4294967291ULL * 4294967279ULL) == vector<uint64_t>{4294967279ULL, 4294967291ULL}));
    assert((factorize(1000000007ULL * 1000000007ULL) == vector<uint64_t>{1000000007ULL, 1000000007ULL}));
    assert(factorize(1).empty() && factorize(0).empty());
    // 与试除法对照。
    for (int round = 0; round < 1000; ++round) {
        uint64_t n = uint64_t(random_ll(1, 10000000000LL));
        vector<uint64_t> expected;
        uint64_t m = n;
        for (uint64_t p = 2; p * p <= m; ++p) while (m % p == 0) { expected.push_back(p); m /= p; }
        if (m > 1) expected.push_back(m);
        assert(factorize(n) == expected);
    }
    for (uint64_t n = 1; n <= 3000; ++n) {
        vector<uint64_t> divs;
        uint64_t phi = 0;
        for (uint64_t d = 1; d <= n; ++d) {
            if (n % d == 0) divs.push_back(d);
            if (gcd(d, n) == 1) ++phi;
        }
        assert(divisors(n) == divs && euler_phi(n) == phi);
        auto pf = prime_factors(n);
        uint64_t product = 1;
        for (auto [p, e] : pf) for (int i = 0; i < e; ++i) product *= p;
        assert(product == n);
    }
    assert(divisors(735134400).size() == 1344);
}

void crt_checks() {
    for (int round = 0; round < 20000; ++round) {
        long long a = random_ll(-1000000, 1000000), b = random_ll(-1000000, 1000000);
        if (random_int(0, 3) == 0) a = random_ll(LLONG_MIN + 1, LLONG_MAX);
        if (random_int(0, 3) == 0) b = random_ll(LLONG_MIN + 1, LLONG_MAX);
        auto [g, x, y] = ext_gcd(a, b);
        assert(g == (long long)gcd((unsigned long long)(a < 0 ? -a : a), (unsigned long long)(b < 0 ? -b : b)));
        if (g) {
            // a*x + b*y == g，用 128 位以外的方式验证：对两个大质数取模。
            for (long long mod : {1000000007LL, 998244353LL}) {
                auto m = [&](long long v) { v %= mod; return v < 0 ? v + mod : v; };
                assert((m(a) * m(x) % mod + m(b) * m(y) % mod) % mod == m(g));
            }
            if (b) assert((x < 0 ? -x : x) <= (b < 0 ? -(b / g) : b / g) || a == 0);
        }
    }
    for (int round = 0; round < 3000; ++round) {
        long long m = random_ll(1, 30), a = random_ll(-50, 50), b = random_ll(-50, 50);
        auto got = solve_congruence(a, b, m);
        vector<long long> sols;
        for (long long x = 0; x < m; ++x) if (((a * x - b) % m + m) % m == 0) sols.push_back(x);
        assert(got.has_value() == !sols.empty());
        if (got) {
            assert(got->first == sols[0] && got->second == m / (long long)gcd(((a % m) + m) % m, m));
            assert(int(sols.size()) == m / got->second);
        }
    }
    for (int round = 0; round < 3000; ++round) {
        int k = random_int(0, 4);
        vector<long long> r(k), m(k);
        for (int i = 0; i < k; ++i) { m[i] = random_ll(1, 12); r[i] = random_ll(-20, 20); }
        auto got = crt(r, m);
        long long l = 1;
        for (long long x : m) l = l / gcd(l, x) * x;
        optional<long long> expected;
        for (long long x = 0; x < l && !expected; ++x) {
            bool ok = true;
            for (int i = 0; i < k; ++i) if (((x - r[i]) % m[i] + m[i]) % m[i] != 0) ok = false;
            if (ok) expected = x;
        }
        assert(got.has_value() == expected.has_value());
        if (got) assert(got->first == *expected && got->second == l);
    }
    // 大模数：lcm 接近 1e18。
    auto big = crt({123456789, 987654321}, {999999937, 999999929});
    assert(big && big->second == 999999937LL * 999999929LL && big->first % 999999937 == 123456789 && big->first % 999999929 == 987654321);
    for (int round = 0; round < 3000; ++round) {
        long long a = random_ll(-20, 20), b = random_ll(-20, 20), c = random_ll(-60, 60);
        if (!a || !b) continue;
        auto got = solve_diophantine(a, b, c);
        // 暴力找最小非负 x。
        optional<long long> min_x;
        for (long long x = 0; x <= 100 && !min_x; ++x) if ((c - a * x) % b == 0) min_x = x;
        assert(got.has_value() == min_x.has_value());
        if (got) {
            assert(got->x0 == *min_x && got->dx > 0);
            for (long long k = -3; k <= 3; ++k)
                assert(a * (got->x0 + k * got->dx) + b * (got->y0 - k * got->dy) == c);
        }
    }
}

void gauss_checks() {
    // 模 p：小域上暴力枚举全部解。
    for (int round = 0; round < 2000; ++round) {
        long long p = random_int(0, 1) ? 3 : 5;
        int n = random_int(1, 4), m = random_int(1, 4);
        vector<vector<long long>> a(n, vector<long long>(m));
        vector<long long> b(n);
        for (auto& row : a) for (auto& x : row) x = random_ll(-6, 6);
        for (auto& x : b) x = random_ll(-6, 6);
        auto got = solve_linear_mod(a, b, p);
        long long count = 0, total = 1;
        for (int i = 0; i < m; ++i) total *= p;
        for (long long code = 0; code < total; ++code) {
            vector<long long> x(m);
            for (long long c = code, i = 0; i < m; ++i, c /= p) x[i] = c % p;
            bool ok = true;
            for (int i = 0; i < n; ++i) {
                long long s = 0;
                for (int j = 0; j < m; ++j) s += a[i][j] * x[j];
                if (((s - b[i]) % p + p) % p) ok = false;
            }
            count += ok;
        }
        assert(got.solvable == (count > 0));
        if (got.solvable) {
            long long expected = 1;
            for (size_t i = 0; i < got.free_variables.size(); ++i) expected *= p;
            assert(count == expected && got.rank + int(got.free_variables.size()) == m);
            for (int i = 0; i < n; ++i) {
                long long s = 0;
                for (int j = 0; j < m; ++j) s += a[i][j] * got.solution[j];
                assert(((s - b[i]) % p + p) % p == 0);
            }
        }
    }
    // 异或：暴力枚举 2^m。
    for (int round = 0; round < 2000; ++round) {
        int n = random_int(1, 8), m = random_int(1, random_int(0, 3) ? 8 : 70);
        vector<vector<int>> a(n, vector<int>(m));
        vector<int> b(n);
        for (auto& row : a) for (auto& x : row) x = random_int(0, 2) == 0;
        for (auto& x : b) x = random_int(0, 1);
        auto got = solve_linear_xor(a, b);
        if (m <= 12) {
            long long count = 0;
            for (int mask = 0; mask < (1 << m); ++mask) {
                bool ok = true;
                for (int i = 0; i < n; ++i) {
                    int s = 0;
                    for (int j = 0; j < m; ++j) s ^= a[i][j] & (mask >> j & 1);
                    if (s != b[i]) ok = false;
                }
                count += ok;
            }
            assert(got.solvable == (count > 0));
            if (got.solvable) assert(count == (1LL << got.free_variables.size()));
        }
        if (got.solvable)
            for (int i = 0; i < n; ++i) {
                int s = 0;
                for (int j = 0; j < m; ++j) s ^= a[i][j] & got.solution[j];
                assert(s == b[i]);
            }
    }
    // 实数：与模大质数的秩对照，并检查残差。
    for (int round = 0; round < 2000; ++round) {
        int n = random_int(1, 5), m = random_int(1, 5);
        vector<vector<double>> a(n, vector<double>(m));
        vector<vector<long long>> ai(n, vector<long long>(m));
        vector<double> b(n);
        vector<long long> bi(n);
        bool dependent = random_int(0, 1);
        for (int i = 0; i < n; ++i) {
            for (int j = 0; j < m; ++j) ai[i][j] = random_ll(-5, 5);
            bi[i] = random_ll(-5, 5);
            if (dependent && i >= 2) { // 制造线性相关的行
                for (int j = 0; j < m; ++j) ai[i][j] = ai[0][j] - 2 * ai[1][j];
                if (random_int(0, 1)) bi[i] = bi[0] - 2 * bi[1];
            }
            for (int j = 0; j < m; ++j) a[i][j] = double(ai[i][j]);
            b[i] = double(bi[i]);
        }
        auto got = solve_linear_real(a, b);
        auto exact = solve_linear_mod(ai, bi, 1000000007);
        assert(got.rank == exact.rank && got.solvable == exact.solvable);
        assert(got.free_variables == exact.free_variables);
        if (got.solvable)
            for (int i = 0; i < n; ++i) {
                double s = 0;
                for (int j = 0; j < m; ++j) s += a[i][j] * got.solution[j];
                assert(abs(s - b[i]) < 1e-6);
            }
    }
}

void permutation_checks() {
    for (int n = 0; n <= 7; ++n) {
        vector<int> p(n);
        iota(p.begin(), p.end(), 0);
        unsigned long long rank = 0;
        do {
            assert(permutation_rank_exact(p) == rank);
            assert(permutation_rank<Mint>(p).val() == int(rank % 998244353));
            assert(kth_permutation(n, rank) == p);
            ++rank;
        } while (next_permutation(p.begin(), p.end()));
    }
    vector<int> big(20);
    iota(big.rbegin(), big.rend(), 0); // 最后一个排列
    assert(permutation_rank_exact(big) == 2432902008176640000ULL - 1);
    assert(kth_permutation(20, 2432902008176640000ULL - 1) == big);
    for (long long n = 1; n <= 60; ++n)
        for (long long k = 1; k <= 70; ++k) {
            vector<long long> circle(n);
            iota(circle.begin(), circle.end(), 0);
            long long pos = 0;
            while (circle.size() > 1) {
                pos = (pos + k - 1) % (long long)circle.size();
                circle.erase(circle.begin() + pos);
            }
            assert(josephus(n, k) == circle[0]);
        }
    // 大 n、小 k 的跳跃版本：与 O(n) 递推对照。
    for (long long n : {1000000LL, 999983LL}) for (long long k : {2LL, 3LL, 17LL, 1000LL}) {
        long long j = 0;
        for (long long i = 2; i <= n; ++i) j = (j + k) % i;
        assert(josephus(n, k) == j);
    }
    assert(josephus(1000000000000000000LL, 2) == 2 * (1000000000000000000LL - (1LL << 59))); // k=2 的闭式解
}

void basic_checks() {
    for (int round = 0; round < 300; ++round) {
        int rows = random_int(1, 6), cols = random_int(0, 6); // 空矩阵没有列数信息，至少一行
        vector<vector<int>> a(rows, vector<int>(cols));
        for (auto& row : a) for (auto& x : row) x = random_int(-9, 9);
        PrefixSum2D<long long> ps(a);
        Difference2D<long long> diff(rows, cols);
        vector<vector<long long>> expected(rows, vector<long long>(cols));
        for (int step = 0; step < 30; ++step) {
            int r1 = random_int(0, rows), r2 = random_int(0, rows), c1 = random_int(0, cols), c2 = random_int(0, cols);
            if (r1 > r2) swap(r1, r2);
            if (c1 > c2) swap(c1, c2);
            long long s = 0;
            for (int i = r1; i < r2; ++i) for (int j = c1; j < c2; ++j) s += a[i][j];
            assert(ps.sum(r1, c1, r2, c2) == s);
            long long d = random_ll(-5, 5);
            diff.add(r1, c1, r2, c2, d);
            for (int i = r1; i < r2; ++i) for (int j = c1; j < c2; ++j) expected[i][j] += d;
        }
        assert(diff.build() == expected);
    }
    for (int round = 0; round < 300; ++round) {
        int n = random_int(0, 15);
        ArithmeticDifference<long long> ad(n);
        vector<long long> expected(n);
        for (int step = 0; step < 20; ++step) {
            int l = random_int(0, n), r = random_int(0, n);
            if (l > r) swap(l, r);
            long long first = random_ll(-9, 9), d = random_ll(-9, 9);
            ad.add(l, r, first, d);
            for (int i = l; i < r; ++i) expected[i] += first + d * (i - l);
        }
        assert(ad.build() == expected);
    }
    for (int round = 0; round < 500; ++round) {
        int n = random_int(0, 15);
        vector<int> a(n);
        for (int& x : a) x = random_int(0, 4);
        auto check = [&](auto cmp) {
            auto left = nearest_left(a, cmp), right = nearest_right(a, cmp);
            for (int i = 0; i < n; ++i) {
                int l = -1, r = n;
                for (int j = i - 1; j >= 0; --j) if (cmp(a[j], a[i])) { l = j; break; }
                for (int j = i + 1; j < n; ++j) if (cmp(a[j], a[i])) { r = j; break; }
                assert(left[i] == l && right[i] == r);
            }
        };
        check(less<int>());
        check(less_equal<int>());
        check(greater<int>());
        check(greater_equal<int>());
        // 笛卡尔树：每个子树 [L,R) 的根是区间内最左的最小值。
        auto tree = cartesian_tree(a);
        if (n == 0) { assert(tree.root == -1); continue; }
        vector<int> lo(n), hi(n);
        function<void(int)> dfs = [&](int x) {
            lo[x] = x; hi[x] = x + 1;
            for (int c : {tree.left[x], tree.right[x]}) if (c != -1) {
                assert(tree.parent[c] == x);
                dfs(c);
                lo[x] = min(lo[x], lo[c]);
                hi[x] = max(hi[x], hi[c]);
            }
            if (tree.left[x] != -1) assert(hi[tree.left[x]] == x);
            if (tree.right[x] != -1) assert(lo[tree.right[x]] == x + 1);
            assert(min_element(a.begin() + lo[x], a.begin() + hi[x]) - a.begin() == x);
        };
        dfs(tree.root);
        assert(lo[tree.root] == 0 && hi[tree.root] == n && tree.parent[tree.root] == -1);
        auto max_tree = cartesian_tree(a, greater<int>());
        assert(max_element(a.begin(), a.end()) - a.begin() == max_tree.root);
    }
}

void rectangle_checks() {
    for (int round = 0; round < 1000; ++round) {
        int k = random_int(0, 6);
        vector<Rectangle> rects(k);
        const int S = 8;
        vector<vector<char>> cell(S, vector<char>(S));
        for (auto& r : rects) {
            r.x1 = random_int(0, S); r.x2 = random_int(0, S);
            r.y1 = random_int(0, S); r.y2 = random_int(0, S);
            if (r.x1 > r.x2) swap(r.x1, r.x2);
            if (r.y1 > r.y2) swap(r.y1, r.y2);
            for (long long x = r.x1; x < r.x2; ++x) for (long long y = r.y1; y < r.y2; ++y) cell[x][y] = 1;
        }
        long long area = 0, perimeter = 0;
        auto filled = [&](int x, int y) { return 0 <= x && x < S && 0 <= y && y < S && cell[x][y]; };
        for (int x = 0; x < S; ++x) for (int y = 0; y < S; ++y) if (cell[x][y]) {
            ++area;
            perimeter += !filled(x - 1, y) + !filled(x + 1, y) + !filled(x, y - 1) + !filled(x, y + 1);
        }
        assert(rectangle_union_area(rects) == area);
        assert(rectangle_union_perimeter(rects) == perimeter);
    }
    long long huge = 1000000000;
    assert(rectangle_union_area({{-huge, -huge, huge, huge}, {0, 0, huge, huge}}) == 4 * huge * huge);
}

int main() {
    prime_checks();
    crt_checks();
    gauss_checks();
    permutation_checks();
    basic_checks();
    rectangle_checks();
    cout << "All math, basic and geometry extension tests passed\n";
}
