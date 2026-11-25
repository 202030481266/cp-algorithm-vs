#include <iostream>
#include <vector>
#include "data_structures/persistent_dsu.hpp"

int main() {
    cp::PersistentDSU dsu(5);
    std::vector<int> version{dsu.initial()};        // version[i]：第 i 次操作之后的版本
    version.push_back(dsu.merge(version[0], 0, 1)); // 1: {0,1}
    version.push_back(dsu.merge(version[1], 2, 3)); // 2: {0,1} {2,3}
    version.push_back(dsu.merge(version[1], 1, 4)); // 3: 从版本 1 分叉，{0,1,4}
    version.push_back(dsu.merge(version[2], 0, 3)); // 4: 接着版本 2，{0,1,2,3}

    std::cout << std::boolalpha;
    std::cout << "v2: same(0,3)=" << dsu.same(version[2], 0, 3) << " size(2)=" << dsu.size(version[2], 2) << '\n';
    std::cout << "v3: same(0,4)=" << dsu.same(version[3], 0, 4) << " same(2,3)=" << dsu.same(version[3], 2, 3) << '\n';
    std::cout << "v4: same(0,3)=" << dsu.same(version[4], 0, 3) << " size(1)=" << dsu.size(version[4], 1) << '\n';
    std::cout << "v0: same(0,1)=" << dsu.same(version[0], 0, 1) << '\n';
}
