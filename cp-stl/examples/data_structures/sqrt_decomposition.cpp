#include <iostream>
#include <vector>
#include "data_structures/sqrt_decomposition.hpp"

int main() {
    // 现成的结构：区间加、区间内小于 x 的个数。
    cp::RangeAddCountLess<long long> blocks(std::vector<long long>{5, 1, 4, 2, 8, 7, 3, 6, 9});
    std::cout << "less than 5 in [0,9): " << blocks.count_less(0, 9, 5) << '\n';
    blocks.add(2, 7, 10); // 5 1 14 12 18 17 13 6 9
    std::cout << "less than 13 in [1,8): " << blocks.count_less(1, 8, 13) << " a[4]=" << blocks.get(4) << '\n';

    // 自己写分块：SqrtBlocks 负责把区间拆成零散部分和整块。这里维护区间加、区间和。
    std::vector<long long> a{1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    cp::SqrtBlocks layout(int(a.size()));
    std::vector<long long> block_sum(layout.block_count), block_add(layout.block_count);
    for (int i = 0; i < layout.n; ++i) block_sum[layout.id(i)] += a[i];
    auto add = [&](int l, int r, long long delta) {
        layout.for_each(l, r,
            [&](int b, int lo, int hi) { for (int i = lo; i < hi; ++i) a[i] += delta; block_sum[b] += delta * (hi - lo); },
            [&](int b) { block_add[b] += delta; });
    };
    auto sum = [&](int l, int r) {
        long long total = 0;
        layout.for_each(l, r,
            [&](int b, int lo, int hi) { for (int i = lo; i < hi; ++i) total += a[i] + block_add[b]; },
            [&](int b) { total += block_sum[b] + block_add[b] * (layout.end(b) - layout.begin(b)); });
        return total;
    };
    add(1, 9, 100);
    std::cout << "blocks=" << layout.block_count << " size=" << layout.block_size
              << " sum[0,10)=" << sum(0, 10) << " sum[2,5)=" << sum(2, 5) << '\n';
}
