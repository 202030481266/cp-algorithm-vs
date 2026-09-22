# 随机数据与可重现对拍

[模板源码](../../../util/random.hpp) · [完整示例](../../../examples/util/random.cpp) · [使用手册索引](../README.md)

用一个 Random 对象连续生成数据；发现对拍失败时记录 seed，在相同运行库下重用同一 seed 和调用顺序，即可重现相同数据。

## 接口和使用顺序

| 接口 | 含义 |
| --- | --- |
| `Random(seed) / Random()` | 指定 uint64_t 种子 / 使用时钟种子。内部生成器为 mt19937_64。 |
| `integer(lo,hi)` | 均匀生成闭区间 `[lo,hi]` 的 long long；必须 lo≤hi。 |
| `array(n,lo,hi)` | 生成 n 个闭区间内的随机数，返回 vector<long long>，要求 n≥0。 |
| `shuffle(a)` | 原地打乱 vector<T>，O(n)。 |
| `engine` | 底层生成器，可传给其他标准库随机分布。 |

## 完整例子

下面是可以直接编译的完整程序，数据写在代码里，**不需要输入**。在 workspace 根目录执行文末的 `python tools/cp.py example ...` 命令即可运行并核对输出。

```cpp
#include <algorithm>
#include <iostream>
#include <vector>
#include "util/random.hpp"

int main() {
    cp::Random first(42), second(42);
    auto a = first.array(8, -3, 3);
    auto b = second.array(8, -3, 3);
    std::cout << std::boolalpha;
    std::cout << "same_seed=" << (a == b) << '\n';
    std::cout << "in_range=" << std::all_of(a.begin(), a.end(), [](long long x) {
        return -3 <= x && x <= 3;
    }) << '\n';
    std::cout << "fixed_interval=" << first.integer(5, 5) << '\n';

    std::vector<int> order{1, 2, 3, 4};
    first.shuffle(order);
    std::sort(order.begin(), order.end());
    std::cout << "same_elements=" << (order == std::vector<int>{1, 2, 3, 4}) << '\n';
}
```

### 预期标准输出

```text
same_seed=true
in_range=true
fixed_interval=5
same_elements=true
```

## 注意事项

- 这里 lo、hi 表示数值范围，两端都包含；与数组查询的左闭右开区间不同。
- 不要在循环里不断用同一个种子重建对象，那样会不断重复随机序列的开头。
- 不同标准库的分布和 shuffle 实现可能不同，不承诺跨平台得到完全相同的具体数组。示例输出验证范围与一致性，不写死某组随机数。
- 数据生成之外，记得保存能触发失败的完整输入；这样可独立于生成器重现问题。

也可以在仓库根目录执行：

```powershell
python tools/cp.py example "util/random"
```
