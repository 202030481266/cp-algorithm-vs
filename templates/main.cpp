#include <algorithm>
#include <array>
#include <bitset>
#include <cassert>
#include <cmath>
#include <cstdio>
#include <functional>
#include <iostream>
#include <limits>
#include <map>
#include <numeric>
#include <queue>
#include <set>
#include <string>
#include <tuple>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

using namespace std;
using ll = long long;

void solve() {
    // Write your solution here.
}

int main() {
    // Enable LOCAL_FILE in project properties only when you want file I/O.
    // These calls must precede sync_with_stdio(false).
#ifdef LOCAL_FILE
    if (!freopen("data/input.txt", "r", stdin)) {
        perror("data/input.txt");
        return 1;
    }
    if (!freopen("data/output.txt", "w", stdout)) {
        perror("data/output.txt");
        return 1;
    }
#endif
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int t = 1;
    // cin >> t;  // Enable only for problems with a test-case count.
    while (t--) solve();
}
