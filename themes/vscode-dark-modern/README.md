# VS Code Dark Modern · Visual Studio 配色复刻

按本机 VS Code 1.142.0 自带的 **Dark Modern** 主题源文件取色，并用 VS Code 实际截图逐像素校准，面向 Visual Studio 2022 / 2026 的 C++ 编辑器。目录中保留两个版本：参考 One Dark Pro、Gruvbox、Monokai 重新配色的鲜艳版，以及在本机显示器上与 VS Code 显示一致的 VX2762 校准版。

## 2026-10-23 鲜艳版重新配色

鲜艳版不再是“Dark Modern 提高彩度”，改为参考 One Dark Pro、Gruvbox、Monokai 重新设计的语法配色，见“鲜艳版”一节。

- 旧鲜艳版的变量、关键字、宏、枚举成员和类型都在蓝到青绿之间，提高彩度后整屏偏蓝；新配色把各类 token 分到不同色相。
- 变量和参数改为暖白色，运算符和标点为暖灰色，代码整体更安静，关键字、函数、类型更醒目。
- `cin >>`、`cout <<`、`vector` 的 `[]` 等重载运算符不再显示为函数色，与内置运算符同色。
- 背景、选区、行号、查找和引用高亮等界面颜色不变；校准版不受影响，重新生成的结果与之前完全相同。
- `generate.py` 改为按调色板生成：`--palette` 选择 `dark-modern` 或 `vivid`，原来的 `--vivid` 倍数改名为 `--chroma`。

已导入旧版的用户重新导入即可；使用 Editor Font Styles 扩展并指向鲜艳版文件时，重新生成后立即生效。

## 2026-10-16 精简

删除了 Dark Modern 原色版和它的预览图，只保留鲜艳版和 VX2762 校准版。不带参数运行 `python generate.py` 只会重新生成这两个文件。

## 2026-10-16 鲜艳版

新增 `VSCode-Dark-Modern-Vivid.vssettings`，不追求与 VS Code 一致，而是让语法颜色更鲜明。当时的做法是把 Dark Modern 语法颜色的彩度乘以 1.4，2026-10-23 已重新配色。

## 2026-10-16 截图校准

在同一台显示器上，用同一段代码的 VS Code 与 VS 截图逐个 token 比较像素颜色，做了以下调整：

- **重载运算符保持函数黄 `#DCDCAA`**。实测 VS Code 中 `cin >>`、`cout <<`、`std::string` 的 `+` 和迭代器的 `==` 都是函数黄。第一次对比时曾把细笔画上的淡黄误判为灰色并改成了灰色，现已改回。
- **彩色括号的第 2、3 层颜色对调**。VS 在嵌套第 2 层使用 `brace pair level three`、第 3 层使用 `brace pair level two`，所以按名称直接填色会得到 金 → 蓝 → 紫。现在两项颜色互换，显示顺序与 VS Code 一样是 金 → 紫 → 蓝。
- **新增显示器校准版** `VSCode-Dark-Modern-VX2762.vssettings`。VS Code 会按显示器的 ICC 配置文件转换颜色，VS 不会，所以同样的色值在 VS 中显得更艳。校准版预先做了同样的转换，见“显示器色彩管理”。
- **字体建议 Consolas 14，行距保持 1.0**。实测 VS Code 行高 24px；VS 中 Consolas 14 在行距 1.0 下为 23px，1.15 会增大到约 26px，反而更远。

已导入旧版的用户重新导入即可，类别同样只选“环境 → 字体和颜色”。

## 选择文件

| 使用情况 | 选择的文件 |
| --- | --- |
| 想要更鲜明、各类 token 区分更清楚的配色，不要求与 VS Code 一致 | **[VSCode-Dark-Modern-Vivid.vssettings](VSCode-Dark-Modern-Vivid.vssettings)** |
| 在 ViewSonic VX2762-2K 显示器上与 VS Code 显示一致 | [VSCode-Dark-Modern-VX2762.vssettings](VSCode-Dark-Modern-VX2762.vssettings) |

两个文件的项目完全相同，只是颜色值不同，只导入其中一个。鲜艳版、旧鲜艳版（Dark Modern 彩度 ×1.4）与 Dark Modern 原色的效果对比见 [vivid-compare.png](vivid-compare.png)；对比图是按 VS 着色规则绘制的示意图。

