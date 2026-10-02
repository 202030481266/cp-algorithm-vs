#pragma once
// 使用说明：cp-stl/docs/usage/data_structures/treap.md
// 完整示例：cp-stl/examples/data_structures/treap.cpp
#include <cassert>
#include <cstdint>
#include <functional>
#include <optional>
#include <type_traits>
#include <utility>
#include <vector>

namespace cp {
namespace treap_detail {
// xorshift32：固定种子，结果可重现；FHQ Treap 只需要优先级足够随机。
struct Priority {
    std::uint32_t state = 2463534242u;
    std::uint32_t operator()() {
        state ^= state << 13;
        state ^= state >> 17;
        state ^= state << 5;
        return state;
    }
};
template<class T, class = void> struct HasReverse : std::false_type {};
template<class T>
struct HasReverse<T, std::void_t<decltype(std::declval<T&>().reverse())>> : std::true_type {};
} // namespace treap_detail

// 有序多重集合（FHQ Treap）：插入、删除一个、排名、第 k 小、前驱、后继，期望 O(log n)。
// 可替代 GNU pb_ds 的 tree（MSVC 没有 pb_ds），并且允许重复元素。删除的节点会被复用。
template<class T, class Compare = std::less<T>>
class OrderedMultiset {
    struct Node { T key; std::uint32_t priority; int left = 0, right = 0, size = 1; };
    std::vector<Node> nodes_;
    std::vector<int> free_;
    int root_ = 0;
    Compare less_;
    treap_detail::Priority random_;
    void pull(int p) { nodes_[p].size = 1 + nodes_[nodes_[p].left].size + nodes_[nodes_[p].right].size; }
    // a 得到键 < key 的部分，b 得到键 >= key 的部分。
    void split(int p, const T& key, int& a, int& b) {
        if (!p) { a = b = 0; return; }
        if (less_(nodes_[p].key, key)) {
            a = p;
            split(nodes_[p].right, key, nodes_[p].right, b);
        } else {
            b = p;
            split(nodes_[p].left, key, a, nodes_[p].left);
        }
        pull(p);
    }
    int merge(int a, int b) {
        if (!a || !b) return a ^ b;
        if (nodes_[a].priority > nodes_[b].priority) {
            nodes_[a].right = merge(nodes_[a].right, b);
            pull(a);
            return a;
        }
        nodes_[b].left = merge(a, nodes_[b].left);
        pull(b);
        return b;
    }
public:
    explicit OrderedMultiset(Compare less = {}) : less_(less) {
        nodes_.push_back(Node{T(), 0, 0, 0, 0}); // 0 号为空节点，size = 0。
    }
    int size() const { return nodes_[root_].size; }
    bool empty() const { return root_ == 0; }
    void reserve(int n) { nodes_.reserve(n + 1); }
    void insert(const T& key) {
        int node;
        if (free_.empty()) { node = int(nodes_.size()); nodes_.push_back(Node{key, random_()}); }
        else { node = free_.back(); free_.pop_back(); nodes_[node] = Node{key, random_()}; }
        // 沿查找路径下降到优先级更小的位置，只拆这一棵子树（比 split+merge 少一半常数）。
        int parent = 0, side = 0, p = root_;
        while (p && nodes_[p].priority > nodes_[node].priority) {
            ++nodes_[p].size;
            parent = p;
            side = less_(key, nodes_[p].key) ? 0 : 1;
            p = side ? nodes_[p].right : nodes_[p].left;
        }
        int a, b;
        split(p, key, a, b);
        nodes_[node].left = a;
        nodes_[node].right = b;
        pull(node);
        if (!parent) root_ = node;
        else if (side) nodes_[parent].right = node;
        else nodes_[parent].left = node;
    }
    // 删除一个等于 key 的元素；不存在时返回 false。
    bool erase(const T& key) {
        if (!contains(key)) return false;
        int parent = 0, side = 0, p = root_;
        while (true) {
            --nodes_[p].size;
            if (less_(key, nodes_[p].key)) side = 0;
            else if (less_(nodes_[p].key, key)) side = 1;
            else break;
            parent = p;
            p = side ? nodes_[p].right : nodes_[p].left;
        }
        int merged = merge(nodes_[p].left, nodes_[p].right);
        if (!parent) root_ = merged;
        else if (side) nodes_[parent].right = merged;
        else nodes_[parent].left = merged;
        free_.push_back(p);
        return true;
    }
    // 严格小于 key 的元素个数（即 key 的 0-based 排名）。
    int count_less(const T& key) const {
        int result = 0;
        for (int p = root_; p;) {
            if (less_(nodes_[p].key, key)) { result += nodes_[nodes_[p].left].size + 1; p = nodes_[p].right; }
            else p = nodes_[p].left;
        }
        return result;
    }
    // 小于等于 key 的元素个数。
    int count_less_equal(const T& key) const {
        int result = 0;
        for (int p = root_; p;) {
            if (!less_(key, nodes_[p].key)) { result += nodes_[nodes_[p].left].size + 1; p = nodes_[p].right; }
            else p = nodes_[p].left;
        }
        return result;
    }
    int count(const T& key) const { return count_less_equal(key) - count_less(key); }
    bool contains(const T& key) const {
        for (int p = root_; p;) {
            if (less_(key, nodes_[p].key)) p = nodes_[p].left;
            else if (less_(nodes_[p].key, key)) p = nodes_[p].right;
            else return true;
        }
        return false;
    }
    // 第 k 小（k 从 0 开始，计重复），要求 0 <= k < size()。
    const T& kth(int k) const {
        assert(0 <= k && k < size());
        int p = root_;
        while (true) {
            int left = nodes_[nodes_[p].left].size;
            if (k < left) p = nodes_[p].left;
            else if (k == left) return nodes_[p].key;
            else { k -= left + 1; p = nodes_[p].right; }
        }
    }
    // 前驱：严格小于 key 的最大元素；后继：严格大于 key 的最小元素。
    std::optional<T> prev(const T& key) const {
        int best = 0;
        for (int p = root_; p;) {
            if (less_(nodes_[p].key, key)) { best = p; p = nodes_[p].right; }
            else p = nodes_[p].left;
        }
        if (!best) return std::nullopt;
        return nodes_[best].key;
    }
    std::optional<T> next(const T& key) const {
        int best = 0;
        for (int p = root_; p;) {
            if (less_(key, nodes_[p].key)) { best = p; p = nodes_[p].left; }
            else p = nodes_[p].right;
        }
        if (!best) return std::nullopt;
        return nodes_[best].key;
    }
};

// 不需要区间修改时作为 ImplicitTreap 的 Tag。
struct NoTag {};

// 序列平衡树（按位置分裂的 FHQ Treap）：插入、删除、区间翻转、区间旋转、区间修改和区间查询。
// 下标 0-based，区间 [l,r)，每次操作期望 O(log n)。
// Info 与 LazySegmentTree 相同：默认构造是单位元，Info + Info 结合，info.apply(tag) 作用标记；
// Info 也可以是 int、long long 等数（聚合为求和）。Info 若不满足交换律且需要 reverse，
// 提供成员函数 void reverse()，在翻转时交换前后缀等方向相关的信息。
template<class Info, class Tag = NoTag>
class ImplicitTreap {
    static constexpr bool kHasTag = !std::is_same<Tag, NoTag>::value;
    struct Node {
        Info value{}, sum{};
        Tag tag{};
        int left = 0, right = 0, size = 0;
        std::uint32_t priority = 0;
        bool reversed = false, tagged = false;
    };
    std::vector<Node> nodes_{Node()};
    std::vector<int> free_;
    int root_ = 0;
    treap_detail::Priority random_;
    int new_node(const Info& value) {
        Node node;
        node.value = node.sum = value;
        node.size = 1;
        node.priority = random_();
        if (free_.empty()) { nodes_.push_back(node); return int(nodes_.size()) - 1; }
        int p = free_.back();
        free_.pop_back();
        nodes_[p] = node;
        return p;
    }
    void pull(int p) {
        Node& x = nodes_[p];
        x.size = 1 + nodes_[x.left].size + nodes_[x.right].size;
        x.sum = nodes_[x.left].sum + x.value + nodes_[x.right].sum;
    }
    void mark_reverse(int p) {
        if (!p) return;
        nodes_[p].reversed = !nodes_[p].reversed;
        if constexpr (treap_detail::HasReverse<Info>::value) nodes_[p].sum.reverse();
    }
    void apply_tag(int p, const Tag& t) {
        if constexpr (kHasTag) {
            if (!p) return;
            nodes_[p].value.apply(t);
            nodes_[p].sum.apply(t);
            if (nodes_[p].tagged) nodes_[p].tag.apply(t);
            else { nodes_[p].tag = t; nodes_[p].tagged = true; }
        } else {
            (void)p; (void)t;
        }
    }
    void push(int p) {
        Node& x = nodes_[p];
        if (x.reversed) {
            std::swap(x.left, x.right);
            mark_reverse(x.left);
            mark_reverse(x.right);
            x.reversed = false;
        }
        if constexpr (kHasTag) {
            if (x.tagged) {
                apply_tag(x.left, x.tag);
                apply_tag(x.right, x.tag);
                x.tag = Tag();
                x.tagged = false;
            }
        }
    }
    // a 得到前 k 个元素，b 得到其余元素。
    void split(int p, int k, int& a, int& b) {
        if (!p) { a = b = 0; return; }
        push(p);
        if (nodes_[nodes_[p].left].size < k) {
            a = p;
            split(nodes_[p].right, k - nodes_[nodes_[p].left].size - 1, nodes_[p].right, b);
        } else {
            b = p;
            split(nodes_[p].left, k, a, nodes_[p].left);
        }
        pull(p);
    }
    int merge(int a, int b) {
        if (!a || !b) return a ^ b;
        if (nodes_[a].priority > nodes_[b].priority) {
            push(a);
            nodes_[a].right = merge(nodes_[a].right, b);
            pull(a);
            return a;
        }
        push(b);
        nodes_[b].left = merge(a, nodes_[b].left);
        pull(b);
        return b;
    }
    void collect(int p, std::vector<Info>& out) {
        // 显式栈的中序遍历，避免退化时递归过深。
        std::vector<int> stack;
        while (p || !stack.empty()) {
            while (p) { push(p); stack.push_back(p); p = nodes_[p].left; }
            p = stack.back();
            stack.pop_back();
            out.push_back(nodes_[p].value);
            p = nodes_[p].right;
        }
    }
    void release(int p) {
        std::vector<int> stack;
        if (p) stack.push_back(p);
        while (!stack.empty()) {
            int x = stack.back();
            stack.pop_back();
            if (nodes_[x].left) stack.push_back(nodes_[x].left);
            if (nodes_[x].right) stack.push_back(nodes_[x].right);
            free_.push_back(x);
        }
    }
    // 在 [l,r) 对应的子树上调用 f(子树根)，然后接回。
    template<class F>
    void with_range(int l, int r, F f) {
        int a, b, c;
        split(root_, l, a, b);
        split(b, r - l, b, c);
        f(b);
        root_ = merge(merge(a, b), c);
    }
public:
    ImplicitTreap() = default;
    // O(n) 建树：按优先级用单调栈构造笛卡尔树。
    explicit ImplicitTreap(const std::vector<Info>& a) {
        nodes_.reserve(a.size() + 1);
        std::vector<int> stack;
        for (const Info& value : a) {
            int p = new_node(value), last = 0;
            while (!stack.empty() && nodes_[stack.back()].priority < nodes_[p].priority) {
                last = stack.back();
                stack.pop_back();
                pull(last);
            }
            nodes_[p].left = last;
            if (!stack.empty()) nodes_[stack.back()].right = p;
            stack.push_back(p);
        }
        root_ = stack.empty() ? 0 : stack.front(); // 栈底是优先级最大的节点。
        while (!stack.empty()) { pull(stack.back()); stack.pop_back(); }
    }
    int size() const { return nodes_[root_].size; }
    // 插入后 value 位于下标 pos，0 <= pos <= size()。
    void insert(int pos, const Info& value) {
        assert(0 <= pos && pos <= size());
        int a, b;
        split(root_, pos, a, b);
        root_ = merge(merge(a, new_node(value)), b);
    }
    void erase(int pos) { erase(pos, pos + 1); }
    void erase(int l, int r) {
        assert(0 <= l && l <= r && r <= size());
        int a, b, c;
        split(root_, l, a, b);
        split(b, r - l, b, c);
        release(b);
        root_ = merge(a, c);
    }
    Info get(int pos) {
        assert(0 <= pos && pos < size());
        Info result{};
        with_range(pos, pos + 1, [&](int p) { result = nodes_[p].value; });
        return result;
    }
    void set(int pos, const Info& value) {
        assert(0 <= pos && pos < size());
        with_range(pos, pos + 1, [&](int p) { nodes_[p].value = nodes_[p].sum = value; });
    }
    Info prod(int l, int r) {
        assert(0 <= l && l <= r && r <= size());
        if (l == r) return Info{};
        Info result{};
        with_range(l, r, [&](int p) { result = nodes_[p].sum; });
        return result;
    }
    void apply(int l, int r, const Tag& t) {
        static_assert(kHasTag, "ImplicitTreap without Tag does not support apply");
        assert(0 <= l && l <= r && r <= size());
        if (l < r) with_range(l, r, [&](int p) { apply_tag(p, t); });
    }
    void reverse(int l, int r) {
        assert(0 <= l && l <= r && r <= size());
        if (r - l > 1) with_range(l, r, [&](int p) { mark_reverse(p); });
    }
    // 与 std::rotate 相同：把 [m,r) 移到 [l,m) 之前，可用来剪切粘贴一段序列。
    void rotate(int l, int m, int r) {
        assert(0 <= l && l <= m && m <= r && r <= size());
        int a, b, c, d;
        split(root_, l, a, b);
        split(b, m - l, b, c);
        split(c, r - m, c, d);
        root_ = merge(merge(a, c), merge(b, d));
    }
    std::vector<Info> to_vector() {
        std::vector<Info> out;
        out.reserve(size());
        collect(root_, out);
        return out;
    }
};
} // namespace cp
