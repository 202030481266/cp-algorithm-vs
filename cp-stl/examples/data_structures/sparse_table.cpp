#include <iostream>
#include <vector>
#include "data_structures/sparse_table.hpp"

int main() {
    std::vector<int> a{12, 18, 6, 9};
    cp::SparseTable<int> minimum(a);
    cp::SparseTable<int, cp::Max> maximum(a);
    cp::SparseTable<int, cp::Gcd> gcd(a);
    std::cout << "size=" << minimum.size() << '\n';
    std::cout << "min(1,4)=" << minimum.query(1, 4) << '\n';
    std::cout << "max(0,3)=" << maximum.query(0, 3) << '\n';
    std::cout << "gcd(0,4)=" << gcd.query(0, 4) << '\n';
    std::cout << "single=" << minimum.query(2, 3) << '\n';
}
