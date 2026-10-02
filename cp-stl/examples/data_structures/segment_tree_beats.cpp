#include <iostream>
#include <vector>
#include "data_structures/segment_tree_beats.hpp"

int main() {
    cp::SegmentTreeBeats seg(std::vector<long long>{5, 1, 7, 3, 9});
    seg.chmin(0, 5, 6);  // 5,1,6,3,6
    seg.chmax(1, 4, 4);  // 5,4,6,4,6
    seg.add(2, 5, -1);   // 5,4,5,3,5
    std::cout << "sum=" << seg.sum(0, 5) << " min=" << seg.min(0, 5) << " max=" << seg.max(0, 5) << '\n';
    seg.chmin(0, 5, 4);  // 4,4,4,3,4
    std::cout << "sum=" << seg.sum(0, 5) << " max(0,3)=" << seg.max(0, 3) << '\n';
}
