#include <iostream>
#include "data_structures/dsu.hpp"

int main() {
    cp::DSU dsu(5);
    dsu.merge(0, 1);
    dsu.merge(1, 2);
    std::cout << std::boolalpha;
    std::cout << "same(0,2)=" << dsu.same(0, 2) << '\n';
    std::cout << "same(0,4)=" << dsu.same(0, 4) << '\n';
    std::cout << "size(1)=" << dsu.size(1) << '\n';
    std::cout << "components=" << dsu.components << '\n';
    std::cout << "merge_again=" << dsu.merge(0, 2) << '\n';
}
