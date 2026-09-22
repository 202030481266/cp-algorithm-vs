#pragma once
// 使用说明：cp-stl/docs/usage/util/graphviz.md
// 完整示例：cp-stl/examples/util/graphviz.cpp

#include <cstddef>
#include <iostream>
#include <optional>
#include <sstream>
#include <stdexcept>
#include <string>
#include <string_view>
#include <vector>

namespace graphviz {

struct Edge {
    int u, v;
    std::optional<long long> w = std::nullopt;
};

struct Options {
    bool directed = false;
    int index_base = 0;             // 只接受 0 或 1
    bool left_to_right = false;     // dot 引擎：横向布局
    bool show_edge_ids = false;     // 显示 edges 中的下标，从 0 开始
};

// 仅借用 out：不关闭流，也不主动调用 flush。
// 无向图要求每条实际边只传入一次；不会自动去重。
inline std::ostream& dump_graph(
    int n,
    const std::vector<Edge>& edges,
    std::ostream& out = std::cout,
    const Options& opt = {}
) {
    if (n < 0)
        throw std::invalid_argument("graphviz: n must be nonnegative");

    if (opt.index_base != 0 && opt.index_base != 1)
        throw std::invalid_argument("graphviz: index_base must be 0 or 1");

    // 合法编号区间：[index_base, index_base + n)。
    // 转成 long long 后相加，避免 n + 1 的 int 溢出。
    const long long end = static_cast<long long>(n) + opt.index_base;
    const auto valid = [&](int v) {
        return v >= opt.index_base && static_cast<long long>(v) < end;
    };

    // 先检查全部输入，避免发现非法边时已经输出了一半。
    for (std::size_t i = 0; i < edges.size(); ++i) {
        const auto& e = edges[i];
        if (!valid(e.u) || !valid(e.v)) {
            throw std::out_of_range(
                "graphviz: invalid endpoint in edge #" + std::to_string(i)
                + " (" + std::to_string(e.u)
                + ", " + std::to_string(e.v) + ")"
            );
        }
    }

    if (!out)
        throw std::runtime_error("graphviz: output stream is not writable");

    // 数字先转成十进制字符串，再用非格式化输出写入。
    // 不受调用者的 std::hex、std::setw 等设置影响。
    const auto emit = [&](std::string_view text) {
        out.write(text.data(), static_cast<std::streamsize>(text.size()));
        if (!out)
            throw std::runtime_error("graphviz: failed to write DOT");
    };

    emit(opt.directed ? "digraph G {\n" : "graph G {\n");
    emit(opt.left_to_right ? "  rankdir=LR;\n" : "  rankdir=TB;\n");
    emit("  node [shape=circle];\n");

    // 显式输出所有点，包括孤立点。
    for (int i = 0; i < n; ++i)
        emit("  " + std::to_string(i + opt.index_base) + ";\n");

    const char* edge_op = opt.directed ? " -> " : " -- ";

    for (std::size_t i = 0; i < edges.size(); ++i) {
        const auto& e = edges[i];
        std::string line = "  " + std::to_string(e.u)
                         + edge_op + std::to_string(e.v);

        std::string label;
        if (opt.show_edge_ids)
            label = "#" + std::to_string(i);

        if (e.w.has_value()) {
            if (!label.empty())
                label += ": ";
            label += std::to_string(*e.w);
        }

        if (!label.empty())
            line += " [label=\"" + label + "\"]";

        line += ";\n";
        emit(line);
    }

    emit("}\n");
    return out;
}

// 需要字符串时，复用同一套输出逻辑。
inline std::string to_dot(
    int n,
    const std::vector<Edge>& edges,
    const Options& opt = {}
) {
    std::ostringstream out;
    dump_graph(n, edges, out, opt);
    return out.str();
}

} // namespace graphviz