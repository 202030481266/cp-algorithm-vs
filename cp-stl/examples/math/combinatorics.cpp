#include <iostream>
#include "math/combinatorics.hpp"

int main() {
    cp::Combinations<cp::Mint> comb(10);
    std::cout << "C(5,2)=" << comb.choose(5, 2) << '\n';
    std::cout << "A(5,2)=" << comb.permute(5, 2) << '\n';
    std::cout << "C(5,0)=" << comb.choose(5, 0) << '\n';
    std::cout << "C(5,7)=" << comb.choose(5, 7) << '\n';
    std::cout << "factorial(5)=" << comb.fact[5] << '\n';
    std::cout << "inverse_check=" << comb.fact[5] * comb.inv_fact[5] << '\n';
}
