#include <cstdint>
#include <iostream>
#include <vector>
#include "data_structures/persistent_binary_trie.hpp"

int main() {
    std::vector<std::uint64_t> a{5, 1, 7, 2};
    cp::PersistentBinaryTrie<3> trie; // 只处理低 3 位，所有值 < 8
    std::vector<int> root{0};         // root[i] 是前 i 个数组成的版本
    for (auto x : a) root.push_back(trie.insert(root.back(), x));
    // 区间 [1,4) = {1,7,2}：与 6 的异或值依次为 7,1,4。
    std::cout << "count=" << trie.count(root[1], root[4])
              << " max=" << trie.max_xor(root[1], root[4], 6)
              << " min=" << trie.min_xor(root[1], root[4], 6) << '\n';
    std::cout << "prefix max=" << trie.max_xor(0, root[2], 6) << '\n'; // {5,1}
}
