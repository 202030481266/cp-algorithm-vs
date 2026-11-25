#pragma once
// 使用说明：cp-stl/docs/usage/basic/parallel_binary_search.md
// 完整示例：cp-stl/examples/basic/parallel_binary_search.cpp
#include <algorithm>
#include <cassert>
#include <vector>

namespace cp {
// 整体二分：有 m 个按顺序执行的操作和 q 个询问，每个询问的条件随操作的执行“先假后真”。
// 返回 answer[i]：最小的 t（0 <= t <= m），使得执行完前 t 个操作后 check(i) 为真；一直为假则是 m + 1。
// 每一轮：reset() 清空数据结构，按 t = 0..m-1 依次 apply(t)，并在恰好执行了 t 个操作时对
// 当前二分中点为 t 的询问调用 check(i)。共 ⌈log₂(m+2)⌉ 轮，每轮至多 m 次 apply、q 次 check。
template<class Reset, class Apply, class Check>
std::vector<int> parallel_binary_search(int m, int q, Reset reset, Apply apply, Check check) {
    assert(m >= 0 && q >= 0);
    std::vector<int> lo(q, 0), hi(q, m + 1), head(m + 1), next(q);
    while (true) {
        std::fill(head.begin(), head.end(), -1);
        int last = -1; // 本轮需要检查的最大中点
        for (int i = q - 1; i >= 0; --i) {
            if (lo[i] == hi[i]) continue;
            int mid = lo[i] + (hi[i] - lo[i]) / 2;
            next[i] = head[mid];
            head[mid] = i;
            last = std::max(last, mid);
        }
        if (last < 0) return lo;
        reset();
        for (int t = 0; t <= last; ++t) {
            for (int i = head[t]; i != -1; i = next[i]) {
                if (check(i)) hi[i] = t;
                else lo[i] = t + 1;
            }
            if (t < last) apply(t);
        }
    }
}
} // namespace cp
