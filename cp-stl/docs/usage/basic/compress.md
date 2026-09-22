# 离散化 Compressor

[模板源码](../../../basic/compress.hpp) · [完整示例](../../../examples/basic/compress.cpp) · [使用手册索引](../README.md)

数值很大但不同取值不多时，把原值映射到 `[0,m)`，方便建立树状数组、线段树或频次数组。离散化只保留大小关系，不保留数值之间的距离。

## 接口和使用顺序

| 接口 | 含义 |
| --- | --- |
| `Compressor<T>(a)` | 排序、去重后保存到 values，预处理 O(n log n)。 |
| `index(x)` | 返回已有值 x 的编号，O(log n)；x 必须出现在原数组中。 |
| `lower_bound(x)` | 返回第一个 ≥x 的位置；x 不必存在，无更大值时返回 size()。 |
| `size() / values[i]` | 不同值个数 / 编号 i 对应的原值。 |

## 完整例子

下面是可以直接编译的完整程序，数据写在代码里，**不需要输入**。在 workspace 根目录执行文末的 `python tools/cp.py example ...` 命令即可运行并核对输出。

```cpp
#include <iostream>
#include <vector>
#include "basic/compress.hpp"

int main() {
    std::vector<long long> a{100, -5, 100, 20};
    cp::Compressor<long long> c(a);
    std::cout << "distinct=" << c.size() << '\n';
    std::cout << "indices:";
    for (auto x : a) std::cout << ' ' << c.index(x);
    std::cout << '\n';
    std::cout << "lower_bound(21)=" << c.lower_bound(21) << '\n';
    std::cout << "lower_bound(101)=" << c.lower_bound(101) << '\n';
    std::cout << "original_at_1=" << c.values[1] << '\n';
}
```

### 预期标准输出

```text
distinct=3
indices: 2 0 2 1
lower_bound(21)=2
lower_bound(101)=3
original_at_1=20
```

## 注意事项

- 允许负数、重复值和空数组。
- 不要用压缩后的下标差来计算原数值差，例如编号相差 1 不代表数值相差 1。
- 要把不存在的值用于阈值查询，调用 `lower_bound`，不要调用有存在性断言的 `index`。

也可以在仓库根目录执行：

```powershell
python tools/cp.py example "basic/compress"
```
