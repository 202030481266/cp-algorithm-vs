#pragma once
// 使用说明：cp-stl/docs/usage/graph/residue_shortest_path.md
// 完整示例：cp-stl/examples/graph/residue_shortest_path.cpp
#include <algorithm>
#include <cassert>
#include <limits>
#include <numeric>
#include <vector>

namespace cp {
// 同余最短路：若干正整数 steps 可以各用任意多次，问能凑出哪些数。
// 取 base = min(steps)，返回长度为 base 的数组 dist：dist[r] 是能凑出的、模 base 余 r 的最小的数，
// 凑不出时为 LLONG_MAX。于是 x 能凑出  <=>  x >= dist[x % base]。
// 每个步长把余数连成若干个环，沿每个环走两圈就完成全部松弛（“转圈”），O(步长个数 · base)，不需要堆。
inline std::vector<long long> residue_distances(const std::vector<long long>& steps) {
    assert(!steps.empty());
    const long long base = *std::min_element(steps.begin(), steps.end());
    const long long unreachable = std::numeric_limits<long long>::max();
    assert(base > 0);
    std::vector<long long> dist(static_cast<std::size_t>(base), unreachable);
    dist[0] = 0;
    for (long long step : steps) {
        long long shift = step % base;
        if (shift == 0) continue;
        long long cycles = std::gcd(shift, base);
        for (long long start = 0; start < cycles; ++start)
            for (long long i = start, rounds = 0; rounds < 2; rounds += i == start) {
                long long j = i + shift >= base ? i + shift - base : i + shift;
                if (dist[i] <= unreachable - step && dist[i] + step < dist[j]) dist[j] = dist[i] + step;
                i = j;
            }
    }
    return dist;
}

// [0, limit] 中能凑出的数的个数（0 也算，表示什么都不取）。
inline long long count_representable(const std::vector<long long>& dist, long long limit) {
    long long base = static_cast<long long>(dist.size()), total = 0;
    for (long long d : dist) if (d <= limit) total += (limit - d) / base + 1;
    return total;
}
} // namespace cp
