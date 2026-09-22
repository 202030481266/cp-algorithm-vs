#include <iostream>
#include "math/matrix.hpp"
#include "math/modint.hpp"

int main() {
    cp::Matrix<cp::Mint> a(2);
    a[0][0] = a[0][1] = a[1][0] = 1;
    auto squared = a * a;
    auto identity = a.pow(0);
    std::cout << "F10=" << a.pow(10)[0][1] << '\n';
    std::cout << "A2: " << squared[0][0] << ' ' << squared[0][1]
              << " / " << squared[1][0] << ' ' << squared[1][1] << '\n';
    std::cout << "I: " << identity[0][0] << ' ' << identity[0][1]
              << " / " << identity[1][0] << ' ' << identity[1][1] << '\n';
}
