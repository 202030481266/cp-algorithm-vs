#pragma once
// 使用说明：cp-stl/docs/usage/data_structures/weighted_dsu.md
// 完整示例：cp-stl/examples/data_structures/weighted_dsu.cpp
#include <cassert>
#include <functional>
#include <optional>
#include <utility>
#include <vector>

namespace cp {
// 带权并查集：维护同一集合内两点的势能差 potential[b] - potential[a]。
// T 与 Add/Sub 构成交换群：默认是 + / -；异或关系用 std::bit_xor<> 同时作为 Add 和 Sub。
// 0-based；按大小合并 + 路径压缩，均摊 O(alpha(n))。
template<class T = long long, class Add = std::plus<T>, class Sub = std::minus<T>>
class WeightedDSU {
    std::vector<int> parent_; // 根存负的集合大小。
    std::vector<T> diff_;     // diff_[x] = potential[x] - potential[parent[x]]。
    Add add_;
    Sub sub_;
public:
    explicit WeightedDSU(int n, Add add = {}, Sub sub = {})
        : parent_(n, -1), diff_(n, T()), add_(add), sub_(sub) {}
    int find(int x) {
        assert(0 <= x && x < int(parent_.size()));
        int root = x;
        T total = T();
        while (parent_[root] >= 0) { total = add_(total, diff_[root]); root = parent_[root]; }
        // 第二遍压缩路径：total 始终是当前点到根的势能差。
        while (x != root) {
            int next = parent_[x];
            T own = diff_[x];
            parent_[x] = root;
            diff_[x] = total;
            total = sub_(total, own);
            x = next;
        }
        return root;
    }
    // potential[x] - potential[find(x)]。
    T potential(int x) {
        find(x);
        return parent_[x] < 0 ? T() : diff_[x];
    }
    // 加入约束 potential[b] - potential[a] = w。已在同一集合时检查是否矛盾：矛盾返回 false。
    bool merge(int a, int b, T w) {
        int ra = find(a), rb = find(b);
        T pa = potential(a), pb = potential(b);
        if (ra == rb) return sub_(pb, pa) == w;
        // potential[rb] - potential[ra] = w + pa - pb。
        T root_diff = sub_(add_(w, pa), pb);
        if (parent_[ra] > parent_[rb]) { // 让 ra 成为较大的集合。
            std::swap(ra, rb);
            root_diff = sub_(T(), root_diff);
        }
        parent_[ra] += parent_[rb];
        parent_[rb] = ra;
        diff_[rb] = root_diff;
        return true;
    }
    // potential[b] - potential[a]；不在同一集合时返回 nullopt。
    std::optional<T> diff(int a, int b) {
        if (find(a) != find(b)) return std::nullopt;
        return sub_(potential(b), potential(a));
    }
    bool same(int a, int b) { return find(a) == find(b); }
    int size(int x) { return -parent_[find(x)]; }
};
} // namespace cp
