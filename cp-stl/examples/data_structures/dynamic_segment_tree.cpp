#include <iostream>
#include "data_structures/dynamic_segment_tree.hpp"

int main() {
    cp::DynamicSegmentTree tree(0, 1000000000); // 下标 [0,1e9)，初始全 0
    tree.add(100, 200, 5);
    tree.add(150, 1000000000, 2);
    std::cout << "sum=" << tree.sum(0, 1000000000) << '\n';
    std::cout << "max[0,1000)=" << tree.max(0, 1000) << " max[200,300)=" << tree.max(200, 300)
              << " all_max=" << tree.all_max() << '\n';
    std::cout << "small_tree=" << (tree.node_count() < 200 ? "yes" : "no") << '\n';
}
