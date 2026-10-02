#pragma once
// 使用说明：cp-stl/docs/usage/data_structures/rollback_dsu.md
// 完整示例：cp-stl/examples/data_structures/rollback_dsu.cpp
#include <cassert>
#include <utility>
#include <vector>

namespace cp {
// 可撤销并查集：只按大小合并、不做路径压缩，find O(log n)。
// snapshot() 记录当前状态，rollback(s) 撤销其后的全部 merge。
class RollbackDSU {
    std::vector<int> parent_; // 根存负的集合大小。
    std::vector<std::pair<int, int>> history_; // (被挂上去的根, 它原来的 parent_ 值)。
    int components_;
public:
    explicit RollbackDSU(int n) : parent_(n, -1), components_(n) {}
    int find(int x) const {
        assert(0 <= x && x < int(parent_.size()));
        while (parent_[x] >= 0) x = parent_[x];
        return x;
    }
    bool merge(int a, int b) {
        a = find(a); b = find(b);
        if (a == b) return false;
        if (parent_[a] > parent_[b]) std::swap(a, b);
        history_.emplace_back(b, parent_[b]);
        parent_[a] += parent_[b];
        parent_[b] = a;
        --components_;
        return true;
    }
    bool same(int a, int b) const { return find(a) == find(b); }
    int size(int x) const { return -parent_[find(x)]; }
    int components() const { return components_; }
    int snapshot() const { return int(history_.size()); }
    void rollback(int snapshot) {
        assert(0 <= snapshot && snapshot <= int(history_.size()));
        while (int(history_.size()) > snapshot) {
            auto [b, old] = history_.back();
            history_.pop_back();
            parent_[parent_[b]] -= old; // 从新根的大小中减去 b 的集合大小。
            parent_[b] = old;
            ++components_;
        }
    }
};

// 线段树分治：离线处理“某个元素只在时间段 [l,r) 内生效”的问题，时间点为 0..time_count-1。
// 每个元素被挂到 O(log T) 个节点上；run 深度优先遍历，进入节点时 apply 其上的元素，
// 到达叶子 t 时调用 leaf(t)，离开节点时用 restore(save 的返回值) 撤销。总调用 O((T + 元素数) log T) 次。
template<class Item>
class SegmentTreeDivide {
    int n_;
    std::vector<std::vector<Item>> items_;
    template<class Save, class Apply, class Restore, class Leaf>
    void dfs(int p, int l, int r, Save& save, Apply& apply, Restore& restore, Leaf& leaf) {
        auto state = save();
        for (const Item& item : items_[p]) apply(item);
        if (r - l == 1) leaf(l);
        else {
            int m = l + (r - l) / 2;
            dfs(2 * p, l, m, save, apply, restore, leaf);
            dfs(2 * p + 1, m, r, save, apply, restore, leaf);
        }
        restore(state);
    }
    void add(int p, int l, int r, int ql, int qr, const Item& item) {
        if (ql <= l && r <= qr) { items_[p].push_back(item); return; }
        int m = l + (r - l) / 2;
        if (ql < m) add(2 * p, l, m, ql, qr, item);
        if (m < qr) add(2 * p + 1, m, r, ql, qr, item);
    }
public:
    explicit SegmentTreeDivide(int time_count) : n_(time_count), items_(4 * time_count + 4) {}
    // 元素 item 在时间点 [l,r) 内生效；空区间忽略。
    void add(int l, int r, const Item& item) {
        assert(0 <= l && l <= r && r <= n_);
        if (l < r) add(1, 0, n_, l, r, item);
    }
    // save() 返回可恢复的状态，apply(item) 加入元素，restore(state) 恢复，leaf(t) 回答时间 t。
    template<class Save, class Apply, class Restore, class Leaf>
    void run(Save save, Apply apply, Restore restore, Leaf leaf) {
        if (n_) dfs(1, 0, n_, save, apply, restore, leaf);
    }
};
} // namespace cp
