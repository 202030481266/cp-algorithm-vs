# 树套树：带修改的区间第 k 小

[模板源码](../../../data_structures/dynamic_range_kth.hpp) · [完整示例](../../../examples/data_structures/dynamic_range_kth.cpp) · [使用手册索引](../README.md)

静态的区间第 k 小用[主席树](persistent_segment_tree.md)；一旦有单点修改，前缀版本就要改成树状数组的形式：
外层树状数组管位置，每个节点挂一棵动态开点的权值线段树，记录它负责的那段位置里每个值出现了多少次。
查询 [l,r) 时取出两组 O(log n) 棵内层树，让它们同时沿值域往下走，于是排名和第 k 小都是 O(log n · log V)。

## 接口和使用顺序

| 接口 | 含义 |
| --- | --- |
| `DynamicRangeKth tree(a, value_limit)` | 初始数组 a，所有值在 [0, value_limit) 内。 |
| `set(pos, value)` / `get(pos)` | 单点赋值 / 读取。 |
| `kth(l, r, k)` | [l,r) 中第 k 小的值，k 从 0 开始，要求 0 ≤ k < r−l。 |
| `count_less(l, r, x)` | [l,r) 中严格小于 x 的个数，也就是 x 的排名（从 0 开始）。 |
| `count(l, r, lo, hi)` | [l,r) 中值在 [lo,hi) 内的个数。 |
| `reserve(nodes)` / `node_count()` | 预留内层节点 / 当前节点数。 |

## 完整例子

下面是可以直接编译的完整程序，数据写在代码里，**不需要输入**。在 workspace 根目录执行文末的 `python tools/cp.py example ...` 命令即可运行并核对输出。

```cpp
#include <iostream>
#include <vector>
#include "data_structures/dynamic_range_kth.hpp"

int main() {
    // 值域是 [0,10)。值很大时先离散化，修改会用到的值也要一起放进去。
    cp::DynamicRangeKth tree(std::vector<int>{5, 2, 6, 3, 7, 4}, 10);
    std::cout << "kth(1,5,0)=" << tree.kth(1, 5, 0) << " kth(1,5,2)=" << tree.kth(1, 5, 2)
              << " count_less(0,6,5)=" << tree.count_less(0, 6, 5) << '\n';
    tree.set(2, 1); // 5 2 1 3 7 4
    std::cout << "after set: kth(1,5,0)=" << tree.kth(1, 5, 0) << " kth(1,5,2)=" << tree.kth(1, 5, 2) << '\n';

    // 在 [0,6) 内查 4 的排名（比它小的个数）、前驱和后继。
    int rank = tree.count_less(0, 6, 4);
    std::cout << "rank(4)=" << rank << " prev=" << tree.kth(0, 6, rank - 1)
              << " next=" << tree.kth(0, 6, tree.count_less(0, 6, 5)) << '\n';
}
```

### 预期标准输出

```text
kth(1,5,0)=2 kth(1,5,2)=6 count_less(0,6,5)=3
after set: kth(1,5,0)=1 kth(1,5,2)=3
rank(4)=3 prev=3 next=5
```

## 注意事项

- 值域大时先[离散化](../basic/compress.md)，并且要把之后所有修改用到的值一起放进去；离散化还能减少内层树的深度和内存。
- 前驱：`c = count_less(l, r, x)`，c > 0 时是 `kth(l, r, c - 1)`；后继：`c = count_less(l, r, x + 1)`，c < r−l 时是 `kth(l, r, c)`。
- 内存是这个结构的主要代价：节点数上界是 (n + 2·修改次数)·⌈log₂n⌉·(⌈log₂V⌉+1)，每个 12 字节。随机数据下 n = 修改次数 = 10⁵、V = 2×10⁵ 实测约 1.7×10⁷ 个节点、190MB，耗时约 0.65 秒。内存限制是 128MB 时改用离线的[整体二分](../basic/parallel_binary_search.md)或 [CDQ 分治](../basic/cdq.md)。
- 题目的 k 通常从 1 开始，调用时减 1。

## 对应课程与练习

- 课程：[讲解160（树套树）](https://github.com/algorithmzuo/algorithm-journey/tree/main/src/class160)
- 练习：
  - [P2617 Dynamic Rankings](https://www.luogu.com.cn/problem/P2617)：`set` 与 `kth`
  - [P3380 【模板】树套树](https://www.luogu.com.cn/problem/P3380)：排名、第 k 小、修改、前驱、后继，全部由 `count_less` 和 `kth` 组合得到
  - [P3157 [CQOI2011] 动态逆序对](https://www.luogu.com.cn/problem/P3157)：删除一个数时，用 `count` 查它前面比它大的和后面比它小的个数

也可以在仓库根目录执行：

```powershell
python tools/cp.py example "data_structures/dynamic_range_kth"
```
