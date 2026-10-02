# 动态开点线段树：超大下标的区间加、区间和、区间最大值

[模板源码](../../../data_structures/dynamic_segment_tree.hpp) · [完整示例](../../../examples/data_structures/dynamic_segment_tree.cpp) · [使用手册索引](../README.md)

下标范围很大（例如 10⁹ 甚至 10¹⁸）、又必须在线处理时，不能离散化，就只为访问过的节点分配内存。
本模板采用**标记永久化**：区间加只把标记留在完全覆盖的节点上，查询时沿路径累加，
因此修改不需要下传、也不会为了下传而额外开点。所有位置初始为 0。

## 接口和使用顺序

| 接口 | 含义 |
| --- | --- |
| `DynamicSegmentTree(lo, hi, reserve=0)` | 下标范围 [lo,hi)，初始全 0；可预留节点数。 |
| `add(l, r, delta)` | `[l,r)` 每个位置加 delta，O(log(hi-lo))。 |
| `sum(l, r)` | 区间和，空区间为 0。 |
| `max(l, r) / all_max()` | 非空区间的最大值 / 整个范围的最大值。 |
| `node_count()` | 已分配的节点数，可用来估计内存。 |

## 完整例子

下面是可以直接编译的完整程序，数据写在代码里，**不需要输入**。在 workspace 根目录执行文末的 `python tools/cp.py example ...` 命令即可运行并核对输出。

```cpp
#include <iostream>
#include "data_structures/dynamic_segment_tree.hpp"

int main() {
    cp::DynamicSegmentTree tree(0, 1000000000); // 下标 [0,1e9)，初始全 0
    tree.add(100, 200, 5);
    tree.add(150, 1000000000, 2);
    std::cout << "sum=" << tree.sum(0, 1000000000) << '\n';
    std::cout << "max[0,1000)=" << tree.max(0, 1000) << " max[200,300)=" << tree.max(200, 300)
              << " all_max=" << tree.all_max() << '\n';
    std::cout << "small_tree=" << (tree.node_count() < 200 ? "yes" : "no") << '\n';
}
```

### 预期标准输出

```text
sum=2000000200
max[0,1000)=7 max[200,300)=2 all_max=7
small_tree=yes
```

## 注意事项

- 单次区间加最多新建约 4·log₂(hi−lo) 个节点；5×10⁵ 次操作、范围 10⁹ 时约数千万字节，必要时用 reserve 预留。
- 要求 hi−lo 在 long long 范围内；区间和与 delta×长度的中间值也必须在 long long 范围内。
- 能离线时优先离散化后用普通线段树或树状数组，常数和内存都更小。
- 最小值查询可以把所有值取相反数后用 max，或者仿照 max 再加一个字段。

## 对应课程与练习

- 课程：[讲解114 开点线段树、区间最值和历史最值](https://github.com/algorithmzuo/algorithm-journey/tree/main/src/class114)
- 练习：
  - [P2781 传教](https://www.luogu.com.cn/problem/P2781)：范围 10⁹ 的区间加、区间和
  - [LeetCode 732 我的日程安排表 III](https://leetcode.cn/problems/my-calendar-iii/)：区间加 1 后求 `all_max()`

也可以在仓库根目录执行：

```powershell
python tools/cp.py example "data_structures/dynamic_segment_tree"
```
