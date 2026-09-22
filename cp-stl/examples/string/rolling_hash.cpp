#include <iostream>
#include <string>
#include "string/rolling_hash.hpp"

int main() {
    std::string s = "banana";
    cp::RollingHash hash(s);
    cp::RollingHash other("ana");
    auto same = [&](int l1, int r1, int l2, int r2) {
        return r1 - l1 == r2 - l2 && hash.get(l1, r1) == hash.get(l2, r2);
    };
    std::cout << std::boolalpha;
    std::cout << "same_ana=" << same(1, 4, 3, 6) << '\n';
    std::cout << "cross_text=" << (hash.get(1, 4) == other.get(0, 3)) << '\n';
    std::cout << "ban_equals_ana=" << same(0, 3, 1, 4) << '\n';
    std::cout << "empty=" << (hash.get(2, 2) == other.get(0, 0)) << '\n';
}
