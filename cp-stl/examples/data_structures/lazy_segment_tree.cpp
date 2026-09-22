#include <iostream>
#include <vector>
#include "data_structures/lazy_segment_tree.hpp"

int main() {
    cp::RangeAddSum seg(std::vector<long long>{1, 2, 3, 4});
    std::cout << "before=" << seg.sum(0, 4) << '\n';
    seg.add(1, 4, 5); // 1,7,8,9
    std::cout << "sum(1,3)=" << seg.sum(1, 3) << '\n';
    seg.add(0, 2, -2); // -1,5,8,9
    std::cout << "after=" << seg.sum(0, 4) << '\n';
    seg.add(2, 2, 100);
    std::cout << "empty=" << seg.sum(2, 2) << '\n';
}
