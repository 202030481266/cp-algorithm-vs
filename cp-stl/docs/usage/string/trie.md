# 字典树：单词与前缀计数

[模板源码](../../../string/trie.hpp) · [完整示例](../../../examples/string/trie.cpp) · [使用手册索引](../README.md)

维护由小写字母组成的字符串多重集合，查询某个单词或某个前缀出现多少次。重复插入会重复计数。

## 接口和使用顺序

| 接口 | 含义 |
| --- | --- |
| `insert(s)` | 加入一个 s，O(\|s\|)。 |
| `count(s)` | 完整单词 s 的出现次数，O(\|s\|)。 |
| `count_prefix(prefix)` | 有指定前缀的字符串数量，含重复，O(\|prefix\|)。 |
| `erase(s)` | 删掉一个 s，成功为 true，不存在为 false，O(\|s\|)。 |

## 完整例子

下面是可以直接编译的完整程序，数据写在代码里，**不需要输入**。在 workspace 根目录执行文末的 `python tools/cp.py example ...` 命令即可运行并核对输出。

```cpp
#include <iostream>
#include "string/trie.hpp"

int main() {
    cp::Trie trie;
    trie.insert("app");
    trie.insert("app");
    trie.insert("apple");
    trie.insert("");
    std::cout << "app=" << trie.count("app") << '\n';
    std::cout << "prefix_ap=" << trie.count_prefix("ap") << '\n';
    std::cout << "size=" << trie.count_prefix("") << '\n';
    std::cout << std::boolalpha;
    std::cout << "erase_app=" << trie.erase("app") << '\n';
    std::cout << "app_after=" << trie.count("app") << '\n';
    std::cout << "erase_ape=" << trie.erase("ape") << '\n';
    std::cout << "empty_word=" << trie.count("") << '\n';
}
```

### 预期标准输出

```text
app=2
prefix_ap=3
size=4
erase_app=true
app_after=1
erase_ape=false
empty_word=1
```

## 注意事项

- 仅接受 a-z；大写字母、数字、中文不能直接传入。
- 空字符串可以插入；count_prefix("") 返回整个多重集合的元素数。
- 删除只减少计数，不回收节点，空间与历史插入经过的节点有关。
- “单词存在”用 count(s)>0；“某前缀存在”用 count_prefix(s)>0，不要混用。

也可以在仓库根目录执行：

```powershell
python tools/cp.py example "string/trie"
```
