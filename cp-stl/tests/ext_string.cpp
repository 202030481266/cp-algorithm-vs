// Deterministic property tests for string/suffix_array.hpp and string/suffix_automaton.hpp.
// Small random inputs are compared with brute force; large inputs are checked with linear-time invariants.
#ifdef NDEBUG
#error Tests require assertions
#endif
#include <algorithm>
#include <cassert>
#include <iostream>
#include <map>
#include <random>
#include <set>
#include <string>
#include <utility>
#include <vector>
#include "string/suffix_array.hpp"
#include "string/suffix_automaton.hpp"

using namespace std;
using namespace cp;
mt19937_64 rng(20261124);
long long random_ll(long long l, long long r) { return uniform_int_distribution<long long>(l, r)(rng); }
int random_int(int l, int r) { return int(random_ll(l, r)); }

string random_string(int n, int sigma, char first = 'a') {
    string s(n, first);
    for (char& c : s) c = char(first + random_int(0, sigma - 1));
    return s;
}

template<class Sequence> vector<int> naive_suffix_array(const Sequence& s) {
    int n = int(s.size());
    vector<int> sa(n);
    for (int i = 0; i < n; ++i) sa[i] = i;
    sort(sa.begin(), sa.end(), [&](int a, int b) {
        return lexicographical_compare(s.begin() + a, s.end(), s.begin() + b, s.end());
    });
    return sa;
}

template<class Sequence> int naive_lcp(const Sequence& s, int a, int b) {
    int n = int(s.size()), h = 0;
    while (a + h < n && b + h < n && s[a + h] == s[b + h]) ++h;
    return h;
}

// O(n) 校验：sa 是排列，且相邻后缀严格递增（比较首字符，相同时比较去掉首字符后的排名）。
template<class Sequence> void check_suffix_array(const Sequence& s, const vector<int>& sa) {
    int n = int(s.size());
    assert(int(sa.size()) == n);
    vector<int> rank(n + 1, -1);
    for (int i = 0; i < n; ++i) {
        assert(0 <= sa[i] && sa[i] < n && rank[sa[i]] == -1);
        rank[sa[i]] = i;
    }
    rank[n] = -1;
    for (int i = 1; i < n; ++i) {
        int a = sa[i - 1], b = sa[i];
        assert(s[a] < s[b] || (s[a] == s[b] && rank[a + 1] < rank[b + 1]));
    }
}

template<class Sequence, class Array, class Pattern>
void check_suffix_array_queries(const Sequence& s, const Array& sa, const Pattern& pattern) {
    int n = int(s.size()), m = int(pattern.size());
    auto [lo, hi] = sa.find(pattern);
    assert(0 <= lo && lo <= hi && hi <= n);
    for (int i = 0; i < n; ++i) {
        int p = sa.sa[i];
        bool occurs = p + m <= n && equal(pattern.begin(), pattern.end(), s.begin() + p);
        assert(occurs == (lo <= i && i < hi));
    }
}

template<class Sequence> void check_small_suffix_array(const Sequence& s, const SuffixArray& sa) {
    int n = int(s.size());
    assert(sa.size() == n && sa.sa == naive_suffix_array(s));
    set<vector<long long>> distinct;
    for (int i = 0; i < n; ++i) {
        assert(sa.rank[sa.sa[i]] == i);
        assert(sa.height[i] == (i ? naive_lcp(s, sa.sa[i - 1], sa.sa[i]) : 0));
        for (int j = i + 1; j <= n; ++j) distinct.insert(vector<long long>(s.begin() + i, s.begin() + j));
        for (int j = 0; j < n; ++j) assert(sa.lcp(i, j) == naive_lcp(s, i, j));
    }
    assert(sa.distinct_substrings() == static_cast<long long>(distinct.size()));
    for (int round = 0; round < 20; ++round) {
        int l1 = random_int(0, n), r1 = random_int(l1, n), l2 = random_int(0, n), r2 = random_int(l2, n);
        vector<long long> x(s.begin() + l1, s.begin() + r1), y(s.begin() + l2, s.begin() + r2);
        int expected = x < y ? -1 : (y < x ? 1 : 0), actual = sa.compare(l1, r1, l2, r2);
        assert((actual > 0) - (actual < 0) == expected);
    }
}

