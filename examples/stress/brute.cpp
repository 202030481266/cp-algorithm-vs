// Enumerate every nonempty interval: O(n^2), independent of solution.cpp.
#include <algorithm>
#include <iostream>
#include <limits>
#include <vector>

int main() {
    int n;
    if (!(std::cin >> n) || n < 1) return 1;
    std::vector<long long> a(n);
    for (auto& x : a) std::cin >> x;
    long long answer = std::numeric_limits<long long>::lowest();
    for (int l = 0; l < n; ++l) {
        long long sum = 0;
        for (int r = l; r < n; ++r) {
            sum += a[r];
            answer = std::max(answer, sum);
        }
    }
    std::cout << answer << '\n';
}
