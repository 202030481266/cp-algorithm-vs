#include <iostream>
#include <vector>
#include "dp/knapsack.hpp"

int main() {
    const int capacity = 10;
    std::vector<long long> once(capacity + 1), unlimited(capacity + 1), bounded(capacity + 1);
    cp::knapsack01(once, 2, 3);
    cp::knapsack01(once, 3, 4);
    cp::knapsack_unbounded(unlimited, 2, 3);
    cp::knapsack_unbounded(unlimited, 3, 4);
    cp::knapsack_bounded(bounded, 2, 3, 2);
    cp::knapsack_bounded(bounded, 3, 4, 1);
    std::cout << "01=" << once[capacity] << '\n';
    std::cout << "unbounded=" << unlimited[capacity] << '\n';
    std::cout << "bounded=" << bounded[capacity] << '\n';
    std::cout << "bounded_capacity_6=" << bounded[6] << '\n';
}
