#pragma once
// 使用说明：cp-stl/docs/usage/string/trie.md
// 完整示例：cp-stl/examples/string/trie.cpp
#include <array>
#include <cassert>
#include <string_view>
#include <vector>

namespace cp {
// 小写 a-z，多重集合；insert/count/count_prefix/erase 都为 O(|s|)。
// erase 每次删一个；空字符串受支持；节点空间不会在删除后回收。
class Trie {
    struct Node { std::array<int, 26> next{}; int pass = 0, end = 0; };
    std::vector<Node> nodes_{1};
    static int code(char c) { assert('a' <= c && c <= 'z'); return c - 'a'; }
    int walk(std::string_view s) const {
        int u = 0;
        for (char c : s) { u = nodes_[u].next[code(c)]; if (!u) return -1; }
        return u;
    }
public:
    void insert(std::string_view s) {
        int u = 0;
        ++nodes_[u].pass;
        for (char c : s) {
            int k = code(c);
            if (!nodes_[u].next[k]) {
                nodes_[u].next[k] = int(nodes_.size());
                nodes_.emplace_back();
            }
            u = nodes_[u].next[k];
            ++nodes_[u].pass;
        }
        ++nodes_[u].end;
    }
    int count(std::string_view s) const { int u = walk(s); return u == -1 ? 0 : nodes_[u].end; }
    int count_prefix(std::string_view s) const { int u = walk(s); return u == -1 ? 0 : nodes_[u].pass; }
    bool erase(std::string_view s) {
        if (!count(s)) return false;
        int u = 0;
        --nodes_[u].pass;
        for (char c : s) { u = nodes_[u].next[code(c)]; --nodes_[u].pass; }
        --nodes_[u].end;
        return true;
    }
};
} // namespace cp
