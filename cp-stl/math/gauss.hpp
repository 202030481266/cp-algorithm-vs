#pragma once
// 使用说明：cp-stl/docs/usage/math/gauss.md
// 完整示例：cp-stl/examples/math/gauss.cpp
#include <algorithm>
#include <cassert>
#include <cmath>
#include <cstdint>
#include <utility>
#include <vector>
#include "number_theory.hpp"

namespace cp {
// 线性方程组 A x = b 的求解结果（n >= 1 个方程、m = a[0].size() 个未知数）。
// solvable 为假表示矛盾；为真时 solution 是一组解（自由元取 0），free_variables 列出自由元下标。
// 解唯一当且仅当 solvable 且 free_variables 为空；模 p 时解的个数为 p^自由元个数，异或时为 2^自由元个数。
template<class T>
struct LinearSystem {
    int rank = 0;
    bool solvable = false;
    std::vector<T> solution;
    std::vector<int> free_variables;
};

namespace gauss_detail {
template<class T>
void finish(LinearSystem<T>& result, const std::vector<int>& where, int m, bool solvable, const std::vector<T>& rhs) {
    result.solvable = solvable;
    if (!solvable) return;
    result.solution.assign(m, T());
    for (int col = 0; col < m; ++col) {
        if (where[col] == -1) result.free_variables.push_back(col);
        else result.solution[col] = rhs[where[col]];
    }
}
} // namespace gauss_detail

// 实数高斯-约旦消元，按列选主元，O(n m min(n,m))。|值| <= eps 视为 0。
template<class Real = double>
LinearSystem<Real> solve_linear_real(std::vector<std::vector<Real>> a, std::vector<Real> b, Real eps = Real(1e-9)) {
    assert(!a.empty() && b.size() == a.size()); // 未知数个数取自 a[0].size()，至少要有一个方程
    int n = int(a.size()), m = int(a[0].size());
    std::vector<int> where(m, -1);
    int row = 0;
    for (int col = 0; col < m && row < n; ++col) {
        int pivot = row;
        for (int i = row + 1; i < n; ++i) if (std::abs(a[i][col]) > std::abs(a[pivot][col])) pivot = i;
        if (std::abs(a[pivot][col]) <= eps) continue;
        std::swap(a[pivot], a[row]);
        std::swap(b[pivot], b[row]);
        Real inv = Real(1) / a[row][col];
        for (int j = col; j < m; ++j) a[row][j] *= inv;
        b[row] *= inv;
        for (int i = 0; i < n; ++i) {
            if (i == row || std::abs(a[i][col]) <= eps) continue;
            Real factor = a[i][col];
            for (int j = col; j < m; ++j) a[i][j] -= factor * a[row][j];
            b[i] -= factor * b[row];
        }
        where[col] = row++;
    }
    LinearSystem<Real> result;
    result.rank = row;
    bool solvable = true;
    for (int i = row; i < n; ++i) if (std::abs(b[i]) > eps) solvable = false;
    gauss_detail::finish(result, where, m, solvable, b);
    return result;
}

// 模质数 mod 的高斯-约旦消元，O(n m min(n,m))。要求 mod 为质数且 mod < 2^31，系数可以为负。
inline LinearSystem<long long> solve_linear_mod(std::vector<std::vector<long long>> a, std::vector<long long> b,
                                                long long mod) {
    assert(1 < mod && mod < (1LL << 31));
    assert(!a.empty() && b.size() == a.size());
    int n = int(a.size()), m = int(a[0].size());
    auto normalize = [&](long long& x) { x %= mod; if (x < 0) x += mod; };
    for (int i = 0; i < n; ++i) {
        for (auto& x : a[i]) normalize(x);
        normalize(b[i]);
    }
    std::vector<int> where(m, -1);
    int row = 0;
    for (int col = 0; col < m && row < n; ++col) {
        int pivot = row;
        while (pivot < n && a[pivot][col] == 0) ++pivot;
        if (pivot == n) continue;
        std::swap(a[pivot], a[row]);
        std::swap(b[pivot], b[row]);
        long long inv = pow_mod(a[row][col], mod - 2, mod);
        for (int j = col; j < m; ++j) a[row][j] = a[row][j] * inv % mod;
        b[row] = b[row] * inv % mod;
        for (int i = 0; i < n; ++i) {
            if (i == row || a[i][col] == 0) continue;
            long long factor = a[i][col];
            for (int j = col; j < m; ++j) a[i][j] = (a[i][j] - factor * a[row][j] % mod + mod) % mod;
            b[i] = (b[i] - factor * b[row] % mod + mod) % mod;
        }
        where[col] = row++;
    }
    LinearSystem<long long> result;
    result.rank = row;
    bool solvable = true;
    for (int i = row; i < n; ++i) if (b[i] != 0) solvable = false;
    gauss_detail::finish(result, where, m, solvable, b);
    return result;
}

// 异或方程组（系数与常数都是 0/1，按位异或），每行压成 64 位字，O(n m min(n,m) / 64)。
inline LinearSystem<int> solve_linear_xor(const std::vector<std::vector<int>>& a, const std::vector<int>& b) {
    assert(!a.empty() && b.size() == a.size());
    int n = int(a.size()), m = int(a[0].size()), words = (m + 64) / 64; // 第 m 位放常数项
    std::vector<std::uint64_t> bits(std::size_t(n) * words);
    auto row_ptr = [&](int i) { return bits.data() + std::size_t(i) * words; };
    auto get = [&](int i, int j) { return int(row_ptr(i)[j >> 6] >> (j & 63) & 1); };
    for (int i = 0; i < n; ++i) {
        assert(int(a[i].size()) == m);
        for (int j = 0; j < m; ++j) if (a[i][j] & 1) row_ptr(i)[j >> 6] |= std::uint64_t{1} << (j & 63);
        if (b[i] & 1) row_ptr(i)[m >> 6] |= std::uint64_t{1} << (m & 63);
    }
    std::vector<int> where(m, -1);
    int row = 0;
    for (int col = 0; col < m && row < n; ++col) {
        int pivot = row;
        while (pivot < n && !get(pivot, col)) ++pivot;
        if (pivot == n) continue;
        if (pivot != row) std::swap_ranges(row_ptr(pivot), row_ptr(pivot) + words, row_ptr(row));
        for (int i = 0; i < n; ++i) {
            if (i == row || !get(i, col)) continue;
            std::uint64_t* target = row_ptr(i);
            const std::uint64_t* source = row_ptr(row);
            for (int w = col >> 6; w < words; ++w) target[w] ^= source[w];
        }
        where[col] = row++;
    }
    LinearSystem<int> result;
    result.rank = row;
    bool solvable = true;
    for (int i = row; i < n; ++i) if (get(i, m)) solvable = false;
    std::vector<int> rhs(n);
    for (int i = 0; i < n; ++i) rhs[i] = get(i, m);
    gauss_detail::finish(result, where, m, solvable, rhs);
    return result;
}
} // namespace cp
