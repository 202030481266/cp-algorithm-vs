# FHQ Treap：有序多重集合与序列平衡树

[模板源码](../../../data_structures/treap.hpp) · [完整示例](../../../examples/data_structures/treap.cpp) · [使用手册索引](../README.md)

两种基于分裂/合并（FHQ Treap）的平衡树：

- `OrderedMultiset<T>`：允许重复元素的有序集合，支持排名、第 k 小、前驱、后继。
  它相当于 GNU pb_ds 的 `tree`（排名树），但 **MSVC 也能用**，而且直接支持重复元素。
- `ImplicitTreap<Info, Tag>`：按位置维护一个序列，支持任意位置插入删除、区间翻转、区间旋转（剪切粘贴）、
  区间修改和区间查询。Info/Tag 的写法与 [懒标记线段树](lazy_segment_tree.md) 相同；只翻转时 Info 可以直接用 int。

## 接口和使用顺序

| 接口 | 含义 |
| --- | --- |
| `OrderedMultiset<T, Compare>()` | 空集合；`reserve(n)` 预留节点。 |
| `insert(x) / erase(x)` | 插入一个 x / 删除一个 x（不存在返回 false），期望 O(log n)。 |
| `count_less(x) / count_less_equal(x) / count(x)` | 小于 x、小于等于 x、等于 x 的个数；count_less 即 0-based 排名。 |
| `kth(k)` | 第 k 小（从 0 开始，计重复）。 |
| `prev(x) / next(x)` | 严格小于 x 的最大值 / 严格大于 x 的最小值，optional，不存在为 nullopt。 |
| `ImplicitTreap<Info,Tag>(a)` | O(n) 从数组建树；Tag 省略时为 `NoTag`，不能调用 apply。 |
| `insert(pos, info) / erase(pos) / erase(l, r)` | 插入后位于 pos / 删除一个或一段，期望 O(log n)。 |
| `reverse(l, r) / rotate(l, m, r)` | 翻转 [l,r) / 与 std::rotate 相同，把 [m,r) 移到 [l,m) 之前。 |
| `apply(l, r, tag) / prod(l, r)` | 区间作用标记 / 区间聚合（Info 为数时是区间和）。 |
| `get(pos) / set(pos, info) / to_vector()` | 单点读写 / 中序输出整个序列。 |

## 完整例子

下面是可以直接编译的完整程序，数据写在代码里，**不需要输入**。在 workspace 根目录执行文末的 `python tools/cp.py example ...` 命令即可运行并核对输出。

```cpp
#include <iostream>
#include <vector>
#include "data_structures/treap.hpp"

int main() {
    std::cout << std::boolalpha;
    cp::OrderedMultiset<int> s;
    for (int x : {5, 1, 4, 1, 3}) s.insert(x); // 1 1 3 4 5
    std::cout << "size=" << s.size() << " rank(4)=" << s.count_less(4) << " kth(2)=" << s.kth(2) << '\n';
    s.erase(1);                                 // 只删一个 1：1 3 4 5
    std::cout << "prev(4)=" << *s.prev(4) << " next(4)=" << *s.next(4)
              << " count(1)=" << s.count(1) << " has_next(5)=" << s.next(5).has_value() << '\n';

    cp::ImplicitTreap<int> seq(std::vector<int>{1, 2, 3, 4, 5});
    seq.reverse(1, 4);   // 1 4 3 2 5
    seq.rotate(0, 2, 5); // 把 [2,5) 移到前面：3 2 5 1 4
    seq.insert(0, 9);    // 9 3 2 5 1 4
    seq.erase(4);        // 9 3 2 5 4
    std::vector<int> v = seq.to_vector();
    std::cout << "seq=";
    for (int i = 0; i < int(v.size()); ++i) std::cout << v[i] << (i + 1 < int(v.size()) ? ' ' : '\n');
    std::cout << "sum(1,4)=" << seq.prod(1, 4) << '\n';
}
```

### 预期标准输出

```text
size=5 rank(4)=3 kth(2)=3
prev(4)=3 next(4)=5 count(1)=1 has_next(5)=false
seq=9 3 2 5 4
sum(1,4)=10
```

## 注意事项

- 优先级来自固定种子的 xorshift，结果可重现；期望深度 O(log n)，分裂合并的递归深度很浅。
- `OrderedMultiset` 需要 T 可默认构造（0 号哨兵节点使用）；删除的节点放入空闲表复用。
- Info 不满足交换律且要翻转时（如最大子段和），给 Info 加成员函数 `void reverse()`，交换前缀、后缀等方向相关的字段；模板会自动调用。
- `std::multiset` 能求前驱、后继，但不能在 O(log n) 内求排名和第 k 小；需要这两个操作时用 OrderedMultiset。
- 所有区间左闭右开；`rotate(l, m, r)` 可以表达“剪切 [m,r) 粘贴到 l”。

## 对应课程与练习

- 课程：[讲解151 笛卡尔树、Treap树](https://github.com/algorithmzuo/algorithm-journey/tree/main/src/class151)、[讲解152 FHQ Treap](https://github.com/algorithmzuo/algorithm-journey/tree/main/src/class152)、[讲解153 Splay树](https://github.com/algorithmzuo/algorithm-journey/tree/main/src/class153)
- 练习：
  - [P3369 普通平衡树](https://www.luogu.com.cn/problem/P3369)：OrderedMultiset，注意题目排名从 1 开始
  - [P6136 普通平衡树（数据加强版）](https://www.luogu.com.cn/problem/P6136)：强制在线，10⁶ 次操作
  - [P3391 文艺平衡树](https://www.luogu.com.cn/problem/P3391)：`ImplicitTreap<int>` + `reverse`
  - [P4008 文本编辑器](https://www.luogu.com.cn/problem/P4008)：插入、删除、剪切一段序列
  - [P2042 维护数列](https://www.luogu.com.cn/problem/P2042)：自定义 Info（最大子段和）+ 赋值标记 + 翻转

也可以在仓库根目录执行：

```powershell
python tools/cp.py example "data_structures/treap"
```
