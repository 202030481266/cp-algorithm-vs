#include <algorithm>
#include <iostream>
#include <vector>
#include "graph/shortest_path.hpp"

int main() {
    std::vector<std::vector<int>> unweighted{{1, 2}, {3}, {1, 3}, {}, {}};
    std::cout << "bfs:";
    for (int d : cp::bfs(unweighted, 0)) std::cout << ' ' << d;
    std::cout << '\n';

    cp::WeightedGraph g(5);
    g[0] = {{1, 4}, {2, 1}};
    g[1] = {{3, 1}};
    g[2] = {{1, 2}, {3, 5}};
    std::cout << "dijkstra:";
    for (auto d : cp::dijkstra(g, 0)) {
        if (d == cp::INF64) std::cout << " INF";
        else std::cout << ' ' << d;
    }
    std::cout << '\n';

    cp::WeightedGraph binary(4);
    binary[0] = {{1, 1}, {2, 0}};
    binary[2] = {{1, 0}, {3, 1}};
    binary[1] = {{3, 1}};
    std::cout << "01bfs:";
    for (auto d : cp::zero_one_bfs(binary, 0)) std::cout << ' ' << d;
    std::cout << '\n';

    std::vector<std::vector<long long>> dist(5, std::vector<long long>(5, cp::INF64));
    for (int u = 0; u < 5; ++u) {
        dist[u][u] = 0;
        for (auto [v, w] : g[u]) dist[u][v] = std::min(dist[u][v], w);
    }
    cp::floyd_warshall(dist);
    std::cout << "floyd(0,3)=" << dist[0][3] << '\n';
}
