#include <iostream>
#include "string/kmp.hpp"

int main() {
    std::cout << "pi:";
    for (int length : cp::prefix_function("ababaca")) std::cout << ' ' << length;
    std::cout << '\n';
    std::cout << "matches:";
    for (int start : cp::kmp_find("ababa", "aba")) std::cout << ' ' << start;
    std::cout << '\n';
    std::cout << "empty_pattern:";
    for (int start : cp::kmp_find("ab", "")) std::cout << ' ' << start;
    std::cout << '\n';
}
