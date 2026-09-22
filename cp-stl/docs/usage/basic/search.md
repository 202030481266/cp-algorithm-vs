# 二分答案与有符号整除

[模板源码](../../../basic/search.hpp) · [完整示例](../../../examples/basic/search.cpp) · [使用手册索引](../README.md)

当判定函数随答案从 false 变为 true 时，用 `first_true` 查找最小可行答案。例如求满足 x²≥10 的最小非负整数。需要数学意义的向上/向下取整时，使用 `ceil_div` / `floor_div`，避免 C++ 对负数截断除法的差异。

## 接口和使用顺序

| 接口 | 含义 |
| --- | --- |
| `first_true(lo, hi, check)` | 在 `[lo,hi)` 内找第一个真值；不存在返回 hi。调用 O(log(hi-lo)) 次 check。 |
| `floor_div(a, b)` | 返回 ⌊a/b⌋，支持正负参数。 |
| `ceil_div(a, b)` | 返回 ⌈a/b⌉，支持正负参数。 |

## 完整例子

下面是可以直接编译的完整程序，数据写在代码里，**不需要输入**。在 workspace 根目录执行文末的 `python tools/cp.py example ...` 命令即可运行并核对输出。

```cpp
#include <iostream>
#include "basic/search.hpp"

int main() {
    auto check = [](long long x) { return x * x >= 10; };
    std::cout << "first=" << cp::first_true(0, 11, check) << '\n';
    std::cout << "missing=" << cp::first_true(0, 11, [](long long x) {
        return x >= 20;
    }) << '\n';
    std::cout << "empty=" << cp::first_true(5, 5, check) << '\n';
    std::cout << "floor=" << cp::floor_div(-7, 3)
              << " ceil=" << cp::ceil_div(-7, 3) << '\n';
}
```

### 预期标准输出

```text
first=4
missing=11
empty=5
floor=-3 ceil=-2
```

x=3 时 9<10，x=4 时 16≥10，所以第一个结果是 4；第二次查找没有可行值，返回开区间右端点 11。

## 注意事项

- `hi` 是开区间右端点，也是无解标记；若可能答案包含 10，就把右端点设为 11。
- 判定必须单调；二分函数避免了中点溢出，但 `check` 内的乘法仍需自行使用足够宽的类型。
- 除数不能为 0；`LLONG_MIN / -1` 的结果超出 long long，不可传入。

也可以在仓库根目录执行：

```powershell
python tools/cp.py example "basic/search"
```
