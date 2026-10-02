# 欧拉路径与欧拉回路

[模板源码](../../../graph/euler_path.hpp) · [完整示例](../../../examples/graph/euler_path.cpp) · [使用手册索引](../README.md)

找一条经过每条边恰好一次的路径（Hierholzer 算法，非递归）。有向图要求至多一个点出度比入度大 1（起点）、
一个点入度比出度大 1（终点），无向图要求奇度点为 0 个或 2 个，并且所有边连通。
`smallest=true` 时先给邻接表排序，得到字典序最小的点序列，这正是很多题目要求的输出。

## 接口和使用顺序

| 接口 | 含义 |
| --- | --- |
| `EulerPath(n, directed)` | n 个点的有向图或无向图，允许重边和自环。 |
| `add_edge(u, v)` | 加边，返回边编号（从 0 开始）。 |
| `find(start=-1, smallest=false)` | optional<EulerTrail>；不存在时为 nullopt。start=-1 时自动选起点。 |
| `EulerTrail::vertices` | 依次经过的点，长度为 边数+1；起点等于终点时是欧拉回路。 |
| `EulerTrail::edges` | 依次经过的边编号，长度为 边数。 |

## 完整例子

下面是可以直接编译的完整程序，数据写在代码里，**不需要输入**。在 workspace 根目录执行文末的 `python tools/cp.py example ...` 命令即可运行并核对输出。

```cpp
#include <iostream>
#include <vector>
#include "graph/euler_path.hpp"

void print(const char* name, const std::vector<int>& path) {
    std::cout << name << ':';
    for (int v : path) std::cout << ' ' << v;
    std::cout << '\n';
}

int main() {
    // 有向图：0 的出度比入度大 1，必须从 0 出发。
    cp::EulerPath directed(3, true);
    directed.add_edge(0, 1);
    directed.add_edge(1, 2);
    directed.add_edge(2, 0);
    directed.add_edge(0, 2);
    auto a = directed.find(-1, true); // 字典序最小
    print("directed", a->vertices);
    print("edge ids", a->edges);

    // 无向“蝴蝶结”：两个三角形共用点 2，所有点度数为偶数，存在欧拉回路。
    cp::EulerPath bowtie(5, false);
    for (auto [u, v] : std::vector<std::pair<int, int>>{{0, 1}, {1, 2}, {2, 0}, {2, 3}, {3, 4}, {4, 2}})
        bowtie.add_edge(u, v);
    print("undirected", bowtie.find(-1, true)->vertices);

    // 星形图有 4 个奇度点，不存在欧拉路径。
    cp::EulerPath star(4, false);
    for (int v = 1; v < 4; ++v) star.add_edge(0, v);
    std::cout << std::boolalpha << "star_has_path=" << star.find().has_value() << '\n';
}
```

### 预期标准输出

```text
directed: 0 1 2 0 2
edge ids: 0 1 2 3
undirected: 0 1 2 3 4 2 0
star_has_path=false
```

## 注意事项

- 自动选择的起点：有向图为出度比入度大 1 的点；无向图为编号较小的奇度点；都没有时为有边的最小编号点，因此 smallest=true 时结果是全局字典序最小。
- 指定 start 但它不能作为起点（度数条件不符、或没有边）时返回 nullopt。没有边时返回只含起点的路径。
- 孤立点不影响结果；有边不连通时返回 nullopt。
- 复杂度 O(n + m)，smallest=true 时多一次排序 O(m log m)。
- de Bruijn 序列（破解保险箱、太鼓达人）：把长度 k-1 的串作为点、长度 k 的串作为边，求欧拉回路。

## 对应课程与练习

- 课程：[讲解188 欧拉路径与相关题目](https://github.com/algorithmzuo/algorithm-journey/tree/main/src/class188)
- 练习：
  - [P7771 欧拉路径](https://www.luogu.com.cn/problem/P7771)：有向图字典序最小：`find(-1, true)`
  - [P2731 骑马修栅栏](https://www.luogu.com.cn/problem/P2731)：无向图、重边、字典序最小
  - [P1127 词链](https://www.luogu.com.cn/problem/P1127)：单词作为边，首尾字母作为点
  - [LeetCode 332 重新安排行程](https://leetcode.cn/problems/reconstruct-itinerary/)：机场离散化后求字典序最小的有向欧拉路径

也可以在仓库根目录执行：

```powershell
python tools/cp.py example "graph/euler_path"
```
