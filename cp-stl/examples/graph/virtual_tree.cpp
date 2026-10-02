#include <iostream>
#include <vector>
#include "graph/virtual_tree.hpp"

int main() {
    // 边：0-1, 0-2, 1-3, 1-4, 4-5, 4-6, 2-7，根为 0。
    std::vector<std::vector<int>> g{{1, 2}, {0, 3, 4}, {0, 7}, {1}, {1, 5, 6}, {4}, {4}, {2}};
    cp::FastLCA tree(g, 0);
    auto vt = cp::virtual_tree(tree, {5, 6, 3, 7}); // 关键点可以乱序

    std::cout << "vertices:";
    for (int v : vt.vertices) std::cout << ' ' << v;
    std::cout << "\nedges:";
    for (int i = 1; i < int(vt.vertices.size()); ++i) {
        int child = vt.vertices[i], parent = vt.vertices[vt.parent[i]];
        std::cout << ' ' << parent << '-' << child << "(len " << tree.depth[child] - tree.depth[parent] << ')';
    }
    std::cout << '\n';
}