## 安装

安装了同仓库的 [Editor Font Styles](../editor-font-styles/README.md) 扩展时可以不导入：把仓库根目录 `.editor-font-styles.json` 中的 `colors` 改为本目录的文件，例如 `themes/vscode-dark-modern/VSCode-Dark-Modern-Vivid.vssettings`，保存后立即生效。下面是不使用扩展时的导入步骤。

导入向导的逐页说明与 [Visual Assist Dark 导入指南](../visual-assist-dark/INSTALL.md) 相同，只是文件换成本目录的文件。

1. 确认 Visual Studio 使用内置的 **深色 / Dark** 主题；已经是深色时跳过这一步。
2. 打开 **工具 → 导入和导出设置 → 导入选定的环境设置**。
3. 按需在向导中保存当前设置；备份是可选的。
4. 浏览并选择上表中对应的文件，例如：

   ```text
   D:\项目\dev\cpp\cp-algorithm\themes\vscode-dark-modern\VSCode-Dark-Modern-Vivid.vssettings
   ```

5. 在类别页勾选 **环境 → 字体和颜色**，点击 **完成**。本文件只包含这个类别。
6. **开启括号对着色**：**工具 → 选项 → 文本编辑器 → 常规 → 显示 → 启用大括号对着色**（英文界面为 Enable brace pair colorization，也可在选项中搜索“大括号对”）。VS Code 默认开启这个功能，VS 中需要手动开启；不开启时所有括号都是灰色。
7. 按下一节调整字体和行距。
8. 在 C++ 项目中打开 `semantic-gallery.cpp`，等待 IntelliSense 完成解析，对照下方“效果检查”。

这是通过 VS 内置导入向导应用的编辑器配色，不需要安装 VSIX 扩展。应用后，主题下拉框仍显示内置深色主题。文件不修改字体、字号、快捷键、窗口布局和项目配置。

本主题包含 `visual-assist-dark` 原生版的全部颜色项目，导入后会完整覆盖 VA 配色，无需先恢复默认。切回 VA 配色时重新导入它的文件即可；本主题独有的少数项目（光标、当前行号、查找和定义高亮、`[[nodiscard]]` 等属性、内联提示、`///` 文档注释等）会保留本主题的颜色。

## 字体与行距

本机 VS Code 使用 Consolas 18px，行高 24px，字宽 9.9px，没有连字。要让 VS 的字形和疏密与之一致：

1. **工具 → 选项 → 环境 → 字体和颜色**，“显示其设置”选 **文本编辑器**，字体选 **Consolas**，大小选 **14**。
2. **行距**保持默认的 **1.0**（可在选项中搜索“行距”确认）。

VS 按整像素排版：Consolas 14 磅实测字宽 10px、行高 23px，与 VS Code 的 9.9px、24px 最接近。13 磅偏小，15 磅偏大；行距 1.15 会把行高增大到约 26px。

Fira Code 会把 `>>`、`==`、`++` 等显示为连字，这是两边观感差异的主要来源之一。若想继续使用 Fira Code，颜色不受影响，只是字形与 VS Code 不同。

## 鲜艳版

鲜艳版是参考 One Dark Pro、Gruvbox、Monokai 重新设计的语法配色。背景、选区、行号、查找和引用高亮等界面颜色与 Dark Modern 相同，只有当前行号和非活动代码随普通文本改为暖白色。

- **各类 token 分到不同色相**。沿用 One Dark Pro 的分工：关键字紫、函数蓝、类型黄、字符串绿、数字橙；控制流和预处理指令用 Monokai 的粉红，宏和转义字符用青色，成员字段用浅鲑红。旧鲜艳版的变量、关键字、宏、枚举成员和类型都在蓝到青绿之间，这是它整体发蓝、不易区分的主要原因。
- **变量用暖白色**。变量和参数是代码里最多的 token，用 Gruvbox 式的暖白色而不是彩色，代码整体更安静，其他颜色才醒目。运算符和标点再暗一级。
- **重载运算符与内置运算符同色**。`cin >> n >> m` 中的 `>>`、`cnt[i]` 中的 `[]` 与 `+`、`=` 一样是暖灰色。VS 把 `;`、`,`、`.` 也归为运算符，所以运算符只能用不显眼的灰色。
- **注释用暖灰色**，与三个参考主题一致；Editor Font Styles 扩展默认还会把注释显示为斜体。
- **亮度和彩度控制在同一范围**。以 OKLCH 衡量，彩色的亮度在 0.70～0.82、彩度在 0.075～0.17，没有特别跳的颜色；所有颜色与背景的对比度都不低于 4.5:1。

