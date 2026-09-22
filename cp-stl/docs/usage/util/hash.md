# unordered_map / unordered_set 随机盐哈希

[模板源码](../../../util/hash.hpp) · [完整示例](../../../examples/util/hash.cpp) · [使用手册索引](../README.md)

给整数键的哈希表配置 SafeHash，降低固定哈希函数被构造碰撞的风险。它只改变哈希分布，不改变键的相等性或 map/set 的用法。

## 接口和使用顺序

| 接口 | 含义 |
| --- | --- |
| `unordered_map<long long,T,cp::SafeHash>` | 以整数为键的哈希映射，正常平均插入/查询 O(1)。 |
| `unordered_set<uint64_t,cp::SafeHash>` | 整数哈希集合。 |
| `SafeHash{}(x)` | 将整数映射为 size_t 哈希；每次进程启动的盐可能不同。 |
| `SafeHash::splitmix64(x)` | 底层确定性 64 位混合函数，普通使用无需直接调用。 |

## 完整例子

下面是可以直接编译的完整程序，数据写在代码里，**不需要输入**。在 workspace 根目录执行文末的 `python tools/cp.py example ...` 命令即可运行并核对输出。

```cpp
#include <cstdint>
#include <iostream>
#include <unordered_map>
#include <unordered_set>
#include "util/hash.hpp"

int main() {
    std::unordered_map<long long, int, cp::SafeHash> count;
    count.max_load_factor(0.7f);
    count.reserve(16);
    for (long long x : {10LL, -7LL, 10LL}) ++count[x];
    std::cout << "distinct=" << count.size() << '\n';
    std::cout << "count(10)=" << count.at(10) << '\n';
    std::cout << "count(-7)=" << count.at(-7) << '\n';
    std::cout << std::boolalpha << "missing=" << (count.find(42) == count.end()) << '\n';

    std::unordered_set<std::uint64_t, cp::SafeHash> seen;
    seen.insert(std::uint64_t{1} << 63);
    std::cout << "highest_bit=" << seen.count(std::uint64_t{1} << 63) << '\n';
}
```

### 预期标准输出

```text
distinct=2
count(10)=2
count(-7)=1
missing=true
highest_bit=1
```

## 注意事项

- 无法保证完全没有碰撞或永远 O(1)；相等键仍由容器的 equality 比较。
- 遍历 unordered 容器的顺序不固定，示例通过指定键查询来保证输出稳定。
- 只适合能转换到 uint64_t 的整数键；pair、字符串或自定义结构需要另写哈希。
- reserve / max_load_factor 仍然有用；随机哈希不能代替合理的容量设置。

也可以在仓库根目录执行：

```powershell
python tools/cp.py example "util/hash"
```
