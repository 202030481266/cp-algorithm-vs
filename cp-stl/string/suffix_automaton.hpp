#pragma once
// 使用说明：cp-stl/docs/usage/string/suffix_automaton.md
// 完整示例：cp-stl/examples/string/suffix_automaton.cpp
#include <algorithm>
#include <array>
#include <cassert>
#include <string_view>
#include <vector>

namespace cp {
// 后缀自动机（SAM）。字符集是从 First 开始的连续 Sigma 个字符；状态 0 是根（空串）。
// 每个状态代表一组 endpos 相同的子串，长度恰好是 (len[link], len] 中的每一个；状态数 <= 2n-1。
// 插入一个字符均摊 O(1)（克隆状态时复制 Sigma 个转移）。多次 add 得到广义后缀自动机。
template<int Sigma, int First>
class BasicSuffixAutomaton {
public:
    struct State {
        std::array<int, Sigma> next{}; // 转移，0 表示没有（没有转移会回到根）
        int link = -1;                 // 后缀链接，即 parent 树上的父亲；根为 -1
        int len = 0;                   // 这个状态中最长子串的长度
        int end = 0;                   // 某一次出现的结束位置（开区间）；只插入一个串时是最早的一次
        int count = 0;                 // 落在这个状态的已插入前缀个数（克隆出的状态为 0）
    };
private:
    std::vector<State> states_ = std::vector<State>(1);
    // p 沿 c 走到 q，但 len[q] != len[p]+1：克隆 q，把长度截到 len[p]+1。
    int split(int p, int c, int q) {
        int clone = int(states_.size());
        State copy = states_[q];
        copy.len = states_[p].len + 1;
        copy.count = 0;
        states_.push_back(copy);
        states_[q].link = clone;
        for (; p != -1 && states_[p].next[c] == q; p = states_[p].link) states_[p].next[c] = clone;
        return clone;
    }
public:
    BasicSuffixAutomaton() = default;
    explicit BasicSuffixAutomaton(std::string_view s) { add(s); }
    static int code(char c) {
        int k = static_cast<unsigned char>(c) - First;
        assert(0 <= k && k < Sigma);
        return k;
    }
    void reserve(int total_length) { states_.reserve(2 * std::size_t(total_length) + 1); }
    // 在状态 last 表示的串后面接字符编号 c（0 <= c < Sigma），返回新串所在的状态。
    // 同一个串内 last 取上一次的返回值，开始新串时取 0。转移已经存在时不会多建状态。
    int extend(int last, int c) {
        assert(0 <= last && last < size() && 0 <= c && c < Sigma);
        int cur = states_[last].next[c];
        if (cur) {
            if (states_[cur].len != states_[last].len + 1) cur = split(last, c, cur);
        } else {
            cur = int(states_.size());
            states_.emplace_back();
            states_[cur].len = states_[cur].end = states_[last].len + 1;
            int p = last;
            for (; p != -1 && !states_[p].next[c]; p = states_[p].link) states_[p].next[c] = cur;
            if (p == -1) states_[cur].link = 0;
            else {
                int q = states_[p].next[c];
                int link = states_[q].len == states_[p].len + 1 ? q : split(p, c, q);
                states_[cur].link = link;
            }
        }
        ++states_[cur].count;
        return cur;
    }
    // 从根开始插入整个串，返回它所在的状态。
    int add(std::string_view s) {
        states_.reserve(states_.size() + 2 * s.size());
        int last = 0;
        for (char c : s) last = extend(last, code(c));
        return last;
    }
    int size() const { return int(states_.size()); }
    const State& operator[](int v) const { return states_[v]; }
    // 全部状态按 len 从小到大排列（根在最前）：既是转移 DAG 的拓扑序，也是 parent 树自顶向下的顺序。
    std::vector<int> order() const {
        int longest = 0;
        for (const State& state : states_) longest = std::max(longest, state.len);
        std::vector<int> head(longest + 2), result(states_.size());
        for (const State& state : states_) ++head[state.len + 1];
        for (int i = 0; i <= longest; ++i) head[i + 1] += head[i];
        for (int v = 0; v < size(); ++v) result[head[states_[v].len]++] = v;
        return result;
    }
    // occ[v]：状态 v 中的子串在全部已插入串中的出现次数（endpos 集合大小）。occ[0] 是总长度。O(状态数)。
    std::vector<long long> occurrences() const {
        std::vector<long long> occ(states_.size());
        for (int v = 0; v < size(); ++v) occ[v] = states_[v].count;
        std::vector<int> ord = order();
        for (int i = size() - 1; i > 0; --i) occ[states_[ord[i]].link] += occ[ord[i]];
        return occ;
    }
    // 本质不同的非空子串个数。
    long long distinct_substrings() const {
        long long total = 0;
        for (int v = 1; v < size(); ++v) total += states_[v].len - states_[states_[v].link].len;
        return total;
    }
    // t 所在的状态；t 不是任何已插入串的子串时返回 -1。O(|t|)。
    int find(std::string_view t) const {
        int v = 0;
        for (char c : t) { v = states_[v].next[code(c)]; if (!v) return -1; }
        return v;
    }
    // 匹配：(v, length) 表示“已读文本的最长后缀，同时是已插入串的子串”所在的状态和长度，读入字符编号 c 后更新。
    void advance(int& v, int& length, int c) const {
        while (v && !states_[v].next[c]) { v = states_[v].link; length = states_[v].len; }
        if (states_[v].next[c]) { v = states_[v].next[c]; ++length; }
    }
    // result[i]：t[0..i] 的最长后缀的长度，要求它是已插入串的子串。O(|t|)。
    std::vector<int> match(std::string_view t) const {
        std::vector<int> result(t.size());
        int v = 0, length = 0;
        for (std::size_t i = 0; i < t.size(); ++i) { advance(v, length, code(t[i])); result[i] = length; }
        return result;
    }
};

using SuffixAutomaton = BasicSuffixAutomaton<26, 'a'>;
} // namespace cp
