# 矩形面积并与周长并（扫描线）

[模板源码](../../../geometry/rectangle_union.hpp) · [完整示例](../../../examples/geometry/rectangle_union.cpp) · [使用手册索引](../README.md)

给出很多轴平行矩形，求它们并集的面积或轮廓周长。扫描线沿 x 方向移动，线段树维护当前被覆盖的 y 长度；
覆盖次数的加减总是成对出现，所以线段树不需要下传标记。周长并分别沿 x、y 扫描，累加覆盖长度的变化量。

## 接口和使用顺序

| 接口 | 含义 |
| --- | --- |
| `Rectangle{x1, y1, x2, y2}` | 左下角 (x1,y1)、右上角 (x2,y2)，要求 x1 ≤ x2、y1 ≤ y2，坐标为 long long。 |
| `rectangle_union_area(rects)` | 并集面积，O(n log n)。 |
| `rectangle_union_perimeter(rects)` | 并集轮廓总长（包括内部空洞的边界），O(n log n)。 |

## 完整例子

下面是可以直接编译的完整程序，数据写在代码里，**不需要输入**。在 workspace 根目录执行文末的 `python tools/cp.py example ...` 命令即可运行并核对输出。

```cpp
#include <iostream>
#include <vector>
#include "geometry/rectangle_union.hpp"

int main() {
    // 两个 2x2 正方形部分重叠，另有一个独立的 1x2 矩形。
    std::vector<cp::Rectangle> rects{{0, 0, 2, 2}, {1, 1, 3, 3}, {5, 5, 6, 7}};
    std::cout << "area=" << cp::rectangle_union_area(rects) << '\n';
    std::cout << "perimeter=" << cp::rectangle_union_perimeter(rects) << '\n';
}
```

### 预期标准输出

```text
area=9
perimeter=18
```

## 注意事项

- 坐标先离散化，范围到 ±10¹⁸ 都可以，但面积必须在 long long 范围内（坐标绝对值 ≤ 10⁹ 时最大约 4×10¹⁸）。
- 退化矩形（面积为 0）被忽略；只在边上相接的矩形，公共边不计入周长。
- 题目给“格子坐标”（如左上角、右下角都是格子）时，先把右边界和下边界加 1，转成左闭右开的连续坐标。

## 对应课程与练习

- 课程：[讲解115 线段树与扫描线结合的题目](https://github.com/algorithmzuo/algorithm-journey/tree/main/src/class115)
- 练习：
  - [P5490 扫描线（矩形面积并）](https://www.luogu.com.cn/problem/P5490)：`rectangle_union_area`
  - [P1856 Picture（矩形周长并）](https://www.luogu.com.cn/problem/P1856)：`rectangle_union_perimeter`
  - [LeetCode 850 矩形面积 II](https://leetcode.cn/problems/rectangle-area-ii/)：结果对 1e9+7 取模前先用本模板求精确值

也可以在仓库根目录执行：

```powershell
python tools/cp.py example "geometry/rectangle_union"
```
