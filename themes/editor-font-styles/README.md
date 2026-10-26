# Editor Font Styles · Visual Studio 字形扩展

这个 Visual Studio 2022 / 2026 编辑器扩展（VSIX）让配色和字形都跟着仓库走：

- 直接按 `.vssettings` 主题文件给编辑器着色，不需要导入；仓库根目录的配置已指定 [VS Code Dark Modern 鲜艳版](../vscode-dark-modern/README.md)，主题文件修改后立即生效；
- 按配置设置代码编辑器的字体和字号，不需要在“字体和颜色”中手动选择；
- 给任意“字体和颜色”项目设置斜体、倾斜、粗体、单独的字体、字号、下划线、删除线和不透明度，这些是 `.vssettings` 做不到的；
- 在 C/C++ 中按 Visual Assist 的方式，把标准库符号显示为斜体（System symbols in italics），把当前文件中声明的符号显示为粗体（Local symbols in bold）；
- 把注释中的 TODO、FIXME 等标记显示为橙色粗体；
- 在 C/C++ 中把光标旁的一对括号显示为青色粗体并加下划线，光标在括号内侧也有效（VS 自带的括号高亮只画一个方框，而且光标必须在括号外侧）。

不需要购买或安装 Visual Assist。

![Visual Assist Dark 主题加默认设置](preview.png)

截图来自 VS 2026 实验实例：Visual Assist Dark 原生版配色加默认字形设置，打开 [style-gallery.cpp](style-gallery.cpp)。

## 默认效果

| 元素 | 效果 | 对应的 VA 选项 |
| --- | --- | --- |
| 注释（所有语言，含文档注释） | 斜体 | — |
| C/C++ 标准库符号：`std`、`vector`、`cin`、`sort`、`push_back`、`size_t`、`INT_MAX` 等 | 斜体，颜色不变 | System symbols in italics |
| C/C++ 当前文件中声明的符号：类型、函数、变量、参数、成员、宏、枚举值、别名等 | 粗体，颜色不变 | Local symbols in bold |
| 注释中的 `TODO`、`FIXME`、`HACK`、`NOTE`、`XXX`、`BUG` | 橙色 `#FF8C00` 粗体 | — |
| C/C++ 光标旁的一对括号 `()`、`[]`、`{}` | 青色 `#40E0D0` 粗体加下划线 | — |

原来使用的 VA 设置中这两个选项都是开启的（注册表值 `LocalSymbolsInBold`、`StableSymbolsInItalics`），所以默认开启；不需要时可在配置中关闭。

## 安装

1. 关闭 Visual Studio。
2. 双击本目录的 `EditorFontStyles.vsix`，在 VSIX Installer 中勾选要安装的 Visual Studio，点击“安装”。
3. 打开本仓库中的文件，即使用 Visual Assist Dark 配色和默认字形；不需要再导入 `.vssettings`。
4. 在 **视图 → 输出** 的“显示输出来源”中选择 **Editor Font Styles**，可以看到使用的配置、配色文件和配置错误。

卸载：**扩展 → 管理扩展 → 已安装 → Editor Font Styles → 卸载**。卸载后编辑器恢复为“字体和颜色”中保存的颜色。

如果仍启用了 Visual Assist 的增强着色，在 VA 中关闭 Local symbols in Bold 和 System symbols in Italics，或在配置中关闭本扩展的 `systemSymbols`、`localSymbols`，只保留一边。

## 配色

仓库根目录的 [.editor-font-styles.json](../../.editor-font-styles.json) 中指定配色文件：

```jsonc
"colors": "themes/vscode-dark-modern/VSCode-Dark-Modern-Vivid.vssettings"
```

扩展读取这个文件中“文本编辑器”类别的全部项目（前景色、背景色、粗体），直接应用到编辑器，效果与导入该文件相同：

