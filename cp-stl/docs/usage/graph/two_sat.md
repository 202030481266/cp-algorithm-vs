# 2-SAT 布尔约束

[模板源码](../../../graph/two_sat.hpp) · [完整示例](../../../examples/graph/two_sat.cpp) · [使用手册索引](../README.md)

每条限制都能写成“两个布尔条件至少一个成立”时，可以用 2-SAT 求任意一个满足全部限制的方案。变量从 0 开始编号。

## 接口和使用顺序

| 接口 | 含义 |
| --- | --- |
| `TwoSAT(n)` | 建立 n 个布尔变量。 |
| `add_clause(x,f,y,g)` | 添加 `(x==f) OR (y==g)`，添加一条子句 O(1)。 |
| `solve()` | O(n+m)；有解返回 optional<vector<bool>>，无解返回 nullopt。 |

## 完整例子

下面是可以直接编译的完整程序，数据写在代码里，**不需要输入**。在 workspace 根目录执行文末的 `python tools/cp.py example ...` 命令即可运行并核对输出。

```cpp
#include <iostream>
#include "graph/two_sat.hpp"

int main() {
    cp::TwoSAT sat(2);
    sat.add_clause(0, true, 0, true);   // x0 必须为 true
    sat.add_clause(0, false, 1, true);  // x0 -> x1
    std::cout << std::boolalpha;
    if (auto answer = sat.solve()) {
        std::cout << "assignment: " << (*answer)[0] << ' ' << (*answer)[1] << '\n';
    } else {
        std::cout << "unsatisfiable\n";
    }

    sat.add_clause(1, false, 1, false); // 再强制 x1=false，产生矛盾
    std::cout << "contradiction=" << !sat.solve().has_value() << '\n';
}
```

### 预期标准输出

```text
assignment: true true
contradiction=true
```

## 注意事项

- 强制 x=true 写 `add_clause(x,true,x,true)`；强制 false 同理。
- 蕴含 x→y 等价于 !x OR y；不要把 add_clause 误读为 AND。
- 可能有多个解，接口不保证字典序最小；增加子句后可以再次调用 solve。
- 依赖 SCC；手动复制实现时也要带上 scc.hpp 的代码。

也可以在仓库根目录执行：

```powershell
python tools/cp.py example "graph/two_sat"
```
