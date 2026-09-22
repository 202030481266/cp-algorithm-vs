#pragma once
// 使用说明：cp-stl/docs/usage/string/kmp.md
// 完整示例：cp-stl/examples/string/kmp.cpp
#include <numeric>
#include <string_view>
#include <vector>

namespace cp {
// pi[i] = s[0..i] 的最长真前后缀长度，O(n)。
inline std::vector<int> prefix_function(std::string_view s) {
    int n = int(s.size());
    std::vector<int> pi(n);
    for (int i = 1; i < n; ++i) {
        int j = pi[i - 1];
        while (j && s[i] != s[j]) j = pi[j - 1];
        if (s[i] == s[j]) ++j;
        pi[i] = j;
    }
    return pi;
}
// 所有匹配起点（含重叠），O(n+m)。空模式匹配 [0,n] 的全部边界。
inline std::vector<int> kmp_find(std::string_view text, std::string_view pattern) {
    std::vector<int> positions;
    if (pattern.empty()) {
        positions.resize(text.size() + 1);
        std::iota(positions.begin(), positions.end(), 0);
        return positions;
    }
    auto pi = prefix_function(pattern);
    for (int i = 0, j = 0; i < int(text.size()); ++i) {
        while (j && text[i] != pattern[j]) j = pi[j - 1];
        if (text[i] == pattern[j]) ++j;
        if (j == int(pattern.size())) {
            positions.push_back(i - j + 1);
            j = pi[j - 1];
        }
    }
    return positions;
}
} // namespace cp