| 元素 | 颜色 | OKLCH（L C h） | 参考 |
| --- | --- | --- | --- |
| 变量、参数、普通文本、标签 | `#D8D0BF` | 0.86 0.025 85 | Gruvbox 前景色 |
| 运算符、标点、重载运算符 | `#A29E96` | 0.70 0.012 80 | — |
| 关键字（`int`、`const`、`class`、`using`、`static_cast` 等） | `#B484E8` | 0.70 0.150 305 | One Dark Pro 紫 |
| 控制流、预处理指令、`new` / `delete` | `#F26A90` | 0.70 0.170 5 | Monokai 粉红 |
| 类型、命名空间、属性 | `#E6BE68` | 0.82 0.115 85 | One Dark Pro、Gruvbox 黄 |
| 函数 | `#61B2F4` | 0.74 0.125 245 | One Dark Pro 蓝 |
| 成员字段 | `#EAACA1` | 0.80 0.075 30 | One Dark Pro 的属性红，降低彩度 |
| 宏、转义字符 | `#56C4CA` | 0.76 0.100 200 | One Dark Pro 青 |
| 数字、枚举成员 | `#EE9B5F` | 0.76 0.125 55 | One Dark Pro 橙 |
| 字符串 | `#93CA73` | 0.78 0.130 135 | One Dark Pro 绿 |
| 注释 | `#8C8579` | 0.62 0.020 85 | Gruvbox 灰 |
| 非活动代码 | `#858077` | 普通文本按 55% 混合 | — |
| 彩色括号 1 / 2 / 3 层 | `#F2CE59` / `#E176C1` / `#6CCDEA` | 0.86 0.14 92 / 0.71 0.16 340 / 0.80 0.10 220 | VS Code 的金 → 紫 → 蓝，色相与关键字、函数错开 |

想整体更艳或更淡，用 `--chroma` 把语法颜色的彩度乘以一个倍数重新生成，例如 1.15 或 0.85：

```text
python generate.py --palette vivid --chroma 1.15 -o VSCode-Dark-Modern-Vivid.vssettings
```

要长期使用其他倍数，同时在 `generate.py` 的 `VARIANTS` 中给鲜艳版加上 `"chroma": 1.15`，否则不带参数重新生成时会恢复。单个颜色在 `generate.py` 的 `VIVID` 中修改。

本机显示器的色域比 sRGB 宽，VS 不做色彩管理，所以实际显示会比对比图更鲜明一些。

## 显示器色彩管理

VS Code（Chromium）会按显示器的 ICC 配置文件转换颜色，Visual Studio（WPF）直接输出原始色值。本机显示器配置文件 `VX2762-2K-HD.ICM` 的红色原色与 sRGB 不同，所以同一个色值在 VS 中会比 VS Code 更艳；例如类型色 `#4EC9B0` 在 VS Code 中实际输出为 `#66CAB0`。

`VSCode-Dark-Modern-VX2762.vssettings` 按 VS Code 的方式预先转换了全部颜色：用配置文件的原色转换色域，亮度曲线保持 sRGB。以 VS Code 截图实测，这一模型对 12 种取样颜色（变量、关键字、类型、字符串、函数、注释、紫色、数字、两种括号色和两种灰色）的误差都不超过 1。灰色在转换后不变，因此背景、行号等与 Dark Modern 原色相同。

校准版只适用于启用了该配置文件的显示器。换用其他显示器时，用那台显示器的配置文件重新生成（不加 `--palette` 时使用 Dark Modern 原色）：

```text
python generate.py --profile C:\Windows\System32\spool\drivers\color\<配置文件>.icm -o VSCode-Dark-Modern-<显示器>.vssettings
```

不带参数运行 `python generate.py` 会按脚本中 `VARIANTS` 记录的参数重新生成鲜艳版和校准版。显示器正在使用的配置文件可在 Windows 的“颜色管理”中查看。

## 复刻依据

