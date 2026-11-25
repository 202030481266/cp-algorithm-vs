#include <iostream>
#include <vector>
#include "graph/residue_shortest_path.hpp"

int main() {
    // 只用 5、7、11 三种面值（数量不限）能恰好凑出哪些金额。
    std::vector<long long> dist = cp::residue_distances({5, 7, 11});
    std::cout << "dist:"; // dist[r]：模 5 余 r 的金额里最小的可凑出的那个
    for (long long d : dist) std::cout << ' ' << d;
    std::cout << '\n';
    for (long long x : {13, 23, 24}) std::cout << x << (x >= dist[x % 5] ? ": yes" : ": no") << '\n';

    // 1..100 中能凑出的个数（去掉 0）；范围到 1e18 也是 O(base)。
    std::cout << "count[1,100]=" << cp::count_representable(dist, 100) - 1 << '\n';
    std::cout << "count[1,1e18]=" << cp::count_representable(dist, 1000000000000000000LL) - 1 << '\n';
}
