#include <iostream>
#include "math/crt.hpp"

int main() {
    auto [g, x, y] = cp::ext_gcd(30, 18); // 30x + 18y = gcd
    std::cout << "gcd=" << g << " x=" << x << " y=" << y << " check=" << 30 * x + 18 * y << '\n';

    auto inverse = cp::solve_congruence(3, 1, 10); // 3x ≡ 1 (mod 10)
    std::cout << "3x=1 (mod 10): x=" << inverse->first << " period=" << inverse->second << '\n';

    auto r = cp::crt({2, 3, 2}, {3, 5, 7}); // 《孙子算经》：x ≡ 2 (mod 3), 3 (mod 5), 2 (mod 7)
    std::cout << "crt: x=" << r->first << " mod " << r->second << '\n';
    auto general = cp::crt({3, 5}, {4, 6}); // 模数不互质
    std::cout << "excrt: x=" << general->first << " mod " << general->second << '\n';
    std::cout << std::boolalpha << "conflict=" << !cp::crt({1, 2}, {4, 6}).has_value() << '\n';

    auto d = cp::solve_diophantine(4, 6, 10); // 4x + 6y = 10
    std::cout << "4x+6y=10: x=" << d->x0 << "+" << d->dx << "k, y=" << d->y0 << "-" << d->dy << "k\n";
}
