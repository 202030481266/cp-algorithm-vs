#pragma once
// 使用说明：cp-stl/docs/usage/math/permutation.md
// 完整示例：cp-stl/examples/math/permutation.cpp
#include <algorithm>
#include <cassert>
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
} // namespace cp
