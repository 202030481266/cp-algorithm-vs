# Visual Studio 中的 ICPC 工作流

## 1. 打开正确的入口

双击根目录 cp-algorithm.sln，使用 Visual Studio 的解决方案模式。
项目仍保留原来的项目 GUID、v145 工具集和本机 AtCoder Library 的包含目录。

工具栏选 Debug / x64。Ctrl+Shift+B 生成；F5 调试；Ctrl+F5 不附加调试器运行。
F9 设置/取消断点，F10 单步跳过函数，F11 进入函数，Shift+F11 跳出。
快捷键按默认键盘方案说明，也可从菜单执行对应操作。
调试时用“局部变量”“监视”“调用堆栈”观察状态，输出诊断优先用 cerr。

Release / x64 用于观察优化后的运行表现；Debug 适合查越界、断言与中间状态。
本机运行时间只作为参考，线上评测的编译器、机器和计时方式不同。

“项目 → 属性”中已经设置：

| 设置 | 当前值 |
| --- | --- |
| 输出目录 | $(ProjectDir)build\bin\$(Platform)\$(Configuration)\ |
| 中间目录 | $(ProjectDir)build\obj\$(Platform)\$(Configuration)\ |
| 调试 → 工作目录 | $(ProjectDir) |
| C/C++ → 语言 | C++23 |
| C/C++ → 预编译头 | 不使用 |
| 额外包含目录 | include/，以及原有 D:\CP-Templates-and-Solutions\ac-library |
| 编译器额外选项 | /utf-8 |
| 预处理器定义 | LOCAL、_CRT_SECURE_NO_WARNINGS 等；没有默认启用 LOCAL_FILE |

这里的 pch.h 现在是普通的公共头文件，不会另外生成预编译头。
modules/ 中三个 import std 的旧模块作为模板保留，当前单文件题目不编译它们。
以后真正使用 import ops 等模块时，需要另行配置模块编译；在线提交通常应展开成普通头文件/单文件源码。

