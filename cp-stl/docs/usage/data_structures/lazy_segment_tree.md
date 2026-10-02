# 懒标记线段树：RangeAddSum 与通用 LazySegmentTree

[模板源码](../../../data_structures/lazy_segment_tree.hpp) · [完整示例](../../../examples/data_structures/lazy_segment_tree.cpp) · [使用手册索引](../README.md)

区间修改 + 区间查询。只需要“区间加、区间和”时用最简单的 `RangeAddSum`；区间赋值、取反、仿射变换、
最大子段和等其他组合用通用的 `LazySegmentTree<Info, Tag>`。通用版采用 AtCoder Library 的非递归写法，
常数小于常见的递归写法；预置的 `SumMinMax` + `AssignAdd` 直接支持“区间赋值、区间加，查询区间和、最小值、最大值”。

## 接口和使用顺序

| 接口 | 含义 |
| --- | --- |
| `RangeAddSum(n) / RangeAddSum(a)` | 建立全零数组 / 从 vector<long long> 建树，O(n)。 |
| `add(l, r, delta) / sum(l, r)` | RangeAddSum 的区间加、区间和，O(log n)。 |
| `LazySegmentTree<Info,Tag>(a)` | 从 vector<Info> 建树，O(n)；`(n, value)` 用同一个叶子填满 n 个位置。 |
| `apply(l, r, tag) / apply(p, tag)` | 给区间 / 单点作用一个标记，O(log n)。 |
| `prod(l, r) / all_prod()` | 区间聚合 / 整体聚合；空区间返回 `Info()`。 |
| `set(p, info) / get(p)` | 单点赋值 / 单点读取。 |
| `max_right(l, pred) / min_left(r, pred)` | 线段树上二分：最大的 r（最小的 l）使 `pred(prod(l,r))` 仍为真。 |
| `AssignAdd::assign(x) / AssignAdd::add(x)` | 预置标记：区间赋值为 x / 区间加 x，可以混合使用。 |
| `SumMinMax` | 预置信息：`sum`、`minimum`、`maximum`、`length`；可由 long long 隐式构造为长度 1 的叶子。 |

## 完整例子

下面是可以直接编译的完整程序，数据写在代码里，**不需要输入**。在 workspace 根目录执行文末的 `python tools/cp.py example ...` 命令即可运行并核对输出。

```cpp
#include <iostream>
#include <vector>
#include "data_structures/lazy_segment_tree.hpp"

// 自定义 Info/Tag：01 序列区间取反，查询 1 的个数（必须写在函数外）。
struct Flip {
    bool on = false;
    void apply(const Flip& newer) { on ^= newer.on; }
};
struct Ones {
    int ones = 0, length = 0;
    void apply(const Flip& t) { if (t.on) ones = length - ones; }
    friend Ones operator+(const Ones& a, const Ones& b) { return {a.ones + b.ones, a.length + b.length}; }
};

int main() {
    cp::RangeAddSum seg(std::vector<long long>{1, 2, 3, 4});
    std::cout << "before=" << seg.sum(0, 4) << '\n';
    seg.add(1, 4, 5); // 1,7,8,9
    std::cout << "sum(1,3)=" << seg.sum(1, 3) << '\n';
    seg.add(0, 2, -2); // -1,5,8,9
    std::cout << "after=" << seg.sum(0, 4) << '\n';
    seg.add(2, 2, 100);
    std::cout << "empty=" << seg.sum(2, 2) << '\n';

    // 预置的区间赋值 + 区间加，查询和、最小值、最大值。
    std::vector<long long> a{5, 1, 4, 2, 3};
    cp::LazySegmentTree<cp::SumMinMax, cp::AssignAdd> tree(std::vector<cp::SumMinMax>(a.begin(), a.end()));
    tree.apply(1, 4, cp::AssignAdd::add(10));   // 5,11,14,12,3
    tree.apply(0, 2, cp::AssignAdd::assign(7)); // 7,7,14,12,3
    auto info = tree.prod(0, 5);
    std::cout << "sum=" << info.sum << " min=" << info.minimum << " max=" << info.maximum << '\n';
    int r = tree.max_right(0, [](const cp::SumMinMax& s) { return s.maximum < 13; });
    std::cout << "prefix_max<13 r=" << r << '\n';

    cp::LazySegmentTree<Ones, Flip> lights(6, Ones{0, 1}); // 6 盏灯全灭
    lights.apply(1, 4, Flip{true});                         // 011100
    lights.apply(2, 6, Flip{true});                         // 010011
    std::cout << "on=" << lights.prod(0, 6).ones << " on[0,3)=" << lights.prod(0, 3).ones << '\n';
}
```

