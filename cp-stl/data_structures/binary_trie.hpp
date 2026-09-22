#pragma once
// 使用说明：cp-stl/docs/usage/data_structures/binary_trie.md
// 完整示例：cp-stl/examples/data_structures/binary_trie.cpp
#include <array>
#include <cassert>
#include <cstdint>
#include <optional>
#include <vector>

namespace cp {
// uint64_t 多重集合，插入/删除/最大异或查询 O(64)，删除不回收节点。
class BinaryTrie {
    struct Node { std::array<int, 2> child{}; int count = 0; };
    std::vector<Node> nodes_{1};
public:
    int size() const { return nodes_[0].count; }
    void insert(std::uint64_t x) {
        int u = 0;
        ++nodes_[u].count;
        for (int i = 63; i >= 0; --i) {
            int bit = int(x >> i & 1);
            if (!nodes_[u].child[bit]) {
                nodes_[u].child[bit] = int(nodes_.size());
                nodes_.emplace_back();
            }
            u = nodes_[u].child[bit];
            ++nodes_[u].count;
        }
    }
    int count(std::uint64_t x) const {
        int u = 0;
        for (int i = 63; i >= 0; --i) {
            u = nodes_[u].child[x >> i & 1];
            if (!u) return 0;
        }
        return nodes_[u].count;
    }
    bool erase(std::uint64_t x) {
        if (!count(x)) return false;
        int u = 0;
        --nodes_[u].count;
        for (int i = 63; i >= 0; --i) { u = nodes_[u].child[x >> i & 1]; --nodes_[u].count; }
        return true;
    }
    // 返回最大异或值（不是被选中的元素），空集合返回 nullopt。
    std::optional<std::uint64_t> max_xor(std::uint64_t x) const {
        if (!size()) return std::nullopt;
        int u = 0;
        std::uint64_t answer = 0;
        for (int i = 63; i >= 0; --i) {
            int bit = int(x >> i & 1), opposite = nodes_[u].child[bit ^ 1];
            if (opposite && nodes_[opposite].count) {
                answer |= std::uint64_t{1} << i;
                u = opposite;
            } else u = nodes_[u].child[bit];
        }
        return answer;
    }
};
} // namespace cp
