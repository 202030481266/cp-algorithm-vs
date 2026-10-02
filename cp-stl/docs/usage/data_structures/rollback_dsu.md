# 可撤销并查集与线段树分治

[模板源码](../../../data_structures/rollback_dsu.hpp) · [完整示例](../../../examples/data_structures/rollback_dsu.cpp) · [使用手册索引](../README.md)

`RollbackDSU` 只按大小合并、不做路径压缩，每次合并记录一条历史，可以撤销到任意快照。
它最常和**线段树分治**一起用：当一条边（或任意元素）只在一段时间 [l,r) 内存在时，把它挂到时间线段树的
O(log T) 个节点上，深度优先遍历时加入、回溯时撤销，从而离线求出每个时刻的答案。`SegmentTreeDivide` 封装了这个遍历过程。

## 接口和使用顺序

| 接口 | 含义 |
| --- | --- |
| `RollbackDSU(n)` | 0-based；`find` O(log n)，不做路径压缩。 |
| `merge / same / size / components()` | 与普通 DSU 相同；merge 已连通时返回 false。 |
| `snapshot() / rollback(s)` | 记录当前状态 / 撤销 s 之后的全部 merge。 |
| `SegmentTreeDivide<Item>(T)` | 时间点 0..T-1。 |
| `add(l, r, item)` | item 在时间 [l,r) 内生效。 |
| `run(save, apply, restore, leaf)` | 进入节点先 `state = save()`，再 `apply(item)`；到达叶子调用 `leaf(t)`；离开时 `restore(state)`。 |

## 完整例子

下面是可以直接编译的完整程序，数据写在代码里，**不需要输入**。在 workspace 根目录执行文末的 `python tools/cp.py example ...` 命令即可运行并核对输出。

```cpp
#include <iostream>
#include <vector>
#include "data_structures/rollback_dsu.hpp"

int main() {
    cp::RollbackDSU dsu(4);
    dsu.merge(0, 1);
    int saved = dsu.snapshot();
    dsu.merge(1, 2);
    std::cout << std::boolalpha << "components=" << dsu.components() << '\n';
    dsu.rollback(saved); // 撤销 merge(1,2)
    std::cout << "after rollback=" << dsu.components() << " same(0,2)=" << dsu.same(0, 2) << '\n';

    // 线段树分治：边只在时间段 [l,r) 内存在，离线求每个时刻的连通块数。
    struct TimedEdge { int u, v; };
    cp::SegmentTreeDivide<TimedEdge> divide(4); // 时刻 0..3
    divide.add(0, 3, {0, 1});
    divide.add(1, 4, {1, 2});
    divide.add(2, 3, {2, 3});
    cp::RollbackDSU graph(4);
    std::vector<int> answer(4);
    divide.run([&] { return graph.snapshot(); },
               [&](const TimedEdge& e) { graph.merge(e.u, e.v); },
               [&](int state) { graph.rollback(state); },
               [&](int t) { answer[t] = graph.components(); });
    std::cout << "components by time:";
    for (int x : answer) std::cout << ' ' << x;
    std::cout << '\n';
}
```

### 预期标准输出

```text
components=2
after rollback=3 same(0,2)=false
components by time: 3 2 1 3
```

## 注意事项

- 可撤销并查集不能路径压缩，否则无法按记录撤销；按大小合并保证 find 为 O(log n)。
- 线段树分治总复杂度 O((T + 元素数) log T × 单次操作)；DSU 版本为 O(m log T log n)。
- 除了 DSU，任何“能加入、能撤销”的结构都可以放进 save/apply/restore，例如线性基（拷贝一份）、计数器。
- 判断二分图时，把每个点拆成 2 个（颜色 0/1）：边 (u,v) 合并 (u0,v1) 和 (u1,v0)，出现 same(u0,u1) 就有奇环。

## 对应课程与练习

- 课程：[讲解165 可持久化并查集、可撤销并查集](https://github.com/algorithmzuo/algorithm-journey/tree/main/src/class165)、[讲解166 线段树分治-上](https://github.com/algorithmzuo/algorithm-journey/tree/main/src/class166)、[讲解167 线段树分治-下](https://github.com/algorithmzuo/algorithm-journey/tree/main/src/class167)
- 练习：
  - [LOJ 121 离线动态图连通性](https://loj.ac/p/121)：例子中的写法
  - [P5787 二分图 /【模板】线段树分治](https://www.luogu.com.cn/problem/P5787)：拆点判断奇环
  - [AT_abc302_h ABC302H Ball Collector](https://www.luogu.com.cn/problem/AT_abc302_h)：可撤销并查集 + DFS 回溯
  - [CF1681F Unique Occurrences](https://www.luogu.com.cn/problem/CF1681F)：按颜色分时间段的线段树分治

也可以在仓库根目录执行：

```powershell
python tools/cp.py example "data_structures/rollback_dsu"
```
