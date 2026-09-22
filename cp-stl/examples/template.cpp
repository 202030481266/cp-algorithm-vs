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
    int n;
    if (!(cin >> n)) return;
    i64 answer = 0;
    for (int i = 0; i < n; ++i) {
        i64 x;
        cin >> x;
        answer += x;
    }
    cout << answer << '\n';
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int t = 1;
    // cin >> t;  // 题目开头有测试组数时再打开。
    while (t--) solve();
    return 0;
}
