#include <iostream>
#include "data_structures/fenwick.hpp"

// 专门用于检查中文路径、空格、头文件搜索和终端输入。
int main() {
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);
    int n;
    if (!(std::cin >> n)) return 0;
    cp::Fenwick<long long> bit(n);
    for (int i = 0; i < n; ++i) {
        long long x;
        std::cin >> x;
        bit.add(i, x);
    }
    std::cout << "总和：" << bit.sum(0, n) << '\n';
}
