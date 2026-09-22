#include <iostream>
#include <vector>
#include "dp/sequence.hpp"

int main() {
    std::vector<int> a{3, 1, 2, 2, 4};
    std::cout << "strict_lis=" << cp::lis_length(a) << '\n';
    std::cout << "nondecreasing=" << cp::lis_length(a, false) << '\n';
    std::cout << "inversions=" << cp::inversion_count(a) << '\n';
    std::cout << "window_min:";
    for (int value : cp::sliding_min(a, 3)) std::cout << ' ' << value;
    std::cout << '\n';
    std::cout << "oversized_window=" << cp::sliding_min(a, 6).size() << '\n';
}
