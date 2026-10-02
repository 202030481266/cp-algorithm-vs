# 二维前缀和、二维差分与等差数列差分

[模板源码](../../../basic/prefix_sum.hpp) · [完整示例](../../../examples/basic/prefix_sum.cpp) · [使用手册索引](../README.md)

前缀和把“区间求和”变成 O(1)，差分把“区间加”变成 O(1) 的端点修改、最后统一还原。本模板提供三种常用形式：
静态矩阵的子矩形求和、离线的子矩形加，以及给一段区间加上等差数列（二阶差分）。

## 接口和使用顺序

| 接口 | 含义 |
| --- | --- |
| `PrefixSum2D<T>(a)` | 由 vector<vector<U>> 建立二维前缀和，O(nm)；至少一行，列数取自 a[0]。 |
| `sum(r1, c1, r2, c2)` | 子矩形 [r1,r2) × [c1,c2) 的和，O(1)。 |
| `Difference2D<T>(rows, cols)` | 全零的二维差分数组。 |
| `add(r1, c1, r2, c2, delta) / build()` | 子矩形加 delta，O(1) / 还原出 rows×cols 的矩阵，O(nm)。 |
| `ArithmeticDifference<T>(n)` | 长度 n 的二阶差分数组。 |
| `add(l, r, first, step) / build()` | [l,r) 依次加 first, first+step, …，O(1) / 还原数组，O(n)。 |

## 完整例子

下面是可以直接编译的完整程序，数据写在代码里，**不需要输入**。在 workspace 根目录执行文末的 `python tools/cp.py example ...` 命令即可运行并核对输出。

```cpp
#include <iostream>
#include <vector>
#include "basic/prefix_sum.hpp"

int main() {
    std::vector<std::vector<int>> grid{{1, 2, 3}, {4, 5, 6}, {7, 8, 9}};
    cp::PrefixSum2D<long long> ps(grid);
    std::cout << "sum all=" << ps.sum(0, 0, 3, 3) << " lower-right 2x2=" << ps.sum(1, 1, 3, 3) << '\n';

    cp::Difference2D<long long> diff(3, 4); // 3 行 4 列，离线子矩形加
    diff.add(0, 0, 2, 2, 1);
    diff.add(1, 1, 3, 4, 10);
    for (const auto& row : diff.build()) {
        for (int j = 0; j < int(row.size()); ++j) std::cout << row[j] << (j + 1 < int(row.size()) ? ' ' : '\n');
    }

    cp::ArithmeticDifference<long long> wave(6);
    wave.add(1, 5, 1, 2);  // 下标 1..4 依次加 1,3,5,7
    wave.add(0, 6, 10, 0); // 全部加 10
    std::cout << "arithmetic:";
    for (long long x : wave.build()) std::cout << ' ' << x;
    std::cout << '\n';
}
```

### 预期标准输出

```text
sum all=45 lower-right 2x2=28
1 1 0 0
1 11 10 10
0 10 10 10
arithmetic: 10 11 13 15 17 10
```

## 注意事项

- 所有区间左闭右开，行列下标从 0 开始。
- 一维前缀和直接用 `std::partial_sum`，一维普通差分是 `ArithmeticDifference` 取 step = 0 的特例。
- 在线的单点修改 + 子矩形求和用 [二维树状数组](../data_structures/range_fenwick.md)；在线子矩形加 + 子矩形和用 `RangeFenwick2D`。
- 前缀和的值可能远大于原数据，T 默认 long long。

## 对应课程与练习

- 课程：[讲解046 构建前缀信息的技巧](https://github.com/algorithmzuo/algorithm-journey/tree/main/src/class046)、[讲解047 一维差分与等差数列差分](https://github.com/algorithmzuo/algorithm-journey/tree/main/src/class047)、[讲解048 二维前缀和、二维差分、离散化技巧](https://github.com/algorithmzuo/algorithm-journey/tree/main/src/class048)
- 练习：
  - [P3397 地毯](https://www.luogu.com.cn/problem/P3397)：`Difference2D`
  - [P4231 三步必杀](https://www.luogu.com.cn/problem/P4231)：`ArithmeticDifference`
  - [LeetCode 304 二维区域和检索 - 矩阵不可变](https://leetcode.cn/problems/range-sum-query-2d-immutable/)：`PrefixSum2D`

也可以在仓库根目录执行：

```powershell
python tools/cp.py example "basic/prefix_sum"
```
