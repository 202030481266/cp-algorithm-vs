#include <iostream>
#include <vector>
#include "data_structures/treap.hpp"

int main() {
    std::cout << std::boolalpha;
    cp::OrderedMultiset<int> s;
    for (int x : {5, 1, 4, 1, 3}) s.insert(x); // 1 1 3 4 5
    std::cout << "size=" << s.size() << " rank(4)=" << s.count_less(4) << " kth(2)=" << s.kth(2) << '\n';
    s.erase(1);                                 // 只删一个 1：1 3 4 5
    std::cout << "prev(4)=" << *s.prev(4) << " next(4)=" << *s.next(4)
              << " count(1)=" << s.count(1) << " has_next(5)=" << s.next(5).has_value() << '\n';

    cp::ImplicitTreap<int> seq(std::vector<int>{1, 2, 3, 4, 5});
    seq.reverse(1, 4);   // 1 4 3 2 5
    seq.rotate(0, 2, 5); // 把 [2,5) 移到前面：3 2 5 1 4
    seq.insert(0, 9);    // 9 3 2 5 1 4
    seq.erase(4);        // 9 3 2 5 4
    std::vector<int> v = seq.to_vector();
    std::cout << "seq=";
    for (int i = 0; i < int(v.size()); ++i) std::cout << v[i] << (i + 1 < int(v.size()) ? ' ' : '\n');
    std::cout << "sum(1,4)=" << seq.prod(1, 4) << '\n';
}
