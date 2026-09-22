# 快速幂、逆元、素数筛与分解

[模板源码](../../../math/number_theory.hpp) · [完整示例](../../../examples/math/number_theory.cpp) · [使用手册索引](../README.md)

这个头文件提供常用数论基础操作：模快速幂、一般模数下的乘法逆元，以及线性筛范围内的素数判断和质因数分解。

## 接口和使用顺序

| 接口 | 含义 |
| --- | --- |
| `pow_mod(a, exponent, mod)` | 返回 a^exponent mod mod；exponent≥0，mod>0，O(log exponent)。 |
| `inverse_mod(a, mod)` | mod>1；返回 optional<long long>，gcd(a,mod)≠1 时无逆元。O(log mod)。 |
| `PrimeSieve(n)` | 在线性时间和空间内预处理到 n，n≥0。 |
| `sieve.is_prime(x)` | 0≤x≤n 时判断素数，O(1)。 |
| `sieve.factorize(x)` | 1≤x≤n，返回 (质因子,指数) 列表；x=1 返回空，O(log x)。 |
| `sieve.primes / sieve.spf[x]` | 递增素数表 / x≥2 的最小质因子。 |

## 完整例子

下面是可以直接编译的完整程序，数据写在代码里，**不需要输入**。在 workspace 根目录执行文末的 `python tools/cp.py example ...` 命令即可运行并核对输出。

```cpp
#include <iostream>
#include "math/number_theory.hpp"

int main() {
    std::cout << "power=" << cp::pow_mod(2, 10, 1000) << '\n';
    if (auto inverse = cp::inverse_mod(3, 11))
        std::cout << "inverse=" << *inverse << '\n';
    std::cout << std::boolalpha;
    std::cout << "inverse(6,9)_exists=" << cp::inverse_mod(6, 9).has_value() << '\n';

    cp::PrimeSieve sieve(20);
    std::cout << "primes:";
    for (int p : sieve.primes) std::cout << ' ' << p;
    std::cout << '\n';
    std::cout << "prime(19)=" << sieve.is_prime(19) << '\n';
    std::cout << "factor(18):";
    for (auto [p, exponent] : sieve.factorize(18))
        std::cout << ' ' << p << '^' << exponent;
    std::cout << '\n';
    std::cout << "factor(1)_size=" << sieve.factorize(1).size() << '\n';
}
```

### 预期标准输出

```text
power=24
inverse=4
inverse(6,9)_exists=false
primes: 2 3 5 7 11 13 17 19
prime(19)=true
factor(18): 2^1 3^2
factor(1)_size=0
```

## 注意事项

- GNU/Clang 使用原生 __int128 模乘；MSVC 对能放入无符号 64 位的乘积直接取模，否则用精确加倍法，单次模乘 O(log mod)。因此大模数下 pow_mod 的最坏复杂度为 O(log exponent · log mod)，inverse_mod 为 O(log² mod)；均支持完整的正 long long 模数。
- 逆元不要求模数为素数，但必须互质；一定先判断 optional，再取值。
- 筛只负责 `[0,n]`，不能把很大的 x 交给一个很小的筛去分解。
- pow_mod 的 a 可以为负，返回标准的非负模值；mod=1 时结果为 0。

也可以在仓库根目录执行：

```powershell
python tools/cp.py example "math/number_theory"
```
