#include <iostream>
#include <vector>
#include "math/modint.hpp"
#include "math/permutation.hpp"

int main() {
    // 0,1,2 的排列按字典序：012 021 102 120 201 210，201 的排名是 4（从 0 开始）。
    std::vector<int> p{2, 0, 1};
    std::cout << "rank=" << cp::permutation_rank_exact(p) << " rank_mod=" << cp::permutation_rank<cp::Mint>(p) << '\n';
    std::cout << "kth_permutation(3,4):";
    for (int x : cp::kth_permutation(3, 4)) std::cout << ' ' << x;
    std::cout << '\n';

    // 7 个人报数到 3 出列（编号 0..6），出列顺序 2,5,1,6,4,0，最后剩 3。
    std::cout << "josephus(7,3)=" << cp::josephus(7, 3) << " josephus(1e18,2)=" << cp::josephus(1000000000000000000LL, 2) << '\n';
}
