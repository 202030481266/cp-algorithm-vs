# 单调栈与笛卡尔树

[模板源码](../../../basic/monotonic_stack.hpp) · [完整示例](../../../examples/basic/monotonic_stack.cpp) · [使用手册索引](../README.md)

单调栈在 O(n) 内求出每个位置左边 / 右边第一个满足比较条件的位置，是“以 a[i] 为最小值的最大区间”
“柱状图最大矩形”“下一个更大元素”等题目的核心。笛卡尔树把同样的信息组织成一棵树：
下标满足二叉搜索树性质，值满足堆性质，子树正好是“以根为最小值的极大区间”。

## 接口和使用顺序

| 接口 | 含义 |
| --- | --- |
| `nearest_left(a, cmp=less)` | left[i] = 最大的 j < i 使 cmp(a[j], a[i])，不存在为 -1。 |
| `nearest_right(a, cmp=less)` | right[i] = 最小的 j > i 使 cmp(a[j], a[i])，不存在为 n。 |
| `cartesian_tree(a, cmp=less)` | 返回 `{root, parent, left, right}`，不存在为 -1；默认小根，值相等时左边的在上。 |

## 完整例子

下面是可以直接编译的完整程序，数据写在代码里，**不需要输入**。在 workspace 根目录执行文末的 `python tools/cp.py example ...` 命令即可运行并核对输出。

```cpp
#include <algorithm>
#include <functional>
#include <iostream>
#include <vector>
#include "basic/monotonic_stack.hpp"

int main() {
    // 柱状图中最大的矩形（LeetCode 84）：以 h[i] 为高，向两边扩展到第一个更矮的柱子。
    std::vector<int> h{2, 1, 5, 6, 2, 3};
    auto left = cp::nearest_left(h);   // 左边第一个严格更小的位置，不存在为 -1
    auto right = cp::nearest_right(h); // 右边第一个严格更小的位置，不存在为 n
    long long best = 0;
    for (int i = 0; i < int(h.size()); ++i) best = std::max(best, 1LL * h[i] * (right[i] - left[i] - 1));
    std::cout << "left:";
    for (int x : left) std::cout << ' ' << x;
    std::cout << "\nright:";
    for (int x : right) std::cout << ' ' << x;
    std::cout << "\nlargest rectangle=" << best << '\n';

    auto next_greater = cp::nearest_right(h, std::greater<int>()); // 右边第一个严格更大
    std::cout << "next greater:";
    for (int x : next_greater) std::cout << ' ' << x;

    auto tree = cp::cartesian_tree(std::vector<int>{3, 1, 4, 1, 5}); // 小根，相等时左边的在上
    std::cout << "\ncartesian root=" << tree.root << " parent:";
    for (int p : tree.parent) std::cout << ' ' << p;
    std::cout << '\n';
}
```

### 预期标准输出

```text
left: -1 -1 1 2 1 4
right: 1 6 4 4 6 6
largest rectangle=10
next greater: 2 2 3 6 5 6
cartesian root=1 parent: 1 -1 3 1 3
```

## 注意事项

- cmp 取 `std::less<T>()`（严格更小）、`std::less_equal<T>()`、`std::greater<T>()`、`std::greater_equal<T>()`。
- 统计“以 a[i] 为最小值的子数组个数”时，一边用严格、一边用非严格，避免相等元素被重复计数：`nearest_left(a, std::less<T>())` 配 `nearest_right(a, std::less_equal<T>())`。
- 笛卡尔树中，节点 x 的子树对应下标区间 [l, r)，a[x] 是区间最小值；用 cmp = std::greater 得到大根树。
- 全部为 O(n) 时间、O(n) 额外空间，不递归。

## 对应课程与练习

- 课程：[讲解052 单调栈-上](https://github.com/algorithmzuo/algorithm-journey/tree/main/src/class052)、[讲解053 单调栈-下](https://github.com/algorithmzuo/algorithm-journey/tree/main/src/class053)、[讲解151 笛卡尔树、Treap树](https://github.com/algorithmzuo/algorithm-journey/tree/main/src/class151)
- 练习：
  - [P5788 单调栈](https://www.luogu.com.cn/problem/P5788)：`nearest_right(a, std::greater<int>())`，注意题目下标从 1 开始
  - [LeetCode 84 柱状图中最大的矩形](https://leetcode.cn/problems/largest-rectangle-in-histogram/)：例子原题
  - [LeetCode 907 子数组的最小值之和](https://leetcode.cn/problems/sum-of-subarray-minimums/)：一边严格一边非严格
  - [P5854 笛卡尔树](https://www.luogu.com.cn/problem/P5854)：`cartesian_tree`

也可以在仓库根目录执行：

```powershell
python tools/cp.py example "basic/monotonic_stack"
```
