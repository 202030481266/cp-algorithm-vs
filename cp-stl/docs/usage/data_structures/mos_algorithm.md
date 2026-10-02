# 莫队算法 MosAlgorithm：普通莫队、回滚莫队、带修莫队

[模板源码](../../../data_structures/mos_algorithm.hpp) · [完整示例](../../../examples/data_structures/mos_algorithm.cpp) · [使用手册索引](../README.md)

离线处理大量区间询问：把询问按左端点分块、块内按右端点排序，让区间的两个端点总共只移动 O(n√q) 次。
`MosAlgorithm` 负责登记询问和排序；`run` 时每移动一步调用你提供的加入 / 删除函数，到达询问区间后调用 `answer(询问编号)`。
删除困难（例如维护最大值、众数）时用 `run_rollback`，只加入、靠快照撤销；有单点修改时用 `MosAlgorithmWithUpdates`。

## 接口和使用顺序

| 接口 | 含义 |
| --- | --- |
| `MosAlgorithm(n, queries={})` | 下标范围 [0,n)；可以直接传入 vector<pair<int,int>> 形式的全部询问 `[l,r)`。 |
| `add_query(l, r) / query_count()` | 登记询问 `[l,r)`，返回询问编号（从 0 开始，即登记顺序）/ 已登记的询问数。 |
| `run(add, remove, answer)` | 普通莫队；加入、删除与方向无关时的写法。 |
| `run(add_left, add_right, remove_left, remove_right, answer)` | 需要区分左右端点时的写法。 |
| `run_rollback(add_left, add_right, snapshot, rollback, answer)` | 回滚莫队，只加不删：`snapshot()` 返回当前状态，`rollback(s)` 撤销到该状态；开始时统计必须为空，结束后恢复为空。 |
| `MosAlgorithmWithUpdates(n)` | 带修莫队，按输入顺序登记：`add_update()` 返回修改编号，`add_query(l, r)` 返回询问编号；询问看到在它之前登记的全部修改。 |
| `run(add, remove, apply_update, answer)` | `apply_update(k, l, r)` 在当前区间为 [l,r) 时切换第 k 次修改；结束前撤销全部修改。 |

## 完整例子

下面是可以直接编译的完整程序，数据写在代码里，**不需要输入**。在 workspace 根目录执行文末的 `python tools/cp.py example ...` 命令即可运行并核对输出。

```cpp
#include <algorithm>
#include <iostream>
#include <utility>
#include <vector>
#include "data_structures/mos_algorithm.hpp"

int main() {
    std::vector<int> a{1, 2, 1, 3, 2, 1};
    cp::MosAlgorithm mo(int(a.size()), {{0, 3}, {1, 5}, {2, 6}}); // 区间 [l,r)，可以构造时一次给出
    mo.add_query(0, 6);                                            // 也可以逐个登记，返回询问编号 3
    int q = mo.query_count();

    // 普通莫队：区间内不同数的个数。
    std::vector<int> cnt(4), distinct_answer(q);
    int distinct = 0;
    mo.run([&](int i) { if (cnt[a[i]]++ == 0) ++distinct; },
           [&](int i) { if (--cnt[a[i]] == 0) --distinct; },
           [&](int id) { distinct_answer[id] = distinct; });

    // 回滚莫队：区间众数的出现次数（只加不删，用操作栈撤销）。
    std::vector<int> freq(4), history, best_history, mode_answer(q);
    int best = 0;
    auto add = [&](int i) {
        history.push_back(a[i]);
        best_history.push_back(best);
        best = std::max(best, ++freq[a[i]]);
    };
    mo.run_rollback(add, add,
                    [&] { return int(history.size()); },
                    [&](int size) {
                        while (int(history.size()) > size) {
                            --freq[history.back()];
                            best = best_history.back();
                            history.pop_back();
                            best_history.pop_back();
                        }
                    },
                    [&](int id) { mode_answer[id] = best; });
    for (int id = 0; id < q; ++id)
        std::cout << "query " << id << " distinct=" << distinct_answer[id] << " mode_count=" << mode_answer[id] << '\n';

    // 带修莫队：按输入顺序登记修改和询问，每个询问看到在它之前登记的全部修改。
    std::vector<int> color{1, 2, 1, 3};
    std::vector<std::pair<int, int>> changes; // 第 k 次修改：(位置, 新值)，由调用方保存
    cp::MosAlgorithmWithUpdates timed(int(color.size()));
    timed.add_query(0, 4);         // 询问 0：1 2 1 3
    timed.add_update();            // 修改 0：color[1] = 3
    changes.emplace_back(1, 3);
    timed.add_query(0, 4);         // 询问 1：1 3 1 3
    timed.add_update();            // 修改 1：color[0] = 2
    changes.emplace_back(0, 2);
    timed.add_query(0, 3);         // 询问 2：2 3 1
    std::vector<int> count(4), timed_answer(timed.query_count());
    int kinds = 0;
    auto add_color = [&](int i) { if (count[color[i]]++ == 0) ++kinds; };
    auto remove_color = [&](int i) { if (--count[color[i]] == 0) --kinds; };
    timed.run(add_color, remove_color,
              [&](int k, int l, int r) { // 切换第 k 次修改：与数组中的值交换，再调用一次就是撤销
                  auto& [pos, value] = changes[k];
                  bool inside = l <= pos && pos < r;
                  if (inside) remove_color(pos);
                  std::swap(color[pos], value);
                  if (inside) add_color(pos);
              },
              [&](int id) { timed_answer[id] = kinds; });
    std::cout << "with updates:";
    for (int x : timed_answer) std::cout << ' ' << x;
    std::cout << "\nrestored:";
    for (int x : color) std::cout << ' ' << x; // run 结束前撤销全部修改，数组恢复原样
    std::cout << '\n';
}
```

