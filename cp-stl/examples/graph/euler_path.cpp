#include <iostream>
#include <vector>
#include "graph/euler_path.hpp"

void print(const char* name, const std::vector<int>& path) {
    std::cout << name << ':';
    for (int v : path) std::cout << ' ' << v;
    std::cout << '\n';
}

int main() {
    // 有向图：0 的出度比入度大 1，必须从 0 出发。
    cp::EulerPath directed(3, true);
    directed.add_edge(0, 1);
    directed.add_edge(1, 2);
    directed.add_edge(2, 0);
    directed.add_edge(0, 2);
    auto a = directed.find(-1, true); // 字典序最小
    print("directed", a->vertices);
    print("edge ids", a->edges);

    // 无向“蝴蝶结”：两个三角形共用点 2，所有点度数为偶数，存在欧拉回路。
    cp::EulerPath bowtie(5, false);
    for (auto [u, v] : std::vector<std::pair<int, int>>{{0, 1}, {1, 2}, {2, 0}, {2, 3}, {3, 4}, {4, 2}})
        bowtie.add_edge(u, v);
    print("undirected", bowtie.find(-1, true)->vertices);

    // 星形图有 4 个奇度点，不存在欧拉路径。
    cp::EulerPath star(4, false);
    for (int v = 1; v < 4; ++v) star.add_edge(0, v);
    std::cout << std::boolalpha << "star_has_path=" << star.find().has_value() << '\n';
}
