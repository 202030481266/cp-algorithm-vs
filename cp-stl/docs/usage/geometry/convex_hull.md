# 整数几何叉积与凸包

[模板源码](../../../geometry/convex_hull.hpp) · [完整示例](../../../examples/geometry/convex_hull.cpp) · [使用手册索引](../README.md)

对平面整数点求包住全部点的最小凸多边形。Andrew 算法排序去重后输出逆时针顶点，不重复首尾，不保留边上的中间共线点。

## 接口和使用顺序

| 接口 | 含义 |
| --- | --- |
| `Point{x,y}` | long long 整数坐标；本模板约定绝对值不超过 1e9。 |
| `cross(a,b,c)` | 返回向量 ab 与 ac 的叉积，类型 cp::CrossProduct（GNU 原生 128 位、MSVC 为 long long）；正/负/零分别表示左转/右转/共线。 |
| `convex_hull(points)` | 返回凸包顶点 vector<Point>，O(n log n)，不会修改调用者的原数组。 |

## 完整例子

下面是可以直接编译的完整程序，数据写在代码里，**不需要输入**。在 workspace 根目录执行文末的 `python tools/cp.py example ...` 命令即可运行并核对输出。

```cpp
#include <iostream>
#include <vector>
#include "geometry/convex_hull.hpp"

int main() {
    std::vector<cp::Point> points{
        {0, 0}, {2, 0}, {2, 2}, {0, 2}, {1, 1}, {1, 0}, {0, 0}
    };
    auto hull = cp::convex_hull(points);
    std::cout << "hull:";
    for (auto p : hull) std::cout << " (" << p.x << ',' << p.y << ')';
    std::cout << '\n';
    cp::CrossProduct twice_area = 0;
    for (int i = 1; i + 1 < int(hull.size()); ++i)
        twice_area += cp::cross(hull[0], hull[i], hull[i + 1]);
    std::cout << "twice_area=" << static_cast<long long>(twice_area) << '\n';
    std::cout << std::boolalpha << "ccw=" << (cp::cross({0, 0}, {2, 0}, {1, 1}) > 0) << '\n';
    std::cout << "collinear_vertices=" << cp::convex_hull({{0, 0}, {1, 1}, {2, 2}}).size() << '\n';
}
```

### 预期标准输出

```text
hull: (0,0) (2,0) (2,2) (0,2)
twice_area=8
ccw=true
collinear_vertices=2
```

## 注意事项

- 全共线时只保留两个端点；0/1 个不同点直接返回。
- 输出没有重复末尾起点；遍历多边形边时用 `(i+1)%hull.size()`，空凸包不要取模。
- 遵守原库坐标限制 |x|,|y| <= 1e9，MSVC 的 long long 叉积仍然精确；GNU 的原生 128 位结果不能直接送给 cout。
- 如果题目要求保留边界上的全部共线点，需要调整模板的弹栈条件及退化情况处理。

也可以在仓库根目录执行：

```powershell
python tools/cp.py example "geometry/convex_hull"
```
