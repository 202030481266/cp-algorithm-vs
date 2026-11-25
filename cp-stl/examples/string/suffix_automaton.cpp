#include <algorithm>
#include <iostream>
#include <string>
#include <vector>
#include "string/suffix_automaton.hpp"

int main() {
    std::string s = "abcbc";
    cp::SuffixAutomaton sam(s);
    std::cout << "states=" << sam.size() << " distinct=" << sam.distinct_substrings() << '\n';

    // 子串的出现次数与最早出现的位置。
    std::vector<long long> occ = sam.occurrences();
    for (std::string t : {"bc", "abc", "c", "cc"}) {
        int v = sam.find(t);
        if (v == -1) std::cout << t << ": absent\n";
        else std::cout << t << ": count=" << occ[v] << " first=" << sam[v].end - int(t.size()) << '\n';
    }

    // 最长公共子串：让 t 在 s 的自动机上匹配，len[i] 是 t[0..i] 能匹配上的最长后缀。
    std::string t = "xbcbcy";
    std::vector<int> len = sam.match(t);
    int end = int(std::max_element(len.begin(), len.end()) - len.begin());
    std::cout << "lcs=" << t.substr(end + 1 - len[end], len[end]) << '\n';

    // 按 order 的逆序在转移 DAG 上 DP：paths[v] 是从 v 出发能读出的不同串的个数（含空串）。
    std::vector<int> order = sam.order();
    std::vector<long long> paths(sam.size(), 1);
    for (int i = sam.size() - 1; i >= 0; --i)
        for (int to : sam[order[i]].next) if (to) paths[order[i]] += paths[to];
    // 据此求字典序第 k 小的子串（k 从 0 开始，相同的子串只算一次）。
    std::string kth;
    long long k = 7;
    for (int v = 0; k >= 0; --k)
        for (int c = 0; c < 26; ++c) {
            int to = sam[v].next[c];
            if (!to) continue;
            if (k < paths[to]) { kth += char('a' + c); v = to; break; }
            k -= paths[to];
        }
    std::cout << "kth(7)=" << kth << '\n';

    // 广义后缀自动机：多次 add，统计的是所有串的子串。
    cp::SuffixAutomaton multi;
    for (std::string w : {"ab", "bab", "abc"}) multi.add(w);
    std::cout << "multi: distinct=" << multi.distinct_substrings()
              << " count(ab)=" << multi.occurrences()[multi.find("ab")] << '\n';
}
