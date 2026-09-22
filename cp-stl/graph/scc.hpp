#pragma once
// 使用说明：cp-stl/docs/usage/graph/scc.md
// 完整示例：cp-stl/examples/graph/scc.cpp
#include <algorithm>
#include <vector>

namespace cp {
struct SCCResult {
    std::vector<int> id;
    std::vector<std::vector<int>> groups;
};
// 迭代 Kosaraju O(n+m)，避免链状图递归爆栈。
// 缩点编号按拓扑序排列：跨分量边 u->v 有 id[u] < id[v]。
inline SCCResult strongly_connected_components(const std::vector<std::vector<int>>& g) {
    int n = int(g.size());
    std::vector<std::vector<int>> reverse(n);
    for (int u = 0; u < n; ++u) for (int v : g[u]) reverse[v].push_back(u);
    std::vector<int> seen(n), next(n), order, stack;
    for (int start = 0; start < n; ++start) if (!seen[start]) {
        seen[start] = 1;
        stack.push_back(start);
        while (!stack.empty()) {
            int u = stack.back();
            if (next[u] == int(g[u].size())) {
                stack.pop_back();
                order.push_back(u);
            } else {
                int v = g[u][next[u]++];
                if (!seen[v]) { seen[v] = 1; stack.push_back(v); }
            }
        }
    }
    SCCResult result{std::vector<int>(n, -1), {}};
    std::reverse(order.begin(), order.end());
    for (int start : order) if (result.id[start] == -1) {
        int component = int(result.groups.size());
        result.groups.emplace_back();
        result.id[start] = component;
        stack.push_back(start);
        while (!stack.empty()) {
            int u = stack.back(); stack.pop_back();
            result.groups.back().push_back(u);
            for (int v : reverse[u]) if (result.id[v] == -1) {
                result.id[v] = component;
                stack.push_back(v);
            }
        }
    }
    return result;
}
} // namespace cp
