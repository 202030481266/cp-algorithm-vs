# 区间加与区间和 RangeAddSum

[模板源码](../../../data_structures/lazy_segment_tree.hpp) · [完整示例](../../../examples/data_structures/lazy_segment_tree.cpp) · [使用手册索引](../README.md)

当一次修改要给整段元素都增加同一个数，并查询任意区间和时使用。当前实现专门维护“区间加 + 区间和”，省去自定义懒标记组合规则。

## 接口和使用顺序

| 接口 | 含义 |
| --- | --- |
| `RangeAddSum(n) / RangeAddSum(a)` | 建立全零数组 / 从 vector<long long> 建树，O(n)。 |
| `add(l, r, delta)` | 给 `[l,r)` 的每个元素加 delta，O(log n)。 |
| `sum(l, r)` | 返回 `[l,r)` 的总和，O(log n)。 |

## 完整例子

下面是可以直接编译的完整程序，数据写在代码里，**不需要输入**。在 workspace 根目录执行文末的 `python tools/cp.py example ...` 命令即可运行并核对输出。

```cpp
#include <iostream>
#include <vector>
#include "data_structures/lazy_segment_tree.hpp"

int main() {
    cp::RangeAddSum seg(std::vector<long long>{1, 2, 3, 4});
    std::cout << "before=" << seg.sum(0, 4) << '\n';
    seg.add(1, 4, 5); // 1,7,8,9
    std::cout << "sum(1,3)=" << seg.sum(1, 3) << '\n';
    seg.add(0, 2, -2); // -1,5,8,9
    std::cout << "after=" << seg.sum(0, 4) << '\n';
    seg.add(2, 2, 100);
    std::cout << "empty=" << seg.sum(2, 2) << '\n';
}
```

### 预期标准输出

```text
before=10
sum(1,3)=15
after=21
empty=0
```

## 注意事项

- 支持负数和负增量；空区间更新无效果，空区间求和为 0。
- 它不提供区间赋值或区间乘法；不能把 add 当作覆盖旧值。复杂标记可参考使用手册中的 ACL 入口。
- 总和及 delta×区间长度都必须在 long long 范围内。

也可以在仓库根目录执行：

```powershell
python tools/cp.py example "data_structures/lazy_segment_tree"
```