- 不需要导入，也不会改动 VS 自己保存的“字体和颜色”。对话框中显示的仍是 VS 保存的颜色，与编辑器中看到的可能不同；
- 主题文件中出现的项目以主题文件为准，在对话框中修改这些项目不会生效；
- 修改或重新生成主题文件（例如运行 `generate.py`）后立即生效；删除 `colors` 后恢复为 VS 保存的颜色；
- 换主题只需改路径，例如 `themes/vscode-dark-modern/VSCode-Dark-Modern-Vivid.vssettings`。也可以写成列表，后面的文件覆盖前面文件中的同名项目，用于在现成主题上做个人调整；
- 只使用“文本编辑器”类别，`.vssettings` 中的字体名称和字号不使用（字体用下一节的 `font` 设置），IDE 外观（深色 / 浅色主题）不受影响；
- 当前 VS 中不存在的项目会在输出窗口列出，例如 VS 2026 已删除的 `CurrentLineInactiveFormat`，以及未安装 VA 时的 `VA Class` 等项目；
- 编辑器底色写在 `TextView Background` 中，`Plain Text` 的背景保持“自动”（`0x02000000`）。给 `Plain Text` 设背景色时，VS 会把它画在每段文字下面并盖住选区，选中时只有行尾显示选区颜色。仓库中的主题文件都已这样设置，测试也会检查。

用 Go To Definition 等方式打开仓库以外、没有配置文件的文件（如标准库头文件）时，继续沿用仓库的配色和字形，不会切换回默认值。

## 字体

从 1.2.0 起，`font` 设置代码编辑器的字体和字号，作用与“字体和颜色”中“文本编辑器”的字体相同：

```jsonc
"font": { "family": "Maple Mono NF CN", "size": 14 }
```

- `size` 的单位与“字体和颜色”中的“大小”相同（磅）；只写其中一项时，另一项沿用“字体和颜色”中的设置；
- 与 `colors` 一样只在内存中生效，不改动 VS 保存的字体，对话框中显示的仍是原来的字体；删除 `font` 或写 `null` 后恢复；
- `styles` 中的 `fontScale` 和斜体项的字号按这里的字号计算；
- 字体未安装时在输出窗口提示，继续使用“字体和颜色”中的字体。

用“导入和导出设置”导入只含颜色的 `.vssettings`（例如本仓库的主题文件）时，VS 2026 会把文本编辑器的字体重置为默认值，需要重新选择；VS 的设置日志 `%LOCALAPPDATA%\Microsoft\VisualStudio\SettingsLogs` 中记为 `profile normal import`。用 `colors` 和 `font` 换配色和字体不需要导入，也就没有这个问题；即使导入过，编辑器中仍显示 `font` 指定的字体。

## 匹配括号

从 1.3.0 起，在 C/C++ 中，光标紧挨着一个括号时，这个括号和与它配对的括号都显示为青色 `#40E0D0` 粗体加下划线：

- `()`、`[]`、`{}` 参与配对，`<>` 不参与。光标在括号左边或右边都可以；同时挨着两个括号时（例如 `)|(`），先看左边的闭括号和右边的开括号，与 VS 自己的高亮一致；
- 注释、字符串、字符常量和 `#if 0` 中的括号不参与配对。预处理指令中的括号只在同一条指令内配对，所以 `#define BEGIN {` 不会打乱代码中的大括号；
- 没有闭合的括号不影响外层：正在输入的 `if (a[i` 在一个块中时，块的 `{` 仍与它的 `}` 配对；
- VS 自带的括号高亮（“字体和颜色”中的 Brace Matching (Rectangle) 方框）照常显示，但只在光标位于开括号左边或闭括号右边时出现；本扩展的样式叠加在方框上；
- 超过 2 MB 的文件不处理。

修改效果：

- 颜色：在 `colors` 指定的主题文件或“字体和颜色”中设置 `EditorFontStyles.MatchedBrace` 的前景色；设置背景色后括号还会带底色，例如 `<Item Name="EditorFontStyles.MatchedBrace" Foreground="0x00D0E040" Background="0x005A5A1F" BoldFont="Yes" />`（`.vssettings` 中颜色按 `0x00BBGGRR` 书写）；
- 字形：在 `styles` 中修改，例如去掉下划线：`"EditorFontStyles.MatchedBrace": { "underline": false }`；
- 关闭：`"matchedBraces": { "enabled": false }`。

## 自行构建

```text
python build.py           # 还原 NuGet 包、构建 Release、运行测试，生成 EditorFontStyles.vsix
python build.py --exp     # 另外部署到实验实例，用 devenv /rootSuffix Exp 试用
```

需要 Visual Studio 2022 或 2026 自带的 MSBuild。扩展 SDK 和 .NET Framework 引用程序集都从 NuGet 还原，不需要安装“Visual Studio 扩展开发”工作负载；首次构建需要联网。

