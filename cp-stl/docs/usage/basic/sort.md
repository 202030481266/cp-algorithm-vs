# 计数排序与基数排序

[模板源码](../../../basic/sort.hpp) · [完整示例](../../../examples/basic/sort.cpp) · [使用手册索引](../README.md)

整数排序可以利用值域或二进制位，避免逐对比较。值域很小时使用计数排序；数值跨度很大、包含负数或 64 位边界时使用基数排序。两种算法都提供直接修改数组和返回原下标的版本，均按升序稳定排序，支持除 `bool` 外的标准整数类型。只需要排序后的原始位置时，调用 `counting_sort_ids` 或 `radix_sort_ids`，原数组保持不变。

## 接口和选择

设 `n` 为元素个数，`K = max(a) - min(a) + 1` 为值域大小，`w` 为对应无符号整数类型的位数。

| 接口 | 适用场景 | 时间 | 额外空间 |
| --- | --- | --- | --- |
| `cp::counting_sort_ids(a, max_range = 1'000'000)` | 返回排序后的原下标，输入为只读数组；小值域，默认最多 1,000,000 个桶。 | O(n + K) | O(n + K) |
| `cp::radix_sort_ids(a)` | 返回排序后的原下标，输入为只读数组；支持完整整数值域。 | O(ceil(w / 8) · (n + 256)) | O(n + 256) |
| `cp::counting_sort(a)` | 小值域，自动计算最小值和最大值；默认最多 1,000,000 个桶。 | O(n + K) | O(n + K) |
| `cp::counting_sort(a, max_range)` | 显式指定允许的最大桶数。 | O(n + K) | O(n + K) |
| `cp::radix_sort(a)` | 任意整数值域；每轮从低到高处理 8 位，32 位整数 4 轮、64 位整数 8 轮。 | O(ceil(w / 8) · (n + 256)) | O(n + 256) |

- 下标版接收 `const std::vector<T>&`，返回 `std::vector<std::size_t>`。`ids[i]` 是排序后第 `i` 个元素的原始下标，`a[ids[i]]` 是对应的值；下标从 0 开始。相等元素按原下标升序排列。
- 空数组和单元素数组直接返回，不分配桶；下标版分别返回空数组和 `{0}`。重复值和全相等数组均可使用。
- 计数排序将 `x` 映射到 `x - min(a)`，所以允许负数。靠近整数最小值或最大值的窄区间也可使用，内部用无符号减法避免溢出。
- 至少有两个元素时，计数排序会在分配前检查 `K`；超过 `max_range` 或计数容器容量上限时抛出 `std::length_error`，原数组不变。不会自动换成其他排序算法。
- `max_range` 是桶数上限，不是最大元素值，也不是字节数。默认计数数组在 64 位环境中至多约 8 MB；修改数组版另需 `n * sizeof(T)` 字节的缓冲区，下标版另需 `n * sizeof(std::size_t)` 字节保存结果。增大上限前按题目内存限制估算；跨度大时直接调用 `radix_sort`。
- 基数排序翻转有符号整数的符号位作为排序键，不对负数取绝对值，支持 `LLONG_MIN`、`LLONG_MAX` 和 `ULLONG_MAX`。

## 完整例子

下面的程序内置数据，**不需要输入**。

```cpp
#include <iostream>
#include <limits>
#include <vector>
#include "basic/sort.hpp"

int main() {
    const std::vector<int> original{3, -2, 0, 3, -2, 1};
    auto ids = cp::counting_sort_ids(original);
    std::cout << "counting_ids:";
    for (auto id : ids) std::cout << ' ' << id;
    std::cout << '\n';
    ids = cp::radix_sort_ids(original);
    std::cout << "radix_ids:";
    for (auto id : ids) std::cout << ' ' << id;
    std::cout << '\n';
    std::cout << "values_by_ids:";
    for (auto id : ids) std::cout << ' ' << original[id];
    std::cout << '\n';
    std::cout << "original:";
    for (int x : original) std::cout << ' ' << x;
    std::cout << '\n';

    std::vector<int> small{3, -2, 0, 3, -2, 1};
    cp::counting_sort(small);
    std::cout << "counting:";
    for (int x : small) std::cout << ' ' << x;
    std::cout << '\n';

    std::vector<long long> wide{170, -45, 75, -90, 0, 170, 802};
    cp::radix_sort(wide);
    std::cout << "radix:";
    for (long long x : wide) std::cout << ' ' << x;
    std::cout << '\n';

    std::vector<long long> edges{
        std::numeric_limits<long long>::max(), 0,
        std::numeric_limits<long long>::min(), -1};
    cp::radix_sort(edges);
    std::cout << "signed:";
    for (long long x : edges) std::cout << ' ' << x;
    std::cout << '\n';

    std::vector<unsigned long long> unsigned_values{
        std::numeric_limits<unsigned long long>::max(), 0, 42, 1};
    cp::radix_sort(unsigned_values);
    std::cout << "unsigned:";
    for (auto x : unsigned_values) std::cout << ' ' << x;
    std::cout << '\n';
}
```

### 预期标准输出

```text
counting_ids: 1 4 2 5 0 3
radix_ids: 1 4 2 5 0 3
values_by_ids: -2 -2 0 1 3 3
original: 3 -2 0 3 -2 1
counting: -2 -2 0 1 3 3
radix: -90 -45 0 75 170 170 802
signed: -9223372036854775808 -1 0 9223372036854775807
unsigned: 0 1 42 18446744073709551615
```

## 注意事项

- `counting_sort_ids` 和 `radix_sort_ids` 只读原数组，直接生成或重排下标，不复制整个值数组。下标版不会使原数组的引用或迭代器失效。
- 基数排序的下标版使用两个长度为 `n` 的下标数组（结果和缓冲区），另有 256 个计数桶。只移动下标不保证一定更快，实际耗时取决于下标大小和间接读取原值的开销。
- `counting_sort` 和 `radix_sort` 返回 `void`，排序结果保存在原数组中，内部会分配缓冲区；不要保留这些修改数组版本调用前的元素引用或迭代器继续使用。
- 接口用于整数数组，不接受浮点数、字符串、结构体、比较器或 `std::vector<bool>`。
- 需要降序时，可以对下标版的结果调用 `std::reverse(ids.begin(), ids.end())`，或对修改数组版的结果调用 `std::reverse(a.begin(), a.end())`；这也会反转相等元素的顺序。
- 元素少或需要自定义比较规则时，也可以直接使用 `std::sort`；基数排序的多轮扫描不保证在所有规模下都更快。

在仓库根目录运行并核对示例：

```powershell
python tools/cp.py example "basic/sort"
```
