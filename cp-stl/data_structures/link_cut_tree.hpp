#pragma once
// 使用说明：cp-stl/docs/usage/data_structures/link_cut_tree.md
// 完整示例：cp-stl/examples/data_structures/link_cut_tree.cpp
#include <cassert>
#include <utility>
#include <vector>

namespace cp {
// Link-Cut Tree：动态维护森林的连边、删边、连通性、换根、LCA、点权修改与路径聚合，均摊 O(log n)。
// 点 0-based。Op 必须结合且交换（如 + / xor / min / max），因为翻转路径时不区分方向。
// 全部操作都是非递归的，不会因为链很长而爆栈。
template<class T, class Op>
class LinkCutTree {
    struct Node { int child[2] = {0, 0}; int parent = 0; bool reversed = false; T value, sum; };
    std::vector<Node> t_; // 内部编号 1..n，0 号为空节点（sum 为单位元）。
    std::vector<int> stack_;
    Op op_;
    bool is_splay_root(int x) const {
        int p = t_[x].parent;
        return t_[p].child[0] != x && t_[p].child[1] != x;
    }
    void pull(int x) {
        t_[x].sum = op_(op_(t_[t_[x].child[0]].sum, t_[x].value), t_[t_[x].child[1]].sum);
    }
    // 翻转标记：x 的左右儿子已经交换，标记表示它的儿子们还没交换。
    void reverse(int x) {
        if (!x) return;
        std::swap(t_[x].child[0], t_[x].child[1]);
        t_[x].reversed = !t_[x].reversed;
    }
    void push(int x) {
        if (t_[x].reversed) {
            reverse(t_[x].child[0]);
            reverse(t_[x].child[1]);
            t_[x].reversed = false;
        }
    }
    void rotate(int x) {
        int y = t_[x].parent, z = t_[y].parent, side = t_[y].child[1] == x;
        if (!is_splay_root(y)) t_[z].child[t_[z].child[1] == y] = x;
        t_[x].parent = z;
        int moved = t_[x].child[side ^ 1];
        t_[y].child[side] = moved;
        if (moved) t_[moved].parent = y;
        t_[x].child[side ^ 1] = y;
        t_[y].parent = x;
        pull(y);
        pull(x);
    }
    void splay(int x) {
        stack_.clear();
        for (int y = x;; y = t_[y].parent) {
            stack_.push_back(y);
            if (is_splay_root(y)) break;
        }
        while (!stack_.empty()) { push(stack_.back()); stack_.pop_back(); }
        while (!is_splay_root(x)) {
            int y = t_[x].parent, z = t_[y].parent;
            if (!is_splay_root(y)) rotate((t_[y].child[1] == x) == (t_[z].child[1] == y) ? y : x);
            rotate(x);
        }
    }
    // 打通根到 x 的实链；返回最后一次切换实链的位置（用于求 LCA）。
    int access(int x) {
        int last = 0;
        for (; x; last = x, x = t_[x].parent) {
            splay(x);
            t_[x].child[1] = last;
            pull(x);
        }
        return last;
    }
    void evert(int x) { access(x); splay(x); reverse(x); }
    int root_of(int x) {
        access(x);
        splay(x);
        while (t_[x].child[0]) { push(x); x = t_[x].child[0]; }
        splay(x);
        return x;
    }
    void check(int u) const { assert(0 <= u && u + 1 < int(t_.size())); (void)u; }
public:
    LinkCutTree(int n, T identity, Op op = {}) : LinkCutTree(std::vector<T>(n, identity), identity, op) {}
    LinkCutTree(const std::vector<T>& values, T identity, Op op = {}) : t_(values.size() + 1), op_(op) {
        t_[0].value = t_[0].sum = identity;
        for (int i = 0; i < int(values.size()); ++i) t_[i + 1].value = t_[i + 1].sum = values[i];
    }
    int size() const { return int(t_.size()) - 1; }
    bool connected(int u, int v) {
        check(u); check(v);
        return root_of(u + 1) == root_of(v + 1);
    }
    // u、v 不连通时连边并返回 true；已经连通时什么也不做并返回 false。
    bool link(int u, int v) {
        check(u); check(v);
        evert(u + 1);
        if (root_of(v + 1) == u + 1) return false;
        t_[u + 1].parent = v + 1;
        return true;
    }
    // 边 (u,v) 存在时删除并返回 true；否则什么也不做并返回 false。
    bool cut(int u, int v) {
        check(u); check(v);
        int x = u + 1, y = v + 1;
        evert(x);
        if (root_of(y) != x || t_[y].parent != x || t_[y].child[0]) return false;
        t_[y].parent = t_[x].child[1] = 0;
        pull(x);
        return true;
    }
    T get(int u) const { check(u); return t_[u + 1].value; }
    void set(int u, T value) {
        check(u);
        splay(u + 1); // 旋到所在 splay 的根，只需更新这一个节点的聚合值。
        t_[u + 1].value = value;
        pull(u + 1);
    }
    // u 到 v 路径上所有点权的聚合，要求两点连通。会把 u 设为所在树的根。
    T path_prod(int u, int v) {
        check(u); check(v);
        assert(connected(u, v));
        evert(u + 1);
        access(v + 1);
        splay(v + 1);
        return t_[v + 1].sum;
    }
    // 把 u 设为所在树的根。
    void make_root(int u) { check(u); evert(u + 1); }
    // u 所在树的当前根。
    int find_root(int u) { check(u); return root_of(u + 1) - 1; }
    // 以当前根为准的最近公共祖先，要求两点连通。
    int lca(int u, int v) {
        check(u); check(v);
        assert(connected(u, v));
        access(u + 1);
        return access(v + 1) - 1;
    }
};
} // namespace cp
