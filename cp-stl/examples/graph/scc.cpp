#include <algorithm>
#include <iostream>
#include <vector>
#include "graph/scc.hpp"

int main() {
    // 0<->1，2<->3，另有 1->2、3->4。
    std::vector<std::vector<int>> g{{1}, {0, 2}, {3}, {2, 4}, {}};
    auto scc = cp::strongly_connected_components(g);
    std::cout << "components=" << scc.groups.size() << '\n';
    std::cout << "ids:";
    for (int id : scc.id) std::cout << ' ' << id;
    std::cout << '\n';

    std::vector<std::vector<int>> dag(scc.groups.size());
    for (int u = 0; u < int(g.size()); ++u)
        for (int v : g[u])
            if (scc.id[u] != scc.id[v]) dag[scc.id[u]].push_back(scc.id[v]);
    for (int u = 0; u < int(dag.size()); ++u) {
        auto& edges = dag[u];
        std::sort(edges.begin(), edges.end());
        edges.erase(std::unique(edges.begin(), edges.end()), edges.end());
        for (int v : edges) std::cout << "edge=" << u << "->" << v << '\n';
    }
}
