# 并查集 DSU

[模板源码](../../../data_structures/dsu.hpp) · [完整示例](../../../examples/data_structures/dsu.cpp) · [使用手册索引](../README.md)

用于动态合并集合、判断两点是否连通、统计连通块大小。构造时每个元素单独成一个集合，随后为每次连接调用 `merge`。

## 接口和使用顺序

| 接口 | 含义 |
| --- | --- |
| `DSU(n)` | 建立 0..n-1 共 n 个独立集合，O(n)。 |
| `merge(a, b)` | 合并两集合；实际发生合并返回 true，原本同集合返回 false。 |
| `same(a, b) / find(a)` | 是否同集合 / 集合代表；均摊 O(α(n))。 |
| `size(a) / components` | a 所在集合大小 / 当前集合总数。 |

## 完整例子

下面是可以直接编译的完整程序，数据写在代码里，**不需要输入**。在 workspace 根目录执行文末的 `python tools/cp.py example ...` 命令即可运行并核对输出。

```cpp
#include <iostream>
#include "data_structures/dsu.hpp"

int main() {
    cp::DSU dsu(5);
    dsu.merge(0, 1);
    dsu.merge(1, 2);
    std::cout << std::boolalpha;
    std::cout << "same(0,2)=" << dsu.same(0, 2) << '\n';
    std::cout << "same(0,4)=" << dsu.same(0, 4) << '\n';
    std::cout << "size(1)=" << dsu.size(1) << '\n';
    std::cout << "components=" << dsu.components << '\n';
    std::cout << "merge_again=" << dsu.merge(0, 2) << '\n';
}
```

### 预期标准输出

```text
same(0,2)=true
same(0,4)=false
size(1)=3
components=3
merge_again=false
```

## 注意事项

- 只支持合并，不能删除连接或撤销合并。
- `find(a)` 返回的代表编号可能变化，不要把它当作固定业务编号。
- 调用前把题目中的 1-based 顶点减 1；不要传入 n。

也可以在仓库根目录执行：

```powershell
python tools/cp.py example "data_structures/dsu"
```
