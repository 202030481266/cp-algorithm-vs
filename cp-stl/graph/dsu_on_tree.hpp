#pragma once
// 使用说明：cp-stl/docs/usage/graph/dsu_on_tree.md
// 完整示例：cp-stl/examples/graph/dsu_on_tree.cpp
#include <cassert>
#include <vector>

namespace cp {
// 树上启发式合并（DSU on tree / sack）：对每个点 v，在统计结构恰好包含“v 的子树中所有点”时调用 answer(v)。
// add(x) 把点 x 加入统计，remove(x) 移除。保留重儿子的统计、暴力加入轻儿子子树，
// 每个点被 add/remove O(log n) 次，总共 O(n log n) 次回调。非递归实现，结束时统计为空。
template<class Add, class Remove, class Answer>
void dsu_on_tree(const std::vector<std::vector<int>>& g, int root, Add add, Remove remove, Answer answer) {
    int n = int(g.size());
    assert(0 <= root && root < n);
    // 先序遍历：子树 v 对应 order[tin[v], tin[v] + size[v])。
    std::vector<int> parent(n, -1), order, tin(n), size(n, 1), heavy(n, -1);
    order.reserve(n);
    std::vector<int> stack{root};
    parent[root] = root;
    while (!stack.empty()) {
        int u = stack.back();
        stack.pop_back();
        tin[u] = int(order.size());
        order.push_back(u);
        for (int v : g[u]) if (parent[v] == -1) { parent[v] = u; stack.push_back(v); }
    }
    assert(int(order.size()) == n); // 必须是一棵树
    for (int i = n - 1; i > 0; --i) {
        int u = order[i], p = parent[u];
        size[p] += size[u];
        if (heavy[p] == -1 || size[u] > size[heavy[p]]) heavy[p] = u;
    }
    auto add_subtree = [&](int v) { for (int i = tin[v]; i < tin[v] + size[v]; ++i) add(order[i]); };
    // 显式栈模拟：先处理所有轻儿子（处理完即清空），最后处理重儿子（保留），再回到自己。
    struct Frame { int v; bool keep, expanded; };
    std::vector<Frame> frames{{root, false, false}};
    while (!frames.empty()) {
        Frame f = frames.back();
        if (!f.expanded) {
            frames.back().expanded = true;
            if (heavy[f.v] != -1) frames.push_back({heavy[f.v], true, false});
            for (int c : g[f.v]) if (c != parent[f.v] && c != heavy[f.v]) frames.push_back({c, false, false});
            continue;
        }
        frames.pop_back();
        for (int c : g[f.v]) if (c != parent[f.v] && c != heavy[f.v]) add_subtree(c);
        add(f.v);
        answer(f.v);
        if (!f.keep) for (int i = tin[f.v]; i < tin[f.v] + size[f.v]; ++i) remove(order[i]);
    }
}
} // namespace cp
