// Usage: gen.exe SEED. Always reproduce the same input for the same seed.
#include <iostream>
#include <string>
#include <vector>
#include "util/random.hpp"

int main(int argc, char** argv) {
    if (argc != 2) return 1;
    const auto seed = std::stoull(argv[1]);
    cp::Random rnd(seed);
    const int mode = static_cast<int>(seed % 6);
    const int n = mode == 0 ? 1 : static_cast<int>(rnd.integer(1, 30));
    std::vector<long long> a = rnd.array(n, -100, 100);
    if (mode == 1) a = rnd.array(n, -100, -1);
    if (mode == 2) a.assign(n, 0);
    if (mode == 3) a = rnd.array(n, 1, 100);
    if (mode == 4) for (int i = 0; i < n; ++i) a[i] = (i % 2 ? -1 : 1) * 1000000000LL;
    std::cout << n << '\n';
    for (int i = 0; i < n; ++i) std::cout << a[i] << (i + 1 == n ? '\n' : ' ');
}
