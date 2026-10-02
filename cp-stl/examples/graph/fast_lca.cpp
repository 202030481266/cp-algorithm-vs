#include <iostream>
#include <vector>
#include "graph/fast_lca.hpp"

int main() {
    // 边：0-1, 0-2, 1-3, 1-4, 2-5，根为 0。
    std::vector<std::vector<int>> g{{1, 2}, {0, 3, 4}, {0, 5}, {1}, {1}, {2}};
    cp::FastLCA tree(g, 0);
    std::cout << std::boolalpha;
    std::cout << "lca(3,4)=" << tree.lca(3, 4) << " lca(3,5)=" << tree.lca(3, 5)
              << " distance(4,5)=" << tree.distance(4, 5) << '\n';
    std::cout << "is_ancestor(1,4)=" << tree.is_ancestor(1, 4) << " is_ancestor(2,4)=" << tree.is_ancestor(2, 4) << '\n';
    std::cout << "subtree(1):";
    for (int i = tree.tin[1]; i < tree.tout[1]; ++i) std::cout << ' ' << tree.order[i];
    std::cout << '\n';
}
