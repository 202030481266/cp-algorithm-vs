#include <algorithm>
#include <functional>
#include <iostream>
#include <limits>
#include <vector>
#include "data_structures/segment_tree.hpp"

struct Minimum {
    long long operator()(long long a, long long b) const {
        return std::min(a, b);
    }
};

int main() {
    std::vector<long long> a{1, 2, 3, 4};
    cp::SegmentTree<long long, std::plus<long long>> sum(a, 0);
    sum.set(1, 10);
    std::cout << "get(1)=" << sum.get(1) << '\n';
    std::cout << "sum(1,3)=" << sum.prod(1, 3) << '\n';
    std::cout << "total=" << sum.all_prod() << '\n';
    std::cout << "empty=" << sum.prod(2, 2) << '\n';
    std::cout << "max_right=" << sum.max_right(0, [](long long x) {
        return x <= 11;
    }) << '\n';

    cp::SegmentTree<long long, Minimum> minimum(
        a, std::numeric_limits<long long>::max());
    std::cout << "min(1,4)=" << minimum.prod(1, 4) << '\n';
}
