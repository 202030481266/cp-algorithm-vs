#include <iostream>
#include <vector>
#include "data_structures/rollback_dsu.hpp"

int main() {
    cp::RollbackDSU dsu(4);
    dsu.merge(0, 1);
    int saved = dsu.snapshot();
    dsu.merge(1, 2);
    std::cout << std::boolalpha << "components=" << dsu.components() << '\n';
    dsu.rollback(saved); // 撤销 merge(1,2)
    std::cout << "after rollback=" << dsu.components() << " same(0,2)=" << dsu.same(0, 2) << '\n';

    // 线段树分治：边只在时间段 [l,r) 内存在，离线求每个时刻的连通块数。
    struct TimedEdge { int u, v; };
    cp::SegmentTreeDivide<TimedEdge> divide(4); // 时刻 0..3
    divide.add(0, 3, {0, 1});
    divide.add(1, 4, {1, 2});
    divide.add(2, 3, {2, 3});
    cp::RollbackDSU graph(4);
    std::vector<int> answer(4);
    divide.run([&] { return graph.snapshot(); },
               [&](const TimedEdge& e) { graph.merge(e.u, e.v); },
               [&](int state) { graph.rollback(state); },
               [&](int t) { answer[t] = graph.components(); });
    std::cout << "components by time:";
    for (int x : answer) std::cout << ' ' << x;
    std::cout << '\n';
}
