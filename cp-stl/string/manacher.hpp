#pragma once
// 使用说明：cp-stl/docs/usage/string/manacher.md
// 完整示例：cp-stl/examples/string/manacher.cpp
#include <algorithm>
#include <string_view>
#include <utility>
#include <vector>

namespace cp {
struct Palindromes {
    // odd[i]: [i-odd[i]+1, i+odd[i])；even[i]: [i-even[i], i+even[i])。
    std::vector<int> odd, even;
    std::pair<int, int> longest{0, 0}; // 最长回文的 [l,r)，空串为 [0,0)。
};
inline Palindromes manacher(std::string_view s) {
    int n = int(s.size());
    Palindromes result{std::vector<int>(n), std::vector<int>(n), {0, 0}};
    auto update = [&](int l, int r) {
        if (r - l > result.longest.second - result.longest.first) result.longest = {l, r};
    };
    for (int i = 0, l = 0, r = -1; i < n; ++i) {
        int k = i > r ? 1 : std::min(result.odd[l + r - i], r - i + 1);
        while (i - k >= 0 && i + k < n && s[i - k] == s[i + k]) ++k;
        result.odd[i] = k;
        update(i - k + 1, i + k);
        if (i + k - 1 > r) { l = i - k + 1; r = i + k - 1; }
    }
    for (int i = 0, l = 0, r = -1; i < n; ++i) {
        int k = i > r ? 0 : std::min(result.even[l + r - i + 1], r - i + 1);
        while (i - k - 1 >= 0 && i + k < n && s[i - k - 1] == s[i + k]) ++k;
        result.even[i] = k;
        update(i - k, i + k);
        if (i + k - 1 > r) { l = i - k; r = i + k - 1; }
    }
    return result;
}
} // namespace cp
