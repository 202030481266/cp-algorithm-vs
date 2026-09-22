#pragma once
// 使用说明：cp-stl/docs/usage/string/z_function.md
// 完整示例：cp-stl/examples/string/z_function.cpp
#include <algorithm>
#include <string_view>
#include <vector>

namespace cp {
// z[i] = s 与 s[i..] 的最长公共前缀长度，约定 z[0]=n。O(n)。
inline std::vector<int> z_function(std::string_view s) {
    int n = int(s.size());
    std::vector<int> z(n);
    if (n) z[0] = n;
    for (int i = 1, l = 0, r = 0; i < n; ++i) {
        if (i < r) z[i] = std::min(r - i, z[i - l]);
        while (i + z[i] < n && s[z[i]] == s[i + z[i]]) ++z[i];
        if (i + z[i] > r) { l = i; r = i + z[i]; }
    }
    return z;
}
} // namespace cp