修改代码后，把 `source.extension.vsixmanifest` 和 `Properties/AssemblyInfo.cs` 中的版本号加一再构建，VSIX Installer 才会按升级安装。

## 配置

配置文件名为 `.editor-font-styles.json`，按以下顺序查找，使用找到的第一个：

1. 文档所在目录及其各级上级目录。放在仓库根目录时，仓库中的所有文件都使用它，并随仓库同步到其他电脑；
2. 用户目录 `%USERPROFILE%\.editor-font-styles.json`；
3. 都没有时使用内置默认值 [resources/default-config.json](resources/default-config.json)。

配置文件只需写要改的部分。`styles` 中与默认值同名的项目按属性合并，写 `null` 删除该项目；其他部分按键合并，列表整体替换。保存后立即生效，不需要重启；新建配置文件后切换一次编辑器标签页即可生效。

“字体和颜色”由所有编辑器共用，所以同一时间只有一份配置的 `colors`、`font` 和 `styles` 生效：以最近获得焦点、且找到了配置文件的文档为准。没有配置文件的文档不会替换已生效的仓库配置。

```jsonc
{
  "$schema": "./themes/editor-font-styles/editor-font-styles.schema.json",
  "styles": {
    "*comment*": { "fontFamily": "JetBrains Mono" },               // 注释换字体，保留默认的斜体
    "CppControlKeywordSyntacticTokenFormat": { "italic": true },   // if、for、return 等斜体
    "Keyword": { "italic": true },                                 // 其他关键字斜体
    "EditorFontStyles.CommentTag": null                            // 删除默认的标记加粗
  },
  "systemSymbols": { "extraNames": [ "gp_hash_table", "tree_order_statistics_node_update" ] },
  "localSymbols": { "enabled": false }
}
```

`$schema` 指向本目录的 [editor-font-styles.schema.json](editor-font-styles.schema.json)（相对于配置文件），在 VS 中编辑时有补全和检查；上例是放在仓库根目录时的写法。

### styles

键是“字体和颜色”的项目名称，与 `.vssettings` 中的 `Name` 相同，例如 `Comment`、`Keyword`、`CppFunctionSemanticTokenFormat`、`CppParameterSemanticTokenFormat`、`VA Class`；也可以写分类名称，例如 `cppFunction`。不区分大小写，可用 `*` 和 `?` 通配，后写的项目覆盖先写的。各项目名称见 [Visual Assist Dark](../visual-assist-dark/README.md) 和 [VS Code Dark Modern](../vscode-dark-modern/README.md) 的对照表；设置 `"listClassifications": true` 后，输出窗口会按优先级列出全部项目。

| 属性 | 取值 | 说明 |
| --- | --- | --- |
| `italic` | `true` / `false` | 斜体，使用字体的斜体字形（自动避开 TTC 字体的显示错误，见下文） |
| `oblique` | `true` / `false` | 倾斜：把常规字形倾斜 12° 显示，不使用斜体字形；同时设置时优先于 `italic` |
| `bold` | `true` / `false` | 粗体，优先于“字体和颜色”中的粗体复选框 |
| `fontFamily` | 字体名称 | 只改这一项的字体，例如 `"Cascadia Code"`；未安装时忽略，并在输出窗口提示 |
| `fontScale` | 0.25～4 | 相对编辑器字号的倍数 |
| `underline` | `true` / `false` | 下划线 |
| `strikethrough` | `true` / `false` | 删除线 |
| `opacity` | 0～1 | 文字不透明度 |

`styles` 只改字形，不改颜色；颜色来自 `colors` 指定的主题文件，未指定时来自“字体和颜色”。

扩展新增的四个项目在“字体和颜色”中显示为 **Editor Font Styles: System Symbol / Local Symbol / Comment Tag / Matched Brace**，名称分别为 `EditorFontStyles.SystemSymbol`、`EditorFontStyles.LocalSymbol`、`EditorFontStyles.CommentTag`、`EditorFontStyles.MatchedBrace`。前两项默认没有颜色，保留符号本来的颜色；也可以在对话框或 `.vssettings` 中给它们设置颜色。扩展会让这四项保持最高优先级，否则 C++ 形参等分类自带的“非粗体”会盖过它们的粗体。

### 其他设置

