#include <iostream>
#include <string>
#include <vector>
#include "string/aho_corasick.hpp"

int main() {
    cp::AhoCorasick ac;
    std::vector<std::string> patterns{"he", "she", "hers", "he"};
    std::vector<int> ids;
    for (const auto& pattern : patterns) ids.push_back(ac.insert(pattern));
    ac.build();

    auto counts = ac.count("ushershe");
    for (int i = 0; i < int(patterns.size()); ++i)
        std::cout << ids[i] << ':' << patterns[i] << '=' << counts[ids[i]] << '\n';
    std::cout << "reused_he=" << ac.count("hehe")[ids[0]] << '\n';
}
