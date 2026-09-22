# 泛型线段树 SegmentTree

[模板源码](../../../data_structures/segment_tree.hpp) · [完整示例](../../../examples/data_structures/segment_tree.cpp) · [使用手册索引](../README.md)

用于单点赋值和区间信息合并。先定义元素类型 T、结合运算 Op 和单位元 identity，再用原数组建树。示例同时展示求和与自定义最小值运算。

## 接口和使用顺序

| 接口 | 含义 |
| --- | --- |
| `SegmentTree<T,Op>(a, identity, op={})` | 由数组 O(n) 建树；也可以传 n 建立全单位元数组。 |
| `set(p, value) / get(p)` | 单点赋值 O(log n) / 读取单点 O(1)。 |
| `prod(l, r) / all_prod()` | 合并 `[l,r)` O(log n) / 全数组合并 O(1)；空区间返回单位元。 |
| `max_right(l, pred)` | 返回最大的 r，使 pred(prod(l,r)) 为真，O(log n)。 |

## 完整例子

下面是可以直接编译的完整程序，数据写在代码里，**不需要输入**。在 workspace 根目录执行文末的 `python tools/cp.py example ...` 命令即可运行并核对输出。

```cpp
#include <algorithm>
#include <functional>
#include <iostream>
#include <limits>
#include <vector>
#include "data_structures/segment_tree.hpp"

struct Minimum {
    long long operator()(long long a, long long b) const {
        return std::min(a, b);
    }
};

int main() {
    std::vector<long long> a{1, 2, 3, 4};
    cp::SegmentTree<long long, std::plus<long long>> sum(a, 0);
    sum.set(1, 10);
    std::cout << "get(1)=" << sum.get(1) << '\n';
    std::cout << "sum(1,3)=" << sum.prod(1, 3) << '\n';
    std::cout << "total=" << sum.all_prod() << '\n';
    std::cout << "empty=" << sum.prod(2, 2) << '\n';
    std::cout << "max_right=" << sum.max_right(0, [](long long x) {
        return x <= 11;
    }) << '\n';

    cp::SegmentTree<long long, Minimum> minimum(
        a, std::numeric_limits<long long>::max());
    std::cout << "min(1,4)=" << minimum.prod(1, 4) << '\n';
}
```

### 预期标准输出

```text
get(1)=10
sum(1,3)=13
total=18
empty=0
max_right=2
min(1,4)=2
```

## 注意事项

- Op 必须满足结合律；单位元须满足 op(e,x)=op(x,e)=x。允许不满足交换律的运算，例如字符串拼接。
- `max_right` 要求 pred(identity)=true，且扩大区间时只能从真变假。区间和≤上限的示例要求元素非负。
- 更新为新值用 `set`，不是增加；增加 delta 可写 `set(p, get(p)+delta)`。

也可以在仓库根目录执行：

```powershell
python tools/cp.py example "data_structures/segment_tree"
```
