# 树状数组 Fenwick

[模板源码](../../../data_structures/fenwick.hpp) · [完整示例](../../../examples/data_structures/fenwick.cpp) · [使用手册索引](../README.md)

用于单点增加、前缀和与区间和。数组作为频次表且所有频次非负时，还能按累计频次查找第 k 小元素。

## 接口和使用顺序

| 接口 | 含义 |
| --- | --- |
| `Fenwick<long long>(n) / Fenwick<long long>(a)` | 建立全零数组 / 由原数组 O(n) 建树。 |
| `add(p, delta)` | a[p] 增加 delta，O(log n)；不是赋值。 |
| `prefix(r)` | 求 `[0,r)` 的和，O(log n)。 |
| `sum(l, r)` | 求 `[l,r)` 的和，O(log n)，空区间为 0。 |
| `lower_bound(k)` | 频次非负时，找最小 p 使 prefix(p+1)≥k；不存在返回 n。 |

## 完整例子

下面是可以直接编译的完整程序，数据写在代码里，**不需要输入**。在 workspace 根目录执行文末的 `python tools/cp.py example ...` 命令即可运行并核对输出。

```cpp
#include <iostream>
#include <vector>
#include "data_structures/fenwick.hpp"

int main() {
    cp::Fenwick<long long> bit(std::vector<long long>{1, 2, 3, 4, 5});
    std::cout << "sum(1,4)=" << bit.sum(1, 4) << '\n';
    bit.add(2, 10); // a 变成 1,2,13,4,5
    std::cout << "prefix(3)=" << bit.prefix(3) << '\n';
    std::cout << "total=" << bit.sum(0, 5) << '\n';
    std::cout << "empty=" << bit.sum(2, 2) << '\n';

    // 下标 0/1/2 分别出现 2/0/3 次。
    cp::Fenwick<long long> counts(std::vector<long long>{2, 0, 3});
    std::cout << "third_index=" << counts.lower_bound(3) << '\n';
    std::cout << "missing_index=" << counts.lower_bound(6) << '\n';
}
```

### 预期标准输出

```text
sum(1,4)=9
prefix(3)=16
total=25
empty=0
third_index=2
missing_index=3
```

## 注意事项

- 下标从 0 开始，区间右端点不包含。1-based 闭区间 `[L,R]` 转为 `sum(L-1,R)`。
- 普通求和可以有负数；`lower_bound` 要求数组中每个元素非负。
- `lower_bound` 中 k 是从 1 开始的名次，返回下标从 0 开始；k≤0 返回 0。
- 单点赋值为 x 时，先计算旧值，再执行 `add(p, x-old)`。

也可以在仓库根目录执行：

```powershell
python tools/cp.py example "data_structures/fenwick"
```
