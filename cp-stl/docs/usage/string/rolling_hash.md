# 双模子串哈希 RollingHash

[模板源码](../../../string/rolling_hash.hpp) · [完整示例](../../../examples/string/rolling_hash.cpp) · [使用手册索引](../README.md)

对固定字符串建立前缀哈希，用 O(1) 时间比较任意两个子串的指纹。不同 RollingHash 对象使用相同参数，因此也可跨文本比较。

## 接口和使用顺序

| 接口 | 含义 |
| --- | --- |
| `RollingHash(s)` | O(n) 时间和空间预处理，不支持后续修改原串。 |
| `get(l,r)` | 返回 `[l,r)` 的双模哈希 array<long long,2>，O(1)。 |

## 完整例子

下面是可以直接编译的完整程序，数据写在代码里，**不需要输入**。在 workspace 根目录执行文末的 `python tools/cp.py example ...` 命令即可运行并核对输出。

```cpp
#include <iostream>
#include <string>
#include "string/rolling_hash.hpp"

int main() {
    std::string s = "banana";
    cp::RollingHash hash(s);
    cp::RollingHash other("ana");
    auto same = [&](int l1, int r1, int l2, int r2) {
        return r1 - l1 == r2 - l2 && hash.get(l1, r1) == hash.get(l2, r2);
    };
    std::cout << std::boolalpha;
    std::cout << "same_ana=" << same(1, 4, 3, 6) << '\n';
    std::cout << "cross_text=" << (hash.get(1, 4) == other.get(0, 3)) << '\n';
    std::cout << "ban_equals_ana=" << same(0, 3, 1, 4) << '\n';
    std::cout << "empty=" << (hash.get(2, 2) == other.get(0, 0)) << '\n';
}
```

### 预期标准输出

```text
same_ana=true
cross_text=true
ban_equals_ana=false
empty=true
```

## 注意事项

- 两张表内部使用 32 位模值、64 位中间乘积；主要数组有效数据为 16×(n+1) 字节，百万字节文本约 15.26 MiB，不含源串和分配器开销。返回值仍是 array<long long,2>。

- 比较子串前先确认长度相同；只比较哈希值不能代替长度检查。
- 这是概率算法，即使双模也存在碰撞。需要绝对正确的匹配时可用 KMP/Z，或在命中后核验原串。
- 空区间可查询；传入的区间必须在构建时字符串范围内。
- 按无符号字节取字符值，不把中文汉字视作一个元素。

也可以在仓库根目录执行：

```powershell
python tools/cp.py example "string/rolling_hash"
```
