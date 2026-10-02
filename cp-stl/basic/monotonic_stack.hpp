#pragma once
// 使用说明：cp-stl/docs/usage/basic/monotonic_stack.md
// 完整示例：cp-stl/examples/basic/monotonic_stack.cpp
#include <functional>
#include <vector>

namespace cp {
// 单调栈：left[i] = 最大的 j < i 使 cmp(a[j], a[i]) 为真，不存在为 -1。O(n)。
// cmp 取 std::less<>（左边第一个严格更小）、std::less_equal<>、std::greater<>、std::greater_equal<>。
template<class T, class Compare = std::less<T>>
std::vector<int> nearest_left(const std::vector<T>& a, Compare cmp = {}) {
    int n = int(a.size());
    std::vector<int> result(n), stack;
    stack.reserve(n);
    for (int i = 0; i < n; ++i) {
        while (!stack.empty() && !cmp(a[stack.back()], a[i])) stack.pop_back();
        result[i] = stack.empty() ? -1 : stack.back();
        stack.push_back(i);
    }
    return result;
}
// right[i] = 最小的 j > i 使 cmp(a[j], a[i]) 为真，不存在为 n。O(n)。
template<class T, class Compare = std::less<T>>
std::vector<int> nearest_right(const std::vector<T>& a, Compare cmp = {}) {
    int n = int(a.size());
    std::vector<int> result(n), stack;
    stack.reserve(n);
    for (int i = n - 1; i >= 0; --i) {
        while (!stack.empty() && !cmp(a[stack.back()], a[i])) stack.pop_back();
        result[i] = stack.empty() ? n : stack.back();
        stack.push_back(i);
    }
    return result;
}

// 笛卡尔树：下标满足二叉搜索树（中序遍历为 0..n-1），值满足堆性质。
// cmp 默认 std::less：父亲的值不大于儿子（小根）；值相等时下标小的在上面。O(n)。
struct CartesianTree {
    int root = -1;
    std::vector<int> parent, left, right; // 不存在为 -1
};
template<class T, class Compare = std::less<T>>
CartesianTree cartesian_tree(const std::vector<T>& a, Compare cmp = {}) {
    int n = int(a.size());
    CartesianTree tree;
    tree.parent.assign(n, -1);
    tree.left.assign(n, -1);
    tree.right.assign(n, -1);
    std::vector<int> stack; // 右链，从根到最右的点
    stack.reserve(n);
    for (int i = 0; i < n; ++i) {
        int last = -1;
        while (!stack.empty() && cmp(a[i], a[stack.back()])) { last = stack.back(); stack.pop_back(); }
        if (last != -1) { tree.left[i] = last; tree.parent[last] = i; }
        if (!stack.empty()) { tree.right[stack.back()] = i; tree.parent[i] = stack.back(); }
        stack.push_back(i);
    }
    if (!stack.empty()) tree.root = stack.front();
    return tree;
}
} // namespace cp
