#include <iostream>
#include "graph/two_sat.hpp"

int main() {
    cp::TwoSAT sat(2);
    sat.add_clause(0, true, 0, true);   // x0 必须为 true
    sat.add_clause(0, false, 1, true);  // x0 -> x1
    std::cout << std::boolalpha;
    if (auto answer = sat.solve()) {
        std::cout << "assignment: " << (*answer)[0] << ' ' << (*answer)[1] << '\n';
    } else {
        std::cout << "unsatisfiable\n";
    }

    sat.add_clause(1, false, 1, false); // 再强制 x1=false，产生矛盾
    std::cout << "contradiction=" << !sat.solve().has_value() << '\n';
}
