#include <iostream>
#include <vector>
#include "graph/hld.hpp"
#include "data_structures/lazy_segment_tree.hpp"

int main() {
    std::vector<std::vector<int>> g{{1, 2}, {0, 3, 4}, {0}, {1}, {1}};
    std::vector<long long> value{1, 2, 3, 4, 5};
    cp::HLD hld(g, 0);
    std::vector<long long> linear(value.size());
    for (int u = 0; u < int(value.size()); ++u) linear[hld.pos[u]] = value[u];
    cp::RangeAddSum seg(linear);

    auto path_sum = [&](int u, int v) {
        long long answer = 0;
        hld.for_each_path(u, v, [&](int l, int r) { answer += seg.sum(l, r); });
        return answer;
    };
    std::cout << "before=" << path_sum(3, 4) << '\n'; // 点 3、1、4
    hld.for_each_path(3, 2, [&](int l, int r) { seg.add(l, r, 10); });
    std::cout << "after=" << path_sum(3, 4) << '\n';
    auto [l, r] = hld.subtree(1);
    std::cout << "subtree(1)=" << seg.sum(l, r) << '\n';
}
