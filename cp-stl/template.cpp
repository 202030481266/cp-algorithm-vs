// 使用说明：cp-stl/docs/usage/template.md
// 完整示例：cp-stl/examples/template.cpp
#include <iostream>
#include <vector>
#include <algorithm>
using namespace std;

using i64 = long long;
using u64 = unsigned long long;
#if defined(__SIZEOF_INT128__)
using i128 = __int128_t;
#endif

#ifdef LOCAL
#include "util/debug.hpp"
#else
#define debug(...) ((void)0)
#endif

void solve() {
    // 在这里写单组数据的解法。
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int t = 1;
    // cin >> t;  // 题目有多组数据时再打开。
    while (t--) solve();
    return 0;
}
