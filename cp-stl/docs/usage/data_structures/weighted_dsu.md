# 带权并查集：维护势能差

[模板源码](../../../data_structures/weighted_dsu.hpp) · [完整示例](../../../examples/data_structures/weighted_dsu.cpp) · [使用手册索引](../README.md)

在并查集的每条边上记录“到父亲的势能差”，路径压缩时累加。这样可以回答同一集合内任意两点的差值
`potential[b] - potential[a]`，并在加入新约束时检查是否矛盾。区间和推导、食物链、奇偶性/异或关系都是这一模型。

## 接口和使用顺序

| 接口 | 含义 |
| --- | --- |
| `WeightedDSU<T, Add, Sub>(n)` | 默认 `T=long long`、加减法；异或关系用 `WeightedDSU<int, std::bit_xor<int>, std::bit_xor<int>>`。 |
| `merge(a, b, w)` | 加入约束 `potential[b] - potential[a] = w`；已连通且矛盾时返回 false，否则返回 true。 |
| `diff(a, b)` | `potential[b] - potential[a]`，optional；不在同一集合时为 nullopt。 |
| `potential(x)` | x 相对所在集合根的势能。 |
| `find / same / size` | 与普通 DSU 相同，均摊 O(α(n))。 |

## 完整例子

下面是可以直接编译的完整程序，数据写在代码里，**不需要输入**。在 workspace 根目录执行文末的 `python tools/cp.py example ...` 命令即可运行并核对输出。

```cpp
#include <functional>
#include <iostream>
#include "data_structures/weighted_dsu.hpp"

int main() {
    // 前缀和 S[0..5]：已知区间和 [l,r) 就是约束 S[r] - S[l] = sum。
    cp::WeightedDSU<long long> dsu(6);
    dsu.merge(0, 3, 10); // a0+a1+a2 = 10
    dsu.merge(3, 5, 7);  // a3+a4 = 7
    dsu.merge(2, 5, 9);  // a2+a3+a4 = 9
    std::cout << std::boolalpha;
    std::cout << "sum[0,2)=" << *dsu.diff(0, 2) << " sum[0,5)=" << *dsu.diff(0, 5) << '\n';
    std::cout << "sum[0,1)_known=" << dsu.diff(0, 1).has_value() << '\n';
    std::cout << "consistent=" << dsu.merge(0, 5, 17) << " conflict=" << !dsu.merge(0, 5, 100) << '\n';

    // 异或关系：Add 与 Sub 都用异或。
    cp::WeightedDSU<int, std::bit_xor<int>, std::bit_xor<int>> parity(3);
    parity.merge(0, 1, 1); // x0 != x1
    parity.merge(1, 2, 1); // x1 != x2
    std::cout << "x0^x2=" << *parity.diff(0, 2) << '\n';
}
```

### 预期标准输出

```text
sum[0,2)=8 sum[0,5)=17
sum[0,1)_known=false
consistent=true conflict=true
x0^x2=0
```

## 注意事项

- T 与 Add、Sub 必须构成交换群（加法、异或、模 k 加法都可以）；模 k 的“食物链”类问题可写一个取模的 Add/Sub 函数对象。
- 区间和问题把区间 [l,r) 看成前缀和的约束 `S[r] - S[l] = sum`，需要 n+1 个点。
- merge 在已连通时不会修改结构，只检查约束是否成立。

## 对应课程与练习

- 课程：[讲解156 带权并查集的原理和扩展题目](https://github.com/algorithmzuo/algorithm-journey/tree/main/src/class156)
- 练习：
  - [P8779 推导部分和](https://www.luogu.com.cn/problem/P8779)：例子中的前缀和模型
  - [P2294 狡猾的商人](https://www.luogu.com.cn/problem/P2294)：检查账本是否矛盾
  - [P1196 银河英雄传说](https://www.luogu.com.cn/problem/P1196)：势能表示到队首的距离
  - [P2024 食物链](https://www.luogu.com.cn/problem/P2024)：模 3 的势能差

也可以在仓库根目录执行：

```powershell
python tools/cp.py example "data_structures/weighted_dsu"
```
