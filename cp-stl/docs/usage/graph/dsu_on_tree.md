# 树上启发式合并（DSU on tree）

[模板源码](../../../graph/dsu_on_tree.hpp) · [完整示例](../../../examples/graph/dsu_on_tree.cpp) · [使用手册索引](../README.md)

回答“每个点的子树里有什么”的离线问题（不同颜色数、出现最多的颜色、某深度的点数……）。
做法是保留重儿子的统计结果，再把轻儿子的子树逐点加进去；每个点只在 O(log n) 个轻边上被重复加入，
因此总共 O(n log n) 次 add/remove。你只需要写“加入一个点”“删除一个点”“读答案”三个函数。

## 接口和使用顺序

| 接口 | 含义 |
| --- | --- |
| `dsu_on_tree(g, root, add, remove, answer)` | 非递归实现，g 为无向树邻接表。 |
| `add(x)` | 把点 x 加入统计。 |
| `remove(x)` | 把点 x 从统计中删除（清空轻儿子的贡献时调用）。 |
| `answer(v)` | 此时统计中恰好是 v 子树的所有点。 |

## 完整例子

下面是可以直接编译的完整程序，数据写在代码里，**不需要输入**。在 workspace 根目录执行文末的 `python tools/cp.py example ...` 命令即可运行并核对输出。

```cpp
#include <iostream>
#include <vector>
#include "graph/dsu_on_tree.hpp"

int main() {
    // 边：0-1, 0-2, 1-3, 1-4, 2-5；求每棵子树中不同颜色的个数。
    std::vector<std::vector<int>> g{{1, 2}, {0, 3, 4}, {0, 5}, {1}, {1}, {2}};
    std::vector<int> color{1, 2, 1, 3, 2, 1};
    std::vector<int> count(4), answer(6);
    int distinct = 0;
    cp::dsu_on_tree(g, 0,
                    [&](int v) { if (count[color[v]]++ == 0) ++distinct; },
                    [&](int v) { if (--count[color[v]] == 0) --distinct; },
                    [&](int v) { answer[v] = distinct; });
    std::cout << "distinct colors:";
    for (int x : answer) std::cout << ' ' << x;
    std::cout << '\n';
}
```

### 预期标准输出

```text
distinct colors: 3 2 1 1 1 1
```

## 注意事项

- answer 按后序调用（儿子先于父亲）；结束后统计为空。
- 统计结构用数组计数，避免 map。remove 只会在“清空整棵子树、统计归零”时成批调用，所以最大值这类不可撤销的量，可以记录已加入的点数，减到 0 时直接重置（CF600E 就这样维护“出现最多的颜色之和”）。
- 需要按深度统计时，在 add/remove 中使用 depth[x]；询问“v 子树中深度为 d 的点”同样在 answer(v) 中读取。
- 也可以用 [线段树合并](../data_structures/segment_tree_merge.md) 解决同类问题，后者在线、但常数和内存更大。

## 对应课程与练习

- 课程：[讲解163 树上启发式合并的原理和相关题目](https://github.com/algorithmzuo/algorithm-journey/tree/main/src/class163)
- 练习：
  - [U41492 树上数颜色](https://www.luogu.com.cn/problem/U41492)：例子中的写法
  - [CF600E Lomsat gelral](https://www.luogu.com.cn/problem/CF600E)：子树中出现次数最多的颜色之和
  - [CF208E Blood Cousins](https://www.luogu.com.cn/problem/CF208E)：按深度计数

也可以在仓库根目录执行：

```powershell
python tools/cp.py example "graph/dsu_on_tree"
```
