#include <cstdint>
#include <iostream>
#include "data_structures/xor_basis.hpp"

int main() {
    cp::XorBasis basis;
    basis.insert(3);
    basis.insert(5);
    std::cout << std::boolalpha;
    std::cout << "independent(6)=" << basis.insert(6) << '\n';
    std::cout << "rank=" << basis.rank() << '\n';
    std::cout << "contains(6)=" << basis.contains(6)
              << " contains(1)=" << basis.contains(1) << '\n';
    std::cout << "maximum=" << basis.max_xor() << '\n';
    std::cout << "sorted:";
    for (std::uint64_t k = 0; k < 4; ++k) {
        if (auto value = basis.kth(k)) std::cout << ' ' << *value;
    }
    std::cout << '\n';
    std::cout << "kth(4)_exists=" << basis.kth(4).has_value() << '\n';
    std::cout << "with_initial_2=" << basis.max_xor(2) << '\n';
}
