#pragma once
// 使用说明：cp-stl/docs/usage/string/aho_corasick.md
// 完整示例：cp-stl/examples/string/aho_corasick.cpp
#include <array>
#include <cassert>
#include <string_view>
#include <vector>

namespace cp {
// 小写 a-z，多模式匹配。先 insert 全部模式，再 build，再 count。
// 不允许空模式；重复模式各有 id；返回各模式出现次数（含重叠）。
class AhoCorasick {
    struct Node { std::array<int, 26> next{}; int fail = 0; };
    std::vector<Node> nodes_{1};
    std::vector<int> terminals_, order_;
    bool built_ = false;
    static int code(char c) { assert('a' <= c && c <= 'z'); return c - 'a'; }
public:
    int insert(std::string_view s) {
        assert(!built_ && !s.empty());
        int u = 0;
        for (char c : s) {
            int k = code(c);
            if (!nodes_[u].next[k]) {
                nodes_[u].next[k] = int(nodes_.size());
                nodes_.emplace_back();
            }
            u = nodes_[u].next[k];
        }
        terminals_.push_back(u);
        return int(terminals_.size()) - 1;
    }
    // O(26 * 节点数)，补齐自动机转移；不可再次插入。
    void build() {
        assert(!built_);
        built_ = true;
        for (int v : nodes_[0].next) if (v) order_.push_back(v);
        for (std::size_t i = 0; i < order_.size(); ++i) {
            int u = order_[i];
            for (int c = 0; c < 26; ++c) {
                int v = nodes_[u].next[c];
                if (v) {
                    nodes_[v].fail = nodes_[nodes_[u].fail].next[c];
                    order_.push_back(v);
                } else nodes_[u].next[c] = nodes_[nodes_[u].fail].next[c];
            }
        }
    }
    // O(|text| + 节点数 + 模式数)，可重复调用。
    std::vector<long long> count(std::string_view text) const {
        assert(built_);
        std::vector<long long> visits(nodes_.size()), result;
        int u = 0;
        for (char c : text) { u = nodes_[u].next[code(c)]; ++visits[u]; }
        for (int i = int(order_.size()) - 1; i >= 0; --i) {
            int v = order_[i];
            visits[nodes_[v].fail] += visits[v];
        }
        for (int v : terminals_) result.push_back(visits[v]);
        return result;
    }
};
} // namespace cp
