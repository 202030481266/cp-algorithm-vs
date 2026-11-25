# CDQ 分治与三维偏序

[模板源码](../../../basic/cdq.hpp) · [完整示例](../../../examples/basic/cdq.cpp) · [使用手册索引](../README.md)

CDQ 分治把“前面的元素对后面的元素的贡献”按下标对半拆开：左半内部、右半内部递归处理，
左半对右半的贡献因为下标这一维已经自动满足，只剩下其余的维度，用排序加树状数组一次算完。
它把一个维度换成了一个 log，常用来离线替代树套树。`dominance_count_3d` 是最常用的三维偏序，`cdq_divide` 是通用的骨架。

## 接口和使用顺序

| 接口 | 含义 |
| --- | --- |
| `dominance_count_3d(points)` | points 是 `vector<array<int,3>>`。对每个点返回“其他点中三维都 ≤ 它的个数”，O(n log² n)。 |
| `cdq_divide(l, r, cross)` | 处理 [l,r)：递归左半 → `cross(l, mid, r)` → 递归右半。cross 负责计算 [l,mid) 对 [mid,r) 的贡献。 |

## 完整例子

下面是可以直接编译的完整程序，数据写在代码里，**不需要输入**。在 workspace 根目录执行文末的 `python tools/cp.py example ...` 命令即可运行并核对输出。

```cpp
#include <algorithm>
#include <array>
#include <iostream>
#include <vector>
#include "basic/cdq.hpp"

int main() {
    // 三维偏序：每个点有多少个其他的点，三个坐标都不超过它。
    std::vector<std::array<int, 3>> points{{1, 1, 1}, {2, 2, 2}, {1, 2, 3}, {2, 2, 2}, {3, 1, 2}, {3, 3, 3}};
    std::cout << "dominated:";
    for (int count : cp::dominance_count_3d(points)) std::cout << ' ' << count;
    std::cout << '\n';

    // 骨架 cdq_divide：选出最长的点序列，要求下标递增且 x、y 都严格递增。
    // dp[j] = max(dp[i] + 1)，i < j、x_i < x_j、y_i < y_j；调用 cross 时左半的 dp 已经是最终值。
    std::vector<std::array<int, 2>> p{{1, 5}, {2, 2}, {3, 3}, {2, 4}, {4, 4}, {5, 6}};
    std::vector<int> dp(p.size(), 1);
    cp::cdq_divide(0, int(p.size()), [&](int l, int mid, int r) {
        for (int j = mid; j < r; ++j)
            for (int i = l; i < mid; ++i) // 示例直接枚举；实际题目把两半各自按 x 排序，用树状数组维护 y
                if (p[i][0] < p[j][0] && p[i][1] < p[j][1]) dp[j] = std::max(dp[j], dp[i] + 1);
    });
    std::cout << "dp:";
    for (int x : dp) std::cout << ' ' << x;
    std::cout << '\n';
}
```

### 预期标准输出

```text
dominated: 0 2 1 2 1 5
dp: 1 1 2 1 3 4
```

## 注意事项

- `dominance_count_3d` 把完全相同的点互相计入（陌上花开的定义）；坐标可以是任意 int，包括负数。
- 严格小于的偏序：把条件改写成 ≤，例如整数的 a_j < a_i 等价于 a_j ≤ a_i − 1，查询点和被统计的点分开处理。
- `cdq_divide` 调用 cross 时，左半的结果已经是最终值、右半还没有开始，因此可以做 DP 转移；cross 内部可以随意排序，但要在返回前恢复，或者只对下标数组排序。
- cross 的总长度是 O(n log n)，里面用树状数组时总复杂度 O(n log² n)。用完树状数组要按“加了什么就减什么”的方式撤销，不能整棵清空。
- 带修改的问题把时间当作第一维：例如动态逆序对是（时间，位置，值）的三维偏序。

## 对应课程与练习

- 课程：[讲解170（CDQ 分治）](https://github.com/algorithmzuo/algorithm-journey/tree/main/src/class170)–[171](https://github.com/algorithmzuo/algorithm-journey/tree/main/src/class171)
- 练习：
  - [P3810 【模板】三维偏序（陌上花开）](https://www.luogu.com.cn/problem/P3810)：`dominance_count_3d`，再统计每个结果出现的次数
  - [P3157 [CQOI2011] 动态逆序对](https://www.luogu.com.cn/problem/P3157)：把删除时间当作第一维
  - [P2487 [SDOI2011] 拦截导弹](https://www.luogu.com.cn/problem/P2487)：`cdq_divide` 做二维限制的 LIS，同示例第二部分
  - [P4169 [Violet] 天使玩偶/SJY摆棋子](https://www.luogu.com.cn/problem/P4169)：四个方向各做一次带时间的二维偏序

也可以在仓库根目录执行：

```powershell
python tools/cp.py example "basic/cdq"
```
