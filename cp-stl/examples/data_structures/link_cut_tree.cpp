#include <functional>
#include <iostream>
#include <vector>
#include "data_structures/link_cut_tree.hpp"

int main() {
    // 5 个点，点权 1..5，维护路径点权和。
    cp::LinkCutTree<long long, std::plus<long long>> lct(std::vector<long long>{1, 2, 3, 4, 5}, 0);
    lct.link(0, 1);
    lct.link(1, 2);
    lct.link(1, 3); // 边：0-1, 1-2, 1-3
    std::cout << std::boolalpha;
    std::cout << "path(2,3)=" << lct.path_prod(2, 3) << " connected(0,4)=" << lct.connected(0, 4) << '\n';

    lct.cut(1, 3);
    lct.link(3, 4);
    std::cout << "link_again=" << lct.link(4, 3) << '\n'; // 已连通，不重复连边
    lct.link(4, 0); // 现在是一条链 2-1-0-4-3
    lct.set(1, 10);
    std::cout << "path(2,3)=" << lct.path_prod(2, 3) << '\n';
    lct.make_root(0);
    std::cout << "lca(2,3)=" << lct.lca(2, 3) << " root(3)=" << lct.find_root(3) << '\n';
}
