# 模整数 ModInt

[模板源码](../../../math/modint.hpp) · [完整示例](../../../examples/math/modint.cpp) · [使用手册索引](../README.md)

把“每次计算后取模”封装进类型，适合计数 DP、组合数学和矩阵计算。模数是编译期常量；加减乘和输入负数会自动规范到 `[0,Mod)`。

## 接口和使用顺序

| 接口 | 含义 |
| --- | --- |
| `ModInt<Mod>(x)` | 从 long long 构造模整数；Mod 是大于 1 的 int 常量。 |
| `val() / mod()` | 读取当前非负值 / 静态模数。 |
| `+, -, *, / 及对应复合赋值` | 加减乘 O(1)，除法乘以逆元 O(log Mod)；支持一元负号、相等比较和流输入输出。 |
| `pow(exponent)` | 非负整数幂，O(log exponent)。 |
| `inv()` | 求逆元，要求当前值与 Mod 互质，O(log Mod)。 |
| `cp::Mint / cp::Mint1000000007` | 分别是模 998244353 / 1000000007 的别名。 |

## 完整例子

下面是可以直接编译的完整程序，数据写在代码里，**不需要输入**。在 workspace 根目录执行文末的 `python tools/cp.py example ...` 命令即可运行并核对输出。

```cpp
#include <iostream>
#include <sstream>
#include "math/modint.hpp"

int main() {
    using M = cp::ModInt<13>;
    M a = -2, b = 5; // a 被规范为 11
    std::cout << "a=" << a.val() << " mod=" << M::mod() << '\n';
    std::cout << "sum=" << a + b << " difference=" << a - b << '\n';
    std::cout << "product=" << a * b << " quotient=" << a / b << '\n';
    std::cout << "power=" << b.pow(3) << " inverse=" << b.inv() << '\n';
    M value;
    std::istringstream input("-1");
    input >> value;
    std::cout << std::boolalpha << "read=" << value
              << " equal=" << (value == M(12)) << '\n';
    std::cout << "default_mod=" << cp::Mint::mod() << '\n';
}
```

### 预期标准输出

```text
a=11 mod=13
sum=3 difference=6
product=3 quotient=10
power=8 inverse=8
read=12 equal=true
default_mod=998244353
```

## 注意事项

- 除法不是整数除法，也不是浮点概率；分母必须有逆元，0 不可做除数。
- 复合模数也可使用加减乘和互质元素的除法，例如模 12 的 5 可逆、6 不可逆。
- 不同模数是不同类型；显式用 val() 取出整数，不提供自动转 int 或大小比较。
- ModInt 依赖 number_theory.hpp；手动复制时需要一起复制依赖。

也可以在仓库根目录执行：

```powershell
python tools/cp.py example "math/modint"
```
