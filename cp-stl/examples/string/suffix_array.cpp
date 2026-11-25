#include <iostream>
#include <string>
#include "string/suffix_array.hpp"

int main() {
    std::string s = "banana";
    cp::SuffixArray sa(s);
    for (int i = 0; i < sa.size(); ++i)
        std::cout << "sa[" << i << "]=" << sa.sa[i] << " height=" << sa.height[i] << ' ' << s.substr(sa.sa[i]) << '\n';
    std::cout << "distinct=" << sa.distinct_substrings() << " lcp(1,3)=" << sa.lcp(1, 3)
              << " compare(ban,nan)=" << sa.compare(0, 3, 2, 5) << '\n';

    auto [lo, hi] = sa.find("ana"); // 出现的位置是 sa.sa[lo..hi)
    std::cout << "ana: count=" << hi - lo << " at";
    for (int i = lo; i < hi; ++i) std::cout << ' ' << sa.sa[i];
    std::cout << '\n';

    // 两个串的最长公共子串：用一个不出现的字符拼接，取“来自不同串的相邻后缀”的 height 最大值。
    std::string a = "xabcdy", b = "zzabcd", joined = a + "#" + b;
    cp::SuffixArray both(joined);
    int best = 0, at = 0;
    for (int i = 1; i < both.size(); ++i) {
        bool first = both.sa[i - 1] < int(a.size()), second = both.sa[i] < int(a.size());
        if (first != second && both.height[i] > best) { best = both.height[i]; at = both.sa[i]; }
    }
    std::cout << "lcs=" << joined.substr(at, best) << '\n';
}
