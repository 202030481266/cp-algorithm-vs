#include <iostream>
#include <vector>
#include "data_structures/range_fenwick.hpp"

int main() {
    cp::RangeFenwick<long long> bit(std::vector<long long>{1, 2, 3, 4, 5});
    bit.add(1, 4, 10); // 1,12,13,14,5
    std::cout << "sum(0,5)=" << bit.sum(0, 5) << " sum(2,4)=" << bit.sum(2, 4)
              << " get(2)=" << bit.get(2) << '\n';

    cp::Fenwick2D<long long> grid(3, 4); // 3 行 4 列，单点加
    grid.add(0, 0, 5);
    grid.add(2, 3, 7);
    std::cout << "grid=" << grid.sum(0, 0, 3, 4) << ' ' << grid.sum(1, 1, 3, 4) << '\n';

    cp::RangeFenwick2D<long long> board(3, 3); // 子矩形加、子矩形和
    board.add(0, 0, 2, 2, 1);                   // 左上 2x2 每格加 1
    board.add(1, 1, 3, 3, 2);                   // 右下 2x2 每格加 2
    std::cout << "board=" << board.sum(0, 0, 3, 3) << " cell(1,1)=" << board.sum(1, 1, 2, 2) << '\n';
}
