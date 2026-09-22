#include <cstdint>
#include <iostream>
#include <unordered_map>
#include <unordered_set>
#include "util/hash.hpp"

int main() {
    std::unordered_map<long long, int, cp::SafeHash> count;
    count.max_load_factor(0.7f);
    count.reserve(16);
    for (long long x : {10LL, -7LL, 10LL}) ++count[x];
    std::cout << "distinct=" << count.size() << '\n';
    std::cout << "count(10)=" << count.at(10) << '\n';
    std::cout << "count(-7)=" << count.at(-7) << '\n';
    std::cout << std::boolalpha << "missing=" << (count.find(42) == count.end()) << '\n';

    std::unordered_set<std::uint64_t, cp::SafeHash> seen;
    seen.insert(std::uint64_t{1} << 63);
    std::cout << "highest_bit=" << seen.count(std::uint64_t{1} << 63) << '\n';
}
