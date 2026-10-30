# Editor Font Styles

Visual Studio 2022 / 2026 编辑器扩展：给代码加上 `.vssettings` 做不到的斜体、粗体和单独字体，在 C++ 中实现 Visual Assist 风格的符号样式，并让配色和字体跟着仓库走。

不需要购买或安装 Visual Assist。

![Editor Font Styles 默认效果](preview.png)

## 功能

- **配色无需导入**：直接读取 `.vssettings` 主题文件给编辑器着色，修改主题文件后立即生效，不改动 VS 保存的设置。
- **字体随配置**：在配置文件中指定编辑器字体和字号，换电脑或重装后不用重新设置。
- **任意项目的字形**：给“字体和颜色”中的任何项目设置斜体、倾斜、粗体、单独字体、字号、下划线、删除线和不透明度。
- **C++ 标准库符号斜体**：`std`、`vector`、`sort`、`INT_MAX` 等显示为斜体，对应 VA 的 System symbols in italics。
- **C++ 本文件符号粗体**：当前文件中声明的类型、函数、变量、宏等显示为粗体，对应 VA 的 Local symbols in bold。
- **注释标记**：注释中的 `TODO`、`FIXME`、`NOTE` 等显示为橙色粗体。
- **括号配对**：光标旁的一对 `()`、`[]`、`{}` 显示为青色粗体加下划线，光标在括号内侧也有效。
- **不依赖 IntelliSense**：符号样式由扩展自己分析，打开文件立即生效，IntelliSense 解析失败也不受影响。

## 默认效果

| 元素 | 效果 |
| --- | --- |
| 注释（所有语言） | 斜体 |
| C/C++ 标准库符号 | 斜体，颜色不变 |
| C/C++ 当前文件中声明的符号 | 粗体，颜色不变 |
| 注释中的 `TODO`、`FIXME`、`HACK`、`NOTE`、`XXX`、`BUG` | 橙色 `#FF8C00` 粗体 |
| C/C++ 光标旁的一对括号 | 青色 `#40E0D0` 粗体加下划线 |

打开 [style-gallery.cpp](style-gallery.cpp) 可以看到全部默认效果，上图即为该文件。

## 安装

