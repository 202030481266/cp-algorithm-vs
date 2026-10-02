# 异或线性基 XorBasis 与前缀线性基

[模板源码](../../../data_structures/xor_basis.hpp) · [完整示例](../../../examples/data_structures/xor_basis.cpp) · [使用手册索引](../README.md)

从若干数中任取一个子集做异或，用线性基查询能否表示某个值、最大异或和、去重后的第 k 小异或和。本模板允许空子集，因此 0 总在可表示集合中。

`PrefixXorBasis` 是它的区间版本：按顺序插入，每一位保留下标最靠后的基向量，于是只用“下标 ≥ l 的元素”就能求最大异或和；把询问按右端点离线排序，即可回答任意区间 [l,r) 的子集最大异或和。

## 接口和使用顺序

| 接口 | 含义 |
| --- | --- |
| `insert(x)` | 插入 uint64_t；增加秩返回 true，否则返回 false，O(64)。 |
| `contains(x)` | 是否可由已插入元素的某个子集异或得到，O(64)。 |
| `max_xor(initial=0)` | 最大化 initial XOR 子集异或值，O(64)。 |
| `kth(k)` | 去重后第 k 小，k 从 0 开始；optional<uint64_t>，无解为 nullopt；秩变化后的首次有效查询 O(64²)，随后每次 O(64)。 |
| `rank()` | 独立基向量个数；可表示值的数量在数学上为 2^rank。 |
| `PrefixXorBasis::push_back(x)` | 按顺序插入，元素下标依次为 0,1,2,…，O(64)。 |
| `PrefixXorBasis::max_xor(l)` | 只用下标在 `[l, size())` 的元素，求子集最大异或和（允许空子集），O(64)。 |

## 完整例子

下面是可以直接编译的完整程序，数据写在代码里，**不需要输入**。在 workspace 根目录执行文末的 `python tools/cp.py example ...` 命令即可运行并核对输出。

```cpp
#include <cstdint>
#include <iostream>
#include "data_structures/xor_basis.hpp"

int main() {
    cp::XorBasis basis;
    basis.insert(3);
    basis.insert(5);
    std::cout << std::boolalpha;
    std::cout << "independent(6)=" << basis.insert(6) << '\n';
    std::cout << "rank=" << basis.rank() << '\n';
    std::cout << "contains(6)=" << basis.contains(6)
              << " contains(1)=" << basis.contains(1) << '\n';
    std::cout << "maximum=" << basis.max_xor() << '\n';
    std::cout << "sorted:";
    for (std::uint64_t k = 0; k < 4; ++k) {
        if (auto value = basis.kth(k)) std::cout << ' ' << *value;
    }
    std::cout << '\n';
    std::cout << "kth(4)_exists=" << basis.kth(4).has_value() << '\n';
    std::cout << "with_initial_2=" << basis.max_xor(2) << '\n';

    // 前缀线性基：按顺序插入，max_xor(l) 只使用下标 >= l 的元素。
    cp::PrefixXorBasis prefix;
    for (std::uint64_t x : {8, 1, 2, 4}) prefix.push_back(x);
    std::cout << "range_max [0,4)=" << prefix.max_xor(0) << " [1,4)=" << prefix.max_xor(1)
              << " [3,4)=" << prefix.max_xor(3) << '\n';
}
```

### 预期标准输出

```text
independent(6)=false
rank=2
contains(6)=true contains(1)=false
maximum=6
sorted: 0 3 5 6
kth(4)_exists=false
with_initial_2=7
range_max [0,4)=15 [1,4)=7 [3,4)=4
```

## 注意事项

- 消元结果按需缓存：插入独立向量后，下次有效查询自动重建；插入相关向量不触发重建。
- 缓存最多保存 64 个 uint64_t（有效数据 512 字节，另有 vector 开销）；不调用 kth 时不分配缓存数组。kth 虽是 const 接口，仍会更新内部缓存，同一对象的并发访问需要同步。

- 是“任选多个数”的异或；只允许选一个数时应该用 BinaryTrie。
- `kth(0)=0`。题目要求非空子集时，要另外判断是否存在非空子集异或为 0。
- 满秩 64 时，不要在调用端计算 `1ULL << 64`；kth 本身已处理这个边界。
- 先判断 optional 是否有值再解引用，不能用 UINT64_MAX 当作无解标记。
- `PrefixXorBasis` 只能回答“当前已插入前缀”的询问：离线把询问按 r 排序，插入到 r 后再查询 `max_xor(l)`；需要在线时为每个前缀保存一份 64 个基向量和下标的拷贝（空间 O(64n)）。

## 对应课程与练习

- 课程：[讲解136 线性基-上](https://github.com/algorithmzuo/algorithm-journey/tree/main/src/class136)、[讲解137 线性基-下](https://github.com/algorithmzuo/algorithm-journey/tree/main/src/class137)
- 练习：
  - [P3812 线性基](https://www.luogu.com.cn/problem/P3812)：`max_xor`
  - [P3857 彩灯](https://www.luogu.com.cn/problem/P3857)：可表示值的个数为 2^rank
  - [CF1100F Ivan and Burgers](https://www.luogu.com.cn/problem/CF1100F)：`PrefixXorBasis` 离线回答区间最大异或和

也可以在仓库根目录执行：

```powershell
python tools/cp.py example "data_structures/xor_basis"
```
