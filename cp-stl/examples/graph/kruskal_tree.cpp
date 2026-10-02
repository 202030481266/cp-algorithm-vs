#include <functional>
#include <iostream>
#include <vector>
#include "graph/kruskal_tree.hpp"

int main() {
    std::vector<cp::UndirectedEdge> edges{{0, 1, 5}, {1, 2, 3}, {2, 3, 4}, {0, 3, 1}, {1, 3, 6}};
    cp::KruskalTree tree(4, edges); // 边权从小到大：最小瓶颈

    std::cout << "bottleneck(0,3)=" << *tree.bottleneck(0, 3) << " bottleneck(0,1)=" << *tree.bottleneck(0, 1)
              << " bottleneck(1,2)=" << *tree.bottleneck(1, 2) << '\n';

    // 从 0 出发只走边权 <= 3 的边能到达哪些点：最高的“点权 <= 3”的祖先的子树叶子。
    int top = tree.highest(0, [](long long w) { return w <= 3; });
    std::cout << "reach from 0 with w<=3:";
    for (int i = tree.leaf_begin[top]; i < tree.leaf_end[top]; ++i) std::cout << ' ' << tree.leaf_order[i];
    std::cout << '\n';

    cp::KruskalTree widest(4, edges, std::greater<long long>()); // 边权从大到小：最大化路径上的最小边
    std::cout << "widest(0,2)=" << *widest.bottleneck(0, 2) << " nodes=" << widest.size() << '\n';
}
