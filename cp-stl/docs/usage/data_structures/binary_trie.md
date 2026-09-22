# 01 Trie：单个元素的最大异或

[模板源码](../../../data_structures/binary_trie.hpp) · [完整示例](../../../examples/data_structures/binary_trie.cpp) · [使用手册索引](../README.md)

维护一个 uint64_t 多重集合，查询给定 x 与集合中某一个元素 y 的最大异或值 x^y。可以重复插入，也可以逐次删除。

## 接口和使用顺序

| 接口 | 含义 |
| --- | --- |
| `insert(x)` | 插入一个 x，O(64)。 |
| `count(x) / size()` | x 的出现次数 O(64) / 集合元素总数（含重复）O(1)。 |
| `erase(x)` | 删除一个 x，成功为 true；不存在为 false，O(64)。 |
| `max_xor(x)` | 返回最大异或值，类型 optional<uint64_t>；空集合返回 nullopt，O(64)。 |

## 完整例子

下面是可以直接编译的完整程序，数据写在代码里，**不需要输入**。在 workspace 根目录执行文末的 `python tools/cp.py example ...` 命令即可运行并核对输出。

```cpp
#include <iostream>
#include "data_structures/binary_trie.hpp"

int main() {
    cp::BinaryTrie trie;
    std::cout << std::boolalpha;
    std::cout << "empty_query=" << trie.max_xor(2).has_value() << '\n';
    trie.insert(3);
    trie.insert(5);
    trie.insert(5);
    std::cout << "size=" << trie.size() << " count(5)=" << trie.count(5) << '\n';
    if (auto value = trie.max_xor(2)) std::cout << "max_xor(2)=" << *value << '\n';
    trie.erase(5);
    std::cout << "after_one_erase=" << trie.count(5) << '\n';
    trie.erase(5);
    std::cout << "after_two_erases=" << *trie.max_xor(2) << '\n';
    std::cout << "erase_missing=" << trie.erase(9) << '\n';
}
```

### 预期标准输出

```text
empty_query=false
size=3 count(5)=2
max_xor(2)=7
after_one_erase=1
after_two_erases=1
erase_missing=false
```

## 注意事项

- 返回的是 x^y 的值，不是被选中的 y；需要 y 时可再异或 x。
- 支持完整 64 位无符号数，含最高位；输入负数应先按题目语义处理。
- 删除后不会回收 Trie 节点；空间与历次插入经过的节点有关。
- 多个元素任选子集异或的问题使用 XorBasis。

也可以在仓库根目录执行：

```powershell
python tools/cp.py example "data_structures/binary_trie"
```
