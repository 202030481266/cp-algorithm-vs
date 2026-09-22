#include <iostream>
#include <vector>
#include "data_structures/fenwick.hpp"

int main() {
    cp::Fenwick<long long> bit(std::vector<long long>{1, 2, 3, 4, 5});
    std::cout << "sum(1,4)=" << bit.sum(1, 4) << '\n';
    bit.add(2, 10); // a 变成 1,2,13,4,5
    std::cout << "prefix(3)=" << bit.prefix(3) << '\n';
    std::cout << "total=" << bit.sum(0, 5) << '\n';
    std::cout << "empty=" << bit.sum(2, 2) << '\n';

    // 下标 0/1/2 分别出现 2/0/3 次。
    cp::Fenwick<long long> counts(std::vector<long long>{2, 0, 3});
    std::cout << "third_index=" << counts.lower_bound(3) << '\n';
    std::cout << "missing_index=" << counts.lower_bound(6) << '\n';
}
