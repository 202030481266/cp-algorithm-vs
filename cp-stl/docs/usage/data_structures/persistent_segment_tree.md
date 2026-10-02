# 可持久化线段树（主席树）与区间第 k 小

[模板源码](../../../data_structures/persistent_segment_tree.hpp) · [完整示例](../../../examples/data_structures/persistent_segment_tree.cpp) · [使用手册索引](../README.md)

每次单点修改只复制根到叶子路径上的 O(log n) 个节点，得到一个新版本，旧版本保持不变。
`PersistentSegmentTree` 维护带版本的数组和区间和；`RangeKth` 在它的基础上，为每个前缀建立值域计数树，
用两个前缀版本相减，在线回答“区间第 k 小”和“区间内小于 x 的个数”。

## 接口和使用顺序

| 接口 | 含义 |
| --- | --- |
| `PersistentSegmentTree<T>(n, reserve=0)` | 下标 [0,n)；根 0 表示全零数组。reserve 可预留节点数。 |
| `build(a)` | 用初始数组建出一个版本，返回其根，新建 2n-1 个节点。 |
| `add(root, pos, delta) / set(root, pos, value)` | 在版本 root 上单点修改，返回新版本的根，O(log n)。 |
| `get(root, pos) / sum(root, l, r)` | 查询某个版本的单点值 / 区间和，O(log n)。 |
| `kth(lo_root, hi_root, k)` | 计数数组 hi−lo 中第 k 小（从 0 开始）所在的下标。 |
| `RangeKth<T>(a)` | 静态数组的区间第 k 小：`kth(l, r, k)` 返回值，`count_less(l, r, x)` 返回个数。 |

## 完整例子

下面是可以直接编译的完整程序，数据写在代码里，**不需要输入**。在 workspace 根目录执行文末的 `python tools/cp.py example ...` 命令即可运行并核对输出。

```cpp
#include <iostream>
#include <vector>
#include "data_structures/persistent_segment_tree.hpp"

int main() {
    // 可持久化数组：每次修改得到一个新版本，旧版本仍可查询。
    cp::PersistentSegmentTree<long long> tree(5);
    int v0 = tree.build(std::vector<long long>{3, 1, 4, 1, 5});
    int v1 = tree.set(v0, 2, 10); // 3,1,10,1,5
    int v2 = tree.add(v1, 0, -3); // 0,1,10,1,5
    std::cout << "sum: v0=" << tree.sum(v0, 0, 5) << " v1=" << tree.sum(v1, 0, 5)
              << " v2=" << tree.sum(v2, 0, 5) << '\n';
    std::cout << "v0[2]=" << tree.get(v0, 2) << " v2[0]=" << tree.get(v2, 0) << '\n';

    // 静态区间第 k 小（k 从 0 开始）与区间内小于 x 的个数。
    cp::RangeKth<int> kth(std::vector<int>{5, 2, 6, 3, 7, 4});
    std::cout << "kth(1,5,0)=" << kth.kth(1, 5, 0) << " kth(1,5,2)=" << kth.kth(1, 5, 2)
              << " count_less(0,6,5)=" << kth.count_less(0, 6, 5) << '\n';
}
```

### 预期标准输出

```text
sum: v0=14 v1=20 v2=17
v0[2]=4 v2[0]=0
kth(1,5,0)=2 kth(1,5,2)=6 count_less(0,6,5)=3
```

## 注意事项

- 版本用根编号表示，自己保存：`roots.push_back(tree.add(roots[v], pos, x))`。根 0 永远是全零版本。
- `RangeKth` 的 k 从 0 开始，要求 `0 <= k < r-l`；预处理 O(n log n) 时间和空间。
- 空间约为 (修改次数) × (⌈log₂n⌉+1) 个节点，每个节点 8 字节加一个 T；10⁶ 次修改约需 10⁶×21 个节点，提前用 reserve 预留可以避免扩容时的双倍内存峰值。
- `kth` 要求两个版本相减后的计数都非负，常见用法是“前缀 r 的版本减去前缀 l 的版本”。
- 区间修改的可持久化（标记永久化）不在本模板中；需要时在节点上增加永久标记，查询时累加经过的标记。

## 对应课程与练习

- 课程：[讲解157 可持久化线段树和标记永久化](https://github.com/algorithmzuo/algorithm-journey/tree/main/src/class157)、[讲解158 可持久化线段树的相关题目](https://github.com/algorithmzuo/algorithm-journey/tree/main/src/class158)
- 练习：
  - [P3919 可持久化线段树 1（可持久化数组）](https://www.luogu.com.cn/problem/P3919)：`set`/`get`
  - [P3834 可持久化线段树 2（区间第 k 小）](https://www.luogu.com.cn/problem/P3834)：`RangeKth`，注意题目的 k 从 1 开始
  - [P2633 Count on a tree](https://www.luogu.com.cn/problem/P2633)：树上路径第 k 小：父亲版本 + LCA 容斥
  - [P4137 Rmq Problem / mex](https://www.luogu.com.cn/problem/P4137)：按值建树记录最后出现位置

也可以在仓库根目录执行：

```powershell
python tools/cp.py example "data_structures/persistent_segment_tree"
```