void suffix_array_checks() {
    assert(suffix_array("").empty() && suffix_array(vector<int>{}).empty());
    assert(SuffixArray("").distinct_substrings() == 0 && SuffixArray("").find("").first == 0);
    assert((SuffixArray("banana").sa == vector<int>{5, 3, 1, 0, 4, 2}));
    assert((SuffixArray("banana").height == vector<int>{0, 1, 3, 0, 0, 2}));
    for (int round = 0; round < 6000; ++round) {
        int n = random_int(0, round % 3 ? 14 : 40), sigma = random_int(1, round % 5 ? 3 : 26);
        string s = random_string(n, sigma);
        vector<unsigned char> bytes(s.begin(), s.end());
        SuffixArray sa(s);
        assert(suffix_array(s) == sa.sa);
        check_small_suffix_array(bytes, sa);
        for (int k = 0; k < 6; ++k) {
            string pattern = random_string(random_int(0, 5), sigma);
            if (n && k % 2) { int l = random_int(0, n - 1); pattern = s.substr(l, random_int(0, n - l)); }
            check_suffix_array_queries(s, sa, pattern);
        }
    }
    // 任意字节（含 0 和大于 127 的值）按 unsigned char 排序。
    for (int round = 0; round < 1000; ++round) {
        int n = random_int(0, 30);
        string s(n, '\0');
        vector<unsigned char> bytes(n);
        for (int i = 0; i < n; ++i) { bytes[i] = static_cast<unsigned char>(random_int(0, 3) * 85); s[i] = char(bytes[i]); }
        check_small_suffix_array(bytes, SuffixArray(s));
    }
    // 整数序列：负数、很大的值，以及 long long。
    for (int round = 0; round < 3000; ++round) {
        int n = random_int(0, 30), spread = random_int(1, 4);
        vector<int> a(n);
        vector<long long> wide(n);
        for (int i = 0; i < n; ++i) {
            a[i] = random_int(-spread, spread) * (round % 2 ? 1 : 500000000);
            wide[i] = a[i] * 4000000000LL;
        }
        SuffixArray sa(a);
        check_small_suffix_array(a, sa);
        assert(suffix_array(a) == sa.sa && suffix_array(wide) == sa.sa);
        vector<int> pattern(random_int(0, 4));
        for (int& x : pattern) x = random_int(-spread, spread) * (round % 2 ? 1 : 500000000);
        check_suffix_array_queries(a, sa, pattern);
    }
    // 大数据：随机串、全相同、周期串、斐波那契串、Thue-Morse 串。
    const int N = 200000;
    vector<string> big;
    big.push_back(random_string(N, 2));
    big.push_back(random_string(N, 26));
    big.push_back(string(N, 'a'));
    string periodic, fibonacci = "a", previous = "b", thue(N, 'a');
    while (int(periodic.size()) < N) periodic += "abcab";
    while (int(fibonacci.size()) < N) { string next = fibonacci + previous; previous = fibonacci; fibonacci = next; }
    for (int i = 0; i < N; ++i) { int bits = 0; for (int x = i; x; x &= x - 1) ++bits; thue[i] = char('a' + bits % 2); }
    big.push_back(periodic);
    big.push_back(fibonacci);
    big.push_back(thue);
    for (const string& s : big) {
        vector<unsigned char> bytes(s.begin(), s.end());
        SuffixArray sa(s);
        check_suffix_array(bytes, sa.sa);
        int n = int(s.size());
        for (int round = 0; round < 2000; ++round) { // height 的定义与极大性
            int i = random_int(1, n - 1), a = sa.sa[i - 1], b = sa.sa[i], h = sa.height[i];
            assert(a + h <= n && b + h <= n && (a + h == n || b + h == n || s[a + h] != s[b + h]));
            int probe = h ? random_int(0, h - 1) : 0;
            assert(h == 0 || s[a + probe] == s[b + probe]);
        }
    }
    SuffixArray same(big[2]);
    assert(same.distinct_substrings() == N && same.lcp(0, N / 2) == N / 2 && same.lcp(7, 7) == N - 7);
    assert(same.find(string(1000, 'a')) == make_pair(999, N) && same.compare(0, 10, 5, 15) == 0);
    vector<int> increasing(N);
    for (int i = 0; i < N; ++i) increasing[i] = i;
    assert(suffix_array(increasing) == increasing);
}

template<class Automaton> long long count_paths(const Automaton& sam) {
    vector<long long> paths(sam.size(), 1);
    vector<int> order = sam.order();
    assert(order[0] == 0 && int(order.size()) == sam.size());
    for (int i = 1; i < sam.size(); ++i) assert(sam[order[i - 1]].len <= sam[order[i]].len);
    for (int i = sam.size() - 1; i >= 0; --i)
        for (int to : sam[order[i]].next) if (to) { assert(sam[to].len > sam[order[i]].len); paths[order[i]] += paths[to]; }
    return paths[0];
}

