#include <algorithm>
#include <functional>
#include <iostream>
#include <vector>
#include "basic/monotonic_stack.hpp"

int main() {
    // 柱状图中最大的矩形（LeetCode 84）：以 h[i] 为高，向两边扩展到第一个更矮的柱子。
    std::vector<int> h{2, 1, 5, 6, 2, 3};
    auto left = cp::nearest_left(h);   // 左边第一个严格更小的位置，不存在为 -1
    auto right = cp::nearest_right(h); // 右边第一个严格更小的位置，不存在为 n
    long long best = 0;
    for (int i = 0; i < int(h.size()); ++i) best = std::max(best, 1LL * h[i] * (right[i] - left[i] - 1));
    std::cout << "left:";
    for (int x : left) std::cout << ' ' << x;
    std::cout << "\nright:";
    for (int x : right) std::cout << ' ' << x;
    std::cout << "\nlargest rectangle=" << best << '\n';

    auto next_greater = cp::nearest_right(h, std::greater<int>()); // 右边第一个严格更大
    std::cout << "next greater:";
    for (int x : next_greater) std::cout << ' ' << x;

    auto tree = cp::cartesian_tree(std::vector<int>{3, 1, 4, 1, 5}); // 小根，相等时左边的在上
    std::cout << "\ncartesian root=" << tree.root << " parent:";
    for (int p : tree.parent) std::cout << ' ' << p;
    std::cout << '\n';
}
