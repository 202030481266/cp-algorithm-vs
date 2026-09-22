#include <iostream>
#include <vector>
#include "graph/topological_sort.hpp"

int main() {
    std::vector<std::vector<int>> dag{{1, 2}, {3}, {3}, {}};
    auto order = cp::topological_sort(dag);
    if (order.size() == dag.size()) {
        std::cout << "order:";
        for (int u : order) std::cout << ' ' << u;
        std::cout << '\n';
    }

    std::vector<std::vector<int>> cycle{{1}, {2}, {0}};
    std::cout << std::boolalpha;
    std::cout << "has_cycle=" << (cp::topological_sort(cycle).size() < cycle.size()) << '\n';
}
