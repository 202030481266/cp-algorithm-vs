# 左程云《算法讲解》与 CP-STL 模板对照表

按讲次查找对应的模板。课程代码在 [algorithm-journey](https://github.com/algorithmzuo/algorithm-journey)（`src/classXXX`），视频在 B 站同名合集。
每个模板的使用手册末尾的“对应课程与练习”一节列出了讲次链接和可以直接套用的洛谷 / LeetCode 题目。

模板是按竞赛中的通用写法独立实现的（0-based、左闭右开、`namespace cp`），没有照搬课程的 Java 代码；
课程中按题目写的数组下标、静态数组等细节，换成本库的接口即可。

[使用手册索引](usage/README.md) · [常用写法](recipes.md) · [主索引](../README.md)

## 基础、排序与数据结构

| 讲次 | 主题 | 模板 |
| --- | --- | --- |
| [006](https://github.com/algorithmzuo/algorithm-journey/tree/main/src/class006)、[051](https://github.com/algorithmzuo/algorithm-journey/tree/main/src/class051) | 二分搜索、二分答案 | [二分答案与有符号整除](usage/basic/search.md) |
| [028](https://github.com/algorithmzuo/algorithm-journey/tree/main/src/class028) | 基数排序 | [计数排序与基数排序](usage/basic/sort.md) |
| [046](https://github.com/algorithmzuo/algorithm-journey/tree/main/src/class046)–[048](https://github.com/algorithmzuo/algorithm-journey/tree/main/src/class048) | 前缀和、一维 / 等差数列 / 二维差分、离散化 | [前缀和与差分](usage/basic/prefix_sum.md)、[离散化](usage/basic/compress.md) |
| [052](https://github.com/algorithmzuo/algorithm-journey/tree/main/src/class052)–[053](https://github.com/algorithmzuo/algorithm-journey/tree/main/src/class053) | 单调栈 | [单调栈与笛卡尔树](usage/basic/monotonic_stack.md) |
| [054](https://github.com/algorithmzuo/algorithm-journey/tree/main/src/class054)–[055](https://github.com/algorithmzuo/algorithm-journey/tree/main/src/class055) | 单调队列 | [滑动窗口最小值](usage/dp/sequence.md) |
| [056](https://github.com/algorithmzuo/algorithm-journey/tree/main/src/class056)–[057](https://github.com/algorithmzuo/algorithm-journey/tree/main/src/class057) | 并查集 | [DSU](usage/data_structures/dsu.md) |
| [044](https://github.com/algorithmzuo/algorithm-journey/tree/main/src/class044)–[045](https://github.com/algorithmzuo/algorithm-journey/tree/main/src/class045) | 前缀树 | [字典树](usage/string/trie.md)、[01 Trie](usage/data_structures/binary_trie.md) |
| [108](https://github.com/algorithmzuo/algorithm-journey/tree/main/src/class108)–[109](https://github.com/algorithmzuo/algorithm-journey/tree/main/src/class109) | 树状数组：区间加区间和、二维 | [Fenwick](usage/data_structures/fenwick.md)、[树状数组扩展](usage/data_structures/range_fenwick.md) |
| [110](https://github.com/algorithmzuo/algorithm-journey/tree/main/src/class110)–[113](https://github.com/algorithmzuo/algorithm-journey/tree/main/src/class113) | 线段树：区间修改、维护更多信息、区间合并 | [SegmentTree](usage/data_structures/segment_tree.md)、[懒标记线段树](usage/data_structures/lazy_segment_tree.md) |
| [114](https://github.com/algorithmzuo/algorithm-journey/tree/main/src/class114) | 开点线段树、区间最值操作 | [动态开点线段树](usage/data_structures/dynamic_segment_tree.md)、[吉司机线段树](usage/data_structures/segment_tree_beats.md) |
| [115](https://github.com/algorithmzuo/algorithm-journey/tree/main/src/class115) | 线段树与扫描线 | [矩形面积并与周长并](usage/geometry/rectangle_union.md) |
| [117](https://github.com/algorithmzuo/algorithm-journey/tree/main/src/class117) | 倍增、ST 表 | [SparseTable](usage/data_structures/sparse_table.md) |
| [136](https://github.com/algorithmzuo/algorithm-journey/tree/main/src/class136)–[137](https://github.com/algorithmzuo/algorithm-journey/tree/main/src/class137) | 线性基 | [异或线性基与前缀线性基](usage/data_structures/xor_basis.md) |
| [148](https://github.com/algorithmzuo/algorithm-journey/tree/main/src/class148)–[153](https://github.com/algorithmzuo/algorithm-journey/tree/main/src/class153) | 有序表：AVL、跳表、替罪羊树、Treap、FHQ Treap、Splay | [FHQ Treap：有序多重集合与序列平衡树](usage/data_structures/treap.md) |
| [151](https://github.com/algorithmzuo/algorithm-journey/tree/main/src/class151) | 笛卡尔树 | [单调栈与笛卡尔树](usage/basic/monotonic_stack.md) |
| [154](https://github.com/algorithmzuo/algorithm-journey/tree/main/src/class154)–[155](https://github.com/algorithmzuo/algorithm-journey/tree/main/src/class155) | 左偏树 | [左偏树](usage/data_structures/leftist_heap.md) |
| [156](https://github.com/algorithmzuo/algorithm-journey/tree/main/src/class156) | 带权并查集 | [带权并查集](usage/data_structures/weighted_dsu.md) |
| [157](https://github.com/algorithmzuo/algorithm-journey/tree/main/src/class157)–[158](https://github.com/algorithmzuo/algorithm-journey/tree/main/src/class158) | 可持久化线段树 | [主席树与区间第 k 小](usage/data_structures/persistent_segment_tree.md) |
| [159](https://github.com/algorithmzuo/algorithm-journey/tree/main/src/class159) | 可持久化前缀树 | [可持久化 01 Trie](usage/data_structures/persistent_binary_trie.md) |
| [165](https://github.com/algorithmzuo/algorithm-journey/tree/main/src/class165)–[167](https://github.com/algorithmzuo/algorithm-journey/tree/main/src/class167) | 可撤销并查集、线段树分治 | [可撤销并查集与线段树分治](usage/data_structures/rollback_dsu.md) |
| [176](https://github.com/algorithmzuo/algorithm-journey/tree/main/src/class176)–[179](https://github.com/algorithmzuo/algorithm-journey/tree/main/src/class179) | 普通莫队、带修莫队、回滚莫队 | [莫队 MosAlgorithm](usage/data_structures/mos_algorithm.md) |
| [181](https://github.com/algorithmzuo/algorithm-journey/tree/main/src/class181)–[182](https://github.com/algorithmzuo/algorithm-journey/tree/main/src/class182) | 线段树的合并与分裂 | [线段树合并与分裂](usage/data_structures/segment_tree_merge.md) |
| [201](https://github.com/algorithmzuo/algorithm-journey/tree/main/src/class201)–[204](https://github.com/algorithmzuo/algorithm-journey/tree/main/src/class204) | LCT | [Link-Cut Tree](usage/data_structures/link_cut_tree.md) |

## 图与树

| 讲次 | 主题 | 模板 |
| --- | --- | --- |
| [059](https://github.com/algorithmzuo/algorithm-journey/tree/main/src/class059)–[060](https://github.com/algorithmzuo/algorithm-journey/tree/main/src/class060) | 建图、拓扑排序 | [拓扑排序](usage/graph/topological_sort.md) |
| [061](https://github.com/algorithmzuo/algorithm-journey/tree/main/src/class061) | 最小生成树 | [Kruskal 与 Prim](usage/graph/mst.md) |
| [062](https://github.com/algorithmzuo/algorithm-journey/tree/main/src/class062)–[064](https://github.com/algorithmzuo/algorithm-journey/tree/main/src/class064) | BFS、01 BFS、Dijkstra、分层图 | [BFS、01 BFS、Dijkstra 与 Floyd](usage/graph/shortest_path.md) |
| [065](https://github.com/algorithmzuo/algorithm-journey/tree/main/src/class065) | Floyd、Bellman-Ford、SPFA | [最短路](usage/graph/shortest_path.md)、[SPFA 与负环](usage/graph/spfa.md) |
| [118](https://github.com/algorithmzuo/algorithm-journey/tree/main/src/class118)–[119](https://github.com/algorithmzuo/algorithm-journey/tree/main/src/class119) | 树上倍增、LCA | [倍增 LCA](usage/graph/lca.md)、[O(1) LCA](usage/graph/fast_lca.md) |
| [120](https://github.com/algorithmzuo/algorithm-journey/tree/main/src/class120) | 树的重心 | [树的重心与点分治](usage/graph/centroid.md) |
| [121](https://github.com/algorithmzuo/algorithm-journey/tree/main/src/class121) | 树的直径 | [树的直径](usage/graph/tree_diameter.md) |
| [122](https://github.com/algorithmzuo/algorithm-journey/tree/main/src/class122) | 树上差分 | [O(1) LCA](usage/graph/fast_lca.md)（手册中有点差分写法） |
| [123](https://github.com/algorithmzuo/algorithm-journey/tree/main/src/class123) | 换根 DP | [换根 DP](usage/graph/rerooting.md) |
| [142](https://github.com/algorithmzuo/algorithm-journey/tree/main/src/class142) | 负环、差分约束 | [SPFA、负环、差分约束](usage/graph/spfa.md) |
| [143](https://github.com/algorithmzuo/algorithm-journey/tree/main/src/class143) | 同余最短路 | [Dijkstra](usage/graph/shortest_path.md)（按余数建点） |
| [161](https://github.com/algorithmzuo/algorithm-journey/tree/main/src/class161)–[162](https://github.com/algorithmzuo/algorithm-journey/tree/main/src/class162) | 树链剖分 | [HLD](usage/graph/hld.md) |
| [163](https://github.com/algorithmzuo/algorithm-journey/tree/main/src/class163) | 树上启发式合并 | [DSU on tree](usage/graph/dsu_on_tree.md) |
| [164](https://github.com/algorithmzuo/algorithm-journey/tree/main/src/class164) | Kruskal 重构树 | [Kruskal 重构树](usage/graph/kruskal_tree.md) |
| [180](https://github.com/algorithmzuo/algorithm-journey/tree/main/src/class180) | 虚树 | [虚树](usage/graph/virtual_tree.md) |
| [183](https://github.com/algorithmzuo/algorithm-journey/tree/main/src/class183)–[185](https://github.com/algorithmzuo/algorithm-journey/tree/main/src/class185) | 点分治、点分树 | [树的重心与点分治](usage/graph/centroid.md) |
| [186](https://github.com/algorithmzuo/algorithm-journey/tree/main/src/class186) | 欧拉序 / dfn 序求 LCA | [O(1) LCA](usage/graph/fast_lca.md) |
| [188](https://github.com/algorithmzuo/algorithm-journey/tree/main/src/class188) | 欧拉路径 | [欧拉路径与欧拉回路](usage/graph/euler_path.md) |
| [189](https://github.com/algorithmzuo/algorithm-journey/tree/main/src/class189)–[190](https://github.com/algorithmzuo/algorithm-journey/tree/main/src/class190) | 强连通分量、缩点 | [SCC](usage/graph/scc.md) |
| [191](https://github.com/algorithmzuo/algorithm-journey/tree/main/src/class191)–[194](https://github.com/algorithmzuo/algorithm-journey/tree/main/src/class194) | 割边、边双、割点、点双、圆方树 | [割点、桥、点双、边双与圆方树](usage/graph/biconnected.md) |
| [195](https://github.com/algorithmzuo/algorithm-journey/tree/main/src/class195)、[198](https://github.com/algorithmzuo/algorithm-journey/tree/main/src/class198) | 优化建图 | [线段树优化建图](usage/graph/segment_tree_graph.md) |
| [196](https://github.com/algorithmzuo/algorithm-journey/tree/main/src/class196)–[197](https://github.com/algorithmzuo/algorithm-journey/tree/main/src/class197) | 2-SAT | [2-SAT](usage/graph/two_sat.md) |
| [207](https://github.com/algorithmzuo/algorithm-journey/tree/main/src/class207) | Johnson 全源最短路 | [SPFA 与 Johnson](usage/graph/spfa.md) |

## 字符串

| 讲次 | 主题 | 模板 |
| --- | --- | --- |
| [100](https://github.com/algorithmzuo/algorithm-journey/tree/main/src/class100)–[101](https://github.com/algorithmzuo/algorithm-journey/tree/main/src/class101) | KMP | [KMP 与前缀函数](usage/string/kmp.md) |
| [102](https://github.com/algorithmzuo/algorithm-journey/tree/main/src/class102) | AC 自动机 | [AC 自动机](usage/string/aho_corasick.md) |
| [103](https://github.com/algorithmzuo/algorithm-journey/tree/main/src/class103)–[104](https://github.com/algorithmzuo/algorithm-journey/tree/main/src/class104) | Manacher、扩展 KMP（Z 函数） | [Manacher](usage/string/manacher.md)、[Z 函数](usage/string/z_function.md) |
| [105](https://github.com/algorithmzuo/algorithm-journey/tree/main/src/class105) | 字符串哈希 | [双模哈希](usage/string/rolling_hash.md) |

## 数学

| 讲次 | 主题 | 模板 |
| --- | --- | --- |
| [041](https://github.com/algorithmzuo/algorithm-journey/tree/main/src/class041) | 最大公约数、同余原理 | [快速幂与逆元](usage/math/number_theory.md)、[ModInt](usage/math/modint.md) |
| [097](https://github.com/algorithmzuo/algorithm-journey/tree/main/src/class097) | 质数判断、质因子分解、质数筛 | [线性筛](usage/math/number_theory.md)、[Miller-Rabin 与 Pollard-Rho](usage/math/prime.md) |
| [098](https://github.com/algorithmzuo/algorithm-journey/tree/main/src/class098) | 快速幂、矩阵快速幂 | [快速幂](usage/math/number_theory.md)、[矩阵](usage/math/matrix.md) |
| [099](https://github.com/algorithmzuo/algorithm-journey/tree/main/src/class099) | 逆元、容斥原理 | [ModInt](usage/math/modint.md)、[组合数](usage/math/combinatorics.md) |
| [133](https://github.com/algorithmzuo/algorithm-journey/tree/main/src/class133)–[135](https://github.com/algorithmzuo/algorithm-journey/tree/main/src/class135) | 高斯消元：加法、异或、同余方程组 | [高斯消元](usage/math/gauss.md) |
| [139](https://github.com/algorithmzuo/algorithm-journey/tree/main/src/class139)–[141](https://github.com/algorithmzuo/algorithm-journey/tree/main/src/class141) | 裴蜀定理、扩展欧几里得、中国剩余定理 | [exgcd 与 CRT](usage/math/crt.md) |
| [144](https://github.com/algorithmzuo/algorithm-journey/tree/main/src/class144)、[147](https://github.com/algorithmzuo/algorithm-journey/tree/main/src/class147) | 二项式定理、卡特兰数 | [组合数](usage/math/combinatorics.md) |
| [146](https://github.com/algorithmzuo/algorithm-journey/tree/main/src/class146) | 康托展开、约瑟夫环 | [康托展开与约瑟夫问题](usage/math/permutation.md) |

## 动态规划

| 讲次 | 主题 | 模板 |
| --- | --- | --- |
| [072](https://github.com/algorithmzuo/algorithm-journey/tree/main/src/class072) | 最长递增子序列 | [LIS 与逆序对](usage/dp/sequence.md) |
| [073](https://github.com/algorithmzuo/algorithm-journey/tree/main/src/class073)–[075](https://github.com/algorithmzuo/algorithm-journey/tree/main/src/class075) | 01 / 完全 / 多重背包 | [背包](usage/dp/knapsack.md) |

## 没有做成模板的内容

这些讲次讲的是解题方法，每道题的状态和转移都不同，抄一份“通用代码”反而难用，建议直接看课程和例题：

- 递归与各类 DP（066–071、076–088、125–132）：区间 DP、树形 DP、状压 DP、数位 DP、轮廓线 DP、DP 优化；
- 贪心（089–094）、博弈（095–096）、01 分数规划（138，用二分答案）；
- 整体二分（168–169）、CDQ 分治（170–171）、分块与根号分治（172–175）：都是离线框架，按题目写；
- 边分治（187）、基环树（199）、仙人掌（200）、K-D 树（205–206）、树套树（160）：出现频率较低，暂未收录；
- 链表、二叉树遍历、Morris 遍历等入门内容（009–018、036–037、124）：用 STL 或直接手写即可。
