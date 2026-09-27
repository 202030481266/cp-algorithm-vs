#include <bitset>
#include <iostream>
#include <cstdlib>
#include <limits>
#include <vector>
#include "graph/mst.hpp"

int main() {
    std::vector<cp::UndirectedEdge> edges{
        {0, 1, 4}, {0, 2, 1}, {1, 2, 2}, {1, 3, 1}, {2, 3, 5}
    };
    auto tree = cp::kruskal(4, edges);
    std::cout << std::boolalpha;
    std::cout << "connected=" << tree.connected << '\n';
    std::cout << "weight=" << tree.weight << " edges=" << tree.edges.size() << '\n';

    // 加入没有任何边的点 4，结果变成生成森林。
    auto forest = cp::kruskal(5, edges);
    std::cout << "forest_connected=" << forest.connected << '\n';
    std::cout << "forest_weight=" << forest.weight << '\n';

    // 稠密图：对称邻接矩阵，缺边使用指定标记。
    const long long no_edge = std::numeric_limits<long long>::max();
    std::vector<std::vector<long long>> matrix{
        {0, 4, 1, no_edge}, {4, 0, 2, 1},
        {1, 2, 0, 5}, {no_edge, 1, 5, 0}
    };
    auto dense_tree = cp::prim_dense(matrix);
    std::cout << "dense_connected=" << dense_tree.connected << '\n';
    std::cout << "dense_weight=" << dense_tree.weight
              << " edges=" << dense_tree.edges.size() << '\n';

    for (auto& row : matrix) row.push_back(no_edge);
    matrix.emplace_back(5, no_edge); // 新增孤立点，对角线会被忽略。
    auto dense_forest = cp::prim_dense(matrix);
    std::cout << "dense_forest_connected=" << dense_forest.connected << '\n';
    std::cout << "dense_forest_weight=" << dense_forest.weight << '\n';

    // 完全图：边权为直线上两点的距离，按需计算，无需保存所有边。
    std::vector<long long> x{0, 2, 5, 9};
    auto complete_tree = cp::prim_dense(int(x.size()), [&](int u, int v) {
        return std::abs(x[u] - x[v]);
    });
    std::cout << "complete_connected=" << complete_tree.connected << '\n';
    std::cout << "complete_weight=" << complete_tree.weight
              << " edges=" << complete_tree.edges.size() << '\n';

    // 最大生成树：比较器同时控制选点和更新，返回的边权仍是原始正权。
    // int 邻接矩阵也可直接传入；这里用 -1 表示缺边。
    std::vector<std::vector<int>> int_matrix{
        {0, 4, 1, -1}, {4, 0, 2, 1}, {1, 2, 0, 5}, {-1, 1, 5, 0}
    };
    auto max_tree = cp::prim_dense(int_matrix, -1, std::greater<long long>{});
    std::cout << "max_weight=" << max_tree.weight
              << " edges=" << max_tree.edges.size() << '\n';

    // 完全图：边权为两列共同为 1 的位数，每个无序点对仅计算一次。
    std::vector<std::bitset<4>> columns{0b1111, 0b0111, 0b0011, 0b0001};
    int queries = 0;
    auto max_complete = cp::prim_dense(int(columns.size()), [&](int u, int v) -> long long {
        ++queries;
        return (columns[u] & columns[v]).count();
    }, no_edge, std::greater<long long>{});
    std::cout << "max_complete_weight=" << max_complete.weight
              << " queries=" << queries << '\n';
}
