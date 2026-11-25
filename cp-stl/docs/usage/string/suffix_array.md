# 后缀数组 SuffixArray：SA-IS、height 与 LCP

[模板源码](../../../string/suffix_array.hpp) · [完整示例](../../../examples/string/suffix_array.cpp) · [使用手册索引](../README.md)

把一个串的所有后缀按字典序排序。`sa` 给出排序结果，`height` 给出排名相邻的两个后缀的最长公共前缀，
这两个数组加上一个 ST 表，就能 O(1) 回答任意两个后缀的 LCP、比较任意两个子串，是处理“重复子串、公共子串、子串排名”这类问题的通用工具。
构造使用 SA-IS（诱导排序），线性时间：10⁶ 长度的随机串连同 rank、height 一起约 40 毫秒（g++ -O2 实测）。

## 接口和使用顺序

| 接口 | 含义 |
| --- | --- |
| `suffix_array(s)` | 只要 sa 时用它。s 可以是字符串（按 unsigned char 比较，O(n)）或任意可比较元素的 vector（先离散化，O(n log n)）。 |
| `SuffixArray sa(s)` | s 是字符串或 `vector<int>`。构造后可直接读下面三个数组。 |
| `sa.sa[i]` | 排名第 i（从 0 开始）的后缀的起点。 |
| `sa.rank[p]` | 后缀 p 的排名，`rank[sa[i]] = i`。 |
| `sa.height[i]` | 后缀 `sa[i-1]` 与 `sa[i]` 的最长公共前缀长度，`height[0] = 0`。 |
| `lcp(i, j)` | 后缀 i 与后缀 j 的最长公共前缀。第一次调用建 ST 表 O(n log n)，之后 O(1)。 |
| `compare(l1, r1, l2, r2)` | 比较子串 [l1,r1) 与 [l2,r2)，返回负数 / 0 / 正数，O(1)。 |
| `find(pattern)` | 返回排名区间 [lo,hi)：pattern 出现 hi−lo 次，位置是 `sa[lo..hi)`。O(\|pattern\| log n)。 |
| `distinct_substrings()` | 本质不同的非空子串个数，即 n(n+1)/2 − Σheight。 |

## 完整例子

下面是可以直接编译的完整程序，数据写在代码里，**不需要输入**。在 workspace 根目录执行文末的 `python tools/cp.py example ...` 命令即可运行并核对输出。

```cpp
#include <iostream>
#include <string>
#include "string/suffix_array.hpp"

int main() {
    std::string s = "banana";
    cp::SuffixArray sa(s);
    for (int i = 0; i < sa.size(); ++i)
        std::cout << "sa[" << i << "]=" << sa.sa[i] << " height=" << sa.height[i] << ' ' << s.substr(sa.sa[i]) << '\n';
    std::cout << "distinct=" << sa.distinct_substrings() << " lcp(1,3)=" << sa.lcp(1, 3)
              << " compare(ban,nan)=" << sa.compare(0, 3, 2, 5) << '\n';

    auto [lo, hi] = sa.find("ana"); // 出现的位置是 sa.sa[lo..hi)
    std::cout << "ana: count=" << hi - lo << " at";
    for (int i = lo; i < hi; ++i) std::cout << ' ' << sa.sa[i];
    std::cout << '\n';

    // 两个串的最长公共子串：用一个不出现的字符拼接，取“来自不同串的相邻后缀”的 height 最大值。
    std::string a = "xabcdy", b = "zzabcd", joined = a + "#" + b;
    cp::SuffixArray both(joined);
    int best = 0, at = 0;
    for (int i = 1; i < both.size(); ++i) {
        bool first = both.sa[i - 1] < int(a.size()), second = both.sa[i] < int(a.size());
        if (first != second && both.height[i] > best) { best = both.height[i]; at = both.sa[i]; }
    }
    std::cout << "lcs=" << joined.substr(at, best) << '\n';
}
```

### 预期标准输出

```text
sa[0]=5 height=0 a
sa[1]=3 height=1 ana
sa[2]=1 height=3 anana
sa[3]=0 height=0 banana
sa[4]=4 height=0 na
sa[5]=2 height=2 nana
distinct=15 lcp(1,3)=3 compare(ban,nan)=-1
ana: count=2 at 3 1
lcs=abcd
```

## 注意事项

- `height` 采用国内常见的定义：`height[i]` 属于排名 i 和 i−1 这一对，长度为 n；AtCoder Library 的 `lcp_array` 长度是 n−1，下标差 1。
- 排名在 [l, r] 内的后缀两两的 LCP 的最小值是 `min(height[l+1..r])`，所以“出现至少 k 次的最长子串”是 height 上长度 k−1 的滑动窗口最小值的最大值。
- 处理多个串时用互不相同、且不在原串中出现的分隔符拼接；分隔符不够用时把串转成 `vector<int>`，分隔符取 256、257、……。
- `lcp` / `compare` 需要的 ST 表占 n·⌈log₂n⌉ 个 int（n = 10⁶ 时约 80MB），只用 sa 和 height 时不会建。
- 字符按 unsigned char 比较，与 `std::string` 的 `<` 一致；所有位置和长度按字节计。
- 循环同构（如字符加密）把串复制一遍再求后缀数组，只看起点在前半段的后缀。

## 练习

- [P3809 【模板】后缀排序](https://www.luogu.com.cn/problem/P3809)：输出 `sa[i] + 1`
- [P2408 不同子串个数](https://www.luogu.com.cn/problem/P2408)：`distinct_substrings()`
- [P2852 [USACO06DEC] Milk Patterns G](https://www.luogu.com.cn/problem/P2852)：整数序列版本，height 上的滑动窗口最小值
- [P4051 [JSOI2007] 字符加密](https://www.luogu.com.cn/problem/P4051)：串复制一遍后排序
- [P4248 [AHOI2013] 差异](https://www.luogu.com.cn/problem/P4248)：height + 单调栈统计所有后缀对的 LCP 之和
- [P2463 [SDOI2008] Sandy 的卡片](https://www.luogu.com.cn/problem/P2463)：多个差分序列拼接，二分长度后在 height 上分组

也可以在仓库根目录执行：

```powershell
python tools/cp.py example "string/suffix_array"
```
