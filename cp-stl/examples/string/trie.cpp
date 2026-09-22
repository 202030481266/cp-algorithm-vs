#include <iostream>
#include "string/trie.hpp"

int main() {
    cp::Trie trie;
    trie.insert("app");
    trie.insert("app");
    trie.insert("apple");
    trie.insert("");
    std::cout << "app=" << trie.count("app") << '\n';
    std::cout << "prefix_ap=" << trie.count_prefix("ap") << '\n';
    std::cout << "size=" << trie.count_prefix("") << '\n';
    std::cout << std::boolalpha;
    std::cout << "erase_app=" << trie.erase("app") << '\n';
    std::cout << "app_after=" << trie.count("app") << '\n';
    std::cout << "erase_ape=" << trie.erase("ape") << '\n';
    std::cout << "empty_word=" << trie.count("") << '\n';
}
