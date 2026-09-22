#include <iostream>
#include <optional>
#include <vector>
#include "util/graphviz.hpp"

int main() {
    std::vector<graphviz::Edge> edges{{0, 1, 7}, {1, 2, std::nullopt}};
    std::cout << "default:\n" << graphviz::to_dot(3, edges);

    graphviz::Options options;
    options.directed = true;
    options.index_base = 1;
    options.left_to_right = true;
    options.show_edge_ids = true;
    std::cout << "custom:\n";
    graphviz::dump_graph(2, {{1, 2, 9}}, std::cout, options);
}
