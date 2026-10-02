# 抓取样例与本地测试

`tools/cp.py` 可以从 Codeforces、AtCoder、LeetCode 抓取题目样例，省去逐个复制粘贴；
`test` 命令只编译一次，就把全部样例跑完并逐个判定。只用 Python 标准库，不需要额外安装。

## 快速开始

```powershell
# 开新题：备份当前工作 → 载入模板 → 抓取全部样例
python tools/cp.py new https://codeforces.com/contest/1610/problem/F

# 写完代码，在 VS 中 Ctrl+Shift+S 保存，然后检查所有样例
python tools/cp.py test

# 归档到 solutions/cf/1610/F；名字取自抓取的题目，不用再输入
python tools/cp.py save
```

已经开始写代码、只想补抓样例时用 `fetch`，它不会改动 main.cpp：

```powershell
python tools/cp.py fetch 1610F
```

## 题目写法

| 平台 | 可以这样写 | 默认归档名 |
| --- | --- | --- |
| Codeforces | 题目链接（contest、problemset、gym）、`1610F`、`1610/F`、`cf/1610/F` | `cf/1610/F`；Gym 为 `cf/gym/104114/A` |
| AtCoder | 题目链接、`abc400_e`、`abc400/E`、`atcoder/abc400/E` | `atcoder/abc400/E` |
| LeetCode | 题目链接（leetcode.cn 或 leetcode.com，周赛链接也可以）、`leetcode/two-sum` | `leetcode/two-sum` |

直接粘贴浏览器地址栏中的链接总是可行的，简写只是为了少打字。

- `leetcode/<slug>` 简写访问 leetcode.cn；用 leetcode.com 时粘贴链接。
- AtCoder 简写按“比赛名_题号”推算任务编号。少数比赛的任务编号与比赛名不一致（如部分 ABC 与 ARC 同场举办），这时请粘贴链接。
- 想换一个归档名，加 `--name`，例如 `python tools/cp.py new 1610F --name cf/1610/F-v2`。

## 会写入哪些文件

| 文件 | 内容 |
| --- | --- |
| data/samples/1.in、1.ans、2.in、2.ans… | 全部样例；`.ans` 是期望输出 |
| data/input.txt、data/expected.txt | 第 1 组样例的副本，供 `run` 和 F5（LOCAL_FILE）使用 |
| data/problem.json | 当前题目的归档名、链接、标题和时限，由工具维护 |

- 写入前和 new/load 一样，先把当前工作（含 data/samples）备份到 backups/。
- 先联网抓取、解析成功后才写文件；网络或解析出错时不会改动任何文件。
- 想用第 2 组样例做 F5 调试，把 data/samples/2.in 的内容复制到 data/input.txt。
- `save` 把 samples/ 和 problem.json 一起归档，并把链接填进该题 README.md 的“题目链接”。只填写空着的这一行，已经写过的内容不会被改。
- `load` 会恢复归档中的样例和题目信息；之后直接 `save` 就会存回同一个归档。
- 当前没有 data/samples 时，`save` 保留归档里原有的样例，不会删除。
- data/samples 不加入 VS 的文件列表（每道题都会变，加入会让项目文件频繁改动），需要时在资源管理器中打开。
  归档里的 samples/ 会显示在 VS 的“03 题解归档”中。

## test：一次检查全部样例

```powershell
python tools/cp.py test                       # 用 g++ 编译 main.cpp，逐个运行 data/samples
python tools/cp.py test --exe build/bin/x64/Debug/cp-algorithm.exe   # 改用 VS 生成的程序
python tools/cp.py test --timeout 5 --exact
```

某组答案错误时的输出：

```text
Sample 1: OK  [0.009s]
Sample 2: OK  [0.005s]
Sample 3: WRONG ANSWER  [0.005s]
  input:
    5
    20
  expected:
    -15
  output:
    999
  first difference: token 1: expected '-15', got '999'
  stderr:
    dbg a=5
2/3 samples passed. Outputs: build\tools\test\output
```

