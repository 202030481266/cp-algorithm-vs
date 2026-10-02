#include <algorithm>
#include <iostream>
#include <utility>
#include <vector>
#include "graph/biconnected.hpp"

int main() {
    // 两个三角形 {0,1,2}、{3,4,5} 由桥 2-3 相连，5-6 也是桥，点 7 孤立。
    std::vector<std::pair<int, int>> edges{{0, 1}, {1, 2}, {2, 0}, {2, 3}, {3, 4}, {4, 5}, {5, 3}, {5, 6}};
    cp::Biconnected bc(8, edges);

    std::cout << "cut vertices:";
    for (int v : bc.cut_vertices()) std::cout << ' ' << v;
    std::cout << "\nbridges:";
    for (int id : bc.bridges()) std::cout << ' ' << edges[id].first << '-' << edges[id].second;
    std::cout << "\n2-edge components=" << bc.two_edge_count << ", id:";
    for (int id : bc.two_edge_id) std::cout << ' ' << id;
    std::cout << '\n';

    auto blocks = bc.blocks;
    for (auto& block : blocks) std::sort(block.begin(), block.end());
    std::sort(blocks.begin(), blocks.end());
    std::cout << "blocks:";
    for (auto& block : blocks) {
        std::cout << " {";
        for (int i = 0; i < int(block.size()); ++i) std::cout << (i ? "," : "") << block[i];
        std::cout << '}';
    }
    std::cout << "\nblock-cut tree nodes=" << bc.block_cut_tree().size() << '\n';
}
