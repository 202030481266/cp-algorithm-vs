# LOCAL 调试输出 debug

[模板源码](../../../util/debug.hpp) · [完整示例](../../../examples/util/debug.cpp) · [使用手册索引](../README.md)

在本地快速打印变量、容器、pair 和 tuple，输出走 stderr，正常答案仍走 stdout。仓库运行任务自动定义 LOCAL；不定义 LOCAL，或定义 ONLINE_JUDGE / NO_DEBUG 时，debug 调用不产生输出，也不求值参数。已有 workspace debug 宏时保留原宏。

## 接口和使用顺序

| 接口 | 含义 |
| --- | --- |
| `debug(value, ...)` | 打印参数名、调用位置的行号和参数值；至少提供一个参数。 |
| `LOCAL` | 在包含头文件之前定义，或用编译参数 -DLOCAL；用于启用调试宏。 |

## 完整例子

下面是可以直接编译的完整程序，数据写在代码里，**不需要输入**。在 workspace 根目录执行文末的 `python tools/cp.py example ...` 命令即可运行并核对输出。

```cpp
#include <iostream>
#include <numeric>
#include <string>
#include <tuple>
#include <vector>
#include "util/debug.hpp"

int main() {
    std::vector<int> a{1, 2, 3};
    long long sum = std::accumulate(a.begin(), a.end(), 0LL);
    debug(a, sum);
    debug(std::make_tuple(7, std::string("ok")));
    std::cout << "answer=" << sum << '\n';
}
```

### 预期标准输出

```text
answer=6
```

### LOCAL 模式下的标准错误输出

```text
[line 11] a, sum = [1, 2, 3], 6
[line 12] std::make_tuple(7, std::string("ok")) = (7, "ok")
```

不定义 LOCAL 时，本例仍输出 `answer=6`，但 stderr 完全为空；测试会同时检查这两种构建方式。

## 注意事项

- `debug_detail` 是实现细节，正常使用只调用 debug(...)。
- 不要把必需的计算或副作用放进 debug 参数，例如 debug(++answer)，否则本地与 OJ 行为可能不同。
- 支持嵌套可迭代容器、pair、tuple、字符串、布尔值和可用 ostream 输出的类型；priority_queue、stack 不是可迭代容器，需自行取出元素。
- 终端可能交错显示 stdout 和 stderr；文档分别列出了两个流的内容。示例里的行号对应所链接的 cpp，复制后行号会改变。

也可以在仓库根目录执行：

```powershell
python tools/cp.py example "util/debug"
```
