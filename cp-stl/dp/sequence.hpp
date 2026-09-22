#pragma once
// 使用说明：cp-stl/docs/usage/dp/sequence.md
// 完整示例：cp-stl/examples/dp/sequence.cpp
#include <algorithm>
#include <cassert>
#include <deque>
#include <vector>
#include "../basic/compress.hpp"
#include "../data_structures/fenwick.hpp"

namespace cp {
// O(n log n)，strict=true 为严格递增，false 为不下降。
template<class T>
int lis_length(const std::vector<T>& a, bool strict = true) {
    std::vector<T> tails;
    for (const T& value : a) {
        auto it = strict ? std::lower_bound(tails.begin(), tails.end(), value)
                         : std::upper_bound(tails.begin(), tails.end(), value);
        if (it == tails.end()) tails.push_back(value);
        else *it = value;
    }
    return int(tails.size());
}
// 严格逆序对 a[i]>a[j]，O(n log n)；重复元素不计。
template<class T>
long long inversion_count(const std::vector<T>& a) {
    Compressor<T> compressed(a);
    Fenwick<long long> counts(compressed.size());
    long long answer = 0, seen = 0;
    for (const T& x : a) {
        int p = compressed.index(x);
        answer += seen++ - counts.prefix(p + 1);
        counts.add(p, 1);
    }
    return answer;
}
// 长度 k 的每个窗口的最小值，O(n)；k>n 返回空。
template<class T>
std::vector<T> sliding_min(const std::vector<T>& a, int k) {
    assert(k > 0);
    std::deque<int> queue;
    std::vector<T> answer;
    for (int i = 0; i < int(a.size()); ++i) {
        while (!queue.empty() && queue.front() <= i - k) queue.pop_front();
        while (!queue.empty() && a[queue.back()] >= a[i]) queue.pop_back();
        queue.push_back(i);
        if (i + 1 >= k) answer.push_back(a[queue.front()]);
    }
    return answer;
}
} // namespace cp
