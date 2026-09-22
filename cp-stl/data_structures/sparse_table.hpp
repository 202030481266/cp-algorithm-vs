#pragma once
// 使用说明：cp-stl/docs/usage/data_structures/sparse_table.md
// 完整示例：cp-stl/examples/data_structures/sparse_table.cpp
#include <algorithm>
#include <cassert>
#include <numeric>
#include <vector>

namespace cp {
struct Min { template<class T> T operator()(T a, T b) const { return std::min(a, b); } };
struct Max { template<class T> T operator()(T a, T b) const { return std::max(a, b); } };
struct Gcd { template<class T> T operator()(T a, T b) const { return std::gcd(a, b); } };
struct BitAnd { template<class T> T operator()(T a, T b) const { return a & b; } };
struct BitOr { template<class T> T operator()(T a, T b) const { return a | b; } };

// 静态数组；op 必须结合且幂等（min/max/gcd/and/or，可以重叠，不能用 sum）。
// 建表 O(n log n)，query(l,r) 查询非空 [l,r)，O(1)。
template<class T, class Op = Min>
class SparseTable {
    int n_;
    Op op_;
    std::vector<int> log_;
    std::vector<std::vector<T>> table_;
public:
    explicit SparseTable(const std::vector<T>& a = {}, Op op = {})
        : n_(int(a.size())), op_(op), log_(n_ + 1) {
        for (int i = 2; i <= n_; ++i) log_[i] = log_[i / 2] + 1;
        if (!n_) return;
        table_.push_back(a);
        for (int k = 1; k <= log_[n_]; ++k) {
            table_.emplace_back(n_ - (1 << k) + 1);
            for (int i = 0; i + (1 << k) <= n_; ++i)
                table_[k][i] = op_(table_[k - 1][i], table_[k - 1][i + (1 << (k - 1))]);
        }
    }
    T query(int l, int r) const {
        assert(0 <= l && l < r && r <= n_);
        int k = log_[r - l];
        return op_(table_[k][l], table_[k][r - (1 << k)]);
    }
    int size() const { return n_; }
};
} // namespace cp
