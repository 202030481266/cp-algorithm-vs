# 整理记录

日期：2026-09-22。

以下记录首次整理时的文件位置与校验结果；历史路径不代表当前仍存在。后续清理见文末。

## 已保留的原稿

原始根目录的 15 个文件已压缩备份到：

backups/before-organize-20260922-140907.zip

[reorganization.json](reorganization.json) 记录每个原文件的名字、新位置、原始大小、
最后修改时间和 SHA-256。ZIP 中保存原文件字节，包括原来的文件名与编码。
该 ZIP 不含可重建的 .vs 和编译输出缓存。

| 原位置 | 现位置 | 处理 |
| --- | --- | --- |
| main.cpp | main.cpp | 原稿保持不变 |
| ; | archive/recovered/semicolon.cpp | C++ 草稿，改可辨认文件名，内容不变 |
| ' | archive/recovered/apostrophe.cpp | C++ 草稿，改可辨认文件名，内容不变 |
| ] | archive/recovered/right_bracket.cpp | C++ 草稿，改可辨认文件名，内容不变 |
| o.cpp | solutions/unclassified/o.cpp | 题号不明，待你补充，内容不变 |
| dbg.h | include/dbg.h | 内容不变 |
| pch.h | include/pch.h | 原 GBK 中文注释转为 UTF-8，并显式补齐常用标准头文件 |
| pch.cpp | archive/legacy/pch.cpp | 原稿保留，当前不编译 |
| ops.cppm | templates/modules/ops.cppm | 原稿保留，当前不编译 |
| random_utils.cppm | templates/modules/random_utils.cppm | 原稿保留，当前不编译 |
| sparse_table.cppm | templates/modules/sparse_table.cppm | 原稿保留，当前不编译 |
| x64/ | build/legacy/x64/ | 移动旧输出，不删除 |
| cp-algorithm/ | build/legacy/cp-algorithm/ | 原目录里只有旧 x64 中间产物，整体移动 |
| .vs/ | .vs/ | VS 正在运行，保留缓存 |

无法从文件名确定恢复草稿的题号、正确性或完成状态，所以没有把它们标记成已通过题解。
异常文件里确实有源码，但无法仅凭内容判断它们当初为什么被保存成符号名。

## 项目设置变化

- 保留现有解决方案入口、项目标识、MSVC v145 与原 AtCoder Library 路径。
- 四种配置统一采用 C++23；原 Win32 配置为 C++20，原 x64 已是 C++23。
- 当前只编译 main.cpp；旧模块和 pch.cpp 作为历史资料保留。
- 输出归入 build/bin/，中间产物归入 build/obj/，调试工作目录固定为项目根目录。
- 增加 UTF-8 编译选项和本地 LOCAL 宏；没有默认启用文件 I/O 的 LOCAL_FILE 宏。
- VS 的分类与真实目录对应，归档/示例文件使用 None 项展示，避免重复 main。
- 新增模板、样例数据位置、归档与对拍工具、中文教程和忽略规则。

题目算法没有修改。能生成可执行文件不代表原题解正确或完整。


## 验证结果

- 整理前 ZIP 内的 15 个原始文件逐一通过 SHA-256 校验。
- 未编辑的 11 个文件（包含 main.cpp、恢复草稿、旧模块及移动后的 dbg.h）与原始哈希一致。
- 原解决方案 Debug/Release × x64/x86 四种配置均生成成功。
- 当前 main.cpp 分别通过 MSVC 与 GCC 编译、文件输入运行；仅验证工具链，不判断原题算法正确性。
- 最大非空子段和示例通过种子 1 至 1000 的 1000 轮对拍。
- 10 项自动化测试通过，包含真实错误答案与超时检测、反例保留、归档/切题恢复、拒绝覆盖、路径边界、VS 编译列表和输出比较。
- 示例样例的文件输出与期望值 6 比较一致。
- GCC 在此中文项目路径下使用相对输出路径，避开本机 MinGW 链接器处理中文绝对输出路径的问题。

## 用户确认后的清理

用户确认 archive/ 内的文件均已无用，已删除：

- archive/legacy/pch.cpp
- archive/recovered/apostrophe.cpp
- archive/recovered/right_bracket.cpp
- archive/recovered/semicolon.cpp

同时移除空的 archive/ 目录，并刷新 VS 文件列表和筛选器。
首次整理的哈希清单仍作为历史记录保留；原始版本可从首次 Git 提交或本地整理前 ZIP 查阅。
