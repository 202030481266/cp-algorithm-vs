# 阶乘、组合数与排列数

[模板源码](../../../math/combinatorics.hpp) · [完整示例](../../../examples/math/combinatorics.cpp) · [使用手册索引](../README.md)

要多次查询 C(n,k) 或 A(n,k) 时，先预处理到所有查询可能用到的最大 n。之后每次查询只需 O(1)。

## 接口和使用顺序

| 接口 | 含义 |
| --- | --- |
| `Combinations<M>(limit)` | M 为模整数类型；预处理阶乘与逆阶乘，O(limit+log mod)，空间 O(limit)。 |
| `choose(n,k)` | 返回 C(n,k)；n<0、k<0 或 k>n 返回 0。 |
| `permute(n,k)` | 返回 A(n,k)=n!/(n-k)!；无效 n、k 返回 0。 |
| `fact[i] / inv_fact[i]` | i! / i! 的乘法逆元。 |

## 完整例子

下面是可以直接编译的完整程序，数据写在代码里，**不需要输入**。在 workspace 根目录执行文末的 `python tools/cp.py example ...` 命令即可运行并核对输出。

```cpp
#include <iostream>
#include "math/combinatorics.hpp"

int main() {
    cp::Combinations<cp::Mint> comb(10);
    std::cout << "C(5,2)=" << comb.choose(5, 2) << '\n';
    std::cout << "A(5,2)=" << comb.permute(5, 2) << '\n';
    std::cout << "C(5,0)=" << comb.choose(5, 0) << '\n';
    std::cout << "C(5,7)=" << comb.choose(5, 7) << '\n';
    std::cout << "factorial(5)=" << comb.fact[5] << '\n';
    std::cout << "inverse_check=" << comb.fact[5] * comb.inv_fact[5] << '\n';
}
```

### 预期标准输出

```text
C(5,2)=10
A(5,2)=20
C(5,0)=1
C(5,7)=0
factorial(5)=120
inverse_check=1
```

## 注意事项

- 要求模数为素数，且 0≤limit<mod；不要预处理到模数或更大。
- 合法 n 必须不超过预处理上限；choose 的越界 n 不会自动扩容。
- C(0,0)=1；超过模数的组合数问题需要 Lucas 等其他方法。
- 默认模板参数为 cp::Mint；想用 1e9+7 可写 Combinations<cp::Mint1000000007>。

也可以在仓库根目录执行：

```powershell
python tools/cp.py example "math/combinatorics"
```
