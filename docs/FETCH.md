# 抓取样例与本地测试

`tools/cp.py` 可以从 Codeforces、AtCoder、LeetCode 抓取题目样例，省去逐个复制粘贴；
`test` 命令只编译一次，就把全部样例跑完并逐个判定。只用 Python 标准库，不需要额外安装。
网站要求浏览器验证时（Codeforces 的 403），改为[从浏览器接收题目](#从浏览器接收题目competitive-companion)。

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

直接粘贴浏览器地址栏中的题目链接总是可行的。为了少打字，也可以只写链接中的“比赛号 + 题号”部分（简写）。

```powershell
python tools/cp.py new 1610F
python tools/cp.py new abc400_e
python tools/cp.py new lc/two-sum
```

### Codeforces：比赛号 + 题号

链接 `https://codeforces.com/contest/1610/problem/F` 中取 `1610` 和 `F`。

| 写法 | 说明 |
| --- | --- |
| `1610F` | 最常用 |
| `1610/F`、`cf1610F`、`cf/1610/F` | 与上面相同 |
| `1856E1` | 带数字的题号（E1、E2）也可以 |
| `1610f` | 题号不区分大小写，自动转成大写 |
| `gym/104114/A`、`cf/gym/104114/A` | Gym 题；比赛号不小于 100000 时也会自动按 Gym 处理 |

链接可以是 contest、problemset 或 gym 页面。默认归档名为 `cf/1610/F`，Gym 为 `cf/gym/104114/A`。

### AtCoder：任务编号

链接 `https://atcoder.jp/contests/abc400/tasks/abc400_e` 中取最后一段 `abc400_e`。

| 写法 | 说明 |
| --- | --- |
| `abc400_e` | 链接最后一段；ARC、AGC 等同理，如 `arc180_a` |
| `abc400/E`、`atcoder/abc400/E` | 比赛名 + 题号，与上面相同 |

默认归档名为 `atcoder/abc400/E`。

简写按“比赛名_题号”推算任务编号。少数比赛的任务编号与比赛名不一致（如部分早期 ABC 与 ARC 同场举办，
ABC 页面里的任务编号是 `arc0xx_a`），按简写推算会找不到题目，这时请粘贴链接。

### LeetCode：题目的英文名（slug）

链接 `https://leetcode.cn/problems/two-sum/` 中取 `problems/` 后面的 `two-sum`。

| 写法 | 说明 |
| --- | --- |
| `leetcode/two-sum`、`lc/two-sum` | 访问力扣中国站 leetcode.cn |

链接可以来自 leetcode.cn 或 leetcode.com，周赛题目的链接也可以；用国际站时请粘贴链接。默认归档名为 `leetcode/two-sum`。

### 其他

- 简写与归档名格式相同：`cf/1610/F`、`atcoder/abc400/E`、`leetcode/two-sum` 既能用于 new/fetch，也是 save/load 使用的名字。
- 无法识别的写法会直接报错并列出可用格式，不会抓取别的题目。例如只写 `two-sum` 时，需要改成 `leetcode/two-sum`。
- 想换一个归档名，加 `--name`，例如 `python tools/cp.py new 1610F --name cf/1610/F-v2`。

## 从浏览器接收题目（Competitive Companion）

Codeforces 的题目页有时会开启 Cloudflare 浏览器验证，脚本直接请求只会得到 403，只有真正的浏览器能通过。
这时改由浏览器把题目交给工具：浏览器扩展 [Competitive Companion](https://github.com/jmerle/competitive-companion)
在页面里解析出样例，发到本机端口，`cp.py` 在那里接收。

准备工作只做一次：在 Chrome/Edge 或 Firefox 的扩展商店安装 Competitive Companion。
工具监听的 10043 是扩展默认就会推送的端口，装好后不用设置。

**用法一：命令不变。** `new`/`fetch` 遇到浏览器验证时自动转为等待浏览器：

```text
> python tools/cp.py new 1545C
Fetching https://codeforces.com/contest/1545/problem/C
codeforces.com answered with a browser check (Cloudflare), which a script cannot pass. Taking the problem from your browser instead.
Waiting for the browser on port 10043 (Ctrl+C cancels, gives up after 300s).
Open https://codeforces.com/contest/1545/problem/C and click the Competitive Companion button (green plus).
```

**命令等待期间**，在浏览器里打开这道题，点扩展的图标（绿色加号；Chrome/Edge 默认把它收在工具栏的拼图菜单里，
可以固定到工具栏），命令就会继续，结果和直接抓取相同。只装扩展不点图标，命令收不到任何东西。
点了别的题目会被忽略并继续等待，不会把样例写错。

扩展点击后页面顶部会闪过一条蓝色细进度条；它发送成功或失败都没有提示（每个端口只等 0.5 秒）。
所以等到超时的时候，工具会说明它看到了什么：

| 超时信息 | 含义 |
| --- | --- |
| `Nothing connected to port 10043` | 等待期间浏览器没有连过来：没有点图标，或者点的那个浏览器没装扩展。在同一个浏览器的地址栏打开 `http://localhost:10043/`，能看到 `cp.py is waiting here…` 说明浏览器连得上本工具 |
| `… connection(s) reached port 10043, but the last one …` | 有连接但不是题目，例如只是在地址栏打开了上面的地址 |
| 页面弹出 `Something went wrong while running Competitive Companion's …` | 扩展解析页面失败（例如页面还停在验证页），与本工具无关 |

**用法二：不写题号。** `listen` 接收浏览器发来的任意一道题：

```powershell
python tools/cp.py listen               # 相当于 new：备份 → 载入模板 → 写入样例
python tools/cp.py listen --keep-code   # 相当于 fetch：只更新样例，不改 main.cpp
```

- `listen` 对扩展支持的所有网站都有效（洛谷、牛客、CSES 等）。Codeforces、AtCoder 的归档名照常自动生成；
  其他网站没有默认归档名，用 `--name luogu/P1001` 指定，或在 `save luogu/P1001` 时写出。
- 每次只接收一道题，收到后监听就结束；扩展的“解析整场比赛”只会取到第一道。
- 等待 300 秒没有收到就放弃，可用 `--wait 600` 调整；和抓取一样，收到并解析成功之前不会改动任何文件。
- 端口被其他工具占用时会报错。用 `--port 12345` 换一个，并在扩展的选项页 Custom ports 中加上同一个端口。
- LeetCode 需要题目的代码框架才能生成本地驱动，扩展不提供，所以 LeetCode 不走这条路，仍然直接抓取。
- 监听只绑定本机回环地址（127.0.0.1 和 ::1），局域网内的其他机器访问不到。

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
- Codeforces 返回浏览器验证页（Cloudflare）时，工具会转为等待浏览器扩展，见[从浏览器接收题目](#从浏览器接收题目competitive-companion)。
  没装扩展时按 Ctrl+C 取消，手动粘贴样例。
- 只有 PDF 题面的题目（常见于 Gym）没有可抓取的样例。
- 交互题的样例是交互过程，工具会给出提示；不能直接用 run/test 判定。
- 网络请求使用系统代理：优先读取 HTTP_PROXY/HTTPS_PROXY 环境变量，未设置时使用 Windows 的代理设置。