// words 的全部子串：出现次数，以及（只有一个串时）最早的结束位置。
template<class Automaton>
void check_automaton(const Automaton& sam, const vector<string>& words, int sigma, char first) {
    map<string, pair<int, int>> info;
    int total = 0;
    for (const string& w : words) {
        int n = int(w.size());
        total += n;
        for (int i = 0; i < n; ++i)
            for (int j = i + 1; j <= n; ++j) {
                auto [it, fresh] = info.try_emplace(w.substr(i, j - i), 0, j);
                ++it->second.first;
                it->second.second = min(it->second.second, j);
            }
    }
    assert(sam.distinct_substrings() == static_cast<long long>(info.size()));
    assert(count_paths(sam) == static_cast<long long>(info.size()) + 1); // 路径与子串一一对应
    assert(sam.size() <= max(total + 1, 2 * total - 1));
    vector<long long> occ = sam.occurrences();
    assert(occ[0] == total);
    for (auto& [t, value] : info) {
        int v = sam.find(t), length = int(t.size());
        assert(v > 0 && occ[v] == value.first);
        assert(sam[sam[v].link].len < length && length <= sam[v].len);
        if (words.size() == 1) assert(sam[v].end == value.second);
    }
    for (int round = 0; round < 30; ++round) {
        string t = random_string(random_int(0, 8), sigma, first);
        assert((sam.find(t) != -1) == (t.empty() || info.count(t)));
        vector<int> matched = sam.match(t);
        for (int i = 0; i < int(t.size()); ++i) {
            int best = 0;
            for (int length = 1; length <= i + 1; ++length) if (info.count(t.substr(i + 1 - length, length))) best = length;
            assert(matched[i] == best);
        }
    }
}

void suffix_automaton_checks() {
    assert(SuffixAutomaton().size() == 1 && SuffixAutomaton("").distinct_substrings() == 0);
    assert(SuffixAutomaton("abcbc").distinct_substrings() == 12);
    for (int round = 0; round < 4000; ++round) {
        int sigma = random_int(1, round % 4 ? 3 : 26);
        string s = random_string(random_int(0, 16), sigma);
        check_automaton(SuffixAutomaton(s), {s}, sigma, 'a');
    }
    // 广义后缀自动机：多个串依次从根插入，含重复的串和空串。
    for (int round = 0; round < 4000; ++round) {
        int sigma = random_int(1, 3), k = random_int(1, 5);
        vector<string> words(k);
        SuffixAutomaton sam;
        for (string& w : words) {
            w = random_int(0, 4) || words[0].empty() ? random_string(random_int(0, 9), sigma) : words[0];
            int last = sam.add(w);
            assert(last == (w.empty() ? 0 : sam.find(w)));
        }
        check_automaton(sam, words, sigma, 'a');
    }
    // 其他字符集，以及直接用字符编号 extend。
    for (int round = 0; round < 1500; ++round) {
        string digits = random_string(random_int(0, 14), 10, '0'), bits = random_string(random_int(0, 16), 2, '0');
        check_automaton(BasicSuffixAutomaton<10, '0'>(digits), {digits}, 10, '0');
        BasicSuffixAutomaton<2, '0'> binary;
        int last = 0;
        for (char c : bits) last = binary.extend(last, c - '0');
        check_automaton(binary, {bits}, 2, '0');
        string letters = bits;
        for (char& c : letters) c = char(c - '0' + 'a');
        assert(SuffixAutomaton(letters).size() == binary.size());
    }
    // 大数据：与后缀数组互相验证本质不同子串个数。
    const int N = 200000;
    for (int sigma : {1, 2, 26}) {
        string s = random_string(N, sigma);
        SuffixAutomaton sam;
        sam.reserve(N);
        sam.add(s);
        assert(sam.size() <= 2 * N - 1 && sam.distinct_substrings() == SuffixArray(s).distinct_substrings());
        assert(count_paths(sam) > 0 && sam.find(s) != -1 && sam.occurrences()[sam.find(s)] == 1);
        assert(sam.match(s).back() == N);
    }
    SuffixAutomaton same(string(N, 'a'));
    assert(same.size() == N + 1 && same.distinct_substrings() == N);
    assert(same.occurrences()[same.find("a")] == N && same.occurrences()[same.find(string(N - 3, 'a'))] == 4);
}

int main() {
    suffix_array_checks();
    suffix_automaton_checks();
    cout << "All string extension tests passed\n";
}
