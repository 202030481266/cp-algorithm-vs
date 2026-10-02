#include <iostream>
#include <vector>
#include "basic/prefix_sum.hpp"

int main() {
    std::vector<std::vector<int>> grid{{1, 2, 3}, {4, 5, 6}, {7, 8, 9}};
    cp::PrefixSum2D<long long> ps(grid);
    std::cout << "sum all=" << ps.sum(0, 0, 3, 3) << " lower-right 2x2=" << ps.sum(1, 1, 3, 3) << '\n';

    cp::Difference2D<long long> diff(3, 4); // 3 行 4 列，离线子矩形加
    diff.add(0, 0, 2, 2, 1);
    diff.add(1, 1, 3, 4, 10);
    for (const auto& row : diff.build()) {
        for (int j = 0; j < int(row.size()); ++j) std::cout << row[j] << (j + 1 < int(row.size()) ? ' ' : '\n');
    }

    cp::ArithmeticDifference<long long> wave(6);
    wave.add(1, 5, 1, 2);  // 下标 1..4 依次加 1,3,5,7
    wave.add(0, 6, 10, 0); // 全部加 10
    std::cout << "arithmetic:";
    for (long long x : wave.build()) std::cout << ' ' << x;
    std::cout << '\n';
}
