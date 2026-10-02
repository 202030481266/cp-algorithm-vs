#include <iostream>
#include <vector>
#include "graph/spfa.hpp"

int main() {
    cp::WeightedGraph g(4);
    g[0].push_back({1, 4});
    g[0].push_back({2, 5});
    g[1].push_back({2, -3}); // 负边
    g[2].push_back({3, 2});
    auto dist = cp::spfa(g, 0);
    std::cout << "dist:";
    for (long long d : *dist) std::cout << ' ' << d;
    std::cout << '\n';

    g[3].push_back({1, -1}); // 1->2->3->1 的总权值为 -2，形成负环
    std::cout << std::boolalpha << "reachable_negative_cycle=" << !cp::spfa(g, 0).has_value()
              << " any_negative_cycle=" << cp::has_negative_cycle(g) << '\n';

    // 差分约束：x1 - x0 <= 3，x2 - x1 <= -2，x0 - x2 <= 1。
    cp::DifferenceConstraints dc(3);
    dc.add_less_equal(0, 1, 3);
    dc.add_less_equal(1, 2, -2);
    dc.add_less_equal(2, 0, 1);
    auto x = dc.solve();
    std::cout << "x:";
    for (long long v : *x) std::cout << ' ' << v;
    std::cout << '\n';

    // Johnson 全源最短路：允许负边，不允许负环。
    cp::WeightedGraph h(3);
    h[0].push_back({1, -2});
    h[1].push_back({2, 3});
    h[2].push_back({0, 1});
    auto all = cp::johnson(h);
    std::cout << "johnson[2][1]=" << (*all)[2][1] << " johnson[0][2]=" << (*all)[0][2] << '\n';
}
