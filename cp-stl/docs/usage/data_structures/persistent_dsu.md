# 可持久化并查集

[模板源码](../../../data_structures/persistent_dsu.hpp) · [完整示例](../../../examples/data_structures/persistent_dsu.cpp) · [使用手册索引](../README.md)

需要在任意历史版本上继续合并、或者回到某个历史版本查询连通性时使用。
父亲数组存放在可持久化数组里，每次合并只新建根到叶子的两条路径；按大小合并保证树高 O(log n)，不做路径压缩（路径压缩会让一次查询新建很多节点）。
只需要按栈的顺序撤销最近的合并时，用更快的[可撤销并查集](rollback_dsu.md)。

## 接口和使用顺序

| 接口 | 含义 |
| --- | --- |
| `PersistentDSU dsu(n, reserve_merges=0)` | n 个点。reserve_merges 是预计的合并次数，用来预留内存。 |
| `initial()` | 初始版本（n 个孤立点）的编号。 |
| `merge(version, a, b)` | 在 version 的基础上合并，返回新版本的编号；已经连通时返回 version 本身。O(log² n)。 |
| `same(version, a, b)` / `find(version, x)` | 在指定版本中查询是否连通 / 代表元。O(log² n)。 |
| `size(version, x)` | 指定版本中 x 所在集合的大小。 |

## 完整例子

下面是可以直接编译的完整程序，数据写在代码里，**不需要输入**。在 workspace 根目录执行文末的 `python tools/cp.py example ...` 命令即可运行并核对输出。

```cpp
#include <iostream>
#include <vector>
#include "data_structures/persistent_dsu.hpp"

int main() {
    cp::PersistentDSU dsu(5);
    std::vector<int> version{dsu.initial()};        // version[i]：第 i 次操作之后的版本
    version.push_back(dsu.merge(version[0], 0, 1)); // 1: {0,1}
    version.push_back(dsu.merge(version[1], 2, 3)); // 2: {0,1} {2,3}
    version.push_back(dsu.merge(version[1], 1, 4)); // 3: 从版本 1 分叉，{0,1,4}
    version.push_back(dsu.merge(version[2], 0, 3)); // 4: 接着版本 2，{0,1,2,3}

    std::cout << std::boolalpha;
    std::cout << "v2: same(0,3)=" << dsu.same(version[2], 0, 3) << " size(2)=" << dsu.size(version[2], 2) << '\n';
    std::cout << "v3: same(0,4)=" << dsu.same(version[3], 0, 4) << " same(2,3)=" << dsu.same(version[3], 2, 3) << '\n';
    std::cout << "v4: same(0,3)=" << dsu.same(version[4], 0, 3) << " size(1)=" << dsu.size(version[4], 1) << '\n';
    std::cout << "v0: same(0,1)=" << dsu.same(version[0], 0, 1) << '\n';
}
```

### 预期标准输出

```text
v2: same(0,3)=false size(2)=2
v3: same(0,4)=true same(2,3)=false
v4: same(0,3)=true size(1)=4
v0: same(0,1)=false
```

## 注意事项

- 版本编号是内部节点的下标，不是 0, 1, 2, …；自己用数组保存每次操作之后的版本，如 `version[i] = dsu.merge(version[k], a, b)`。
- “回到第 k 次操作之后的状态”只需要 `version[i] = version[k]`，不产生任何新节点。
- 每次成功的合并新建 2·(⌈log₂n⌉+1) 个节点，每个 8 字节：n = 10⁵ 时一次合并约 290 字节，2×10⁵ 次约 58MB。
- 同一个版本可以被多次分叉，各个分支互不影响。

## 对应课程与练习

- 课程：[讲解165（可持久化并查集、可撤销并查集）](https://github.com/algorithmzuo/algorithm-journey/tree/main/src/class165)
- 练习：
  - [P3402 【模板】可持久化并查集](https://www.luogu.com.cn/problem/P3402)：三种操作分别对应 `merge`、版本赋值和 `same`
  - [P4768 [NOI2018] 归程](https://www.luogu.com.cn/problem/P4768)：按海拔从高到低加边，用版本回答“只走海拔高于 p 的边能到哪些点”（也可以用 Kruskal 重构树）

也可以在仓库根目录执行：

```powershell
python tools/cp.py example "data_structures/persistent_dsu"
```