从本机安装的软件只读提取：

- **VS Code 1.142.0**：`theme-defaults` 扩展中的 `dark_modern.json` → `dark_plus.json` → `dark_vs.json` 继承链，以及编辑器内置的默认 UI 颜色（选区、当前行、括号匹配、彩色括号、引用高亮等）。
- **C/C++ 扩展 ms-vscode.cpptools 1.34.4**：`semanticTokenScopes` 决定每类 C++ 语义 token 映射到哪个 TextMate scope，从而决定它在 Dark+ 中取哪种颜色。
- **Visual Studio Community 2026 18.10.3**：文件中的每个颜色项目名称都从 VS 程序集的格式定义和内置深色主题数据中读出核对，例如 `brace pair level one`、`Selected Line Number`、`CppAttributesSyntacticTokenFormat`、`keyword - control`。
- **2026-10-16 的 VS Code 与 VS 截图**：同一段代码逐 token 取像素颜色，用于确认重载运算符、括号层级和显示器色彩管理。

### 语法颜色

下表为 Dark Modern 原色，是校准版的基础，校准版的实际值由 `generate.py` 按显示器配置文件转换。鲜艳版使用另一套语法颜色，见“鲜艳版”一节；颜色项目的对应关系相同，只是成员字段、宏、重载运算符在鲜艳版中各有单独的颜色。

| 元素 | RGB | VS Code 来源 | VS 颜色项目 |
| --- | --- | --- | --- |
| 编辑器背景 | `#1F1F1F` | `editor.background` | Plain Text 背景 |
| 普通文本、标点 | `#CCCCCC` | `editor.foreground` | Plain Text、punctuation |
| 运算符 | `#D4D4D4` | `keyword.operator` | Operator |
| 关键字 | `#569CD6` | `keyword`、`storage.type`、`storage.modifier` | Keyword |
| 控制流关键字 | `#C586C0` | `keyword.control` | C++ 控制关键字、keyword - control |
| 预处理指令 | `#C586C0` | `keyword.control.directive` | Preprocessor Keyword |
| `new` / `delete` | `#C586C0` | 语义 `newOperator` | CppNewDelete |
| 类型、类模板 | `#4EC9B0` | `entity.name.type` | CppType、CppRefType、CppValueType、CppClassTemplate、CppGenericType |
| 命名空间 | `#4EC9B0` | `entity.name.namespace` | CppNamespace |
| 属性 `[[nodiscard]]` | `#4EC9B0` | `entity.other.attribute` | CppAttributes |
| 函数 | `#DCDCAA` | `entity.name.function` | CppFunction、CppMemberFunction、CppStaticMemberFunction、CppFunctionTemplate |
| 重载运算符 | `#DCDCAA` | 语义 `operatorOverload` → `entity.name.function.operator`，截图实测 | CppOperator、CppMemberOperator |
| 变量、参数、成员字段 | `#9CDCFE` | `variable`、`variable.parameter`、`variable.other.property` | CppLocalVariable、CppGlobalVariable、CppParameter、CppMemberField 等；Identifier |
| 枚举成员 | `#4FC1FF` | `variable.other.enummember` | CppEnum |
| 宏 | `#569CD6` | `entity.name.function.preprocessor` | CppMacro |
| 注释 | `#6A9955` | `comment` | Comment |
| 字符串 | `#CE9178` | `string` | String、字符串定界符 |
| 转义字符 | `#D7BA7D` | `constant.character.escape` | C++ 字符串转义字符 |
| 数字 | `#B5CEA8` | `constant.numeric`、语义 `numberLiteral` | Number、CppUDLNumber |
| 标签 | `#C8C8C8` | `entity.name.label` | CppLabel |

所有项目均为常规字重，与 Dark Modern 一致。

### 编辑器界面

