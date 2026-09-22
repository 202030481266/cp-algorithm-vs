// Maximum NONEMPTY subarray sum: O(n), including all-negative arrays.
#include <algorithm>
#include <iostream>

int main() {
    int n;
    if (!(std::cin >> n) || n < 1) return 1;
    long long x;
    std::cin >> x;
    long long ending = x, answer = x;
    for (int i = 1; i < n; ++i) {
        std::cin >> x;
        ending = std::max(x, ending + x);
        answer = std::max(answer, ending);
    }
    std::cout << answer << '\n';
}
