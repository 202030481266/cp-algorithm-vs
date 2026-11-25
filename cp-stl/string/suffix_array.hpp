#pragma once
// 使用说明：cp-stl/docs/usage/string/suffix_array.md
// 完整示例：cp-stl/examples/string/suffix_array.cpp
#include <algorithm>
#include <cassert>
#include <string_view>
#include <utility>
#include <vector>
#include "../data_structures/sparse_table.hpp"

namespace cp {
// SA-IS（诱导排序）：s[i] 在 [0, upper] 内，返回后缀数组，O(n + upper)。
// 每层递归的规模至少减半，深度 O(log n)。一般直接用下面的 suffix_array / SuffixArray。
inline std::vector<int> sa_is(const std::vector<int>& s, int upper) {
    int n = int(s.size());
    if (n == 0) return {};
    if (n == 1) return {0};
    std::vector<int> sa(n);
    std::vector<char> is_s(n); // S 型：后缀 i 小于后缀 i+1；最后一个后缀是 L 型。
    for (int i = n - 2; i >= 0; --i) is_s[i] = s[i] == s[i + 1] ? is_s[i + 1] : s[i] < s[i + 1];
    // 首字符相同的后缀在 sa 中连续（一个桶），桶内先 L 型后 S 型。
    std::vector<int> l_begin(upper + 2), s_begin(upper + 1), buf(upper + 1);
    for (int c : s) ++l_begin[c + 1];
    for (int c = 0; c <= upper; ++c) { l_begin[c + 1] += l_begin[c]; s_begin[c] = l_begin[c]; }
    for (int i = 0; i < n; ++i) if (!is_s[i]) ++s_begin[s[i]];
    // 已知 LMS 后缀的相对顺序 lms，诱导出全部后缀的顺序。
    auto induce = [&](const std::vector<int>& lms) {
        std::fill(sa.begin(), sa.end(), -1);
        std::copy(s_begin.begin(), s_begin.end(), buf.begin());
        for (int p : lms) sa[buf[s[p]]++] = p;
        std::copy(l_begin.begin(), l_begin.end() - 1, buf.begin());
        sa[buf[s[n - 1]]++] = n - 1;
        for (int i = 0; i < n; ++i) {
            int p = sa[i] - 1;
            if (p >= 0 && !is_s[p]) sa[buf[s[p]]++] = p;
        }
        std::copy(l_begin.begin() + 1, l_begin.end(), buf.begin()); // 每个桶的右端
        for (int i = n - 1; i >= 0; --i) {
            int p = sa[i] - 1;
            if (p >= 0 && is_s[p]) sa[--buf[s[p]]] = p;
        }
    };
    std::vector<int> lms, lms_id(n, -1); // LMS：S 型且前一个是 L 型的位置
    for (int i = 1; i < n; ++i)
        if (is_s[i] && !is_s[i - 1]) { lms_id[i] = int(lms.size()); lms.push_back(i); }
    induce(lms); // 第一遍把 LMS 子串排好序
    int m = int(lms.size());
    if (m > 1) {
        std::vector<int> sorted, name(m);
        sorted.reserve(m);
        for (int p : sa) if (lms_id[p] != -1) sorted.push_back(p);
        int kinds = 0; // 给 LMS 子串编号，相同的子串同号
        for (int i = 1; i < m; ++i) {
            int a = sorted[i - 1], b = sorted[i];
            int end_a = lms_id[a] + 1 < m ? lms[lms_id[a] + 1] : n;
            int end_b = lms_id[b] + 1 < m ? lms[lms_id[b] + 1] : n;
            bool same = end_a - a == end_b - b;
            for (; same && a < end_a; ++a, ++b) same = s[a] == s[b];
            if (same) same = a < n && b < n && s[a] == s[b]; // 右端的 LMS 字符也要相同，串尾视为最小
            if (!same) ++kinds;
            name[lms_id[sorted[i]]] = kinds;
        }
        if (kinds + 1 < m) { // 有同号的才需要递归确定 LMS 后缀的顺序
            std::vector<int> order = sa_is(name, kinds);
            for (int i = 0; i < m; ++i) sorted[i] = lms[order[i]];
        }
        induce(sorted);
    }
    return sa;
}

// 后缀数组：sa[i] 是字典序第 i 小的后缀的起点。字符按 unsigned char 比较，O(n)。
inline std::vector<int> suffix_array(std::string_view s) {
    std::vector<int> codes(s.size());
    for (std::size_t i = 0; i < s.size(); ++i) codes[i] = static_cast<unsigned char>(s[i]);
    return sa_is(codes, 255);
}

// 任意可比较元素的序列（整数数组、用互不相同的分隔符拼接的多个串等），先离散化，O(n log n)。
template<class T>
std::vector<int> suffix_array(const std::vector<T>& s) {
    std::vector<T> values(s);
    std::sort(values.begin(), values.end());
    values.erase(std::unique(values.begin(), values.end()), values.end());
    std::vector<int> codes(s.size());
    for (std::size_t i = 0; i < s.size(); ++i)
        codes[i] = int(std::lower_bound(values.begin(), values.end(), s[i]) - values.begin());
    return sa_is(codes, std::max(0, int(values.size()) - 1));
}

// 后缀数组 + rank + height，支持任意两个后缀的 LCP、子串比较和模式串查找。
//   sa[i]     排名第 i（从 0 开始）的后缀的起点
//   rank[p]   后缀 p 的排名，rank[sa[i]] = i
//   height[i] 排名相邻的后缀 sa[i-1] 与 sa[i] 的最长公共前缀，height[0] = 0
// 构造 O(n)（整数序列 O(n log n)）；lcp / compare 第一次调用时建 ST 表 O(n log n)，之后 O(1)。
class SuffixArray {
    std::vector<int> text_;
    mutable SparseTable<int, Min> table_;
    mutable bool table_ready_ = false;
    static int code(char c) { return static_cast<unsigned char>(c); }
    static int code(int c) { return c; }
    static std::vector<int> codes(std::string_view s) {
        std::vector<int> result(s.size());
        for (std::size_t i = 0; i < s.size(); ++i) result[i] = code(s[i]);
        return result;
    }
    void build() { // Kasai：按原串位置顺序计算，h 每步至多减 1
        int n = size();
        rank.assign(n, 0);
        height.assign(n, 0);
        for (int i = 0; i < n; ++i) rank[sa[i]] = i;
        for (int i = 0, h = 0; i < n; ++i) {
            if (h > 0) --h;
            if (rank[i] == 0) continue;
            int j = sa[rank[i] - 1];
            while (i + h < n && j + h < n && text_[i + h] == text_[j + h]) ++h;
            height[rank[i]] = h;
        }
    }
    template<class Pattern>
    std::pair<int, int> range(const Pattern& pattern) const {
        int n = size(), m = int(pattern.size());
        // 后缀 p 与 pattern 比较：-1 更小，0 以 pattern 为前缀，1 更大。
        auto compare = [&](int p) {
            for (int k = 0; k < m; ++k) {
                if (p + k == n) return -1;
                int c = code(pattern[k]);
                if (text_[p + k] != c) return text_[p + k] < c ? -1 : 1;
            }
            return 0;
        };
        auto first = std::partition_point(sa.begin(), sa.end(), [&](int p) { return compare(p) < 0; });
        auto last = std::partition_point(first, sa.end(), [&](int p) { return compare(p) == 0; });
        return {int(first - sa.begin()), int(last - sa.begin())};
    }
public:
    std::vector<int> sa, rank, height;
    explicit SuffixArray(std::string_view s) : text_(codes(s)), sa(sa_is(text_, 255)) { build(); }
    explicit SuffixArray(const std::vector<int>& s) : text_(s), sa(suffix_array(s)) { build(); }
    int size() const { return int(text_.size()); }
    // 后缀 i 与后缀 j 的最长公共前缀长度。
    int lcp(int i, int j) const {
        int n = size();
        assert(0 <= i && i < n && 0 <= j && j < n);
        if (i == j) return n - i;
        if (!table_ready_) { table_ = SparseTable<int, Min>(height); table_ready_ = true; }
        int l = rank[i], r = rank[j];
        if (l > r) std::swap(l, r);
        return table_.query(l + 1, r + 1);
    }
    // 比较子串 [l1,r1) 与 [l2,r2)：返回负数 / 0 / 正数。
    int compare(int l1, int r1, int l2, int r2) const {
        assert(0 <= l1 && l1 <= r1 && r1 <= size() && 0 <= l2 && l2 <= r2 && r2 <= size());
        int a = r1 - l1, b = r2 - l2, shorter = std::min(a, b);
        if (shorter > 0) {
            int common = lcp(l1, l2);
            if (common < shorter) return text_[l1 + common] < text_[l2 + common] ? -1 : 1;
        }
        return (a > b) - (a < b);
    }
    // 以 pattern 为前缀的后缀在 sa 中的排名区间 [lo,hi)：出现 hi-lo 次，位置是 sa[lo..hi)。O(|pattern| log n)。
    std::pair<int, int> find(std::string_view pattern) const { return range(pattern); }
    std::pair<int, int> find(const std::vector<int>& pattern) const { return range(pattern); }
    // 本质不同的非空子串个数。
    long long distinct_substrings() const {
        long long n = size(), total = n * (n + 1) / 2;
        for (int h : height) total -= h;
        return total;
    }
};
} // namespace cp
