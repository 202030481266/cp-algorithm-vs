#include <iostream>
#include "data_structures/segment_tree_merge.hpp"

int main() {
    cp::MergeableSegmentTrees pool(10); // 所有树的值域都是 [0,10)
    int a = 0, b = 0;                   // 0 表示空树
    pool.add(a, 3, 2);                  // a = {3,3}
    pool.add(a, 7, 1);                  // a = {3,3,7}
    pool.add(b, 5, 4);                  // b = {5,5,5,5}
    a = pool.merge(a, b);               // a = {3,3,5,5,5,5,7}，b 不再单独使用
    std::cout << "total=" << pool.total(a) << " kth(2)=" << pool.kth(a, 2)
              << " mode=" << pool.max_pos(a) << " mode_count=" << pool.max_count(a) << '\n';

    int c = pool.split(a, 4, 10); // c 取走值在 [4,10) 的部分
    std::cout << "a=" << pool.total(a) << " c=" << pool.total(c)
              << " c.sum[6,10)=" << pool.sum(c, 6, 10) << '\n';
}