| 元素 | RGB | VS Code 来源 | VS 颜色项目 |
| --- | --- | --- | --- |
| 行号 | `#6E7681` | `editorLineNumber.foreground` | Line Number |
| 当前行号 | `#CCCCCC` | `editorLineNumber.activeForeground` | Selected Line Number |
| 选区 | `#264F78` | `editor.selectionBackground` | Selected Text |
| 非活动选区 | `#3A3D41` | `editor.inactiveSelectionBackground` | Inactive Selected Text |
| 当前行 | 边框 `#282828`，无填充 | `editor.lineHighlightBorder` | CurrentLineActiveFormat |
| 光标 | `#AEAFAD` | `editorCursor.foreground` | Caret (Primary) |
| 彩色括号 1 / 2 / 3 层 | `#FFD700` / `#DA70D6` / `#179FFF` | `editorBracketHighlight.foreground1`–`3` | brace pair level one / **three** / **two**（界面显示为“大括号匹配 - 级别 1/3/2”；VS 第 2 层使用 level three） |
| 不匹配的括号 | `#D21515` | `editorBracketHighlight.unexpectedBracket.foreground` | mismatched brace |
| 匹配括号 | 边框 `#888888`，填充 `#1C261C` | `editorBracketMatch.border` / `.background` | Brace Matching (Rectangle) |
| 引用高亮 | `#474747` | `editor.wordHighlightBackground` | HighlightedReference、HighlightedDefinition、CppHighlightedDefinition |
| 写入引用高亮 | `#093D5B` | `editor.wordHighlightStrongBackground` | HighlightedWrittenReference |
| 其他查找结果 | `#633315` | `editor.findMatchHighlightBackground` | FindHighlight |
| 可见空白 | `#3F3F3E` | `editorWhitespace.foreground` | Visible Whitespace |
| 非活动代码 | `#7E7E7E` | `C_Cpp.inactiveRegionOpacity` 0.55 | Excluded Code |
| 内联提示 | 前景 `#969696`，背景 `#262626` | `editorInlayHint.*` | CppInlineHintsFormat |

VS 的颜色没有透明度。VS Code 中带透明度的颜色按 `#1F1F1F` 背景预先混合。例如引用高亮 `#575757B8` 混合后为 `#474747`，括号匹配填充 `#0064001A` 为 `#1C261C`；VS Code 截图中引用高亮实测为 `#484848`。

括号匹配、当前行和引用高亮属于标记类项目：VS 用前景色画边框，用背景色填充。因此当前行只设边框，引用高亮的边框与填充同色，与 VS Code 的无边框效果一致。

RGB 转换为 `.vssettings` 所需的 `0x00BBGGRR`；例如函数黄 `#DCDCAA` 导出为 `0x00AADCDC`。

## 与 VS Code 的已知差异

以下针对校准版；鲜艳版本来就不追求与 VS Code 一致。

- **`using` 和 `operator` 关键字**：Dark+ 把它们着为紫色 `#C586C0`。VS 的 C++ 关键字只分“普通关键字”和固定的控制关键字两类，这两个词在 VS 中显示为蓝色。
- **`vector`、`string` 的 `[]`**：VS Code 的彩色括号会覆盖语义颜色，所以 `a[i]` 的方括号按层级显示金 / 紫 / 蓝。VS 把它们识别为成员运算符，且该颜色优先于括号对着色，所以显示为重载运算符的函数黄。重载运算符只能共用一个颜色，为了让 `cin >>`、`cout <<` 与 VS Code 一致，这里保留黄色。
- **标点**：`;`、`,`、`.` 和模板的 `<>` 在 VS Code 中是普通文本色 `#CCCCCC`，VS 把它们归为 Operator，显示为 `#D4D4D4`，相差很小。
- **行高**：VS Code 为 24px，VS 中 Consolas 14 为 23px，见“字体与行距”。
- **IntelliSense 完成前**：VS Code 的语法高亮会先把函数调用着为黄色。VS 在语义结果到达前统一使用 Identifier，本主题将其设为变量浅蓝；解析完成后与 VS Code 一致。
- **当前查找结果**：VS Code 用 `#9E6A03` 单独标出当前匹配；VS 把当前匹配作为选区显示，使用选区颜色。其余匹配的颜色一致。
- **缩进参考线**：VS Code 为 1px 的 `#404040`，VS 为 2px 的 `#303030`，整体亮度相近，保持 VS 默认。选中文本的同名高亮和滚动条概览在 `.vssettings` 中没有对应项目。
- **IDE 外壳**：侧边栏 `#181818`、标题栏、标签页等属于整体主题，`.vssettings` 无法设置；本主题只覆盖编辑器。

