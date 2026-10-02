# 可持久化 01 Trie：区间内的最大 / 最小异或

[模板源码](../../../data_structures/persistent_binary_trie.hpp) · [完整示例](../../../examples/data_structures/persistent_binary_trie.cpp) · [使用手册索引](../README.md)

把数按顺序插入 01 Trie，每次插入都得到一个新版本（只复制一条路径）。两个版本的计数相减就是
“这段区间里插入的数”，于是可以在线回答：从区间里选一个数 y，使 x XOR y 最大或最小。

## 接口和使用顺序

| 接口 | 含义 |
| --- | --- |
| `PersistentBinaryTrie<Bits>(reserve_values=0)` | 只处理低 Bits 位，插入值必须 < 2^Bits；可预留插入次数。 |
| `insert(root, x)` | 在版本 root 上插入 x，返回新版本的根，O(Bits)。根 0 是空集合。 |
| `count(lo_root, hi_root)` | hi 版本比 lo 版本多出的元素个数。 |
| `max_xor(lo_root, hi_root, x)` | 在 hi−lo 的集合中取 y，返回最大的 x XOR y（不是 y 本身），集合必须非空。 |
| `min_xor(lo_root, hi_root, x)` | 同上，返回最小的 x XOR y。 |

## 完整例子

下面是可以直接编译的完整程序，数据写在代码里，**不需要输入**。在 workspace 根目录执行文末的 `python tools/cp.py example ...` 命令即可运行并核对输出。

```cpp
#include <cstdint>
#include <iostream>
#include <vector>
#include "data_structures/persistent_binary_trie.hpp"

int main() {
    std::vector<std::uint64_t> a{5, 1, 7, 2};
    cp::PersistentBinaryTrie<3> trie; // 只处理低 3 位，所有值 < 8
    std::vector<int> root{0};         // root[i] 是前 i 个数组成的版本
    for (auto x : a) root.push_back(trie.insert(root.back(), x));
    // 区间 [1,4) = {1,7,2}：与 6 的异或值依次为 7,1,4。
    std::cout << "count=" << trie.count(root[1], root[4])
              << " max=" << trie.max_xor(root[1], root[4], 6)
              << " min=" << trie.min_xor(root[1], root[4], 6) << '\n';
    std::cout << "prefix max=" << trie.max_xor(0, root[2], 6) << '\n'; // {5,1}
}
```

### 预期标准输出

```text
count=3 max=7 min=1
prefix max=7
```

## 注意事项

- 常见写法：`root[0] = 0`，`root[i+1] = trie.insert(root[i], a[i])`，区间 [l,r) 用 `(root[l], root[r])`。
- Bits 决定树高：值 ≤ 10⁷ 用 24，≤ 10⁹ 用 30，任意 uint64 用 64。Bits 越小越省时间和内存。
- 节点数为 插入次数 × (Bits+1)，每个节点 12 字节；6×10⁵ 次插入、Bits=24 约 180MB，注意题目的内存限制。
- 查询前确认区间非空（`count > 0`），否则会触发断言。

## 对应课程与练习

- 课程：[讲解159 可持久化前缀树和相关题目](https://github.com/algorithmzuo/algorithm-journey/tree/main/src/class159)
- 练习：
  - [P4735 最大异或和](https://www.luogu.com.cn/problem/P4735)：对前缀异或和建可持久化 Trie，查询 `max_xor(root[l-1], root[r], s[n]^x)`
  - [P4592 TJOI2018 异或](https://www.luogu.com.cn/problem/P4592)：树上路径与子树：DFS 序版本 + 父亲版本
  - [P5795 异或运算](https://www.luogu.com.cn/problem/P5795)：可持久化 Trie 上二分第 k 大

也可以在仓库根目录执行：

```powershell
python tools/cp.py example "data_structures/persistent_binary_trie"
```
