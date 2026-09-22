#include <iostream>
#include <vector>
#include "graph/tree_diameter.hpp"

int main() {
    std::vector<std::vector<int>> g{{1, 2}, {0, 3, 4}, {0}, {1}, {1}};
    auto path = cp::tree_diameter(g);
    if (!path.empty()) {
        std::cout << "length=" << int(path.size()) - 1 << '\n';
        std::cout << "path:";
        for (int u : path) std::cout << ' ' << u;
        std::cout << '\n';
    }
    std::vector<std::vector<int>> single(1);
    std::cout << "single_length=" << int(cp::tree_diameter(single).size()) - 1 << '\n';
    std::cout << "empty_size=" << cp::tree_diameter({}).size() << '\n';
}