### 预期标准输出

```text
query 0 distinct=2 mode_count=2
query 1 distinct=3 mode_count=2
query 2 distinct=3 mode_count=2
query 3 distinct=3 mode_count=3
with updates: 3 2 3
restored: 1 2 1 3
```

## 注意事项

- 块长取 n/√q，普通莫队按奇偶块交替排序，减少右端点来回移动；排序键预先算好，比在比较函数里做除法快约一倍。区间端点先扩张再收缩，任何时刻都有 l ≤ r。
- 回滚莫队中，短询问（左右端点在同一块内）会从空状态逐个加入；长询问的右半部分在同一块内只增不减。
- 带修莫队的 `apply_update(k, l, r)` 必须是“切换”：同一次修改第二次调用表示撤销。通常写成：位置在 [l,r) 内就先删旧值，再交换数组值与修改值，然后加新值（见例子）。因为 run 结束前会撤销全部修改，数组和修改列表都恢复原样。
- 回调里只做 O(1) 的工作；需要值域计数时先离散化成小整数，用数组而不是 map。
- 树上莫队：用欧拉序（括号序）把路径变成区间，出现两次的点视为不在路径上，LCA 单独处理。

## 对应课程与练习

- 课程：[讲解176 普通莫队、带修莫队](https://github.com/algorithmzuo/algorithm-journey/tree/main/src/class176)、[讲解177 回滚莫队、树上莫队](https://github.com/algorithmzuo/algorithm-journey/tree/main/src/class177)、[讲解178 莫队二次离线](https://github.com/algorithmzuo/algorithm-journey/tree/main/src/class178)、[讲解179 莫队综合应用](https://github.com/algorithmzuo/algorithm-journey/tree/main/src/class179)
- 练习：
  - [P1494 小 Z 的袜子](https://www.luogu.com.cn/problem/P1494)：普通莫队，维护 Σcnt²
  - [P1903 数颜色 / 维护队列](https://www.luogu.com.cn/problem/P1903)：`MosAlgorithmWithUpdates`，例子中的写法
  - [AT_joisc2014_c 歴史の研究](https://www.luogu.com.cn/problem/AT_joisc2014_c)：`run_rollback`，只增的区间最大值
  - [P5906 相同数最远距离](https://www.luogu.com.cn/problem/P5906)：`run_rollback`

也可以在仓库根目录执行：

```powershell
python tools/cp.py example "data_structures/mos_algorithm"
```
