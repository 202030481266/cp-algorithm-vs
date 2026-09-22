#include <iostream>
#include "data_structures/binary_trie.hpp"

int main() {
    cp::BinaryTrie trie;
    std::cout << std::boolalpha;
    std::cout << "empty_query=" << trie.max_xor(2).has_value() << '\n';
    trie.insert(3);
    trie.insert(5);
    trie.insert(5);
    std::cout << "size=" << trie.size() << " count(5)=" << trie.count(5) << '\n';
    if (auto value = trie.max_xor(2)) std::cout << "max_xor(2)=" << *value << '\n';
    trie.erase(5);
    std::cout << "after_one_erase=" << trie.count(5) << '\n';
    trie.erase(5);
    std::cout << "after_two_erases=" << *trie.max_xor(2) << '\n';
    std::cout << "erase_missing=" << trie.erase(9) << '\n';
}