| 设置 | 默认值 | 说明 |
| --- | --- | --- |
| `colors` | `[]` | 用于着色的 `.vssettings` 文件，一个路径或路径列表，相对路径以配置文件所在目录为起点 |
| `font.family` | `null` | 代码编辑器的字体，见“字体”一节；`null` 使用“字体和颜色”中的字体 |
| `font.size` | `null` | 代码编辑器的字号（磅，6～72）；`null` 使用“字体和颜色”中的字号 |
| `systemSymbols.enabled` | `true` | 标准库符号使用 System Symbol 样式 |
| `systemSymbols.extraNames` | `[]` | 另外视为系统符号的名称，例如 GCC 的 `__gnu_pbds` 容器名 |
| `systemSymbols.excludeNames` | `[]` | 不视为系统符号的名称 |
| `localSymbols.enabled` | `true` | 当前文件中声明的符号使用 Local Symbol 样式 |
| `commentTags.enabled` | `true` | 注释中的标记使用 Comment Tag 样式 |
| `commentTags.words` | `TODO` 等 6 个 | 区分大小写，按整词匹配 |
| `matchedBraces.enabled` | `true` | 光标旁的一对括号使用 Matched Brace 样式，见“匹配括号”一节 |
| `includeDirectories` | `[]` | 查找 `#include "..."` 的额外目录，相对路径以配置文件所在目录为起点 |
| `listClassifications` | `false` | 在输出窗口按优先级列出全部分类 |

## 判断规则

符号样式按名称判断，与 VA 官方说明的粗体规则相同：同名的符号得到相同的样式。

1. 被系统名称限定的名称是系统符号，例如 `std::max`、`ios::sync_with_stdio`、`vector<int>::iterator`、`chrono::steady_clock::now`；
2. 否则，在当前文件中任意位置声明过的名称是本文件符号，包括参数、局部变量、成员、模板参数、结构化绑定、宏名和宏参数；
3. 否则，在当前文件用 `#include "..."` 包含的项目头文件中声明过的名称两种样式都不加；
4. 否则，出现在系统符号表中的名称是系统符号。

项目头文件按编译器查找带引号头文件的顺序解析：先找当前文件所在目录，再找最近的 `.vcxproj` 中的 `AdditionalIncludeDirectories` 和 `IncludePath`，最后找 `includeDirectories`。对本仓库来说，`cp-stl` 中的 `add`、`size`、`query` 等名称因此不会被当作标准库符号。

系统符号表 [resources/system-symbols.txt](resources/system-symbols.txt) 由 `generate_system_symbols.py` 生成：从运行脚本的电脑上最新 MSVC 工具集的每个标准头文件出发，沿 `#include` 读取 MSVC 和 UCRT 头文件，收集其中不以下划线开头的标识符和宏名。编译器内部函数头文件（`*intrin*.h` 等）和单字母名称不收录。当前表由 MSVC 14.51 和 UCRT 10.0.26100 生成，包含 4185 个名称。更新 MSVC 后可重新生成：

```text
python generate_system_symbols.py
python build.py
```

注释、字符串、`#include` 和 `#pragma` 行、`[[...]]` 属性，以及 `#if 0` 中的代码都不会加样式。超过 2 MB 的文件不分析。

### 与 VA 的差异

- **同名符号**：文件中声明了 `count` 后，除了 `std::count`，其他所有 `count` 都加粗。成员调用同理：只要文件里有成员叫 `size`，所有 `.size()` 都加粗。
- **系统符号范围**：只认 MSVC 标准库和 UCRT 中的名称。Windows SDK、第三方库和 GCC 独有扩展不算系统符号，需要时写进 `extraNames`。
- **宏展开**：看不到宏展开后才出现的声明。例如 `#define DECLARE(x) int x` 之后，`DECLARE(foo)` 里的 `foo` 不算本文件声明；宏名和宏参数本身算。
- **分析时机**：本扩展不依赖 IntelliSense，打开文件立即生效，也不受 IntelliSense 解析失败影响。

### 字体注意事项

- 给一项换成行高不同的字体后，含这一项的行会变高。可以选行高接近的字体，或用 `fontScale` 调整。
- 斜体的效果取决于字体。Consolas、JetBrains Mono 等有真正的斜体字形；没有斜体字形的字体（如 Fira Code）会被机械倾斜。只安装了常规字重文件时也是这样，例如只装了 `Sarasa-Regular.ttc`；更纱黑体要安装包含全部字重的 `Sarasa-SuperTTC`。
- 不喜欢某款字体的斜体字形，或斜体显示错误时，改用 `oblique`。

