# cp-algorithm-vs

用于 ICPC 训练的 Visual Studio C++ 写题工作区。

直接用 Visual Studio 打开 **cp-algorithm.sln**，日常只编辑根目录的 **main.cpp**。
新题通过工具载入 templates/main.cpp，历史题解保存到 solutions/。

第一次使用：保存 VS 中尚未保存的编辑；若 VS 提示项目在外部被修改，选择重新加载。
如果目录视图没刷新，关闭并重新打开解决方案。平时在解决方案资源管理器中关闭“显示所有文件”，按已配置的分类浏览。

## 文件放哪里

| 路径 | 用途 |
| --- | --- |
| main.cpp | 当前题目，VS 唯一参与编译的 .cpp |
| solutions/平台/比赛/题号/ | 已保存题解、样例、题目链接和算法标签 |
| include/ | 公共头文件；原来的 pch.h、dbg.h 在这里 |
| templates/ | 新题模板；modules/ 保留原来的 C++ 模块实验 |
| examples/stress/ | 可以直接运行的“最大非空子段和”对拍示例 |
| data/input.txt、expected.txt | 当前题目的输入、期望输出 |
| tools/cp.py | 归档、切题、运行和对拍工具 |
| build/ | 所有编译输出、对拍反例、历史编译产物 |
| backups/ | 整理前的 ZIP；每次 new/load 前自动备份当前工作 |
| docs/ | 详细教程、整理记录与哈希清单 |
| .vs/ | VS 自动维护的隐藏缓存 |

按平台与题号保存实体文件，比如 solutions/codeforces/2100/A/main.cpp。
“二分、图论、DP”等算法标签写在该题 README.md 里，同一题无需为多个标签复制多份。

## 每天只需这几步

在 VS 的“视图 → 终端”打开 PowerShell，切到这个项目根目录。下列命令使用电脑上已有的 Python。

```powershell
# 1. 写完或临时离开当前题目，先在 VS 中 Ctrl+Shift+S 保存磁盘文件。
python tools/cp.py save codeforces/2100/A

# 2. 开始新题：先自动备份当前工作，再把 templates/main.cpp 复制到 main.cpp。
python tools/cp.py new

# 3. 回到旧题：先自动备份当前工作，再加载对应代码与样例。
python tools/cp.py load codeforces/2100/A
```

把示例中的平台、比赛号、题号换成你自己的。
save 遇到已有目录会报错，不会覆盖旧解；再次保存可用 A-v2 等新名称。
命令操作的是**已保存到磁盘**的文件；new/load 后让 VS 重新加载外部改动。
想改新题默认代码，只编辑 templates/main.cpp。

手工新增或移动归档、模板文件后，运行：

```powershell
python tools/cp.py sync
```

它会更新 VS 分类；归档、模板、对拍代码可以打开查看，但不参与主项目编译。

## 文件输入输出

把题目输入放进 data/input.txt；把正确答案放进 data/expected.txt。可以先用 VS 生成，再运行已有程序：

```powershell
# VS 中选择 Debug / x64，Ctrl+Shift+B 生成后执行：
python tools/cp.py run --exe build/bin/x64/Debug/cp-algorithm.exe

# 比较样例答案（默认忽略空白差异，字符大小写仍区分）：
python tools/cp.py run --exe build/bin/x64/Debug/cp-algorithm.exe --expected data/expected.txt

# 也可以让工具用 g++ 自动编译当前 main.cpp 并运行：
python tools/cp.py run
```

标准输出写到 data/output.txt，cerr / 标准错误写到 data/output.stderr.txt。
run 默认每次限时 2 秒，可加 --timeout 5。
未给 --expected 时只运行，不判断答案是否正确。
整理时保留的 main.cpp 中 solve() 还没有最终输出，因此它能编译不等于题目已做完。
初始 input.txt/expected.txt 留空，使用前请填入当前题目的有效数据。

如果想 **F5 断点调试时自动读文件**，使用模板中的 LOCAL_FILE 分支，具体设置见
[Visual Studio、文件 I/O 与对拍教程](docs/WORKFLOW.md)。

## 先跑一次完整对拍示例

```powershell
python tools/cp.py stress --solution examples/stress/solution.cpp --brute examples/stress/brute.cpp --gen examples/stress/gen.cpp --iterations 1000
```

工具用 g++ 分别编译三个程序，按种子造输入，把同一份输入交给正解与暴力，再比较输出。
不一致、超时或异常退出就停止，将反例、标准错误、种子、编译命令和当时的源码保存到
build/stress/failures/ 下；三个程序都要用标准输入输出，不要在代码里硬编码 freopen。

针对自己的题，把 brute.cpp 和 gen.cpp 放在 data/，实现对应逻辑后运行：

```powershell
python tools/cp.py stress --brute data/brute.cpp --gen data/gen.cpp --iterations 10000
```

这条命令默认把 main.cpp 当待测程序。示例里的暴力和生成器只适用于最大非空子段和，
要改成当前题目的输入格式、约束与暴力解法。详见[完整教程](docs/WORKFLOW.md)。

本机现有 VS 使用 MSVC v145；工具的自动编译使用 PATH 中的 g++，默认 C++23。
新模板至少需要 C++20，可加 --std c++20；工具也支持用 --std c++17 编译只使用 C++17 的其他源码。
--cxx 指定 GCC/Clang 路径，-I 添加第三方头文件目录。
新模板沿用 ll/vi/vii/vp 等别名、常量与多测开关，提供 all/rall/sz、rep/per、debug、debug_matrix 和 print。
debug 支持嵌套容器、pair/tuple、optional/variant、栈和队列；print(a) 向标准输出打印一行答案。具体示例见[模板使用说明](docs/WORKFLOW.md#9-模板宏与数据结构输出)。
部分旧题解使用 format/concepts/ranges，需要支持相应特性的编译器。

整理前的备份位置及文件映射见[整理记录](docs/REORGANIZATION.md)。


## GitHub 仓库与日常同步

每次完成一批题目或修改工具后，先在 VS 中保存文件，再运行：

```powershell
git status
git add .
git diff --cached --stat
git commit -m "Add solutions for today's practice"
git push
```

第一次在另一台电脑使用：

```powershell
git clone https://github.com/202030481266/cp-algorithm-vs.git
cd cp-algorithm-vs
```

然后用 Visual Studio 打开 cp-algorithm.sln。项目使用 v145 工具集，
本机原有的 AtCoder Library 绝对路径需按新电脑的实际位置调整。
工具的编译运行功能另需 Python 和 g++。

.gitignore 已排除 VS 缓存、个人配置、编译产物、Python 缓存、运行输出和本地备份。
源码、VS 共享项目配置、模板、样例输入与期望输出、工具和文档会纳入版本管理。
没有全局忽略 *.out，以便保存题目的样例答案。
backups/ 中的历史 ZIP 和切题备份只存在于本机，克隆仓库不会带上它们。
重要对拍反例请复制到对应题目目录再提交，因为 build/ 不上传。
