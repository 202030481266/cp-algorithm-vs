#pragma once
// 使用说明：cp-stl/docs/usage/data_structures/range_fenwick.md
// 完整示例：cp-stl/examples/data_structures/range_fenwick.cpp
#include <cassert>
#include <vector>

namespace cp {
// 区间加、区间和：维护差分 d 与 d[i]*i 两个树状数组。0-based，区间 [l,r)，O(log n)。
// T 需支持 + - * 和由 int 构造；sum 及 delta*下标的中间值都必须在 T 范围内。
template<class T = long long>
class RangeFenwick {
    int n_;
    std::vector<T> d_, di_; // d_ 存差分，di_ 存差分乘下标。
    void add_point(int p, T delta) {
        T weighted = delta * T(p);
        for (++p; p <= n_; p += p & -p) { d_[p] += delta; di_[p] += weighted; }
    }
    T prefix(int r) const { // [0,r) 的和 = r*sum(d) - sum(d[i]*i)。
        T s = T(), si = T();
        for (int p = r; p > 0; p -= p & -p) { s += d_[p]; si += di_[p]; }
        return s * T(r) - si;
    }
public:
    explicit RangeFenwick(int n = 0) : n_(n), d_(n + 1, T()), di_(n + 1, T()) {}
    explicit RangeFenwick(const std::vector<T>& a) : RangeFenwick(int(a.size())) {
        for (int i = 1; i <= n_; ++i) { // O(n) 建树。
            T diff = a[i - 1] - (i > 1 ? a[i - 2] : T());
            d_[i] += diff;
            di_[i] += diff * T(i - 1);
            int j = i + (i & -i);
            if (j <= n_) { d_[j] += d_[i]; di_[j] += di_[i]; }
        }
    }
    int size() const { return n_; }
    void add(int l, int r, T delta) {
        assert(0 <= l && l <= r && r <= n_);
        if (l == r) return;
        add_point(l, delta);
        if (r < n_) add_point(r, -delta);
    }
    T sum(int l, int r) const {
        assert(0 <= l && l <= r && r <= n_);
        return prefix(r) - prefix(l);
    }
    T get(int p) const {
        assert(0 <= p && p < n_);
        T value = T();
        for (int i = p + 1; i > 0; i -= i & -i) value += d_[i];
        return value;
    }
};

// 二维树状数组：单点加、子矩形和。行列都 0-based，矩形为 [r1,r2) x [c1,c2)，O(log n log m)。
template<class T = long long>
class Fenwick2D {
    int rows_, cols_;
    std::vector<T> bit_; // (rows+1)*(cols+1) 的扁平数组，下标从 1 开始。
    T prefix(int r, int c) const {
        T result = T();
        for (int i = r; i > 0; i -= i & -i)
            for (int j = c; j > 0; j -= j & -j) result += bit_[i * (cols_ + 1) + j];
        return result;
    }
public:
    Fenwick2D(int rows, int cols) : rows_(rows), cols_(cols), bit_((rows + 1) * (cols + 1), T()) {}
    void add(int r, int c, T delta) {
        assert(0 <= r && r < rows_ && 0 <= c && c < cols_);
        for (int i = r + 1; i <= rows_; i += i & -i)
            for (int j = c + 1; j <= cols_; j += j & -j) bit_[i * (cols_ + 1) + j] += delta;
    }
    T sum(int r1, int c1, int r2, int c2) const {
        assert(0 <= r1 && r1 <= r2 && r2 <= rows_ && 0 <= c1 && c1 <= c2 && c2 <= cols_);
        return prefix(r2, c2) - prefix(r1, c2) - prefix(r2, c1) + prefix(r1, c1);
    }
};

// 二维区间加、区间和：四个树状数组维护 d、d*i、d*j、d*i*j。矩形均为 [r1,r2) x [c1,c2)。
// 每次操作 O(log n log m)；中间值约为 总和 * 行数 * 列数，注意溢出。
template<class T = long long>
class RangeFenwick2D {
    struct Cell { T d = T(), di = T(), dj = T(), dij = T(); };
    int rows_, cols_;
    std::vector<Cell> bit_;
    void add_point(int r, int c, T delta) {
        T di = delta * T(r), dj = delta * T(c), dij = di * T(c);
        for (int i = r + 1; i <= rows_; i += i & -i)
            for (int j = c + 1; j <= cols_; j += j & -j) {
                Cell& cell = bit_[i * (cols_ + 1) + j];
                cell.d += delta; cell.di += di; cell.dj += dj; cell.dij += dij;
            }
    }
    T prefix(int r, int c) const { // [0,r) x [0,c) 的和。
        Cell s;
        for (int i = r; i > 0; i -= i & -i)
            for (int j = c; j > 0; j -= j & -j) {
                const Cell& cell = bit_[i * (cols_ + 1) + j];
                s.d += cell.d; s.di += cell.di; s.dj += cell.dj; s.dij += cell.dij;
            }
        return s.d * T(r) * T(c) - s.di * T(c) - s.dj * T(r) + s.dij;
    }
public:
    RangeFenwick2D(int rows, int cols) : rows_(rows), cols_(cols), bit_((rows + 1) * (cols + 1)) {}
    void add(int r1, int c1, int r2, int c2, T delta) {
        assert(0 <= r1 && r1 <= r2 && r2 <= rows_ && 0 <= c1 && c1 <= c2 && c2 <= cols_);
        if (r1 == r2 || c1 == c2) return;
        add_point(r1, c1, delta);
        if (c2 < cols_) add_point(r1, c2, -delta);
        if (r2 < rows_) add_point(r2, c1, -delta);
        if (r2 < rows_ && c2 < cols_) add_point(r2, c2, delta);
    }
    T sum(int r1, int c1, int r2, int c2) const {
        assert(0 <= r1 && r1 <= r2 && r2 <= rows_ && 0 <= c1 && c1 <= c2 && c2 <= cols_);
        return prefix(r2, c2) - prefix(r1, c2) - prefix(r2, c1) + prefix(r1, c1);
    }
};
} // namespace cp