属性页的设置与调试工作目录位置可参考
[Microsoft：C++ 项目属性](https://learn.microsoft.com/en-us/cpp/build/reference/property-pages-visual-cpp?view=msvc-170)
和[调试属性](https://learn.microsoft.com/en-us/cpp/build/reference/debugging-prop-pages?view=msvc-170)。

## 2. 磁盘目录、筛选器与 main() 的关系

VS 中“源文件”“头文件”或本项目的编号分类叫**筛选器**，是逻辑分组。
只在 VS 里拖到另一个筛选器，磁盘上的文件位置不会跟着改变。
本次已经同时整理了真实目录和 .vcxproj.filters。参见
[Microsoft：vcxproj.filters](https://learn.microsoft.com/en-us/cpp/build/reference/vcxproj-filters-files?view=msvc-170)。

最省事的方式是始终编辑根目录 main.cpp，归档代码由工具展示在分类下。
VS 的 F5 编译和运行的是启动项目，不是“当前编辑器里打开的那个 .cpp”。
所以打开旧题 main.cpp 查看，并不意味着 F5 会运行旧题；需要先 load 到工作入口。

普通 C++ 可执行程序不能把多个独立 main() 一起链接。
本项目只把根目录 main.cpp 标成 ClCompile，其他题解、模板和对拍文件标成“不参与生成”的 None 项。

手工加 .cpp 时，可将“文件属性 → 配置属性 → 常规 → 项类型”设为“不参与生成（Does not participate in build）”。
若保留“C/C++ 编译器”项类型，则在**所有配置、所有平台**下把“从生成中排除”设为“是”。
这两种方法都可避免归档文件一起编译；工具生成的条目使用前一种方法。
文件需要在磁盘上真的移动时，先在资源管理器移动，再运行 python tools/cp.py sync。

如果看到 build、backups、.vs 混在树中，检查解决方案资源管理器是否开启了“显示所有文件”。

## 3. 如何分类而不越分越乱

建议实体目录按来源保存：

```text
solutions/
  codeforces/2100/A/
    main.cpp
    input.txt
    expected.txt
    README.md
  atcoder/abc400/E/
  luogu/Pxxxx/
  training/2026-09-22/A/
  unclassified/o.cpp
```

一个题目的“二分 + 贪心”“DP + 数据结构”等标签写在该题 README.md。
同一份源码不必为了算法标签复制到多个文件夹。
模板放 templates/；被多份本地代码包含的公共实现放 include/；尚未确认题号的旧代码暂放 unclassified/。
名称用字母、数字、下划线和短横线；题号和比赛号是稳定标识，题名与笔记可写中文。

工具命令：

| 命令 | 行为 |
| --- | --- |
| python tools/cp.py save codeforces/2100/A | 复制 main.cpp、输入、期望输出到新目录，生成笔记骨架，刷新 VS 分类 |
| python tools/cp.py new | 将当前工作备份到 backups/work-时间-随机后缀/，载入模板，清空当前样例与输出 |
| python tools/cp.py load codeforces/2100/A | 先备份当前工作，再载入已归档源码和样例 |
| python tools/cp.py sync | 刷新 VS 中的文件列表与筛选器 |

save 不覆盖已有目录，也不会清空当前题目。
new/load 的备份包括 main.cpp、input.txt、expected.txt 和 output.txt。
data/brute.cpp、data/gen.cpp 等自建辅助文件保持原位，不会随 save/load 一起归档；
要保留每题的生成器与暴力，请把它们复制到该题目录，再 sync。
不要把上一题的生成器和暴力误用到下一题。

工具从自身位置确定项目根目录，参数中的源码、输入和输出相对路径都按项目根目录解释。
-I 的目录由当前终端工作目录解释。
不要一边有未保存的编辑，一边执行 new/load；先 Ctrl+Shift+S，再切题，最后重新加载外部改动。

## 4. 文件输入输出的三种用法

### A. 不改代码，外部把文件接到 cin/cout 上

写题继续使用 cin/cout，使用工具来读写文件：

```powershell
python tools/cp.py run --input data/input.txt --output data/output.txt
python tools/cp.py run --expected data/expected.txt
```

默认用 g++ -O2 -std=c++23 编译 main.cpp，附加 include/ 目录。
每次重新编译成功后才会运行；编译失败不会偷偷运行旧程序。
调试输出 stderr 单独保存在 output.stderr.txt，不会混进待比较答案。
输出文件会被本次结果替换；需要保留时，指定另外的 --output 路径。

如果想直接运行 VS 刚生成的 MSVC 程序：

```powershell
python tools/cp.py run --exe build/bin/x64/Debug/cp-algorithm.exe --expected data/expected.txt
```

--exe 不触发生成；修改代码后要先在 VS 中重新生成。
也可使用 Windows CMD 的重定向语法；在 PowerShell 中通过 cmd 执行：

```powershell
cmd /d /c ".\build\bin\x64\Debug\cp-algorithm.exe < .\data\input.txt > .\data\output.txt"
```

以上是 cmd 的 < 输入重定向；不要直接在 PowerShell 中照抄成裸 < 命令。
工具使用子进程管道传递原始字节，项目路径含中文、空格也可以使用。

### B. F5 断点调试时读文件

新模板已经包含以下分支。当前保留的旧 main.cpp 没有自动插入，可自行把它加在 main() 开头：

```cpp
#ifdef LOCAL_FILE
    if (!freopen("data/input.txt", "r", stdin)) {
        perror("data/input.txt");
        return 1;
    }
    if (!freopen("data/output.txt", "w", stdout)) {
        perror("data/output.txt");
        return 1;
    }
#endif
    ios::sync_with_stdio(false);
    cin.tie(nullptr);
```

需有 #include <cstdio>。
freopen 要放在关闭 iostream 同步及任何输入输出之前，并检查打开失败。
项目工作目录已经固定为根目录，所以 data/input.txt 指向根目录下的数据文件，与 .exe 所在位置无关。

右键项目 → 属性，选择 Debug、x64 → C/C++ → 预处理器 → 预处理器定义，
在保留现有定义的前提下添加 LOCAL_FILE。随后 F5 即可从文件读入并停在断点。
想让输出继续显示在控制台，就只保留输入的 freopen。
切回键盘输入时，从该配置移除 LOCAL_FILE；只修改当前编辑器文件的宏并不会改变其他配置。

LOCAL_FILE 由本机项目配置提供，不要在提交源码中无条件 #define LOCAL_FILE。
对拍工具只定义 LOCAL，不定义 LOCAL_FILE，因此模板中的文件重定向默认不会干扰对拍。

### C. 题目明确要求文件读写

若题面指定例如 test.in/test.out，就严格使用题目要求的文件名，不套用只在本地启用的宏。
普通 stdin/stdout 评测则继续使用标准输入输出。

关闭 sync_with_stdio 后，避免交替混用 cin 和 scanf、cout 和 printf 来操作同一条流。
数据文件建议保存为 UTF-8 无 BOM，避免文件开头的 BOM 影响第一个数字的解析。
大量数据用 '\n'，无需每行 endl 强制刷新。

## 5. 怎样写对拍

需要三个各自可独立编译的程序：

1. solution：你的高效算法，可能写错；默认是 main.cpp。
2. brute：小规模下容易确信正确的朴素算法，宁可慢一些。
3. gen：根据命令行给出的整数 seed 生成一份合法输入，数据写 stdout，诊断写 stderr。

```text
seed → gen → 同一份输入 → solution → 输出 A
                       → brute    → 输出 B
                                 比较 A 与 B
```

不要让两个解法共享同一段核心转移，否则很可能同时出错。
先手算几个小样例验证 brute，再随机比较。
gen 的范围要小到 brute 能完成，并覆盖题面约束：非负/正数、图是否连通、边是否允许重复等。
还应主动造 n=1、全相等、全负数、边界值、重复值等容易漏掉的情况。
示例题是“最大**非空**连续子段和”，正解 O(n)，暴力枚举区间 O(n²)；
生成器包含全负、全零、全正、正负交替、大值等模式。

```powershell
python tools/cp.py stress --solution examples/stress/solution.cpp --brute examples/stress/brute.cpp --gen examples/stress/gen.cpp --iterations 1000 --seed 1

# 换成自己的题；先准备 data/brute.cpp 和 data/gen.cpp：
python tools/cp.py stress --brute data/brute.cpp --gen data/gen.cpp --iterations 10000 --seed 1 --timeout 2
```

每个源码仅在开始时编译一次。每轮使用 seed、seed+1、seed+2……，
把同一份生成结果依次交给两个程序。固定种子便于复现。
这里的限时针对每个程序的一次运行，超时会终止该进程。
发现生成器/暴力异常会单独报告，不会当作待测解法的 WA。

默认比较分隔出的 token，忽略行尾和多余空白，但区分大小写。
1、1.0 和 1e0 是三个不同 token。
--exact 改成逐字节比较，此时 CRLF/LF 也有区别。
浮点误差、多解构造、输出顺序自由等题目需要按题意编写 checker；
交互题需要交互器；当前工具不处理这些特殊判题方式。

失败后保存目录类似：

```text
build/stress/failures/日期时间-seed-种子-随机后缀/
  input.txt
  solution.out
  brute.out
  generator.stderr.txt
  solution.stderr.txt
  brute.stderr.txt
  metadata.json
  sources/
    solution.cpp
    brute.cpp
    generator.cpp
```

未运行的程序不会有输出文件。
metadata.json 记录状态、种子、编译命令、退出码和耗时；sources/ 保存本轮源码快照。
依赖的公共头文件仍来自工作区，反例目录不是完整编译环境快照。
把工具打印的真实目录替换到以下命令中：

```powershell
python tools/cp.py run --input build/stress/failures/实际目录/input.txt
python tools/cp.py run --input build/stress/failures/实际目录/input.txt --expected build/stress/failures/实际目录/brute.out
```

要在 VS 调试，复制反例到 data/input.txt，按上节启用 LOCAL_FILE，然后 F5。
如果想重新生成同一个随机用例，使用原生成器加 --seed 对应值 --iterations 1。
“随机 10000 组通过”只提供错误筛查，不能代替正确性证明。

## 6. 提交与编译器差异

VS 用 MSVC，工具默认用 g++；两者都跑一遍有助于发现依赖差异。
本机现有 g++ 是 15.2.0；比赛环境版本和标准仍以题目平台为准。

新模板列出标准头文件，单独提交也能使用。
旧 main.cpp 用 _MSC_VER 分支包含 pch.h，GCC 分支包含 bits/stdc++.h；
其 format、concepts、ranges 依赖相应标准库支持，降低 --std 不会自动改写这些代码。
pch.h、dbg.h、AtCoder Library 等本地头文件不一定在评测环境中存在。
需确认平台提供的库，或按比赛规则把自己的必要实现合并进提交文件。
__int128、PBDS、bits/stdc++.h 等 GCC 扩展不适用于一般 MSVC 编译。

工具自动编译不会读取 .vcxproj 中所有选项。
例如需要 AtCoder Library 时，显式加包含目录：

```powershell
python tools/cp.py run -I D:\CP-Templates-and-Solutions\ac-library
```

提交前检查输入是否有 T、整数范围、数组边界、复杂度和输出格式；
标准答案不要夹带调试信息。cerr 适合本地诊断，提交前也应关掉大量日志。

## 7. 缓存、备份和恢复

build/ 存放可重新生成的程序与中间文件，但其中 stress/failures/ 的反例值得保留，
清理 build 前先把重要反例移入对应题目的目录。
旧 x64/ 与 cp-algorithm/x64/ 已移动到 build/legacy/，没有删除原始文件。

.vs/ 是 IDE 的隐藏缓存。检查时约 2.39 GiB，VS 正在使用，整理时保留。
需要回收这部分空间时先保存工作、关闭 VS，之后可以删除 .vs，再打开解决方案让它重建；
窗口布局、断点等部分本地状态也可能随之重置，重新索引需要时间。
不要误删 .sln、.vcxproj、源码或 include/。

此目录使用 Git 管理，默认分支为 main，远端仓库为
[202030481266/cp-algorithm-vs](https://github.com/202030481266/cp-algorithm-vs)。
源代码、模板、工具、文档、样例输入/期望输出和 VS 共享项目配置纳入版本管理；
缓存、编译产物、个人 .user 配置和本地备份由 .gitignore 排除。
backups/ 只存在于本机，不会随 Git 克隆下载；日常提交与同步命令见根目录 README.md。

恢复切题前的工作：在 backups/work-.../ 中找 main.cpp 与 data/，复制所需文件回原位置。
恢复整理前的布局：先备份现有工作，再把 backups/before-organize-...zip 解压到独立目录查看，
不要直接覆盖现在继续写过的题目。ZIP 中保留了当时的全部根目录源码和 VS 配置。

## 8. 工具自检

```powershell
python -m unittest discover -s tools -p "test_*.py"
```

测试使用 build/ 下的隔离临时目录，验证归档/切题备份、拒绝覆盖、路径边界、
VS 编译列表、输出比较与子进程超时。不会对当前 main.cpp 执行 new/load。