### 预期标准输出

```text
before=10
sum(1,3)=15
after=21
empty=0
sum=43 min=3 max=14
prefix_max<13 r=2
on=3 on[0,3)=1
```

## 自定义 Info 和 Tag

1. `Info` 默认构造必须是**单位元**：和为 0、长度为 0、最小值为 +∞ 等。
2. `Info operator+(const Info& left, const Info& right)` 按下标从左到右拼接，可以不满足交换律（如最大子段和、哈希）。
3. `void Info::apply(const Tag& t)` 计算整段作用标记后的新信息；与长度有关时把长度存进 Info。
4. `Tag` 默认构造表示“什么也不做”；`void Tag::apply(const Tag& newer)` 把较新的标记复合到旧标记之后。
5. 结构体写在函数外：C++ 不允许在函数内的局部类里定义友元 `operator+`。

下面是“区间乘、区间加、区间求和”（洛谷 P3373）的 Info/Tag，ModInt 的模数按题目修改：

```cpp
#include "data_structures/lazy_segment_tree.hpp"
#include "math/modint.hpp"
using M = cp::ModInt<998244353>;
struct Affine { // x -> mul * x + add
    M mul = 1, add = 0;
    void apply(const Affine& t) { mul = mul * t.mul; add = add * t.mul + t.add; }
};
struct Sum {
    M sum = 0;
    int length = 0;
    void apply(const Affine& t) { sum = sum * t.mul + t.add * length; }
    friend Sum operator+(const Sum& a, const Sum& b) { return {a.sum + b.sum, a.length + b.length}; }
};
// std::vector<Sum> init(n); init[i] = {a[i], 1};
// cp::LazySegmentTree<Sum, Affine> seg(init);
// seg.apply(l, r, {c, 0}); // 区间乘 c
// seg.apply(l, r, {1, c}); // 区间加 c
// M total = seg.prod(l, r).sum;
```

## 注意事项

- 区间统一为 `[l,r)`，下标从 0 开始；空区间修改无效，空区间查询返回 `Info()`。
- 每个叶子都必须是真实元素（例如 `SumMinMax(0)`、`Ones{0, 1}`）。`LazySegmentTree(n, Info())` 的叶子长度为 0，区间加不会生效。
- `SumMinMax` 的单位元把 minimum/maximum 设为 LLONG_MAX/LLONG_MIN，只作查询哨兵；sum 与 delta×长度必须在 long long 范围内。
- `max_right`/`min_left` 要求谓词对 `Info()` 为真，并且区间扩大时真假只能从真变假。
- `RangeAddSum` 保留原有接口；新代码需要其他标记时直接换成 `LazySegmentTree`，不必再引入 ACL。

## 对应课程与练习

- 课程：[讲解110 线段树原理和代码详解](https://github.com/algorithmzuo/algorithm-journey/tree/main/src/class110)、[讲解111 离散化、二分搜索、特别修改](https://github.com/algorithmzuo/algorithm-journey/tree/main/src/class111)、[讲解112 维护更多类型的信息](https://github.com/algorithmzuo/algorithm-journey/tree/main/src/class112)、[讲解113 区间合并](https://github.com/algorithmzuo/algorithm-journey/tree/main/src/class113)
- 练习：
  - [P3372 线段树 1](https://www.luogu.com.cn/problem/P3372)：`RangeAddSum`
  - [P1253 扶苏的问题](https://www.luogu.com.cn/problem/P1253)：区间赋值 + 区间加 + 区间最大值，直接用 `SumMinMax`/`AssignAdd`
  - [P3870 开关](https://www.luogu.com.cn/problem/P3870)：例子里的 `Flip`/`Ones`
  - [P3373 线段树 2](https://www.luogu.com.cn/problem/P3373)：上面的 `Affine`/`Sum`
  - [P2572 序列操作](https://www.luogu.com.cn/problem/P2572)：自定义 Info：1 的个数、最长连续 1，标记为赋值和取反

也可以在仓库根目录执行：

```powershell
python tools/cp.py example "data_structures/lazy_segment_tree"
```
