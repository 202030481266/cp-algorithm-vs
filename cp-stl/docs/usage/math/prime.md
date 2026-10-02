# Miller-Rabin 与 Pollard-Rho：64 位素性测试和质因数分解

[模板源码](../../../math/prime.hpp) · [完整示例](../../../examples/math/prime.cpp) · [使用手册索引](../README.md)

线性筛（[number_theory.hpp](number_theory.md)）只能处理不超过筛的范围的数；对单个大数（到 2^64）判断素性、
分解质因数时用本模板。素性测试使用 7 个固定底数的确定性 Miller-Rabin，对全部 64 位整数正确；
分解使用 Brent 改进的 Pollard-Rho，并用 Montgomery 乘法避免 128 位取模，MSVC 上同样快。

## 接口和使用顺序

| 接口 | 含义 |
| --- | --- |
| `is_prime(n)` | n 是否为质数，n 为任意 uint64_t。 |
| `factorize(n)` | 升序质因子，重复出现，例如 12 → {2,2,3}；n ≤ 1 返回空。 |
| `prime_factors(n)` | (质数, 指数) 列表，例如 12 → {(2,2),(3,1)}。 |
| `divisors(n)` | 全部正因数，升序，n ≥ 1。 |
| `euler_phi(n)` | 欧拉函数 φ(n)，n ≥ 1。 |

## 完整例子

下面是可以直接编译的完整程序，数据写在代码里，**不需要输入**。在 workspace 根目录执行文末的 `python tools/cp.py example ...` 命令即可运行并核对输出。

```cpp
#include <cstdint>
#include <iostream>
#include "math/prime.hpp"

int main() {
    std::cout << std::boolalpha;
    std::cout << "is_prime(998244353)=" << cp::is_prime(998244353)
              << " is_prime(561)=" << cp::is_prime(561) // 最小的 Carmichael 数
              << " is_prime(2^61-1)=" << cp::is_prime((1ULL << 61) - 1) << '\n';

    std::uint64_t n = 600851475143ULL;
    std::cout << "factorize(" << n << "):";
    for (auto p : cp::factorize(n)) std::cout << ' ' << p;
    std::cout << '\n';

    std::uint64_t big = 1000000007ULL * 1000000007ULL * 6; // 约 6e18
    std::cout << "prime_factors(" << big << "):";
    for (auto [p, e] : cp::prime_factors(big)) std::cout << ' ' << p << '^' << e;
    std::cout << "\ndivisors(36):";
    for (auto d : cp::divisors(36)) std::cout << ' ' << d;
    std::cout << "\neuler_phi(36)=" << cp::euler_phi(36) << '\n';
}
```

### 预期标准输出

```text
is_prime(998244353)=true is_prime(561)=false is_prime(2^61-1)=true
factorize(600851475143): 71 839 1471 6857
prime_factors(6000000084000000294): 2^1 3^1 1000000007^2
divisors(36): 1 2 3 4 6 9 12 18 36
euler_phi(36)=12
```

## 注意事项

- 大量 ≤ 10⁷ 的数请用线性筛的 `spf`，比 Pollard-Rho 快得多；本模板适合“少量大数”。
- 分解的期望复杂度约 O(n^{1/4}) 次乘法，10⁴ 个随机 64 位数一般在 0.2 秒内完成。
- Pollard-Rho 是随机化算法，但这里的参数序列是固定的，结果确定、可重现。
- 10¹⁸ 以内的数因数个数最多约 1.03×10⁵，divisors 的结果可以放心存下。

## 对应课程与练习

- 课程：[讲解041 最大公约数、同余原理](https://github.com/algorithmzuo/algorithm-journey/tree/main/src/class041)、[讲解097 质数判断、质因子分解、质数筛](https://github.com/algorithmzuo/algorithm-journey/tree/main/src/class097)
- 练习：
  - [U148828 大素数判断](https://www.luogu.com.cn/problem/U148828)：`is_prime`
  - [P4718 Pollard-Rho](https://www.luogu.com.cn/problem/P4718)：输出最大质因子，`factorize(n).back()`
  - [LeetCode 952 按公因数计算最大组件大小](https://leetcode.cn/problems/largest-component-size-by-common-factor/)：分解后用并查集合并

也可以在仓库根目录执行：

```powershell
python tools/cp.py example "math/prime"
```
