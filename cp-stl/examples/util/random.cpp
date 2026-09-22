#include <algorithm>
#include <iostream>
#include <vector>
#include "util/random.hpp"

int main() {
    cp::Random first(42), second(42);
    auto a = first.array(8, -3, 3);
    auto b = second.array(8, -3, 3);
    std::cout << std::boolalpha;
    std::cout << "same_seed=" << (a == b) << '\n';
    std::cout << "in_range=" << std::all_of(a.begin(), a.end(), [](long long x) {
        return -3 <= x && x <= 3;
    }) << '\n';
    std::cout << "fixed_interval=" << first.integer(5, 5) << '\n';

    std::vector<int> order{1, 2, 3, 4};
    first.shuffle(order);
    std::sort(order.begin(), order.end());
    std::cout << "same_elements=" << (order == std::vector<int>{1, 2, 3, 4}) << '\n';
}
