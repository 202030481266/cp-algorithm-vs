#include <cstdint>
#include <iostream>
#include "math/prime.hpp"

int main() {
    std::cout << std::boolalpha;
    std::cout << "is_prime(998244353)=" << cp::is_prime(998244353)
              << " is_prime(561)=" << cp::is_prime(561) // 最小的 Carmichael 数
              << " is_prime(2^61-1)=" << cp::is_prime((1ULL << 61) - 1) << '\n';

    std::uint64_t n = 600851475143ULL;
    std::cout << "factorize(" << n << "):";
    for (auto p : cp::factorize(n)) std::cout << ' ' << p;
    std::cout << '\n';

    std::uint64_t big = 1000000007ULL * 1000000007ULL * 6; // 约 6e18
    std::cout << "prime_factors(" << big << "):";
    for (auto [p, e] : cp::prime_factors(big)) std::cout << ' ' << p << '^' << e;
    std::cout << "\ndivisors(36):";
    for (auto d : cp::divisors(36)) std::cout << ' ' << d;
    std::cout << "\neuler_phi(36)=" << cp::euler_phi(36) << '\n';
}
