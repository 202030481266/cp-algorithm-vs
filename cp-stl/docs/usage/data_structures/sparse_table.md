# 静态区间查询 SparseTable

[模板源码](../../../data_structures/sparse_table.hpp) · [完整示例](../../../examples/data_structures/sparse_table.cpp) · [使用手册索引](../README.md)

数组不再修改，但要进行很多次区间最小值、最大值或 gcd 查询时使用。一次建表后，每次查询只需 O(1)。

## 接口和使用顺序

| 接口 | 含义 |
| --- | --- |
| `SparseTable<T,Op>(a, op={})` | 预处理 O(n log n)，空间 O(n log n)；Op 默认 cp::Min。 |
| `query(l, r)` | 查询非空 `[l,r)`，O(1)。 |
| `size()` | 原数组长度。 |
| `cp::Min / Max / Gcd / BitAnd / BitOr` | 可直接作为 Op 的运算对象。 |

## 完整例子

下面是可以直接编译的完整程序，数据写在代码里，**不需要输入**。在 workspace 根目录执行文末的 `python tools/cp.py example ...` 命令即可运行并核对输出。

```cpp
#include <iostream>
#include <vector>
#include "data_structures/sparse_table.hpp"

int main() {
    std::vector<int> a{12, 18, 6, 9};
    cp::SparseTable<int> minimum(a);
    cp::SparseTable<int, cp::Max> maximum(a);
    cp::SparseTable<int, cp::Gcd> gcd(a);
    std::cout << "size=" << minimum.size() << '\n';
    std::cout << "min(1,4)=" << minimum.query(1, 4) << '\n';
    std::cout << "max(0,3)=" << maximum.query(0, 3) << '\n';
    std::cout << "gcd(0,4)=" << gcd.query(0, 4) << '\n';
    std::cout << "single=" << minimum.query(2, 3) << '\n';
}
```

### 预期标准输出

```text
size=4
min(1,4)=6
max(0,3)=18
gcd(0,4)=3
single=6
```

## 注意事项

- 运算必须结合且幂等，即 op(x,x)=x；查询会重叠取段，不能直接使用求和、异或或乘法。
- 只支持静态查询；修改原数组不会更新 ST 表。
- 空数组可以构造，但不可调用 query；所有查询必须满足 l<r。

也可以在仓库根目录执行：

```powershell
python tools/cp.py example "data_structures/sparse_table"
```
