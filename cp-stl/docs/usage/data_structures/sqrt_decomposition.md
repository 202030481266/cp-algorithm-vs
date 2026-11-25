# 分块：SqrtBlocks 与区间加、区间小于 x 的个数

[模板源码](../../../data_structures/sqrt_decomposition.hpp) · [完整示例](../../../examples/data_structures/sqrt_decomposition.cpp) · [使用手册索引](../README.md)

分块把序列切成长度约 √n 的块：一次区间操作最多碰到两个零散的块（逐个元素处理）和若干个整块（用块上的标记或预处理的信息处理）。
它的复杂度不如线段树，但整块上可以维护线段树难以合并的信息，例如一份排好序的副本。
`SqrtBlocks` 只负责下标的划分和区间的拆分；`RangeAddCountLess` 是一个完整的例子，也可以直接拿来用。

## 接口和使用顺序

| 接口 | 含义 |
| --- | --- |
| `SqrtBlocks blocks(n, block_size=0)` | 把 [0,n) 分块；block_size 传 0 取 ⌊√n⌋。成员 `n`、`block_size`、`block_count`。 |
| `id(i)` / `begin(b)` / `end(b)` | 下标 i 所在的块；块 b 的下标范围 [begin, end)。 |
| `for_each(l, r, part, whole)` | 拆分 [l,r)：零散部分调用 `part(b, lo, hi)`（[lo,hi) 在块 b 内，最多两次），整块调用 `whole(b)`。 |
| `RangeAddCountLess<T> s(a, block_size=0)` | 由数组构造；`add(l, r, delta)` 区间加，`count_less(l, r, x)` 返回区间内小于 x 的个数，`get(i)` 单点值。 |

## 完整例子

下面是可以直接编译的完整程序，数据写在代码里，**不需要输入**。在 workspace 根目录执行文末的 `python tools/cp.py example ...` 命令即可运行并核对输出。

```cpp
#include <iostream>
#include <vector>
#include "data_structures/sqrt_decomposition.hpp"

int main() {
    // 现成的结构：区间加、区间内小于 x 的个数。
    cp::RangeAddCountLess<long long> blocks(std::vector<long long>{5, 1, 4, 2, 8, 7, 3, 6, 9});
    std::cout << "less than 5 in [0,9): " << blocks.count_less(0, 9, 5) << '\n';
    blocks.add(2, 7, 10); // 5 1 14 12 18 17 13 6 9
    std::cout << "less than 13 in [1,8): " << blocks.count_less(1, 8, 13) << " a[4]=" << blocks.get(4) << '\n';

    // 自己写分块：SqrtBlocks 负责把区间拆成零散部分和整块。这里维护区间加、区间和。
    std::vector<long long> a{1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    cp::SqrtBlocks layout(int(a.size()));
    std::vector<long long> block_sum(layout.block_count), block_add(layout.block_count);
    for (int i = 0; i < layout.n; ++i) block_sum[layout.id(i)] += a[i];
    auto add = [&](int l, int r, long long delta) {
        layout.for_each(l, r,
            [&](int b, int lo, int hi) { for (int i = lo; i < hi; ++i) a[i] += delta; block_sum[b] += delta * (hi - lo); },
            [&](int b) { block_add[b] += delta; });
    };
    auto sum = [&](int l, int r) {
        long long total = 0;
        layout.for_each(l, r,
            [&](int b, int lo, int hi) { for (int i = lo; i < hi; ++i) total += a[i] + block_add[b]; },
            [&](int b) { total += block_sum[b] + block_add[b] * (layout.end(b) - layout.begin(b)); });
        return total;
    };
    add(1, 9, 100);
    std::cout << "blocks=" << layout.block_count << " size=" << layout.block_size
              << " sum[0,10)=" << sum(0, 10) << " sum[2,5)=" << sum(2, 5) << '\n';
}
```

### 预期标准输出

```text
less than 5 in [0,9): 4
less than 13 in [1,8): 3 a[4]=18
blocks=4 size=3 sum[0,10)=855 sum[2,5)=312
```

## 注意事项

- 区间恰好覆盖一个块时走 `whole`，所以零散部分一定不是完整的块；`part` 里修改了元素之后，记得重算这个块的信息。
- `RangeAddCountLess` 的 add 是 O(B log B + n/B)，count_less 是 O(B + (n/B) log B)。默认块长 √n；询问明显多于修改时把块长调到 √(n log n) 左右更快。
- “大于等于 x 的个数”是 `(r - l) - count_less(l, r, x)`，“小于等于 x”是 `count_less(l, r, x + 1)`（整数）。
- 需要的块信息更复杂时，照示例第二部分的写法，用两个 lambda 自己维护。
- 只有区间询问、没有修改时，先考虑[莫队](mos_algorithm.md)。

## 对应课程与练习

- 课程：[讲解172（分块）](https://github.com/algorithmzuo/algorithm-journey/tree/main/src/class172)–[175](https://github.com/algorithmzuo/algorithm-journey/tree/main/src/class175)
- 练习：
  - [P2801 教主的魔法](https://www.luogu.com.cn/problem/P2801)：`RangeAddCountLess`，答案是区间长度减去 `count_less`
  - [LOJ 6278 数列分块入门 2](https://loj.ac/p/6278)：区间加、区间内小于 x 的个数
  - [LOJ 6279 数列分块入门 3](https://loj.ac/p/6279)：区间加、区间内 x 的前驱，在排好序的块里二分
  - [P4168 [Violet] 蒲公英](https://www.luogu.com.cn/problem/P4168)：区间众数，预处理块与块之间的答案

也可以在仓库根目录执行：

```powershell
python tools/cp.py example "data_structures/sqrt_decomposition"
```
