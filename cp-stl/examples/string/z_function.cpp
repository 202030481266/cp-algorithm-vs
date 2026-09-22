#include <iostream>
#include <string>
#include "string/z_function.hpp"

int main() {
    std::string text = "abacaba";
    std::cout << "z:";
    for (int length : cp::z_function(text)) std::cout << ' ' << length;
    std::cout << '\n';

    std::string pattern = "aba";
    auto z = cp::z_function(pattern + "#" + text); // '#' 不在两者中
    int offset = int(pattern.size()) + 1;
    std::cout << "matches:";
    for (int i = offset; i < int(z.size()); ++i)
        if (z[i] >= int(pattern.size())) std::cout << ' ' << i - offset;
    std::cout << '\n';
}
