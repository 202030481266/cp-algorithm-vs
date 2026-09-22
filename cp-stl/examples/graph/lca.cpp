#include <iostream>
#include <vector>
#include "graph/lca.hpp"

int main() {
    // 边：0-1、0-2、1-3、1-4、2-5。
    std::vector<std::vector<int>> g{{1, 2}, {0, 3, 4}, {0, 5}, {1}, {1}, {2}};
    cp::LCA tree(g, 0);
    std::cout << "lca(3,4)=" << tree.lca(3, 4) << '\n';
    std::cout << "lca(4,5)=" << tree.lca(4, 5) << '\n';
    std::cout << "distance(4,5)=" << tree.distance(4, 5) << '\n';
    std::cout << "depth(4)=" << tree.depth[4] << '\n';
    std::cout << "ancestors:";
    for (int k = 0; k <= 3; ++k) std::cout << ' ' << tree.kth_ancestor(4, k);
    std::cout << '\n';
}
