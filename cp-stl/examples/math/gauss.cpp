#include <iomanip>
#include <iostream>
#include "math/gauss.hpp"

int main() {
    std::cout << std::boolalpha << std::fixed << std::setprecision(2);
    // 实数：x + y + z = 6，2y + 5z = -4，2x + 5y - z = 27。
    auto real = cp::solve_linear_real<double>({{1, 1, 1}, {0, 2, 5}, {2, 5, -1}}, {6, -4, 27});
    std::cout << "real: unique=" << (real.solvable && real.free_variables.empty()) << " x =";
    for (double v : real.solution) std::cout << ' ' << v;
    std::cout << '\n';

    // 模 7：第二个方程是第一个的 3 倍，秩为 1，有一个自由元，共 7 组解。
    auto mod = cp::solve_linear_mod({{1, 2}, {3, 6}}, {3, 2}, 7);
    std::cout << "mod 7: rank=" << mod.rank << " free=" << mod.free_variables.size()
              << " one solution=" << mod.solution[0] << ',' << mod.solution[1] << '\n';

    // 异或：x0^x1=1, x1^x2=0, x0^x2=0 三式相加得 0=1，矛盾。
    auto bits = cp::solve_linear_xor({{1, 1, 0}, {0, 1, 1}, {1, 0, 1}}, {1, 0, 0});
    std::cout << "xor: solvable=" << bits.solvable << " rank=" << bits.rank << '\n';
}
