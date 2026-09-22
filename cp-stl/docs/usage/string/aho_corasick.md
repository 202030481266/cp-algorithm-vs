# AC 自动机：多个模式的出现次数

[模板源码](../../../string/aho_corasick.hpp) · [完整示例](../../../examples/string/aho_corasick.cpp) · [使用手册索引](../README.md)

模式串很多，需要统计每个模式在文本中出现多少次时，先插入全部模式串，再构建 AC 自动机，最后扫描文本。多次 count 可以复用同一个自动机。

## 接口和使用顺序

| 接口 | 含义 |
| --- | --- |
| `insert(pattern)` | 插入非空小写模式串，返回其 0-based id，O(\|pattern\|)。 |
| `build()` | 所有插入完成后调用一次，O(26×节点数)，补齐 fail 和转移。 |
| `count(text)` | 返回 vector<long long>，answer[id] 为该模式的出现次数，含重叠。O(\|text\|+节点数+模式数)。 |

## 完整例子

下面是可以直接编译的完整程序，数据写在代码里，**不需要输入**。在 workspace 根目录执行文末的 `python tools/cp.py example ...` 命令即可运行并核对输出。

```cpp
#include <iostream>
#include <string>
#include <vector>
#include "string/aho_corasick.hpp"

int main() {
    cp::AhoCorasick ac;
    std::vector<std::string> patterns{"he", "she", "hers", "he"};
    std::vector<int> ids;
    for (const auto& pattern : patterns) ids.push_back(ac.insert(pattern));
    ac.build();

    auto counts = ac.count("ushershe");
    for (int i = 0; i < int(patterns.size()); ++i)
        std::cout << ids[i] << ':' << patterns[i] << '=' << counts[ids[i]] << '\n';
    std::cout << "reused_he=" << ac.count("hehe")[ids[0]] << '\n';
}
```

### 预期标准输出

```text
0:he=2
1:she=2
2:hers=1
3:he=2
reused_he=2
```

## 注意事项

- 必须先全部 insert，再 build，再 count；build 后不能继续插入，也不能再次 build。
- 文本与模式只接受 a-z；不支持空模式。
- 重复插入同一字符串会获得不同 id，每个 id 都会返回相同的出现次数。
- count 每次独立统计，不会把上一次文本的次数带到下一次。

也可以在仓库根目录执行：

```powershell
python tools/cp.py example "string/aho_corasick"
```
