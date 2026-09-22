#include <iostream>
#include <sstream>
#include "math/modint.hpp"

int main() {
    using M = cp::ModInt<13>;
    M a = -2, b = 5; // a 被规范为 11
    std::cout << "a=" << a.val() << " mod=" << M::mod() << '\n';
    std::cout << "sum=" << a + b << " difference=" << a - b << '\n';
    std::cout << "product=" << a * b << " quotient=" << a / b << '\n';
    std::cout << "power=" << b.pow(3) << " inverse=" << b.inv() << '\n';
    M value;
    std::istringstream input("-1");
    input >> value;
    std::cout << std::boolalpha << "read=" << value
              << " equal=" << (value == M(12)) << '\n';
    std::cout << "default_mod=" << cp::Mint::mod() << '\n';
}
