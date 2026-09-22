#include <iostream>
#include <vector>
#include "basic/compress.hpp"

int main() {
    std::vector<long long> a{100, -5, 100, 20};
    cp::Compressor<long long> c(a);
    std::cout << "distinct=" << c.size() << '\n';
    std::cout << "indices:";
    for (auto x : a) std::cout << ' ' << c.index(x);
    std::cout << '\n';
    std::cout << "lower_bound(21)=" << c.lower_bound(21) << '\n';
    std::cout << "lower_bound(101)=" << c.lower_bound(101) << '\n';
    std::cout << "original_at_1=" << c.values[1] << '\n';
}
