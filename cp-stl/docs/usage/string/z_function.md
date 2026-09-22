# Z 函数与前缀匹配

[模板源码](../../../string/z_function.hpp) · [完整示例](../../../examples/string/z_function.cpp) · [使用手册索引](../README.md)

`z[i]` 表示整个字符串与从 i 开始的后缀之间的最长公共前缀长度。将模式与文本拼接后，可以据此寻找模式出现的位置。

## 接口和使用顺序

| 接口 | 含义 |
| --- | --- |
| `z_function(s)` | 返回长度为 \|s\| 的 vector<int>，时间与空间 O(\|s\|)。 |
| `z[i]` | s 与 s[i..] 的最长公共前缀长度；本库约定 z[0]=\|s\|。 |

## 完整例子

下面是可以直接编译的完整程序，数据写在代码里，**不需要输入**。在 workspace 根目录执行文末的 `python tools/cp.py example ...` 命令即可运行并核对输出。

```cpp
#include <iostream>
#include <string>
#include "string/z_function.hpp"

int main() {
    std::string text = "abacaba";
    std::cout << "z:";
    for (int length : cp::z_function(text)) std::cout << ' ' << length;
    std::cout << '\n';

    std::string pattern = "aba";
    auto z = cp::z_function(pattern + "#" + text); // '#' 不在两者中
    int offset = int(pattern.size()) + 1;
    std::cout << "matches:";
    for (int i = offset; i < int(z.size()); ++i)
        if (z[i] >= int(pattern.size())) std::cout << ' ' << i - offset;
    std::cout << '\n';
}
```

### 预期标准输出

```text
z: 7 0 1 0 3 0 1
matches: 0 4
```

## 注意事项

- 不同资料可能设 z[0]=0，本库设为 n；使用周期或统计公式时注意这一点。
- 拼接 pattern + 分隔符 + text 时，分隔符必须不在两者中出现。
- 转回文本下标要减去 pattern.size()+1。
- 空串返回空数组；所有位置和长度按字节计。

也可以在仓库根目录执行：

```powershell
python tools/cp.py example "string/z_function"
```