### 斜体显示成别的字母（已自动处理）

使用“等距更纱黑体 SC”（Sarasa Mono SC）这类 TTC 字体时，VS 编辑器会把紧跟在常规字重文字后面的斜体画错，例如 `using namespace std;` 显示成 `using namespace OPB;`、`return vector` 显示成 `return PCARKN`；Visual Assist 的斜体也有同样的问题。复制出来的文字仍是正确的，只是显示错了。

原因是 WPF 的排版：它会把字号相同、字体“相同”的相邻文字合在一起排字形，而 TTC 文件里的常规体和斜体被当作同一个字体，于是斜体那一段用了常规体的字形编号，画的时候却用斜体字形。用 fontTools 核对：常规体中 `s`、`t`、`d` 的字形编号 3504、3505、3491，在斜体中正好是 `O`、`P`、`B`。在 VS 2026 中验证的触发条件是：斜体前面紧挨着常规字重的文字（关键字、普通标识符、字符串、注释），中间只有空格或注释；前面是粗体、数字、`=`、`(` 时不出错。

从 1.1.0 起，扩展把每个斜体项的字号放大 0.01%（16 号字只差约 0.002 像素，看不出来，也不影响列对齐），WPF 就不会再把它和前面的文字合在一起，斜体仍然使用字体自带的斜体字形。不需要任何配置；`fontScale` 与斜体同时设置时同样生效。

另外提供 `"oblique": true`：不使用斜体字形，把常规字形倾斜 12° 显示，适合喜欢倾斜效果或字体没有斜体字形的情况。

### 100% 缩放时笔画有锯齿

字号较大时，Consolas、Hack 等字体在 100% 缩放下，斜线和圆弧边缘出现台阶状锯齿，一放大或缩小反而正常。这与本扩展无关，是 VS 的“文本格式设置方法”默认为“自动”造成的：除 Cascadia 系列字体外，100% 缩放时使用 WPF 的 Display 模式，字形对齐到整像素，ClearType 只在水平方向平滑；缩放后改用 Ideal 模式。

在“工具 > 选项 > 文本编辑器 > 高级”中把“文本格式设置方法”改为“理想”，100% 缩放就和缩放后一样平滑；如果已打开的文件没有变化，重新打开即可。字号很小时“理想”比“显示”略软一些。

## 效果检查

[style-gallery.cpp](style-gallery.cpp) 是独立的 C++20 示例，不参与工作区构建，可直接在 VS 中打开。编译运行的预期输出为：

```text
best = 15, edges = 2, seen = 5
```

对照默认效果检查：

- 注释为斜体，`NOTE`、`TODO`、`FIXME` 为橙色粗体。
- `std`、`vector`、`iota`、`begin`、`sort`、`map`、`to_string`、`accumulate`、`ios::sync_with_stdio`、`chrono::steady_clock::now` 为斜体，颜色与主题中的类型或函数颜色相同。
- `REP`、`ll`、`gallery`、`Edge`、`count`、`max`、`build`、`values`，以及形参 `a`、`b`、`n` 为粗体。
- 第 26、28 行声明了 `count` 和 `max`，所以 `gallery::max` 为粗体，`std::max` 仍为斜体。
- 光标放在第 38 行 `main` 函数体的 `{` 左边或右边时，这个 `{` 和第 55 行的 `}` 为青色粗体加下划线；光标放在第 46 行 `{{1, 7}` 的两个 `{` 之间时，内层的 `{1, 7}` 配对。

## 文件

| 文件 | 内容 |
| --- | --- |
| `src/Cpp/` | 词法分析、声明收集、样式判断和括号配对，不依赖 Visual Studio |
| `src/Styling/` | 把 `colors` 和 `styles` 应用到编辑器，并在“字体和颜色”变化后重新应用 |
| `src/Tagging/` | 四个新增分类，以及 C/C++ 的符号样式和匹配括号标记器 |
| `src/Configuration/` | 配置查找、合并、检查和文件监视，读取 `.vssettings` 配色 |
| `tests/` | 单元测试，`python build.py` 自动运行 |
| `resources/` | 内置默认配置和系统符号表 |
