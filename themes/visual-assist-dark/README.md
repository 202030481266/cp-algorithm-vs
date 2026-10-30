# Visual Assist Dark

在 Visual Studio 2022 / 2026 中还原 Visual Assist 的深色 C++ 配色：金色类型、橙色函数、卡其色变量、紫色宏。使用 VS 原生 IntelliSense 的语义着色，不需要购买或安装 Visual Assist。

![Visual Assist Dark 配色预览](preview.png)

## 特点

- **取自 VA 实际配置**：核心语法颜色从 Visual Assist 保存的配色（注册表 `Whole Tomato\Visual Assist X\VaNet17\Format`）逐项提取。
- **原生语义着色**：颜色映射到 VS 的 C++ 语义分类，例如 `CppTypeSemanticTokenFormat`、`CppFunctionSemanticTokenFormat`，不依赖 VA。
- **局部变量和形参加粗**：用原生分类近似 VA 的 Local symbols in bold，让局部值在代码中更醒目。
- **运算符保持中性**：`cin >>`、`cout <<`、`v[i]` 等重载运算符与内置运算符同为灰色，不会染成函数色。
- **完整的编辑器配套色**：选区、当前行、行号、括号匹配、引用高亮统一调整；选区按 VS 的半透明绘制方式换算，多行选中时显示为完整的一块。
- **纯配色文件**：只包含“字体和颜色”，不改动字体、字号、快捷键和窗口布局。

## 选择版本

| 文件 | 适用情况 |
| --- | --- |
| [Visual-Assist-Dark.vssettings](Visual-Assist-Dark.vssettings) | 使用 VS 原生 C++ 着色（推荐） |
| [Visual-Assist-Dark-with-VA.vssettings](Visual-Assist-Dark-with-VA.vssettings) | 仍在使用 Visual Assist 的增强着色；另外设置 VA 的颜色项目，符号字重交给 VA 判断 |

两个文件任选其一。

## 安装

1. 确认 Visual Studio 使用内置的 **深色 / Dark** 主题。
2. 打开 **工具 → 导入和导出设置 → 导入选定的环境设置**。
3. 选择上表中的文件，类别勾选 **环境 → 字体和颜色**，点击 **完成**。
4. 打开 C++ 项目中的文件，等待 IntelliSense 解析完成。

逐页说明、更新和恢复方法见 [导入指南](INSTALL.md)。

**使用 [Editor Font Styles](../editor-font-styles/README.md) 扩展时无需导入**：在仓库根目录的 `.editor-font-styles.json` 中写入

```jsonc
"colors": "themes/visual-assist-dark/Visual-Assist-Dark.vssettings"
```

即可直接使用本配色，修改主题文件后立即生效。

## 配色

| 元素 | 颜色 | 应用范围 |
| --- | --- | --- |
| 背景 | `#1E1E1E` | 编辑器背景 |
| 普通文本 | `#DCDCDC` | 未分类的文本 |
| 类型 | `#FFD700` | 类、结构体、类型别名、类模板 |
| 函数 | `#FF8000` | 全局函数、成员函数、静态成员函数、函数模板 |
| 变量 | `#BDB76B` | 局部 / 全局变量、参数、成员字段；局部变量和参数加粗 |
| 宏 | `#BD63C5` | 预处理宏 |
| 关键字 | `#569CD6` | 普通关键字、控制流关键字 |
| 注释 | `#57A64A` | 普通注释、文档注释 |
| 字符串 | `#D69D85` | 字符串、转义字符 |
| 数字 | `#B5CEA8` | 数字字面量 |
| 枚举成员 | `#B9771E` | 枚举值 |
| 命名空间 | `#B8D7A3` | 命名空间名称 |
| 运算符 | `#B4B4B4` | 内置运算符和重载运算符 |
| 括号匹配 | `#1F5A5A` | 光标旁一对括号的底色 |

行号、选区、当前行和引用高亮等辅助颜色为配套设计，不属于 VA 原有配色。

## 粗体与斜体

Visual Assist 有两项字形功能，主题文件只能部分实现：

| VA 功能 | 原生版 | 完整效果 |
| --- | --- | --- |
| **Local symbols in bold**：当前文件中定义的符号加粗 | 局部变量和形参加粗（按分类，不判断定义位置） | [Editor Font Styles](../editor-font-styles/README.md) |
| **System symbols in italics**：系统库符号斜体 | 不支持，库函数保持常规字形 | [Editor Font Styles](../editor-font-styles/README.md) |

不使用 VA 时，安装 Editor Font Styles 扩展即可获得这两项效果，颜色仍来自本主题。

仍在使用 VA 时，导入 VA 兼容版，并在 **Visual Assist Options → Coloring and Attributes** 中启用 **Local symbols in Bold** 和 **System symbols in Italics**（粗体还需要启用 **Enhanced Syntax Coloring**）。

## 效果预览

打开 [semantic-gallery.cpp](semantic-gallery.cpp)（独立的 C++20 示例，可直接编译运行）检查效果：

- `Fenwick`、`Counter`、`Node` 等类型为金色，`add`、`prefix_sum` 等函数为橙色；
- 函数参数和局部变量为卡其色加粗，`tree_` 等成员字段为卡其色常规字重；
- `operator_color_demo()` 中流的 `<<`、`>>`、容器的 `[]` 和内置移位都为灰色；
- `VA_TRACE` 为紫色，`State::complete` 等枚举成员为棕金色。

变量和函数全是灰色时，确认文件属于已加载的 C++ 项目，并且 C/C++ IntelliSense 已启用。之前为了 VA 关闭过原生 IntelliSense 的，需要重新启用。

## 已知限制

- VS 与 VA 对未解析符号、依赖类型、构造函数等的分类可能不同，这部分颜色取决于语言服务，主题文件无法完全一致。
- `CurrentLineInactiveFormat`（编辑器失去焦点时的当前行）在 VS 2026 中已不存在，导入时会被忽略，不影响其他颜色。
- VA 兼容版中的 `VA ` 开头的项目只在安装了 Visual Assist 时生效。
- 预览图是按配色绘制的示意图，实际的符号分类由 VS 的 C++ IntelliSense 决定。

## 参考

- [Visual Assist：Enhanced Syntax Coloring](https://www.wholetomato.com/en/documentation/code-understanding/enhanced-syntax-coloring)
- [Visual Assist：Local Symbols in Bold](https://www.wholetomato.com/documentation/code-understanding/local-symbols-in-bold)
- [Visual Assist：System Symbols in Italics](https://www.wholetomato.com/documentation/code-understanding/system-symbols-in-italics)
- [Visual Studio：更改字体和颜色](https://learn.microsoft.com/en-us/visualstudio/ide/how-to-change-fonts-and-colors-in-visual-studio)
