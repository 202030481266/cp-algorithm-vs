#pragma once
// 使用说明：cp-stl/docs/usage/basic/cdq.md
// 完整示例：cp-stl/examples/basic/cdq.cpp
#include <algorithm>
#include <array>
#include <cassert>
#include <numeric>
#include <vector>
#include "../data_structures/fenwick.hpp"

namespace cp {
// CDQ 分治骨架：处理下标区间 [l,r)。先递归左半，再调用 cross(l, mid, r) 计算左半 [l,mid) 对
// 右半 [mid,r) 的贡献，最后递归右半。右半开始递归时，来自左边的贡献已经全部算完，
// 所以也适用于“先算出前面的 dp 才能推后面”的转移。cross 共被调用 O(n) 次，区间总长 O(n log n)。
template<class Cross>
void cdq_divide(int l, int r, Cross&& cross) {
    if (r - l <= 1) return;
    int mid = l + (r - l) / 2;
    cdq_divide(l, mid, cross);
    cross(l, mid, r);
    cdq_divide(mid, r, cross);
}

// 三维偏序：对每个点 i，统计 j != i 且三个坐标都不超过点 i 的点数（完全相同的点互相计入）。
// 第一维排序、第二维归并、第三维树状数组，O(n log² n)。坐标可以是任意 int。
inline std::vector<int> dominance_count_3d(const std::vector<std::array<int, 3>>& points) {
    int n = int(points.size());
    std::vector<int> order(n), group(n), cs(n);
    std::iota(order.begin(), order.end(), 0);
    std::sort(order.begin(), order.end(), [&](int i, int j) { return points[i] < points[j]; });
    for (int i = 0; i < n; ++i) cs[i] = points[i][2];
    std::sort(cs.begin(), cs.end());
    cs.erase(std::unique(cs.begin(), cs.end()), cs.end());
    struct Item { int b, c, weight, id; }; // 相同的点合并成一个带重数的点
    std::vector<Item> items;
    for (int k = 0; k < n; ++k) {
        const auto& p = points[order[k]];
        if (k > 0 && p == points[order[k - 1]]) ++items.back().weight;
        else items.push_back({p[1], int(std::lower_bound(cs.begin(), cs.end(), p[2]) - cs.begin()), 1, int(items.size())});
        group[order[k]] = int(items.size()) - 1;
    }
    int m = int(items.size());
    std::vector<int> weight(m), below(m);
    for (const Item& item : items) weight[item.id] = item.weight;
    std::vector<Item> buffer(m);
    Fenwick<int> bit(int(cs.size()));
    auto by_b = [](const Item& x, const Item& y) { return x.b < y.b; };
    // 进入时 [l,r) 按第一维有序，返回时按第二维有序。
    auto solve = [&](auto&& self, int l, int r) -> void {
        if (r - l <= 1) return;
        int mid = l + (r - l) / 2;
        self(self, l, mid);
        self(self, mid, r);
        int i = l;
        for (int j = mid; j < r; ++j) {
            for (; i < mid && items[i].b <= items[j].b; ++i) bit.add(items[i].c, items[i].weight);
            below[items[j].id] += bit.prefix(items[j].c + 1);
        }
        for (int k = l; k < i; ++k) bit.add(items[k].c, -items[k].weight);
        std::merge(items.begin() + l, items.begin() + mid, items.begin() + mid, items.begin() + r,
                   buffer.begin() + l, by_b);
        std::copy(buffer.begin() + l, buffer.begin() + r, items.begin() + l);
    };
    solve(solve, 0, m);
    std::vector<int> result(n);
    for (int i = 0; i < n; ++i) result[i] = below[group[i]] + weight[group[i]] - 1;
    return result;
}
} // namespace cp
