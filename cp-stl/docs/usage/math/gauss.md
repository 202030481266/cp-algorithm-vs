# 高斯消元：实数、模质数与异或方程组

[模板源码](../../../math/gauss.hpp) · [完整示例](../../../examples/math/gauss.cpp) · [使用手册索引](../README.md)

解线性方程组 Ax = b 的三个版本，都用高斯-约旦消元，并区分**无解、唯一解、无穷多解**：

- `solve_linear_real`：实数系数，按列选绝对值最大的主元，误差阈值 eps；
- `solve_linear_mod`：模质数 p，常用于计数题；
- `solve_linear_xor`：系数是 0/1、运算是异或（开关灯、异或方程），每行压成 64 位字，快 64 倍。

## 接口和使用顺序

| 接口 | 含义 |
| --- | --- |
| `solve_linear_real<Real>(a, b, eps=1e-9)` | a 为 n×m 系数（n ≥ 1），b 为长度 n 的常数项。 |
| `solve_linear_mod(a, b, mod)` | mod 为质数且 < 2³¹，系数可为负。 |
| `solve_linear_xor(a, b)` | a、b 的元素按奇偶看作 0/1。 |
| `result.solvable` | 是否有解；无解时 solution 为空。 |
| `result.rank / free_variables` | 系数矩阵的秩 / 自由元下标。唯一解 ⟺ 有解且没有自由元。 |
| `result.solution` | 一组解，自由元取 0；模 p 时共 p^自由元个数 组解，异或时 2^自由元个数 组。 |

## 完整例子

下面是可以直接编译的完整程序，数据写在代码里，**不需要输入**。在 workspace 根目录执行文末的 `python tools/cp.py example ...` 命令即可运行并核对输出。

```cpp
#include <iomanip>
#include <iostream>
#include "math/gauss.hpp"

int main() {
    std::cout << std::boolalpha << std::fixed << std::setprecision(2);
    // 实数：x + y + z = 6，2y + 5z = -4，2x + 5y - z = 27。
    auto real = cp::solve_linear_real<double>({{1, 1, 1}, {0, 2, 5}, {2, 5, -1}}, {6, -4, 27});
    std::cout << "real: unique=" << (real.solvable && real.free_variables.empty()) << " x =";
    for (double v : real.solution) std::cout << ' ' << v;
    std::cout << '\n';

    // 模 7：第二个方程是第一个的 3 倍，秩为 1，有一个自由元，共 7 组解。
    auto mod = cp::solve_linear_mod({{1, 2}, {3, 6}}, {3, 2}, 7);
    std::cout << "mod 7: rank=" << mod.rank << " free=" << mod.free_variables.size()
              << " one solution=" << mod.solution[0] << ',' << mod.solution[1] << '\n';

    // 异或：x0^x1=1, x1^x2=0, x0^x2=0 三式相加得 0=1，矛盾。
    auto bits = cp::solve_linear_xor({{1, 1, 0}, {0, 1, 1}, {1, 0, 1}}, {1, 0, 0});
    std::cout << "xor: solvable=" << bits.solvable << " rank=" << bits.rank << '\n';
}
```

### 预期标准输出

```text
real: unique=true x = 5.00 3.00 -2.00
mod 7: rank=1 free=1 one solution=3,0
xor: solvable=false rank=2
```

## 注意事项

- 未知数个数取自 `a[0].size()`，所以至少要有一个方程。
- 判断“无解 / 无穷多解”的顺序：先看 solvable，再看 free_variables（P2455 就是这样要求的）。
- 实数版本使用 double；数值较大或病态时改用 `solve_linear_real<long double>`，必要时调大 eps。
- 复杂度 O(n·m·min(n,m))；异或版本再除以 64，2000×2000 的异或方程组也很快。
- 需要行列式、矩阵求逆时，可以仿照模质数版本：行列式是主元乘积（注意交换行要变号）。

## 对应课程与练习

- 课程：[讲解133 高斯消元解决加法方程组](https://github.com/algorithmzuo/algorithm-journey/tree/main/src/class133)、[讲解134 高斯消元解决异或方程组](https://github.com/algorithmzuo/algorithm-journey/tree/main/src/class134)、[讲解135 高斯消元解决同余方程组](https://github.com/algorithmzuo/algorithm-journey/tree/main/src/class135)
- 练习：
  - [P3389 高斯消元法](https://www.luogu.com.cn/problem/P3389)：实数，唯一解
  - [P2455 线性方程组](https://www.luogu.com.cn/problem/P2455)：区分无解与无穷多解
  - [P2962 Lights G](https://www.luogu.com.cn/problem/P2962)：异或方程组 + 自由元枚举
  - [P2447 外星千足虫](https://www.luogu.com.cn/problem/P2447)：异或方程组，求最少用到的方程数

也可以在仓库根目录执行：

```powershell
python tools/cp.py example "math/gauss"
```