非活动代码：VS Code 以 55% 不透明度保留原有语法颜色。VS 的 C/C++ 选项 **对非活动代码块使用不透明度百分比** 开启、**非活动代码不透明度百分比** 设为 55 时效果相同；若关闭了透明度方式，VS 改用 Excluded Code 颜色，本主题已设为普通文本色的 55% 混合值。

## 效果检查

`semantic-gallery.cpp` 是独立的 C++20 示例，不参与当前工作区的默认构建。可直接在 VS 打开查看。编译运行的预期输出为：

```text
acc.total() = 8000
alpha: 251, instances: 1
```

校准版优先检查以下细节：

- `Accumulator`、`Point`、`Level`、`modern`、`std` 呈青绿色。
- `add`、`total`、`instances`、`operator_color_demo` 呈淡黄色。
- 在 `operator_color_demo()` 中，`>>`、`[]`、`Point` 的 `+` 和流的 `<<` 为淡黄色；`1 << a` 的移位和 `|` 为浅灰色。
- `total_`、`value`、`cells` 等变量呈浅蓝色；`Level::high` 等枚举成员为更亮的蓝色。
- `#include`、`if`、`for`、`switch`、`return`、`new`、`delete` 呈紫色；`int`、`const`、`class`、`static_cast` 呈蓝色。
- `DM_TRACE` 呈蓝色；`'\t'`、`'\n'` 中的转义部分为黄褐色。
- `#if 0` 区块半透明；开启括号对着色后，嵌套括号按金 → 紫 → 蓝循环。

鲜艳版：

- `Accumulator`、`Point`、`Level`、`modern`、`std` 和 `[[nodiscard]]` 呈黄色；`add`、`total`、`instances`、`operator_color_demo` 呈蓝色。
- `total_`、`count_`、`created_` 和 `Point` 的 `x`、`y` 等成员字段呈浅鲑红色；`value`、`cells`、`a` 等局部变量和参数为暖白色；`Level::high` 等枚举成员和数字为橙色。
- `#include`、`if`、`for`、`switch`、`return`、`new`、`delete` 呈粉红色；`int`、`const`、`class`、`static_cast` 呈紫色。
- `DM_TRACE` 和 `'\t'`、`'\n'` 中的转义部分呈青色。
- 在 `operator_color_demo()` 中，`>>`、`[]`、`Point` 的 `+`、流的 `<<` 与 `1 << a` 的移位、`|` 一样为暖灰色。
- 开启括号对着色后，嵌套括号按金 → 品红 → 天蓝循环。

如果变量和函数仍然全是同一种颜色，检查文件是否属于已加载的 C++ 项目，以及 C/C++ 的 IntelliSense 和语义着色是否启用。

如果切换主题后颜色被覆盖，选择内置深色主题，再重新导入本配色。恢复时使用向导保存的设置；“字体和颜色 → 使用默认值”也可重置颜色，但会一起重置该类别的其他自定义项。

## 保真范围与验证

两个主题文件各包含 97 个项目，由 `generate.py` 按上述来源颜色生成；鲜艳版相对 Dark Modern 原色改动了 76 个项目，包括语法颜色、彩色括号，以及跟随普通文本的当前行号和非活动代码。已验证 XML 结构、无重复项目和 RGB/COLORREF 转换，且不设置字体或字号。

所有项目名称均已对照 VS 2026 程序集中的格式定义和内置深色主题数据核对。其中 `CurrentLineInactiveFormat`（编辑器失去焦点时的当前行）在 VS 2026 中已没有对应项目，导入时会被忽略，不影响其他颜色。

2026-10-16 第二次对比（同一段代码、当时导入的原色版、Consolas 14）：变量、函数、类型、关键字、宏、字符串、转义字符、数字和内置运算符在 VS 中都是主题原值，与 VS Code 只差显示器色彩管理那一层；不一致的只有重载运算符和第 2、3 层括号，均已修正。修正后的校准版和鲜艳版随后都已在 VS 中导入使用。

示例已用 g++ 15.2（`-std=c++20 -Wall -Wextra -Wpedantic`）编译，无警告，运行输出与上面一致。

参考：[Visual Studio 字体与颜色设置](https://learn.microsoft.com/en-us/visualstudio/ide/how-to-change-fonts-and-colors-in-visual-studio)、[VS Code 主题颜色参考](https://code.visualstudio.com/api/references/theme-color)。
