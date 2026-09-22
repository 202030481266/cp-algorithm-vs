# KMP 与前缀函数

[模板源码](../../../string/kmp.hpp) · [完整示例](../../../examples/string/kmp.cpp) · [使用手册索引](../README.md)

在文本中寻找模式串的全部出现位置，包含重叠匹配。`prefix_function` 也可单独用于前后缀、周期等问题。

## 接口和使用顺序

| 接口 | 含义 |
| --- | --- |
| `prefix_function(s)` | pi[i] 是 s[0..i] 的最长真前后缀长度，O(\|s\|)。 |
| `kmp_find(text, pattern)` | 返回所有匹配起点，0-based，O(\|text\|+\|pattern\|)。 |

## 完整例子

下面是可以直接编译的完整程序，数据写在代码里，**不需要输入**。在 workspace 根目录执行文末的 `python tools/cp.py example ...` 命令即可运行并核对输出。

```cpp
#include <iostream>
#include "string/kmp.hpp"

int main() {
    std::cout << "pi:";
    for (int length : cp::prefix_function("ababaca")) std::cout << ' ' << length;
    std::cout << '\n';
    std::cout << "matches:";
    for (int start : cp::kmp_find("ababa", "aba")) std::cout << ' ' << start;
    std::cout << '\n';
    std::cout << "empty_pattern:";
    for (int start : cp::kmp_find("ab", "")) std::cout << ' ' << start;
    std::cout << '\n';
}
```

### 预期标准输出

```text
pi: 0 0 1 2 3 0 1
matches: 0 2
empty_pattern: 0 1 2
```

## 注意事项

- 参数顺序是文本在前、模式在后。
- 支持重叠，例如 ababa 中 aba 在 0 和 2 处出现。
- 空模式匹配文本的全部 n+1 个边界；空字符串的 prefix_function 返回空数组。
- 按字节处理字符串；UTF-8 中文的返回位置是字节偏移，不是汉字编号。

也可以在仓库根目录执行：

```powershell
python tools/cp.py example "string/kmp"
```
