#pragma once
// 使用说明：cp-stl/docs/usage/graph/two_sat.md
// 完整示例：cp-stl/examples/graph/two_sat.cpp
#include <cassert>
#include <optional>
#include <vector>
#include "scc.hpp"

namespace cp {
class TwoSAT {
    int n_;
    std::vector<std::vector<int>> graph_;
public:
    explicit TwoSAT(int n) : n_(n), graph_(2 * n) {}
    // 加入 (x == f) OR (y == g)，变量下标从 0 开始。
    void add_clause(int x, bool f, int y, bool g) {
        assert(0 <= x && x < n_ && 0 <= y && y < n_);
        int a = 2 * x + int(f), b = 2 * y + int(g);
        graph_[a ^ 1].push_back(b);
        graph_[b ^ 1].push_back(a);
    }
    // O(变量数+子句数)。nullopt 表示无解。
    std::optional<std::vector<bool>> solve() const {
        auto result = strongly_connected_components(graph_);
        std::vector<bool> answer(n_);
        for (int i = 0; i < n_; ++i) {
            if (result.id[2 * i] == result.id[2 * i + 1]) return std::nullopt;
            answer[i] = result.id[2 * i] < result.id[2 * i + 1];
        }
        return answer;
    }
};
} // namespace cp
