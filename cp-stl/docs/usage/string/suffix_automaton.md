# 后缀自动机 SuffixAutomaton 与广义 SAM

[模板源码](../../../string/suffix_automaton.hpp) · [完整示例](../../../examples/string/suffix_automaton.cpp) · [使用手册索引](../README.md)

后缀自动机是能识别一个串全部子串的最小自动机：从根出发的每条路径恰好对应一个不同的子串，状态数不超过 2n−1。
每个状态代表一组结束位置集合（endpos）相同的子串，它们的长度恰好是 (len[link], len] 中的每一个；
沿后缀链接 `link` 向上，得到的是越来越短的后缀，所有状态构成一棵 parent 树。
子串的出现次数、不同子串个数、最长公共子串、字典序第 k 小子串都归结为这张 DAG 或这棵树上的简单 DP。
对多个串依次调用 `add`，得到的就是广义后缀自动机。

## 接口和使用顺序

| 接口 | 含义 |
| --- | --- |
| `SuffixAutomaton sam(s)` / `sam.add(s)` | 小写 a-z。`add` 从根开始插入一个串，返回整个串所在的状态；多次调用即广义 SAM。 |
| `BasicSuffixAutomaton<Sigma, First>` | 其他字符集：从字符 First 开始的连续 Sigma 个字符，如 `BasicSuffixAutomaton<10, '0'>`。 |
| `extend(last, c)` | 底层接口：在状态 last 后面接字符编号 c（0 ≤ c < Sigma），返回新状态。新串从 last = 0 开始；在 Trie 上建广义 SAM 时 last 取父节点对应的状态。 |
| `size()`、`sam[v]` | 状态数（含根 0）；`sam[v].next[c]` 转移（0 表示没有）、`.link` 后缀链接（根为 −1）、`.len` 最长长度、`.end` 一次出现的结束位置（开区间）。 |
| `order()` | 全部状态按 len 从小到大排列。正序是 DAG 的拓扑序，逆序是 parent 树的自底向上顺序。 |
| `occurrences()` | `occ[v]`：状态 v 中的子串的出现次数（广义 SAM 中是在所有串里出现的总次数）。 |
| `distinct_substrings()` | 本质不同的非空子串个数，即 Σ(len[v] − len[link[v]])。 |
| `find(t)` | t 所在的状态，t 不是子串时返回 −1。配合 `occ[v]` 得到 t 的出现次数。 |
| `match(t)` | `result[i]`：t[0..i] 的最长后缀的长度，要求它是已插入串的子串。最大值就是最长公共子串。 |
| `advance(v, length, c)` | `match` 的单步版本，自己维护当前状态 v 和匹配长度，用于需要知道状态的题目。 |

## 完整例子

下面是可以直接编译的完整程序，数据写在代码里，**不需要输入**。在 workspace 根目录执行文末的 `python tools/cp.py example ...` 命令即可运行并核对输出。

```cpp
#include <algorithm>
#include <iostream>
#include <string>
#include <vector>
#include "string/suffix_automaton.hpp"

int main() {
    std::string s = "abcbc";
    cp::SuffixAutomaton sam(s);
    std::cout << "states=" << sam.size() << " distinct=" << sam.distinct_substrings() << '\n';

    // 子串的出现次数与最早出现的位置。
    std::vector<long long> occ = sam.occurrences();
    for (std::string t : {"bc", "abc", "c", "cc"}) {
        int v = sam.find(t);
        if (v == -1) std::cout << t << ": absent\n";
        else std::cout << t << ": count=" << occ[v] << " first=" << sam[v].end - int(t.size()) << '\n';
    }

    // 最长公共子串：让 t 在 s 的自动机上匹配，len[i] 是 t[0..i] 能匹配上的最长后缀。
    std::string t = "xbcbcy";
    std::vector<int> len = sam.match(t);
    int end = int(std::max_element(len.begin(), len.end()) - len.begin());
    std::cout << "lcs=" << t.substr(end + 1 - len[end], len[end]) << '\n';

    // 按 order 的逆序在转移 DAG 上 DP：paths[v] 是从 v 出发能读出的不同串的个数（含空串）。
    std::vector<int> order = sam.order();
    std::vector<long long> paths(sam.size(), 1);
    for (int i = sam.size() - 1; i >= 0; --i)
        for (int to : sam[order[i]].next) if (to) paths[order[i]] += paths[to];
    // 据此求字典序第 k 小的子串（k 从 0 开始，相同的子串只算一次）。
    std::string kth;
    long long k = 7;
    for (int v = 0; k >= 0; --k)
        for (int c = 0; c < 26; ++c) {
            int to = sam[v].next[c];
            if (!to) continue;
            if (k < paths[to]) { kth += char('a' + c); v = to; break; }
            k -= paths[to];
        }
    std::cout << "kth(7)=" << kth << '\n';

    // 广义后缀自动机：多次 add，统计的是所有串的子串。
    cp::SuffixAutomaton multi;
    for (std::string w : {"ab", "bab", "abc"}) multi.add(w);
    std::cout << "multi: distinct=" << multi.distinct_substrings()
              << " count(ab)=" << multi.occurrences()[multi.find("ab")] << '\n';
}
```

### 预期标准输出

```text
states=8 distinct=12
bc: count=2 first=1
abc: count=1 first=0
c: count=2 first=2
cc: absent
lcs=bcbc
kth(7)=bcb
multi: distinct=8 count(ab)=3
```

## 注意事项

- 子串 t 最早出现的起点是 `sam[v].end - |t|`（只插入了一个串时）；广义 SAM 中 `end` 是它在某一个串里的结束位置。
- 状态 v 中的子串长度范围是 `sam[sam[v].link].len + 1 .. sam[v].len`，这些子串的出现次数都是 `occ[v]`。
- 每个状态占 4·Sigma + 16 字节，状态数最多 2n：n = 10⁶、Sigma = 26 时最坏约 240MB（随机串约 160MB，构造约 0.1 秒）。内存紧张时改用后缀数组，或把字符集缩小到题目实际用到的范围。
- `occurrences()` 和 `order()` 每次调用都是 O(状态数)，查询多次时先存到变量里。
- 广义 SAM 的 `occurrences()` 统计的是总出现次数；要统计“在多少个不同的串中出现”，需要给每个状态记录最后一次被哪个串访问，沿 link 向上暴力跳并打标记。
- 同一个串 add 两次，出现次数会算两遍，但不会多建状态。

## 练习

- [P3804 【模板】后缀自动机（SAM）](https://www.luogu.com.cn/problem/P3804)：`occurrences()`，对 occ[v] > 1 的状态取 occ[v]·len[v] 的最大值
- [P6139 【模板】广义后缀自动机（广义 SAM）](https://www.luogu.com.cn/problem/P6139)：多次 `add` 后 `distinct_substrings()` 与 `size()`
- [SP1811 LCS - Longest Common Substring](https://www.luogu.com.cn/problem/SP1811)：`match` 的最大值
- [P3975 [TJOI2015] 弦论](https://www.luogu.com.cn/problem/P3975)：示例中的 DAG 路径计数；可重复时把每个状态的权值换成 occ[v]
- [P3181 [HAOI2016] 找相同字符](https://www.luogu.com.cn/problem/P3181)：两个串分别统计 occ，按状态相乘
- [P3346 [ZJOI2015] 诸神眷顾的幻想乡](https://www.luogu.com.cn/problem/P3346)：以每个叶子为根遍历树，用 `extend(父状态, c)` 建广义 SAM

也可以在仓库根目录执行：

```powershell
python tools/cp.py example "string/suffix_automaton"
```
