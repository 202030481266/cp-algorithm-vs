#include <iostream>
#include <numeric>
#include <string>
#include "string/manacher.hpp"

int main() {
    std::string s = "abba";
    auto result = cp::manacher(s);
    auto [l, r] = result.longest;
    std::cout << "longest=" << s.substr(l, r - l) << " [" << l << ',' << r << ")\n";
    std::cout << "odd_radius(aba,1)=" << cp::manacher("aba").odd[1] << '\n';
    std::cout << "even_radius(abba,2)=" << result.even[2] << '\n';
    long long count = std::accumulate(result.odd.begin(), result.odd.end(), 0LL)
                    + std::accumulate(result.even.begin(), result.even.end(), 0LL);
    std::cout << "palindrome_count=" << count << '\n';
    auto empty = cp::manacher("").longest;
    std::cout << "empty=[" << empty.first << ',' << empty.second << ")\n";
}