- 判定结果有 OK、WRONG ANSWER、RUNTIME ERROR、TIME LIMIT；没有 `.ans` 的样例只运行、不判定。
- 比较规则与 `run` 相同：默认按空白分隔的 token 比较，区分大小写；`--exact` 改为逐字节比较。
- 出错时显示输入、期望输出、实际输出、第一处不同和标准错误（例如 debug 输出）；
  每组的完整输出保存在 build/tools/test/output/。
- 没有 data/samples 时，`test` 改用 data/input.txt 和 data/expected.txt；expected.txt 为空时只运行。
- 编译选项与 `run` 相同：`--cxx`、`--std`、`-I`。全部通过时退出码为 0，否则为 1。
- 允许多种答案或有浮点误差的题目，WRONG ANSWER 需要自己判断。

`run` 只处理 data/input.txt 这一份输入，适合跑自己构造的数据。

## LeetCode

LeetCode 按函数签名评测，没有标准输入输出。用 `new` 开题时会生成一个可以直接在本地运行的 main.cpp：

```cpp
// LeetCode 1. 两数之和
// https://leetcode.cn/problems/two-sum/
// Submit only the code between the two markers; the rest runs it locally.
// Local input uses LeetCode's test case format: one value per line.
#include "leetcode.h"

// ---- LeetCode submission begins ----
class Solution {
public:
    vector<int> twoSum(vector<int>& nums, int target) {

    }
};
// ---- LeetCode submission ends ----

int main() {
    return lc::run_solution(&Solution::twoSum);
}
```

在框架里写解答，之后 `test`、`run`、F5 调试的用法都和普通题目一样。
**提交时只复制两条分隔线之间的代码。**

- include/leetcode.h 和 LeetCode 环境一样提供 ListNode、TreeNode、常用标准头文件和 `using namespace std`。
  它负责读入样例、调用你的代码，并按 LeetCode 的格式输出（`[0,1]`、`"abc"`、`true`、`2.50000`、`null`）。
- 样例格式与 LeetCode “测试用例”面板相同：每个参数占一行；设计类题目（如 LRU 缓存）每组两行，
  分别是操作名列表和参数列表。data/input.txt 中可以连续写多组，空行会被忽略。
- 支持的参数和返回类型：int、long long、double、bool、char、string、ListNode*、TreeNode*，
  以及由它们任意嵌套的 vector。
- 原地修改的题目（如“旋转图像”）输出修改后的参数；“返回 k 并修改数组”的题目输出
  `k, nums = [前 k 个元素,_,_]`，与题面写法一致。这类题通常允许任意顺序，顺序不同的 WRONG ANSWER 可能仍会被接受。
- 用到 Node（图、N 叉树等）这类不支持的类型时，生成的 main() 只有提示注释，需要自己写读入。
- 样例格式有误时，程序以退出码 1 结束，并在标准错误中指出出错的行号。
- 可以用 `debug(x)` 输出调试信息（来自 cp-stl/util/debug.hpp），提交前删掉。
- MSVC 要求有返回值的函数必须写 return，刚生成的空函数要先补上返回语句，VS 才能生成；g++ 只给警告。
- 对 LeetCode 题目用 `fetch` 只更新样例，不会重新生成 main.cpp。

## 限制与排错

- 会员题、需要登录或正在进行的比赛可能抓不到（工具不保存账号信息）。这时把样例手动粘贴到
  data/input.txt 和 data/expected.txt，`test` 会使用它们。
- Codeforces 偶尔返回浏览器验证页（Cloudflare），稍等片刻重试即可。
- 只有 PDF 题面的题目（常见于 Gym）没有可抓取的样例。
- 交互题的样例是交互过程，工具会给出提示；不能直接用 run/test 判定。
- 网络请求使用系统代理：优先读取 HTTP_PROXY/HTTPS_PROXY 环境变量，未设置时使用 Windows 的代理设置。
