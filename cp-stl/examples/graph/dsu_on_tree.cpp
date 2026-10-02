#include <iostream>
#include <vector>
#include "graph/dsu_on_tree.hpp"

int main() {
    // 边：0-1, 0-2, 1-3, 1-4, 2-5；求每棵子树中不同颜色的个数。
    std::vector<std::vector<int>> g{{1, 2}, {0, 3, 4}, {0, 5}, {1}, {1}, {2}};
    std::vector<int> color{1, 2, 1, 3, 2, 1};
    std::vector<int> count(4), answer(6);
    int distinct = 0;
    cp::dsu_on_tree(g, 0,
                    [&](int v) { if (count[color[v]]++ == 0) ++distinct; },
                    [&](int v) { if (--count[color[v]] == 0) --distinct; },
                    [&](int v) { answer[v] = distinct; });
    std::cout << "distinct colors:";
    for (int x : answer) std::cout << ' ' << x;
    std::cout << '\n';
}
