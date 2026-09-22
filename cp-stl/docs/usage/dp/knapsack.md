# 01、完全与多重背包

[模板源码](../../../dp/knapsack.hpp) · [完整示例](../../../examples/dp/knapsack.cpp) · [使用手册索引](../README.md)

容量为 C 时，创建 C+1 个 0 的 dp 数组。每加入一种物品调用一次相应函数；dp[c] 始终表示容量不超过 c 时的最大价值。

## 接口和使用顺序

| 接口 | 含义 |
| --- | --- |
| `knapsack01(dp, weight, value)` | 一个物品最多选一次，倒序更新，O(C)；weight≥0。 |
| `knapsack_unbounded(dp, weight, value)` | 物品数量无限，正序更新，O(C)；weight>0。 |
| `knapsack_bounded(dp, weight, value, count)` | 最多选 count 次，二进制拆分，O(C log(count+1))；weight>0、count≥0。 |

## 完整例子

下面是可以直接编译的完整程序，数据写在代码里，**不需要输入**。在 workspace 根目录执行文末的 `python tools/cp.py example ...` 命令即可运行并核对输出。

```cpp
#include <iostream>
#include <vector>
#include "dp/knapsack.hpp"

int main() {
    const int capacity = 10;
    std::vector<long long> once(capacity + 1), unlimited(capacity + 1), bounded(capacity + 1);
    cp::knapsack01(once, 2, 3);
    cp::knapsack01(once, 3, 4);
    cp::knapsack_unbounded(unlimited, 2, 3);
    cp::knapsack_unbounded(unlimited, 3, 4);
    cp::knapsack_bounded(bounded, 2, 3, 2);
    cp::knapsack_bounded(bounded, 3, 4, 1);
    std::cout << "01=" << once[capacity] << '\n';
    std::cout << "unbounded=" << unlimited[capacity] << '\n';
    std::cout << "bounded=" << bounded[capacity] << '\n';
    std::cout << "bounded_capacity_6=" << bounded[6] << '\n';
}
```

### 预期标准输出

```text
01=7
unbounded=15
bounded=10
bounded_capacity_6=7
```

两种物品分别为（重量 2，价值 3）和（重量 3，价值 4）。容量 10 时，01 背包两件都选得 7；完全背包选择 5 件第一种得 15；多重背包限制为 2 件第一种、1 件第二种，最大为 10。

## 注意事项

- 这是“不超过容量”的最大价值，不要求恰好装满；允许不选任何物品，故初值为 0。
- 恰好装满要另写不可达状态处理，不能直接把本接口套在负无穷 dp 上。
- dp.size() 是容量上限加 1，不是物品个数；对每种物品调用一次函数。
- 价值及合并后的 value×count 必须放得进 long long。完全背包不允许零重量，否则正价值物品会产生无界解。

也可以在仓库根目录执行：

```powershell
python tools/cp.py example "dp/knapsack"
```
