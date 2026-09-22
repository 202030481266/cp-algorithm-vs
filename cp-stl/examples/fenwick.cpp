#include <iostream>
#include <vector>
#include "data_structures/fenwick.hpp"

// 输入：n q，n 个数，q 次操作。
// 0 p delta：a[p] += delta；1 l r：输出 [l,r) 的和。
int main() {
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);
    int n, q;
    if (!(std::cin >> n >> q)) return 0;
    std::vector<long long> a(n);
    for (auto& x : a) std::cin >> x;
    cp::Fenwick<long long> bit(a);
    while (q--) {
        int type, l;
        long long r;
        std::cin >> type >> l >> r;
        if (type == 0) bit.add(l, r);
        else std::cout << bit.sum(l, int(r)) << '\n';
    }
}
