# 扩展欧几里得、线性同余与中国剩余定理

[模板源码](../../../math/crt.hpp) · [完整示例](../../../examples/math/crt.cpp) · [使用手册索引](../README.md)

数论中的“解方程”工具：扩展欧几里得求 ax + by = gcd(a,b)；线性同余方程 ax ≡ b (mod m)；
模数可以不互质的中国剩余定理（扩展 CRT）；以及二元一次不定方程 ax + by = c 的全部整数解。
所有中间乘法都用安全的模乘，不会溢出。

## 接口和使用顺序

| 接口 | 含义 |
| --- | --- |
| `ext_gcd(a, b)` | 返回 `{g, x, y}`，满足 ax + by = g = gcd(a,b) ≥ 0。 |
| `solve_congruence(a, b, m)` | ax ≡ b (mod m)：optional<(最小非负解, 周期 m/g)>；无解为 nullopt。 |
| `crt(remainders, moduli)` | x ≡ rᵢ (mod mᵢ)，模数可以不互质：optional<(x, lcm)>，0 ≤ x < lcm。 |
| `solve_diophantine(a, b, c)` | ax + by = c：optional<{x0, y0, dx, dy}>，通解 x = x0 + k·dx，y = y0 − k·dy。 |

## 完整例子

下面是可以直接编译的完整程序，数据写在代码里，**不需要输入**。在 workspace 根目录执行文末的 `python tools/cp.py example ...` 命令即可运行并核对输出。

```cpp
#include <iostream>
#include "math/crt.hpp"

int main() {
    auto [g, x, y] = cp::ext_gcd(30, 18); // 30x + 18y = gcd
    std::cout << "gcd=" << g << " x=" << x << " y=" << y << " check=" << 30 * x + 18 * y << '\n';

    auto inverse = cp::solve_congruence(3, 1, 10); // 3x ≡ 1 (mod 10)
    std::cout << "3x=1 (mod 10): x=" << inverse->first << " period=" << inverse->second << '\n';

    auto r = cp::crt({2, 3, 2}, {3, 5, 7}); // 《孙子算经》：x ≡ 2 (mod 3), 3 (mod 5), 2 (mod 7)
    std::cout << "crt: x=" << r->first << " mod " << r->second << '\n';
    auto general = cp::crt({3, 5}, {4, 6}); // 模数不互质
    std::cout << "excrt: x=" << general->first << " mod " << general->second << '\n';
    std::cout << std::boolalpha << "conflict=" << !cp::crt({1, 2}, {4, 6}).has_value() << '\n';

    auto d = cp::solve_diophantine(4, 6, 10); // 4x + 6y = 10
    std::cout << "4x+6y=10: x=" << d->x0 << "+" << d->dx << "k, y=" << d->y0 << "-" << d->dy << "k\n";
}
```

### 预期标准输出

```text
gcd=6 x=-1 y=2 check=6
3x=1 (mod 10): x=7 period=10
crt: x=23 mod 105
excrt: x=11 mod 12
conflict=true
4x+6y=10: x=1+3k, y=1-2k
```

## 注意事项

- 逆元就是 `solve_congruence(a, 1, m)`；模数为质数时更常用 `pow_mod(a, m-2, m)` 或 ModInt。
- crt 要求最终的 lcm 不超过 LLONG_MAX（超过时断言失败）；P4777 这类题 lcm ≤ 10¹⁸ 可以直接用。
- solve_diophantine 返回最小非负 x，`dx = |b|/g > 0`。求最小正整数 x：`x0 == 0 ? dx : x0`；求 x、y 都为正的解数：先令 x 取最小正值，再看 y 能减几次 dy。
- solve_diophantine 要求 a、b 都不为 0，且 |a·b/g| + |c| 在 long long 范围内。

## 对应课程与练习

- 课程：[讲解139 裴蜀定理和扩展欧几里得算法](https://github.com/algorithmzuo/algorithm-journey/tree/main/src/class139)、[讲解140 扩展欧几里得和二元一次不定方程](https://github.com/algorithmzuo/algorithm-journey/tree/main/src/class140)、[讲解141 中国剩余定理及其扩展](https://github.com/algorithmzuo/algorithm-journey/tree/main/src/class141)
- 练习：
  - [P1082 同余方程](https://www.luogu.com.cn/problem/P1082)：`solve_congruence(a, 1, b)`
  - [P5656 二元一次不定方程 (exgcd)](https://www.luogu.com.cn/problem/P5656)：`solve_diophantine`
  - [P1495 曹冲养猪（中国剩余定理）](https://www.luogu.com.cn/problem/P1495)：`crt`
  - [P4777 扩展中国剩余定理](https://www.luogu.com.cn/problem/P4777)：`crt`，模数不互质

也可以在仓库根目录执行：

```powershell
python tools/cp.py example "math/crt"
```
