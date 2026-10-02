#include <functional>
#include <iostream>
#include "data_structures/weighted_dsu.hpp"

int main() {
    // 前缀和 S[0..5]：已知区间和 [l,r) 就是约束 S[r] - S[l] = sum。
    cp::WeightedDSU<long long> dsu(6);
    dsu.merge(0, 3, 10); // a0+a1+a2 = 10
    dsu.merge(3, 5, 7);  // a3+a4 = 7
    dsu.merge(2, 5, 9);  // a2+a3+a4 = 9
    std::cout << std::boolalpha;
    std::cout << "sum[0,2)=" << *dsu.diff(0, 2) << " sum[0,5)=" << *dsu.diff(0, 5) << '\n';
    std::cout << "sum[0,1)_known=" << dsu.diff(0, 1).has_value() << '\n';
    std::cout << "consistent=" << dsu.merge(0, 5, 17) << " conflict=" << !dsu.merge(0, 5, 100) << '\n';

    // 异或关系：Add 与 Sub 都用异或。
    cp::WeightedDSU<int, std::bit_xor<int>, std::bit_xor<int>> parity(3);
    parity.merge(0, 1, 1); // x0 != x1
    parity.merge(1, 2, 1); // x1 != x2
    std::cout << "x0^x2=" << *parity.diff(0, 2) << '\n';
}