1. 关闭 Visual Studio，双击 [EditorFontStyles.vsix](EditorFontStyles.vsix)，选择要安装到的 Visual Studio。
2. 按需在仓库根目录添加 `.editor-font-styles.json`（见[配置](#配置)）；没有配置文件时使用内置默认值。
3. 打开代码文件即生效。在 **视图 → 输出** 中选择 **Editor Font Styles**，可以查看使用的配置和配置错误。

卸载：**扩展 → 管理扩展 → 已安装 → Editor Font Styles → 卸载**，编辑器恢复为“字体和颜色”中保存的设置。

> 如果同时启用了 Visual Assist 的增强着色，请只保留一边：在 VA 中关闭 Local symbols in Bold 和 System symbols in Italics，或在本扩展中关闭 `systemSymbols` 和 `localSymbols`。

## 配置

```jsonc
{
  "$schema": "./themes/editor-font-styles/editor-font-styles.schema.json",
  "colors": "themes/visual-assist-dark/Visual-Assist-Dark.vssettings",
  "font": { "family": "Maple Mono NF CN", "size": 14 },
  "styles": {
    "CppControlKeywordSyntacticTokenFormat": { "italic": true },   // if、for、return 斜体
    "*comment*": { "fontFamily": "JetBrains Mono" },               // 注释换字体，保留默认的斜体
    "EditorFontStyles.CommentTag": null                            // 不要注释标记样式
  },
  "systemSymbols": { "extraNames": [ "gp_hash_table" ] }
}
```

配置文件名为 `.editor-font-styles.json`，使用按以下顺序找到的第一个：

1. 文档所在目录及其各级上级目录。放在仓库根目录即对整个仓库生效，并随仓库同步；
2. 用户目录 `%USERPROFILE%\.editor-font-styles.json`；
3. 内置默认值 [resources/default-config.json](resources/default-config.json)。

配置文件只需写要改的部分：`styles` 按项目和属性合并，写 `null` 删除一项；其他设置按键合并，列表整体替换。保存后立即生效，不需要重启。`$schema` 指向 [editor-font-styles.schema.json](editor-font-styles.schema.json)，在 VS 中编辑时有补全和检查。

“字体和颜色”由所有编辑器共用，所以同一时间只有一份配置生效：以最近获得焦点、且找到了配置文件的文档为准。没有配置文件的文档（例如用“转到定义”打开的标准库头文件）沿用当前配置。

### colors

一个或多个 `.vssettings` 文件，相对路径以配置文件所在目录为起点。扩展读取其中“文本编辑器”类别的全部项目（前景色、背景色、粗体）并应用到编辑器，效果与导入相同：

- 只在内存中生效，不改动 VS 保存的“字体和颜色”；删除 `colors` 即恢复。主题文件中出现的项目以主题文件为准，在对话框中修改不会生效。
- 写成列表时，后面的文件覆盖前面文件中的同名项目，可以在现成主题上叠加个人调整。
- 只使用颜色和粗体，不使用文件中的字体和字号，也不改变 IDE 的深色 / 浅色外观。
- 当前 VS 中不存在的项目会在输出窗口列出，例如未安装 VA 时的 `VA Class`。

### font

```jsonc
"font": { "family": "Consolas", "size": 14 }
```

代码编辑器的字体和字号（磅，与“字体和颜色”中的“大小”相同）。只写一项时另一项沿用“字体和颜色”中的设置；字体未安装时在输出窗口提示并回退。与 `colors` 一样不改动 VS 保存的设置。

### styles

键是“字体和颜色”的项目名称（与 `.vssettings` 中的 `Name` 相同），例如 `Comment`、`Keyword`、`CppFunctionSemanticTokenFormat`、`VA Class`，也可以写分类名称如 `cppFunction`。不区分大小写，支持 `*` 和 `?` 通配，后写的覆盖先写的。设置 `"listClassifications": true` 后，输出窗口会列出全部项目名称。

| 属性 | 取值 | 说明 |
| --- | --- | --- |
| `italic` | `true` / `false` | 斜体，使用字体自带的斜体字形 |
| `oblique` | `true` / `false` | 把常规字形倾斜 12° 显示；与 `italic` 同时设置时优先 |
| `bold` | `true` / `false` | 粗体，优先于“字体和颜色”中的粗体设置 |
| `fontFamily` | 字体名称 | 只改这一项的字体；未安装时忽略 |
| `fontScale` | 0.25～4 | 相对编辑器字号的倍数 |
| `underline` | `true` / `false` | 下划线 |
| `strikethrough` | `true` / `false` | 删除线 |
| `opacity` | 0～1 | 文字不透明度 |

`styles` 只改字形，颜色来自 `colors` 或“字体和颜色”。

扩展新增四个项目，在“字体和颜色”中显示为 **Editor Font Styles: System Symbol / Local Symbol / Comment Tag / Matched Brace**，名称分别为 `EditorFontStyles.SystemSymbol`、`EditorFontStyles.LocalSymbol`、`EditorFontStyles.CommentTag`、`EditorFontStyles.MatchedBrace`。前两项默认没有颜色，保留符号原本的颜色；也可以在对话框或 `.vssettings` 中给它们设置颜色，例如给括号配对加底色：

```xml
<Item Name="EditorFontStyles.MatchedBrace" Foreground="0x00D0E040" Background="0x005A5A1F" BoldFont="Yes" />
```

### 其他设置

| 设置 | 默认值 | 说明 |
| --- | --- | --- |
| `systemSymbols.enabled` | `true` | 标准库符号使用 System Symbol 样式 |
| `systemSymbols.extraNames` | `[]` | 另外视为系统符号的名称，例如 GCC 的 `__gnu_pbds` 容器 |
| `systemSymbols.excludeNames` | `[]` | 不视为系统符号的名称 |
| `localSymbols.enabled` | `true` | 当前文件中声明的符号使用 Local Symbol 样式 |
| `commentTags.enabled` | `true` | 注释标记使用 Comment Tag 样式 |
| `commentTags.words` | `TODO` 等 6 个 | 区分大小写，按整词匹配 |
| `matchedBraces.enabled` | `true` | 光标旁的一对括号使用 Matched Brace 样式 |
| `includeDirectories` | `[]` | 查找 `#include "..."` 的额外目录 |
| `listClassifications` | `false` | 在输出窗口按优先级列出全部项目 |

## 符号识别规则

符号样式按名称判断，同名的符号得到相同的样式，与 VA 的做法一致：

1. 被系统名称限定的名称是系统符号，例如 `std::max`、`ios::sync_with_stdio`、`chrono::steady_clock::now`；
2. 否则，在当前文件中任意位置声明过的名称是本文件符号，包括参数、局部变量、成员、模板参数、结构化绑定、宏名和宏参数；
3. 否则，在用 `#include "..."` 包含的项目头文件中声明过的名称不加样式；
4. 否则，出现在系统符号表中的名称是系统符号。

项目头文件按编译器的顺序查找：当前文件所在目录、最近的 `.vcxproj` 中的包含目录、`includeDirectories`。因此自己模板库里的 `add`、`query` 等名称不会被误认为标准库符号。

系统符号表 [resources/system-symbols.txt](resources/system-symbols.txt) 收录 MSVC 标准库和 UCRT 头文件中的公开名称，由 `generate_system_symbols.py` 从本机最新的 MSVC 工具集生成。

注释、字符串、`#include` 和 `#pragma` 行、`[[...]]` 属性以及 `#if 0` 中的代码不加样式；超过 2 MB 的文件不分析。

**与 Visual Assist 的差异**

- 文件中声明了 `count` 后，除了 `std::count`，其他所有 `count` 都加粗；成员同理，文件中有成员叫 `size` 时，所有 `.size()` 都加粗。
- 只认 MSVC 标准库和 UCRT 中的名称。Windows SDK、第三方库和 GCC 扩展需要写进 `extraNames`。
- 看不到宏展开后才出现的声明，例如 `DECLARE(foo)` 中的 `foo`。

## 括号配对

- `()`、`[]`、`{}` 参与配对，`<>` 不参与；光标在括号左边或右边都可以。
- 注释、字符串、字符常量和 `#if 0` 中的括号不参与配对；预处理指令中的括号只在同一条指令内配对，`#define BEGIN {` 不会打乱代码中的大括号。
- 未闭合的括号不影响外层，正在输入的 `if (a[i` 不会让整个块失去配对。
- VS 自带的括号高亮方框照常显示，本扩展的样式叠加在上面。

## 常见问题

**斜体字显示成了别的字母**

VS 在使用更纱黑体（Sarasa Mono）等 TTC 字体时，会把紧跟在常规文字后面的斜体画错，例如 `using namespace std;` 显示成 `using namespace OPB;`，Visual Assist 的斜体也有这个问题。扩展会自动避开：每个斜体项的字号放大 0.01%（肉眼不可见、不影响对齐），WPF 就不会把它与前面的常规文字合并排版。不需要任何配置。

**斜体看起来只是倾斜，不像真正的斜体**

字体没有斜体字形时（如 Fira Code），或只安装了常规字重文件时（如只装了 `Sarasa-Regular.ttc`），VS 会机械倾斜。请安装字体的完整字重，例如更纱黑体的 `Sarasa-SuperTTC`。不喜欢某款字体的斜体字形时，可以改用 `oblique`。

**给某一项换字体后行变高了**

不同字体的行高不同，换成行高更大的字体会撑高所在行。选择行高接近的字体，或用 `fontScale` 缩小。

**导入 `.vssettings` 后编辑器字体被重置了**

VS 2026 导入只含颜色的设置文件时会把编辑器字体重置为默认值。使用 `colors` 和 `font` 不需要导入，也就没有这个问题；即使导入过，编辑器仍显示 `font` 指定的字体。

**100% 缩放时笔画有锯齿**

这是 VS 的“文本格式设置方法”默认为“自动”造成的，与本扩展无关。在 **工具 → 选项 → 文本编辑器 → 高级** 中把它改为“理想”即可。

**自制主题的选区看不清**

不要给 `Plain Text` 设置背景色，VS 会把它画在选区之上；编辑器底色请写在 `TextView Background` 中，`Plain Text` 的背景保持“自动”（`0x02000000`）。本仓库的主题都已这样设置。

## 从源码构建

```text
python build.py           # 还原 NuGet 包、构建 Release、运行测试，生成 EditorFontStyles.vsix
python build.py --exp     # 同时部署到实验实例，用 devenv /rootSuffix Exp 试用
```

需要 Visual Studio 2022 或 2026 自带的 MSBuild；扩展 SDK 从 NuGet 还原，不需要安装“Visual Studio 扩展开发”工作负载，首次构建需要联网。发布新版本时，同时把 `source.extension.vsixmanifest` 和 `Properties/AssemblyInfo.cs` 中的版本号加一，VSIX Installer 才会按升级安装。

更新 MSVC 后可以重新生成系统符号表：

```text
python generate_system_symbols.py
python build.py
```

| 目录 | 内容 |
| --- | --- |
| `src/Cpp/` | 词法分析、声明收集、符号判断和括号配对，不依赖 Visual Studio |
| `src/Styling/` | 把 `colors`、`font` 和 `styles` 应用到编辑器 |
| `src/Tagging/` | 新增的四个分类和 C/C++ 标记器 |
| `src/Configuration/` | 配置查找、合并、校验和文件监视，读取 `.vssettings` |
| `tests/` | 单元测试，`python build.py` 自动运行 |
| `resources/` | 内置默认配置和系统符号表 |
