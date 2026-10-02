#include <algorithm>
#include <iostream>
#include <utility>
#include <vector>
#include "graph/rerooting.hpp"

int main() {
    int n = 6;
    std::vector<std::pair<int, int>> edges{{0, 1}, {0, 2}, {2, 3}, {2, 4}, {2, 5}};

    // 每个点到其他所有点的距离和（LeetCode 834）：Info = (子树点数, 子树内各点到根的距离和)。
    struct Info { long long size, dist_sum; };
    auto sums = cp::rerooting(n, edges, Info{0, 0},
        [](Info a, Info b) { return Info{a.size + b.size, a.dist_sum + b.dist_sum}; },
        [](Info child, int, int, int) { return Info{child.size, child.dist_sum + child.size}; }, // 每个点多走一条边
        [](Info merged, int) { return Info{merged.size + 1, merged.dist_sum}; });               // 加上根自己
    std::cout << "sum of distances:";
    for (auto info : sums) std::cout << ' ' << info.dist_sum;

    // 每个点的最远距离：子树高度取最大值。
    auto farthest = cp::rerooting(n, edges, 0,
        [](int a, int b) { return std::max(a, b); },
        [](int height, int, int, int) { return height + 1; },
        [](int merged, int) { return merged; });
    std::cout << "\nfarthest:";
    for (int h : farthest) std::cout << ' ' << h;
    std::cout << '\n';
}
