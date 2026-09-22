#pragma once
// 使用说明：cp-stl/docs/usage/basic/compress.md
// 完整示例：cp-stl/examples/basic/compress.cpp
#include <algorithm>
#include <cassert>
#include <vector>
#include <utility>

namespace cp {
// O(n log n) 离散化，保留大小关系。
template<class T>
struct Compressor {
    std::vector<T> values;
    explicit Compressor(std::vector<T> a) : values(std::move(a)) {
        std::sort(values.begin(), values.end());
        values.erase(std::unique(values.begin(), values.end()), values.end());
    }
    int lower_bound(const T& x) const {
        return int(std::lower_bound(values.begin(), values.end(), x) - values.begin());
    }
    int index(const T& x) const {
        int i = lower_bound(x);
        assert(i < int(values.size()) && values[i] == x);
        return i;
    }
    int size() const { return int(values.size()); }
};
} // namespace cp
