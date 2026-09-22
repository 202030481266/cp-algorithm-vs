#include <iostream>
#include "graph/dinic.hpp"

int main() {
    cp::Dinic network(4);
    network.add_edge(0, 1, 3);
    network.add_edge(0, 2, 2);
    network.add_edge(1, 2, 1);
    network.add_edge(1, 3, 2);
    network.add_edge(2, 3, 3);

    long long first = network.flow(0, 3, 2); // 本次最多送 2
    long long rest = network.flow(0, 3);     // 继续求到最大流
    std::cout << "first=" << first << " rest=" << rest << '\n';
    std::cout << "total=" << first + rest << '\n';
    std::cout << "additional=" << network.flow(0, 3) << '\n';
    auto source_side = network.min_cut(0);
    std::cout << "source_side:";
    for (int u = 0; u < 4; ++u) if (source_side[u]) std::cout << ' ' << u;
    std::cout << '\n';
}
