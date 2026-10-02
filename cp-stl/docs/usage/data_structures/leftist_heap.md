# 左偏树：可合并堆

[模板源码](../../../data_structures/leftist_heap.hpp) · [完整示例](../../../examples/data_structures/leftist_heap.cpp) · [使用手册索引](../README.md)

std::priority_queue 不能高效合并两个堆。左偏树把所有堆放在一个节点池里，每个堆用根节点编号表示，
合并两个堆只沿右链递归，O(log n)。适合“集合合并 + 删除最小值”的题目，配合并查集记录每个元素所在的堆。

## 接口和使用顺序

| 接口 | 含义 |
| --- | --- |
| `LeftistHeap<T, Compare>()` | Compare 默认 `std::less<T>`，堆顶最小；`std::greater<T>` 为大根堆。 |
| `push(value)` | 新建只含 value 的堆，返回节点编号；编号按 push 顺序从 0 开始。 |
| `push(root, value)` | 把 value 插入堆 root，返回新根；root 为 -1 表示空堆。 |
| `meld(a, b)` | 合并两个堆，返回新根，O(log n)。 |
| `top(root) / pop(root)` | 堆顶值 / 删除堆顶并返回新根（堆空时为 -1）。 |
| `value(node)` | 读取任意节点保存的值。 |

## 完整例子

下面是可以直接编译的完整程序，数据写在代码里，**不需要输入**。在 workspace 根目录执行文末的 `python tools/cp.py example ...` 命令即可运行并核对输出。

```cpp
#include <functional>
#include <iostream>
#include "data_structures/leftist_heap.hpp"

int main() {
    cp::LeftistHeap<int> heaps; // 小根堆，所有堆共用节点池
    int a = -1, b = -1;         // -1 表示空堆
    for (int x : {5, 1, 8}) a = heaps.push(a, x);
    for (int x : {3, 7}) b = heaps.push(b, x);
    std::cout << "top(a)=" << heaps.top(a) << " top(b)=" << heaps.top(b) << '\n';
    int c = heaps.meld(a, b); // 合并后 a、b 不再单独使用
    c = heaps.pop(c);         // 删除最小值 1
    std::cout << "after pop top=" << heaps.top(c) << " node2_value=" << heaps.value(2) << '\n';

    cp::LeftistHeap<int, std::greater<int>> max_heap; // 大根堆
    int m = -1;
    for (int x : {4, 9, 1}) m = max_heap.push(m, x);
    std::cout << "max_top=" << max_heap.top(m) << '\n';
}
```

### 预期标准输出

```text
top(a)=1 top(b)=3
after pop top=3 node2_value=8
max_top=9
```

## 注意事项

- 合并后旧根不能再单独使用；被 pop 的节点编号不会复用，可以用 `deleted[node]` 记录。
- 相同值需要按编号决定先后时，用 `std::pair<值, 编号>` 作为 T。
- 查找某个元素所在的堆：并查集维护集合，另开 `root_of[find(x)]` 保存这个集合的堆根，合并、弹出时同步更新。
- 合并沿右链递归，深度 O(log n)，不会爆栈。

## 对应课程与练习

- 课程：[讲解154 左偏树的原理、代码、相关题目](https://github.com/algorithmzuo/algorithm-journey/tree/main/src/class154)、[讲解155 左偏树和懒更新、可持久化左偏树、k短路问题](https://github.com/algorithmzuo/algorithm-journey/tree/main/src/class155)
- 练习：
  - [P3377 左偏树/可并堆](https://www.luogu.com.cn/problem/P3377)：T 取 pair<值, 编号>，并查集找所在的堆
  - [P1456 Monkey King](https://www.luogu.com.cn/problem/P1456)：大根堆：弹出、减半、再插回、合并
  - [P1552 派遣](https://www.luogu.com.cn/problem/P1552)：子树合并大根堆，超预算时弹出最大值

也可以在仓库根目录执行：

```powershell
python tools/cp.py example "data_structures/leftist_heap"
```
