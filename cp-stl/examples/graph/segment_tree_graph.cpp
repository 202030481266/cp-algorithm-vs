#include <iostream>
#include "graph/segment_tree_graph.hpp"

int main() {
    cp::SegmentTreeGraph stg(5);
    stg.add_edge_to_range(0, 1, 3, 5);         // 0 -> {1,2}，花费 5
    stg.add_edge(0, 3, 20);                    // 0 -> 3，花费 20
    stg.add_edge_from_range(1, 3, 4, 2);       // {1,2} -> 4，花费 2
    stg.add_range_to_range(4, 5, 3, 4, 1);     // {4} -> {3}，花费 1
    auto dist = cp::dijkstra(stg.graph(), 0);  // 前 5 个就是原图点的距离
    std::cout << "dist:";
    for (int v = 0; v < stg.size(); ++v) std::cout << ' ' << dist[v];
    std::cout << "\nnodes=" << stg.node_count() << '\n';
}
