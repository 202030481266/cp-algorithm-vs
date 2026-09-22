// Usage: gen.exe SEED. Always reproduce the same input for the same seed.
#include <cstdint>
#include <iostream>
#include <random>
#include <string>

int main(int argc, char** argv) {
    if (argc != 2) return 1;
    const auto seed = std::stoull(argv[1]);
    std::mt19937_64 rng(seed);
    const int mode = static_cast<int>(seed % 6);
    const int n = mode == 0 ? 1 : 1 + static_cast<int>(rng() % 30);
    std::cout << n << '\n';
    for (int i = 0; i < n; ++i) {
        long long value = static_cast<long long>(rng() % 201) - 100;
        if (mode == 1) value = -1 - static_cast<long long>(rng() % 100);
        if (mode == 2) value = 0;
        if (mode == 3) value = 1 + static_cast<long long>(rng() % 100);
        if (mode == 4) value = (i % 2 ? -1 : 1) * 1000000000LL;
        std::cout << value << (i + 1 == n ? '\n' : ' ');
    }
}
