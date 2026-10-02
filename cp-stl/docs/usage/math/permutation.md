# 康托展开与约瑟夫问题

[模板源码](../../../math/permutation.hpp) · [完整示例](../../../examples/math/permutation.cpp) · [使用手册索引](../README.md)

康托展开把 0..n-1 的一个排列映射为它在全部 n! 个排列中的字典序排名，逆康托展开反过来由排名得到排列；
它们常用于把排列压成整数（状态压缩、BFS 判重）以及“求排列的排名”类题目。约瑟夫问题求报数出列游戏最后剩下的人。

## 接口和使用顺序

| 接口 | 含义 |
| --- | --- |
| `permutation_rank<M>(p)` | 排列 p（值为 0..n-1）的排名（从 0 开始），对 ModInt 类型 M 取模，O(n log n)。 |
| `permutation_rank_exact(p)` | 精确排名，unsigned long long，n ≤ 20，O(n²)。 |
| `kth_permutation(n, k)` | 排名为 k（从 0 开始）的排列，n ≤ 20 且 k < n!。 |
| `josephus(n, k)` | 编号 0..n-1 围成一圈，从 0 开始每数到第 k 个就出列，返回最后剩下的编号。 |

## 完整例子

下面是可以直接编译的完整程序，数据写在代码里，**不需要输入**。在 workspace 根目录执行文末的 `python tools/cp.py example ...` 命令即可运行并核对输出。

```cpp
#include <iostream>
#include <vector>
#include "math/modint.hpp"
#include "math/permutation.hpp"

int main() {
    // 0,1,2 的排列按字典序：012 021 102 120 201 210，201 的排名是 4（从 0 开始）。
    std::vector<int> p{2, 0, 1};
    std::cout << "rank=" << cp::permutation_rank_exact(p) << " rank_mod=" << cp::permutation_rank<cp::Mint>(p) << '\n';
    std::cout << "kth_permutation(3,4):";
    for (int x : cp::kth_permutation(3, 4)) std::cout << ' ' << x;
    std::cout << '\n';

    // 7 个人报数到 3 出列（编号 0..6），出列顺序 2,5,1,6,4,0，最后剩 3。
    std::cout << "josephus(7,3)=" << cp::josephus(7, 3) << " josephus(1e18,2)=" << cp::josephus(1000000000000000000LL, 2) << '\n';
}
```

### 预期标准输出

```text
rank=4 rank_mod=4
kth_permutation(3,4): 2 0 1
josephus(7,3)=3 josephus(1e18,2)=847078495393153024
```

## 注意事项

- 题目给 1..n 的排列时先把每个值减 1；题目的排名通常从 1 开始，输出时加 1。
- permutation_rank 需要模数为质数（内部用 ModInt 计算阶乘），配合 `cp::Mint` 等类型使用。
- josephus 使用递推 J(i+1) = (J(i)+k) mod (i+1)，并在不需要取模时成批跳过，复杂度 O(min(n, k log n))：n 很大、k 很小也能秒出。
- 编号从 1 开始、或者从第 s 个人开始报数时，把结果加 1 或做一次 (结果 + s) mod n 的平移。

## 对应课程与练习

- 课程：[讲解146 康托展开、约瑟夫环、完美洗牌](https://github.com/algorithmzuo/algorithm-journey/tree/main/src/class146)
- 练习：
  - [P5367 康托展开](https://www.luogu.com.cn/problem/P5367)：`permutation_rank<Mint>`，输出时加 1
  - [P8671 约瑟夫环](https://www.luogu.com.cn/problem/P8671)：`josephus(n, k) + 1`
  - [P1088 火星人](https://www.luogu.com.cn/problem/P1088)：`kth_permutation` 或 std::next_permutation

也可以在仓库根目录执行：

```powershell
python tools/cp.py example "math/permutation"
```
