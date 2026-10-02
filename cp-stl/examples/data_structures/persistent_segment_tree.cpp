#include <iostream>
#include <vector>
#include "data_structures/persistent_segment_tree.hpp"

int main() {
    // 可持久化数组：每次修改得到一个新版本，旧版本仍可查询。
    cp::PersistentSegmentTree<long long> tree(5);
    int v0 = tree.build(std::vector<long long>{3, 1, 4, 1, 5});
    int v1 = tree.set(v0, 2, 10); // 3,1,10,1,5
    int v2 = tree.add(v1, 0, -3); // 0,1,10,1,5
    std::cout << "sum: v0=" << tree.sum(v0, 0, 5) << " v1=" << tree.sum(v1, 0, 5)
              << " v2=" << tree.sum(v2, 0, 5) << '\n';
    std::cout << "v0[2]=" << tree.get(v0, 2) << " v2[0]=" << tree.get(v2, 0) << '\n';

    // 静态区间第 k 小（k 从 0 开始）与区间内小于 x 的个数。
    cp::RangeKth<int> kth(std::vector<int>{5, 2, 6, 3, 7, 4});
    std::cout << "kth(1,5,0)=" << kth.kth(1, 5, 0) << " kth(1,5,2)=" << kth.kth(1, 5, 2)
              << " count_less(0,6,5)=" << kth.count_less(0, 6, 5) << '\n';
}
