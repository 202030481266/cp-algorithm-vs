#pragma once
// 使用说明：cp-stl/docs/usage/basic/prefix_sum.md
// 完整示例：cp-stl/examples/basic/prefix_sum.cpp
#include <cassert>
#include <vector>

namespace cp {
// 二维前缀和：静态矩阵的子矩形和，预处理 O(nm)，查询 O(1)。矩形为 [r1,r2) x [c1,c2)。
template<class T = long long>
class PrefixSum2D {
    int rows_, cols_;
    std::vector<T> s_; // (rows+1)*(cols+1)，s_[i][j] = [0,i) x [0,j) 的和
    T& at(int i, int j) { return s_[std::size_t(i) * (cols_ + 1) + j]; }
    const T& at(int i, int j) const { return s_[std::size_t(i) * (cols_ + 1) + j]; }
public:
    template<class U>
    explicit PrefixSum2D(const std::vector<std::vector<U>>& a)
        : rows_(int(a.size())), cols_(a.empty() ? 0 : int(a[0].size())),
          s_(std::size_t(rows_ + 1) * (cols_ + 1), T()) {
        for (int i = 0; i < rows_; ++i) {
            assert(int(a[i].size()) == cols_);
            for (int j = 0; j < cols_; ++j)
                at(i + 1, j + 1) = at(i, j + 1) + at(i + 1, j) - at(i, j) + T(a[i][j]);
        }
    }
    T sum(int r1, int c1, int r2, int c2) const {
        assert(0 <= r1 && r1 <= r2 && r2 <= rows_ && 0 <= c1 && c1 <= c2 && c2 <= cols_);
        return at(r2, c2) - at(r1, c2) - at(r2, c1) + at(r1, c1);
    }
};

// 二维差分：离线做很多次子矩形加，最后一次性 build 出整个矩阵。每次 add O(1)，build O(nm)。
template<class T = long long>
class Difference2D {
    int rows_, cols_;
    std::vector<T> d_; // (rows+1)*(cols+1)
    T& at(int i, int j) { return d_[std::size_t(i) * (cols_ + 1) + j]; }
public:
    Difference2D(int rows, int cols) : rows_(rows), cols_(cols), d_(std::size_t(rows + 1) * (cols + 1), T()) {}
    // [r1,r2) x [c1,c2) 每格加 delta。
    void add(int r1, int c1, int r2, int c2, T delta) {
        assert(0 <= r1 && r1 <= r2 && r2 <= rows_ && 0 <= c1 && c1 <= c2 && c2 <= cols_);
        at(r1, c1) += delta;
        at(r1, c2) -= delta;
        at(r2, c1) -= delta;
        at(r2, c2) += delta;
    }
    std::vector<std::vector<T>> build() const {
        std::vector<std::vector<T>> a(rows_, std::vector<T>(cols_, T()));
        for (int i = 0; i < rows_; ++i)
            for (int j = 0; j < cols_; ++j) {
                a[i][j] = d_[std::size_t(i) * (cols_ + 1) + j];
                if (i) a[i][j] += a[i - 1][j];
                if (j) a[i][j] += a[i][j - 1];
                if (i && j) a[i][j] -= a[i - 1][j - 1];
            }
        return a;
    }
};

// 一维等差数列差分（二阶差分）：离线给 [l,r) 依次加上 first, first+step, first+2*step, ...，最后 build。
// 每次 add O(1)，build O(n)。常数数列取 step = 0 即普通差分。
template<class T = long long>
class ArithmeticDifference {
    int n_;
    std::vector<T> e_; // 二阶差分，长度 n+2
public:
    explicit ArithmeticDifference(int n) : n_(n), e_(n + 2, T()) {}
    void add(int l, int r, T first, T step) {
        assert(0 <= l && l <= r && r <= n_);
        if (l == r) return;
        T last = first + step * T(r - l - 1);
        e_[l] += first;
        e_[l + 1] += step - first;
        e_[r] -= step + last;
        e_[r + 1] += last;
    }
    std::vector<T> build() const {
        std::vector<T> a(n_);
        T diff = T(), value = T();
        for (int i = 0; i < n_; ++i) {
            diff += e_[i];
            value += diff;
            a[i] = value;
        }
        return a;
    }
};
} // namespace cp
