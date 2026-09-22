# LIS、逆序对与滑动窗口最小值

[模板源码](../../../dp/sequence.hpp) · [完整示例](../../../examples/dp/sequence.cpp) · [使用手册索引](../README.md)

同一个头文件提供三个常用序列操作：最长递增/不下降子序列长度、严格逆序对数量，以及固定窗口的最小值。它们都不会修改原数组。

## 接口和使用顺序

| 接口 | 含义 |
| --- | --- |
| `lis_length(a, strict=true)` | 严格递增长度；strict=false 求不下降长度，O(n log n)。 |
| `inversion_count(a)` | 统计 i<j 且 a[i]>a[j] 的对数，返回 long long，O(n log n)。 |
| `sliding_min(a,k)` | 每个长度 k 的窗口的最小值，O(n)；要求 k>0。 |

## 完整例子

下面是可以直接编译的完整程序，数据写在代码里，**不需要输入**。在 workspace 根目录执行文末的 `python tools/cp.py example ...` 命令即可运行并核对输出。

```cpp
#include <iostream>
#include <vector>
#include "dp/sequence.hpp"

int main() {
    std::vector<int> a{3, 1, 2, 2, 4};
    std::cout << "strict_lis=" << cp::lis_length(a) << '\n';
    std::cout << "nondecreasing=" << cp::lis_length(a, false) << '\n';
    std::cout << "inversions=" << cp::inversion_count(a) << '\n';
    std::cout << "window_min:";
    for (int value : cp::sliding_min(a, 3)) std::cout << ' ' << value;
    std::cout << '\n';
    std::cout << "oversized_window=" << cp::sliding_min(a, 6).size() << '\n';
}
```

### 预期标准输出

```text
strict_lis=3
nondecreasing=4
inversions=3
window_min: 1 1 2
oversized_window=0
```

## 注意事项

- LIS 是可以跳过元素的子序列，不是必须连续的子数组；这里只返回长度，不还原方案。
- 相等元素不算严格逆序对；不下降子序列可以包含相等元素。
- sliding_min 的返回值位置 i 对应原数组窗口 `[i,i+k)`；k>n 时为空。
- 空数组的 LIS 和逆序对都是 0。逆序对实现依赖离散化和 Fenwick，手动复制时要带依赖。

也可以在仓库根目录执行：

```powershell
python tools/cp.py example "dp/sequence"
```
