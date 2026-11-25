#pragma once
// 使用说明：cp-stl/docs/usage/math/permutation.md
// 完整示例：cp-stl/examples/math/permutation.cpp
#include <algorithm>
#include <cassert>
#include <utility>
#include <vector>
#include "../data_structures/fenwick.hpp"

namespace cp {
// 康托展开：0..n-1 的排列 p 在全部 n! 个排列中的字典序排名（从 0 开始），对模整数类型 M 取模。
// 用树状数组统计“后面还没用过的更小的数”，O(n log n)。M 例如 cp::Mint。
template<class M>
M permutation_rank(const std::vector<int>& p) {
    int n = int(p.size());
    Fenwick<int> unused(std::vector<int>(n, 1));
    std::vector<M> factorial(n + 1, M(1));
    for (int i = 1; i <= n; ++i) factorial[i] = factorial[i - 1] * M(i);
    M rank = 0;
    for (int i = 0; i < n; ++i) {
        assert(0 <= p[i] && p[i] < n && unused.sum(p[i], p[i] + 1) == 1);
        rank += M(unused.prefix(p[i])) * factorial[n - 1 - i];
        unused.add(p[i], -1);
    }
    return rank;
}

// 精确的康托展开，n <= 20（20! < 2^63），O(n^2)。
inline unsigned long long permutation_rank_exact(const std::vector<int>& p) {
    int n = int(p.size());
    assert(n <= 20);
    unsigned long long rank = 0;
    for (int i = 0; i < n; ++i) {
        int smaller = 0;
        for (int j = i + 1; j < n; ++j) smaller += p[j] < p[i];
        rank = rank * (n - i) + smaller; // 秦九韶形式：sum smaller_i * (n-1-i)!
    }
    return rank;
}

// 逆康托展开：长度 n 的排列中字典序排名为 k（从 0 开始）的那一个，要求 n <= 20 且 k < n!。O(n^2)。
inline std::vector<int> kth_permutation(int n, unsigned long long k) {
    assert(0 <= n && n <= 20);
    std::vector<unsigned long long> factorial(n + 1, 1);
    for (int i = 1; i <= n; ++i) factorial[i] = factorial[i - 1] * i;
    assert(k < factorial[n]);
    std::vector<int> pool(n), result;
    for (int i = 0; i < n; ++i) pool[i] = i;
    for (int i = n - 1; i >= 0; --i) {
        int index = int(k / factorial[i]);
        k %= factorial[i];
        result.push_back(pool[index]);
        pool.erase(pool.begin() + index);
    }
    return result;
}

// 约瑟夫问题：编号 0..n-1 的人围成一圈，从 0 号开始报数，每报到第 k 个人就让他出列，返回最后剩下的编号。
// 递推 J(i+1) = (J(i) + k) mod (i+1)；k 较小时成批跳过不需要取模的步骤，复杂度 O(min(n, k log n))。
inline long long josephus(long long n, long long k) {
    assert(n >= 1 && k >= 1);
    if (k == 1) return n - 1;
    long long survivor = 0, size = 1; // survivor = J(size)
    while (size < n) {
        long long steps = (size - survivor - 1) / (k - 1); // 这些步里 survivor + k 都小于新的人数
        if (steps > 0) {
            steps = std::min(steps, n - size);
            survivor += steps * k;
            size += steps;
        } else {
            ++size;
            survivor = (survivor + k) % size;
        }
    }
    return survivor;
}

// 完美洗牌：把偶数长度的 a[l..r) = [L1..Lk, R1..Rk] 原地重排成 [R1, L1, R2, L2, ..., Rk, Lk]，
// O(长度) 时间、O(1) 额外空间。位置 i（从 1 开始）的元素要去 2i mod (2k+1)；当长度是 3^t - 1 时，
// 这个置换的环恰好以 1, 3, 9, ..., 3^(t-1) 为起点，所以每次切下最长的这样一段，剩下的继续处理。
template<class T>
void perfect_shuffle(std::vector<T>& a, int l, int r) {
    assert(0 <= l && l <= r && r <= int(a.size()) && (r - l) % 2 == 0);
    while (l < r) {
        int len = r - l, part = 3, cycles = 1; // part = 3^cycles，取满足 part - 1 <= len 的最大值
        while (part <= (len + 1) / 3) { part *= 3; ++cycles; }
        int half = (part - 1) / 2, mid = l + len / 2;
        // 旋转之后，前 part - 1 个元素是 [L1..Lhalf, R1..Rhalf]，其余元素仍是“左半 + 右半”。
        std::rotate(a.begin() + (l + half), a.begin() + mid, a.begin() + (mid + half));
        for (int start = 1, c = 0; c < cycles; ++c, start *= 3) {
            T carried = std::move(a[l + start - 1]);
            for (int cur = 2 * start % part; cur != start; cur = 2 * cur % part) std::swap(carried, a[l + cur - 1]);
            a[l + start - 1] = std::move(carried);
        }
        l += part - 1;
    }
}
template<class T>
void perfect_shuffle(std::vector<T>& a) { perfect_shuffle(a, 0, int(a.size())); }
} // namespace cp
