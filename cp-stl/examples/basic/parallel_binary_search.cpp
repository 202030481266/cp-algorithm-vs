#include <algorithm>
#include <iostream>
#include <numeric>
#include <vector>
#include "basic/parallel_binary_search.hpp"
#include "data_structures/fenwick.hpp"

int main() {
    // 离线区间第 k 小：把位置按值从小到大依次“点亮”，
    // 询问 (l, r, k) 的答案就是 [l,r) 内点亮的个数第一次达到 k 时点亮的那个值。
    std::vector<int> a{5, 2, 6, 3, 7, 4};
    struct Query { int l, r, k; }; // [l,r) 中第 k 小，k 从 1 开始
    std::vector<Query> queries{{1, 5, 1}, {1, 5, 3}, {0, 6, 4}, {2, 3, 1}};

    int n = int(a.size());
    std::vector<int> order(n); // 第 t 个操作：点亮位置 order[t]
    std::iota(order.begin(), order.end(), 0);
    std::sort(order.begin(), order.end(), [&](int i, int j) { return a[i] < a[j]; });

    cp::Fenwick<int> lit(n);
    std::vector<int> answer = cp::parallel_binary_search(n, int(queries.size()),
        [&] { lit = cp::Fenwick<int>(n); },
        [&](int t) { lit.add(order[t], 1); },
        [&](int i) { return lit.sum(queries[i].l, queries[i].r) >= queries[i].k; });

    // answer[i] 是需要执行的操作个数，最后执行的是第 answer[i] - 1 个操作。
    for (int i = 0; i < int(queries.size()); ++i)
        std::cout << "kth(" << queries[i].l << ',' << queries[i].r << ',' << queries[i].k << ")="
                  << a[order[answer[i] - 1]] << '\n';
}
