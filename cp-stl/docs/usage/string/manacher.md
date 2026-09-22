# Manacher 回文半径与最长回文

[模板源码](../../../string/manacher.hpp) · [完整示例](../../../examples/string/manacher.cpp) · [使用手册索引](../README.md)

在线性时间内获得每个中心的奇数、偶数回文半径，既能取最长回文子串，也能统计按位置区分的回文子串数量。

## 接口和使用顺序

| 接口 | 含义 |
| --- | --- |
| `manacher(s)` | 返回 Palindromes，时间与空间 O(n)。 |
| `result.longest` | 最长回文的 `[l,r)`；用 s.substr(l,r-l) 取出，空串返回 {0,0}。 |
| `result.odd[i]` | 奇回文半径：区间 `[i-odd[i]+1,i+odd[i])`，包含中心字符。 |
| `result.even[i]` | 偶回文半径：中心在 i-1 与 i 之间，区间 `[i-even[i],i+even[i])`。 |

## 完整例子

下面是可以直接编译的完整程序，数据写在代码里，**不需要输入**。在 workspace 根目录执行文末的 `python tools/cp.py example ...` 命令即可运行并核对输出。

```cpp
#include <iostream>
#include <numeric>
#include <string>
#include "string/manacher.hpp"

int main() {
    std::string s = "abba";
    auto result = cp::manacher(s);
    auto [l, r] = result.longest;
    std::cout << "longest=" << s.substr(l, r - l) << " [" << l << ',' << r << ")\n";
    std::cout << "odd_radius(aba,1)=" << cp::manacher("aba").odd[1] << '\n';
    std::cout << "even_radius(abba,2)=" << result.even[2] << '\n';
    long long count = std::accumulate(result.odd.begin(), result.odd.end(), 0LL)
                    + std::accumulate(result.even.begin(), result.even.end(), 0LL);
    std::cout << "palindrome_count=" << count << '\n';
    auto empty = cp::manacher("").longest;
    std::cout << "empty=[" << empty.first << ',' << empty.second << ")\n";
}
```

### 预期标准输出

```text
longest=abba [0,4)
odd_radius(aba,1)=2
even_radius(abba,2)=2
palindrome_count=6
empty=[0,0)
```

## 注意事项

- 半径不是回文长度：奇回文长度为 2*odd[i]-1，偶回文长度为 2*even[i]。
- 所有 odd 与 even 之和等于回文子串出现次数；相同内容的不同位置分别计数。
- 最长回文可能不唯一，不应依赖某个特定并列答案。
- 按字节处理；UTF-8 字符串如要按字符判断回文，需要先转成字符序列并调整实现。

也可以在仓库根目录执行：

```powershell
python tools/cp.py example "string/manacher"
```
