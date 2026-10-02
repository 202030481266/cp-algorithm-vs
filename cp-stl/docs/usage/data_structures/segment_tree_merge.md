# 线段树合并与分裂

[模板源码](../../../data_structures/segment_tree_merge.hpp) · [完整示例](../../../examples/data_structures/segment_tree_merge.cpp) · [使用手册索引](../README.md)

很多“每个点一棵值域计数线段树”的问题，要把子树的信息合并到父亲，或者把一棵树按值域拆成两棵。
`MergeableSegmentTrees` 把所有树放在同一个节点池里：每棵树用根编号表示，0 是空树。
合并时递归地把两棵树的对应节点相加，总代价不超过所有树节点数之和，即 n 次插入后合并 O(n log V)。

## 接口和使用顺序

| 接口 | 含义 |
| --- | --- |
| `MergeableSegmentTrees(n, reserve=0)` | 所有树的值域都是 [0,n)；可预留节点数。 |
| `add(root, pos, delta)` | `cnt[pos] += delta`；root 是引用，空树会被新建并写回。 |
| `merge(a, b)` | 把 b 合并进 a，返回新根；之后不能再单独使用 a、b 的旧根。 |
| `split(root, l, r)` | 取出值在 [l,r) 的部分作为新树返回，root 保留剩余部分。 |
| `sum(root, l, r) / total(root)` | 值在 [l,r) 的计数和 / 整棵树的计数和。 |
| `kth(root, k)` | 计数非负时，第 k 小（从 0 开始）元素的值；k ≥ total 返回 -1。 |
| `max_pos(root) / max_count(root)` | 计数最大的值（并列取最小）/ 最大计数；空树返回 -1 / LLONG_MIN。 |

## 完整例子

下面是可以直接编译的完整程序，数据写在代码里，**不需要输入**。在 workspace 根目录执行文末的 `python tools/cp.py example ...` 命令即可运行并核对输出。

```cpp
#include <iostream>
#include "data_structures/segment_tree_merge.hpp"

int main() {
    cp::MergeableSegmentTrees pool(10); // 所有树的值域都是 [0,10)
    int a = 0, b = 0;                   // 0 表示空树
    pool.add(a, 3, 2);                  // a = {3,3}
    pool.add(a, 7, 1);                  // a = {3,3,7}
    pool.add(b, 5, 4);                  // b = {5,5,5,5}
    a = pool.merge(a, b);               // a = {3,3,5,5,5,5,7}，b 不再单独使用
    std::cout << "total=" << pool.total(a) << " kth(2)=" << pool.kth(a, 2)
              << " mode=" << pool.max_pos(a) << " mode_count=" << pool.max_count(a) << '\n';

    int c = pool.split(a, 4, 10); // c 取走值在 [4,10) 的部分
    std::cout << "a=" << pool.total(a) << " c=" << pool.total(c)
              << " c.sum[6,10)=" << pool.sum(c, 6, 10) << '\n';
}
```

### 预期标准输出

```text
total=7 kth(2)=5 mode=5 mode_count=4
a=2 c=5 c.sum[6,10)=1
```

## 注意事项

- 合并是破坏性的：b 的节点被并入 a。若之后还要查询合并前的子树，先把答案记下来再合并（离线 DFS 后序处理）。
- `max_pos`/`max_count` 只考虑**已经创建过的叶子**：计数被减到 0 的叶子仍参与比较，没有出现过的值不参与。树上差分（+1/−1）后计数非负时，这正是“出现次数最多、编号最小”的值。
- 节点数约为 插入次数 × (⌈log₂n⌉+1)，split 每次再新建 O(log n) 个节点；递归深度 O(log n)。
- 树上每个点一棵树时，按 DFS 后序 `root[u] = merge(root[u], root[child])`；用显式栈或 BFS 逆序处理即可避免深递归。

## 对应课程与练习

- 课程：[讲解181 线段树的合并与分裂-上](https://github.com/algorithmzuo/algorithm-journey/tree/main/src/class181)、[讲解182 线段树的合并与分裂-下](https://github.com/algorithmzuo/algorithm-journey/tree/main/src/class182)
- 练习：
  - [P4556 雨天的尾巴](https://www.luogu.com.cn/problem/P4556)：树上差分 + 合并，`max_pos` 求出现最多的救济粮
  - [P3605 Promotion Counting](https://www.luogu.com.cn/problem/P3605)：子树内比自己大的个数：合并后 `sum`
  - [P3224 永无乡](https://www.luogu.com.cn/problem/P3224)：并查集合并时合并值域树，`kth` 查第 k 小
  - [P5494 线段树分裂](https://www.luogu.com.cn/problem/P5494)：`split` + `merge` + `kth` 的模板题

也可以在仓库根目录执行：

```powershell
python tools/cp.py example "data_structures/segment_tree_merge"
```
