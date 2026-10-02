#pragma once
// 使用说明：cp-stl/docs/usage/data_structures/leftist_heap.md
// 完整示例：cp-stl/examples/data_structures/leftist_heap.cpp
#include <cassert>
#include <functional>
#include <utility>
#include <vector>

namespace cp {
// 左偏树（可并堆）：所有堆共用一个节点池，堆用根节点编号表示，-1 是空堆。
// push 新建节点 O(1)，meld / pop O(log n)；Compare 默认 std::less，即堆顶最小。
// 节点编号按 push 的顺序从 0 开始，可用来记录“这个值是谁”。
template<class T, class Compare = std::less<T>>
class LeftistHeap {
    std::vector<T> value_;
    std::vector<int> left_, right_, dist_;
    Compare less_;
    int dist(int p) const { return p < 0 ? -1 : dist_[p]; }
public:
    explicit LeftistHeap(Compare less = {}) : less_(less) {}
    void reserve(int n) { value_.reserve(n); left_.reserve(n); right_.reserve(n); dist_.reserve(n); }
    int node_count() const { return int(value_.size()); }
    // 新建只含 value 的堆，返回它的节点编号（也是这个单元素堆的根）。
    int push(const T& value) {
        value_.push_back(value);
        left_.push_back(-1);
        right_.push_back(-1);
        dist_.push_back(0);
        return int(value_.size()) - 1;
    }
    // 把 value 插入堆 root，返回新根。
    int push(int root, const T& value) { return meld(root, push(value)); }
    // 合并两个堆，返回新根；两个旧根都不能再单独使用。
    int meld(int a, int b) {
        if (a < 0 || b < 0) return a < 0 ? b : a;
        if (less_(value_[b], value_[a])) std::swap(a, b);
        right_[a] = meld(right_[a], b); // 只沿右链递归，深度 O(log n)。
        if (dist(left_[a]) < dist(right_[a])) std::swap(left_[a], right_[a]);
        dist_[a] = dist(right_[a]) + 1;
        return a;
    }
    const T& top(int root) const {
        assert(0 <= root && root < node_count());
        return value_[root];
    }
    // 删除堆顶，返回新根（堆变空时为 -1）。被删除的节点编号不会复用。
    int pop(int root) {
        assert(0 <= root && root < node_count());
        return meld(left_[root], right_[root]);
    }
    const T& value(int node) const { return value_[node]; }
};
} // namespace cp
