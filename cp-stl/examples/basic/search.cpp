#include <iostream>
#include "basic/search.hpp"

int main() {
    auto check = [](long long x) { return x * x >= 10; };
    std::cout << "first=" << cp::first_true(0, 11, check) << '\n';
    std::cout << "missing=" << cp::first_true(0, 11, [](long long x) {
        return x >= 20;
    }) << '\n';
    std::cout << "empty=" << cp::first_true(5, 5, check) << '\n';
    std::cout << "floor=" << cp::floor_div(-7, 3)
              << " ceil=" << cp::ceil_div(-7, 3) << '\n';
}
