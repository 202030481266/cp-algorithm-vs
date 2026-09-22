#include <iostream>
#include <limits>
#include <vector>
#include "basic/sort.hpp"

int main() {
    const std::vector<int> original{3, -2, 0, 3, -2, 1};
    auto ids = cp::counting_sort_ids(original);
    std::cout << "counting_ids:";
    for (auto id : ids) std::cout << ' ' << id;
    std::cout << '\n';
    ids = cp::radix_sort_ids(original);
    std::cout << "radix_ids:";
    for (auto id : ids) std::cout << ' ' << id;
    std::cout << '\n';
    std::cout << "values_by_ids:";
    for (auto id : ids) std::cout << ' ' << original[id];
    std::cout << '\n';
    std::cout << "original:";
    for (int x : original) std::cout << ' ' << x;
    std::cout << '\n';

    std::vector<int> small{3, -2, 0, 3, -2, 1};
    cp::counting_sort(small);
    std::cout << "counting:";
    for (int x : small) std::cout << ' ' << x;
    std::cout << '\n';

    std::vector<long long> wide{170, -45, 75, -90, 0, 170, 802};
    cp::radix_sort(wide);
    std::cout << "radix:";
    for (long long x : wide) std::cout << ' ' << x;
    std::cout << '\n';

    std::vector<long long> edges{
        std::numeric_limits<long long>::max(), 0,
        std::numeric_limits<long long>::min(), -1};
    cp::radix_sort(edges);
    std::cout << "signed:";
    for (long long x : edges) std::cout << ' ' << x;
    std::cout << '\n';

    std::vector<unsigned long long> unsigned_values{
        std::numeric_limits<unsigned long long>::max(), 0, 42, 1};
    cp::radix_sort(unsigned_values);
    std::cout << "unsigned:";
    for (auto x : unsigned_values) std::cout << ' ' << x;
    std::cout << '\n';
}
