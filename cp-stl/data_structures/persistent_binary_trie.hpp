#pragma once
// 使用说明：cp-stl/docs/usage/data_structures/persistent_binary_trie.md
// 完整示例：cp-stl/examples/data_structures/persistent_binary_trie.cpp
#include <algorithm>
#include <cassert>
#include <cstdint>
#include <vector>

namespace cp {
// 可持久化 01 Trie：每次 insert 返回新版本的根，根 0 是空集合。
// 两个版本相减（hi 的计数减去 lo 的计数）得到一个多重集合，在其中查询与 x 的最大/最小异或。
// 只处理低 Bits 位（插入值必须 < 2^Bits），插入与查询 O(Bits)。
template<int Bits = 30>
class PersistentBinaryTrie {
    static_assert(1 <= Bits && Bits <= 64, "Bits must be in [1,64]");
    struct Node { int child[2] = {0, 0}; int count = 0; };
    std::vector<Node> nodes_{Node()}; // 0 号是空节点。
    static void check_value([[maybe_unused]] std::uint64_t x) {
        if constexpr (Bits < 64) assert(x >> Bits == 0);
    }
    // 在 hi-lo 的集合中逐位贪心：求最大时优先走与 x 相反的位，求最小时优先走相同的位。
    template<bool Maximize>
    std::uint64_t walk(int lo, int hi, std::uint64_t x) const {
        assert(nodes_[hi].count - nodes_[lo].count > 0);
        std::uint64_t answer = 0;
        for (int i = Bits - 1; i >= 0; --i) {
            int bit = int(x >> i & 1), want = Maximize ? bit ^ 1 : bit;
            if (nodes_[nodes_[hi].child[want]].count - nodes_[nodes_[lo].child[want]].count > 0) {
                if (want != bit) answer |= std::uint64_t{1} << i;
            } else {
                want ^= 1;
                if (want != bit) answer |= std::uint64_t{1} << i;
            }
            lo = nodes_[lo].child[want];
            hi = nodes_[hi].child[want];
        }
        return answer;
    }
public:
    // reserve_values 为预计插入次数，用于一次性预留 (Bits+1) 倍的节点。
    explicit PersistentBinaryTrie(int reserve_values = 0) {
        nodes_.reserve(std::size_t(reserve_values) * (Bits + 1) + 1);
    }
    int insert(int root, std::uint64_t x) {
        check_value(x);
        int new_root = int(nodes_.size());
        nodes_.push_back(nodes_[root]);
        int p = new_root;
        ++nodes_[p].count;
        for (int i = Bits - 1; i >= 0; --i) {
            int bit = int(x >> i & 1);
            Node copy = nodes_[nodes_[p].child[bit]];
            ++copy.count;
            nodes_.push_back(copy);
            nodes_[p].child[bit] = int(nodes_.size()) - 1;
            p = nodes_[p].child[bit];
        }
        return new_root;
    }
    // hi 版本相对 lo 版本多出的元素个数（lo 应是 hi 的历史版本）。
    int count(int lo_root, int hi_root) const { return nodes_[hi_root].count - nodes_[lo_root].count; }
    // 返回 max(x XOR y)（不是 y 本身），y 取自 hi-lo 的集合，集合必须非空。
    std::uint64_t max_xor(int lo_root, int hi_root, std::uint64_t x) const { return walk<true>(lo_root, hi_root, x); }
    std::uint64_t min_xor(int lo_root, int hi_root, std::uint64_t x) const { return walk<false>(lo_root, hi_root, x); }
    int node_count() const { return int(nodes_.size()); }
};
} // namespace cp
