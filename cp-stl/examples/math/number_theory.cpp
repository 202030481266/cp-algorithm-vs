#include <iostream>
#include "math/number_theory.hpp"

int main() {
    std::cout << "power=" << cp::pow_mod(2, 10, 1000) << '\n';
    if (auto inverse = cp::inverse_mod(3, 11))
        std::cout << "inverse=" << *inverse << '\n';
    std::cout << std::boolalpha;
    std::cout << "inverse(6,9)_exists=" << cp::inverse_mod(6, 9).has_value() << '\n';

    cp::PrimeSieve sieve(20);
    std::cout << "primes:";
    for (int p : sieve.primes) std::cout << ' ' << p;
    std::cout << '\n';
    std::cout << "prime(19)=" << sieve.is_prime(19) << '\n';
    std::cout << "factor(18):";
    for (auto [p, exponent] : sieve.factorize(18))
        std::cout << ' ' << p << '^' << exponent;
    std::cout << '\n';
    std::cout << "factor(1)_size=" << sieve.factorize(1).size() << '\n';
}
