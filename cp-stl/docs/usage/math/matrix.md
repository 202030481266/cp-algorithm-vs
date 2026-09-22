# 矩阵乘法与快速幂

[模板源码](../../../math/matrix.hpp) · [完整示例](../../../examples/math/matrix.cpp) · [使用手册索引](../README.md)

线性递推可以写成固定维度的状态转移矩阵时，用矩阵快速幂跳过很多步。本例用 2×2 矩阵求斐波那契数。

## 接口和使用顺序

| 接口 | 含义 |
| --- | --- |
| `Matrix<T>(n, identity=false)` | 建立 n×n 全零矩阵；identity=true 时建立单位阵。 |
| `matrix[i][j] / matrix.n` | 读写 0-based 行列元素 / 维数。 |
| `a * b` | 同阶方阵乘法，O(n³)。 |
| `a.pow(exponent)` | 非负整数幂，O(n³ log exponent)；0 次幂为单位阵。 |

## 完整例子

下面是可以直接编译的完整程序，数据写在代码里，**不需要输入**。在 workspace 根目录执行文末的 `python tools/cp.py example ...` 命令即可运行并核对输出。

```cpp
#include <iostream>
#include "math/matrix.hpp"
#include "math/modint.hpp"

int main() {
    cp::Matrix<cp::Mint> a(2);
    a[0][0] = a[0][1] = a[1][0] = 1;
    auto squared = a * a;
    auto identity = a.pow(0);
    std::cout << "F10=" << a.pow(10)[0][1] << '\n';
    std::cout << "A2: " << squared[0][0] << ' ' << squared[0][1]
              << " / " << squared[1][0] << ' ' << squared[1][1] << '\n';
    std::cout << "I: " << identity[0][0] << ' ' << identity[0][1]
              << " / " << identity[1][0] << ' ' << identity[1][1] << '\n';
}
```

### 预期标准输出

```text
F10=55
A2: 2 1 / 1 1
I: 1 0 / 0 1
```

转移矩阵 A=[[1,1],[1,0]] 满足 A^n 的第 0 行第 1 列是 F(n)，取 F(0)=0、F(1)=1。

## 注意事项

- 只支持方阵，乘法两边维数必须相同。
- T 需要支持零初始化、从 1 构造、乘法和 +=。计数题推荐 ModInt，使用 long long 要自行防溢出。
- 矩阵乘法一般不满足交换律；行向量与列向量建模的转移方向不要混用。
- `matrix.hpp` 不强制依赖模整数，本例另外包含 modint.hpp。

也可以在仓库根目录执行：

```powershell
python tools/cp.py example "math/matrix"
```
