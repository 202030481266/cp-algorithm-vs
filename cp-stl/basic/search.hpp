#pragma once
// 使用说明：cp-stl/docs/usage/basic/search.md
// 完整示例：cp-stl/examples/basic/search.cpp
#include <cassert>
#include <limits>

namespace cp {
// [lo,hi) 中找第一个为真的位置；check 单调 false -> true，无解返回 hi。
// 无符号差值避免溢出，支持完整 long long 边界（含 MSVC）。
template<class F>
long long first_true(long long lo, long long hi, F check) {
    assert(lo <= hi);
    while (lo < hi) {
        auto distance = static_cast<unsigned long long>(hi) - static_cast<unsigned long long>(lo);
        long long mid = lo + static_cast<long long>(distance / 2);
        if (check(mid)) hi = mid;
        else lo = mid + 1;
    }
    return lo;
}
// 支持负数；排除除零和 LLONG_MIN / -1（结果超出 long long）。
inline long long floor_div(long long a, long long b) {
    assert(b != 0 && !(a == std::numeric_limits<long long>::min() && b == -1));
    long long q = a / b, r = a % b;
    return q - (r != 0 && ((r > 0) != (b > 0)));
}
inline long long ceil_div(long long a, long long b) {
    assert(b != 0 && !(a == std::numeric_limits<long long>::min() && b == -1));
    long long q = a / b, r = a % b;
    return q + (r != 0 && ((r > 0) == (b > 0)));
}
} // namespace cp
