# 吉司机线段树：区间取最值与区间加

[模板源码](../../../data_structures/segment_tree_beats.hpp) · [完整示例](../../../examples/data_structures/segment_tree_beats.cpp) · [使用手册索引](../README.md)

Segment Tree Beats（吉如一线段树）支持把区间内每个数改成 `min(a[i], x)` 或 `max(a[i], x)`，
同时支持区间加，并查询区间和、最小值、最大值。每个节点维护最大值、严格次大值、最大值个数（最小值同理），
只有当 x 落在“最大值与次大值之间”时才在节点上直接修改，否则继续向下递归；均摊复杂度有保证。

## 接口和使用顺序

| 接口 | 含义 |
| --- | --- |
| `SegmentTreeBeats(a)` | 从 vector<long long> 建树，O(n)。 |
| `chmin(l, r, x)` | `a[i] = min(a[i], x)`，i ∈ [l,r)。 |
| `chmax(l, r, x)` | `a[i] = max(a[i], x)`，i ∈ [l,r)。 |
| `add(l, r, x)` | 区间加 x。 |
| `sum(l, r) / min(l, r) / max(l, r)` | 区间和（空区间为 0）/ 非空区间的最小值 / 最大值。 |

## 完整例子

下面是可以直接编译的完整程序，数据写在代码里，**不需要输入**。在 workspace 根目录执行文末的 `python tools/cp.py example ...` 命令即可运行并核对输出。

```cpp
#include <iostream>
#include <vector>
#include "data_structures/segment_tree_beats.hpp"

int main() {
    cp::SegmentTreeBeats seg(std::vector<long long>{5, 1, 7, 3, 9});
    seg.chmin(0, 5, 6);  // 5,1,6,3,6
    seg.chmax(1, 4, 4);  // 5,4,6,4,6
    seg.add(2, 5, -1);   // 5,4,5,3,5
    std::cout << "sum=" << seg.sum(0, 5) << " min=" << seg.min(0, 5) << " max=" << seg.max(0, 5) << '\n';
    seg.chmin(0, 5, 4);  // 4,4,4,3,4
    std::cout << "sum=" << seg.sum(0, 5) << " max(0,3)=" << seg.max(0, 3) << '\n';
}
```

### 预期标准输出

```text
sum=22 min=3 max=5
sum=19 max(0,3)=4
```

## 注意事项

- 只有 chmin/chmax 时均摊 O((n+q) log n)，混合区间加时为 O((n+q) log² n)。
- 元素与区间和必须在 long long 范围内；LLONG_MIN、LLONG_MAX 是内部哨兵，元素不能等于它们，chmin/chmax 的 x 也不能取这两个值。
- 历史最值（P6242 那类“区间历史最大值”）需要额外的标记，不在本模板范围内。
- 递归深度只有 O(log n)，不会爆栈；节点数组为 4n。

## 对应课程与练习

- 课程：[讲解114 开点线段树、区间最值和历史最值](https://github.com/algorithmzuo/algorithm-journey/tree/main/src/class114)
- 练习：
  - [LOJ 6565 最假女选手](https://loj.ac/p/6565)：chmin + chmax + 区间加 + 和/最值，正好是本模板
  - [P6242 线段树 3](https://www.luogu.com.cn/problem/P6242)：需要历史最值，作为进阶参考

也可以在仓库根目录执行：

```powershell
python tools/cp.py example "data_structures/segment_tree_beats"
```
