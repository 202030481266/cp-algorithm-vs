#pragma once
// 使用说明：cp-stl/docs/usage/math/matrix.md
// 完整示例：cp-stl/examples/math/matrix.cpp
#include <cassert>
#include <vector>

namespace cp {
// T 支持 0/1 构造、+、*（推荐 ModInt）；只处理方阵。
// 乘法 O(n^3)，快速幂 O(n^3 log exponent)。
template<class T>
struct Matrix {
    int n;
    std::vector<std::vector<T>> a;
    explicit Matrix(int size, bool identity = false) : n(size), a(n, std::vector<T>(n)) {
        if (identity) for (int i = 0; i < n; ++i) a[i][i] = T(1);
    }
    std::vector<T>& operator[](int i) { return a[i]; }
    const std::vector<T>& operator[](int i) const { return a[i]; }
    friend Matrix operator*(const Matrix& x, const Matrix& y) {
        assert(x.n == y.n);
        Matrix result(x.n);
        for (int i = 0; i < x.n; ++i)
            for (int k = 0; k < x.n; ++k)
                for (int j = 0; j < x.n; ++j) result[i][j] += x[i][k] * y[k][j];
        return result;
    }
    Matrix pow(long long exponent) const {
        assert(exponent >= 0);
        Matrix base = *this, result(n, true);
        while (exponent) {
            if (exponent & 1) result = result * base;
            exponent >>= 1;
            if (exponent) base = base * base; // 最后一位处理后不再计算无用的平方。
        }
        return result;
    }
};
} // namespace cp
