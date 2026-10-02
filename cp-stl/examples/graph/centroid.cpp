#include <algorithm>
#include <array>
#include <iostream>
#include <vector>
#include "graph/centroid.hpp"

// 无序对 (i,j) 中 d[i] + d[j] <= k 的个数：排序后双指针。
long long count_pairs(std::vector<int> d, int k) {
    std::sort(d.begin(), d.end());
    long long result = 0;
    for (int i = 0, j = int(d.size()) - 1; i < j;) {
        if (d[i] + d[j] <= k) { result += j - i; ++i; }
        else --j;
    }
    return result;
}

int main() {
    // 0-1-2-3-4 是一条链，5 挂在 2 上。
    std::vector<std::vector<int>> g{{1}, {0, 2}, {1, 3, 5}, {2, 4}, {3}, {2}};
    std::cout << "centroids:";
    for (int c : cp::tree_centroids(g)) std::cout << ' ' << c;
    cp::CentroidDecomposition cd(g);
    std::cout << "\ncentroid tree parent:";
    for (int p : cd.parent) std::cout << ' ' << p;
    std::cout << '\n';

    // 点分治：统计距离 <= K 的无序点对，每个重心只统计经过它的路径。
    const int K = 2;
    long long pairs = 0;
    for (int c : cd.order) {
        std::vector<int> all{0}; // 重心自己到自己的距离
        for (int s : g[c]) {
            if (!cd.can_visit(c, s)) continue;
            std::vector<int> branch;
            std::vector<std::array<int, 3>> queue{{s, c, 1}}; // (点, 父亲, 到重心的距离)
            for (std::size_t i = 0; i < queue.size(); ++i) {
                auto [u, p, d] = queue[i];
                branch.push_back(d);
                for (int v : g[u]) if (v != p && cd.can_visit(c, v)) queue.push_back({v, u, d + 1});
            }
            pairs -= count_pairs(branch, K); // 同一分支内的点对不经过 c，在更深层统计
            all.insert(all.end(), branch.begin(), branch.end());
        }
        pairs += count_pairs(all, K);
    }
    std::cout << "pairs with distance <= " << K << ": " << pairs << '\n';
}
