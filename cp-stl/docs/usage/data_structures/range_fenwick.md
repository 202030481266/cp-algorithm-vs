# 树状数组扩展：区间加区间和、二维树状数组

[模板源码](../../../data_structures/range_fenwick.hpp) · [完整示例](../../../examples/data_structures/range_fenwick.cpp) · [使用手册索引](../README.md)

在普通 Fenwick 的基础上扩展三种常见用法：一维区间加 + 区间和（两个树状数组维护差分），
二维单点加 + 子矩形和，二维子矩形加 + 子矩形和（四个树状数组）。常数明显小于懒标记线段树，
只要操作是“加法”就优先使用它们。

## 接口和使用顺序

| 接口 | 含义 |
| --- | --- |
| `RangeFenwick<T>(n) / RangeFenwick<T>(a)` | 全零 / 从数组 O(n) 建树。 |
| `add(l, r, delta)` | `[l,r)` 每个元素加 delta，O(log n)。 |
| `sum(l, r) / get(p)` | 区间和 / 单点值，O(log n)。 |
| `Fenwick2D<T>(rows, cols)` | 二维单点加、子矩形和；`add(r, c, delta)`、`sum(r1, c1, r2, c2)`。 |
| `RangeFenwick2D<T>(rows, cols)` | 二维子矩形加、子矩形和；`add(r1, c1, r2, c2, delta)`、`sum(r1, c1, r2, c2)`。 |

## 完整例子

下面是可以直接编译的完整程序，数据写在代码里，**不需要输入**。在 workspace 根目录执行文末的 `python tools/cp.py example ...` 命令即可运行并核对输出。

```cpp
#include <iostream>
#include <vector>
#include "data_structures/range_fenwick.hpp"

int main() {
    cp::RangeFenwick<long long> bit(std::vector<long long>{1, 2, 3, 4, 5});
    bit.add(1, 4, 10); // 1,12,13,14,5
    std::cout << "sum(0,5)=" << bit.sum(0, 5) << " sum(2,4)=" << bit.sum(2, 4)
              << " get(2)=" << bit.get(2) << '\n';

    cp::Fenwick2D<long long> grid(3, 4); // 3 行 4 列，单点加
    grid.add(0, 0, 5);
    grid.add(2, 3, 7);
    std::cout << "grid=" << grid.sum(0, 0, 3, 4) << ' ' << grid.sum(1, 1, 3, 4) << '\n';

    cp::RangeFenwick2D<long long> board(3, 3); // 子矩形加、子矩形和
    board.add(0, 0, 2, 2, 1);                   // 左上 2x2 每格加 1
    board.add(1, 1, 3, 3, 2);                   // 右下 2x2 每格加 2
    std::cout << "board=" << board.sum(0, 0, 3, 3) << " cell(1,1)=" << board.sum(1, 1, 2, 2) << '\n';
}
```

### 预期标准输出

```text
sum(0,5)=45 sum(2,4)=27 get(2)=13
grid=12 7
board=12 cell(1,1)=3
```

## 注意事项

- 所有区间左闭右开：一维 `[l,r)`，二维 `[r1,r2) x [c1,c2)`，行列下标都从 0 开始。
- `RangeFenwick` 内部的中间值约为 区间和 × n；`RangeFenwick2D` 约为 总和 × 行数 × 列数，注意 long long 溢出。
- 二维版本使用 (rows+1)×(cols+1) 的扁平数组，单次操作 O(log rows × log cols)，内存与矩阵同阶。
- 需要区间赋值、区间最值时树状数组无能为力，改用 [懒标记线段树](lazy_segment_tree.md)。

## 对应课程与练习

- 课程：[讲解108 树状数组原理、扩展、代码详解](https://github.com/algorithmzuo/algorithm-journey/tree/main/src/class108)、[讲解109 树状数组相关题目](https://github.com/algorithmzuo/algorithm-journey/tree/main/src/class109)
- 练习：
  - [P3368 树状数组 2](https://www.luogu.com.cn/problem/P3368)：区间加、单点查询：`RangeFenwick::get`
  - [P3372 线段树 1](https://www.luogu.com.cn/problem/P3372)：区间加、区间和
  - [P4514 上帝造题的七分钟](https://www.luogu.com.cn/problem/P4514)：二维区间加、区间和：`RangeFenwick2D`
  - [LeetCode 308 二维区域和检索 - 可变](https://leetcode.cn/problems/range-sum-query-2d-mutable/)：`Fenwick2D`

也可以在仓库根目录执行：

```powershell
python tools/cp.py example "data_structures/range_fenwick"
```
