#include <iostream>
#include <numeric>
#include <string>
#include <tuple>
#include <vector>
#include "util/debug.hpp"

int main() {
    std::vector<int> a{1, 2, 3};
    long long sum = std::accumulate(a.begin(), a.end(), 0LL);
    debug(a, sum);
    debug(std::make_tuple(7, std::string("ok")));
    std::cout << "answer=" << sum << '\n';
}
