#include <iostream>
#include <vector>
#include "data_structures/dynamic_range_kth.hpp"

int main() {
    // 值域是 [0,10)。值很大时先离散化，修改会用到的值也要一起放进去。
    cp::DynamicRangeKth tree(std::vector<int>{5, 2, 6, 3, 7, 4}, 10);
    std::cout << "kth(1,5,0)=" << tree.kth(1, 5, 0) << " kth(1,5,2)=" << tree.kth(1, 5, 2)
              << " count_less(0,6,5)=" << tree.count_less(0, 6, 5) << '\n';
    tree.set(2, 1); // 5 2 1 3 7 4
    std::cout << "after set: kth(1,5,0)=" << tree.kth(1, 5, 0) << " kth(1,5,2)=" << tree.kth(1, 5, 2) << '\n';

    // 在 [0,6) 内查 4 的排名（比它小的个数）、前驱和后继。
    int rank = tree.count_less(0, 6, 4);
    std::cout << "rank(4)=" << rank << " prev=" << tree.kth(0, 6, rank - 1)
              << " next=" << tree.kth(0, 6, tree.count_less(0, 6, 5)) << '\n';
}
