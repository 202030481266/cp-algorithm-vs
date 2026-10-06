#include <iomanip>
#include <iostream>
#include <vector>
#include "util/random.hpp"

template<class T> void print(const char* name, const std::vector<T>& a) {
    std::cout << name << ':';
    for (const auto& x : a) std::cout << ' ' << x;
    std::cout << '\n';
}

int main() {
    // 对拍生成器中写 cp::Random rnd(std::stoull(argv[1]))；同一种子在 MSVC 和 GCC 上生成相同的数据。
    cp::Random rnd(42);
    int n = int(rnd.integer(5, 8));
    std::cout << "n: " << n << '\n';
    print("array", rnd.array(n, -9, 9));                // 可重复，范围 [-9,9]
    print("distinct", rnd.distinct_array(n, 1, 20));    // 互不相同
    print("permutation", rnd.permutation(n, 1));        // 1..n 的排列
    print("partition", rnd.partition(20, 4, 1));        // 4 个正整数，和为 20
    auto [l, r] = rnd.interval(1, n);                    // 1 <= l <= r <= n
    std::cout << "interval: " << l << ' ' << r << '\n';
    std::cout << "strings: " << rnd.string(n, 'a', 'c') << ' ' << rnd.string(n, "01") << '\n';
    std::cout << "choice: " << rnd.choice({1, 10, 100}) << '\n';
    std::cout << "real: " << std::fixed << std::setprecision(3) << rnd.real(0, 1) << '\n';
    std::cout << "chance: " << std::boolalpha << rnd.chance(0.5) << '\n';
    std::vector<int> cards{1, 2, 3, 4, 5};
    rnd.shuffle(cards);
    print("shuffle", cards);

    // 树和图的点编号是 0..n-1，下面按 1-based 输出。
    std::cout << "tree:";
    for (auto [u, v] : rnd.tree(n)) std::cout << ' ' << u + 1 << '-' << v + 1;
    std::cout << "\nparents:";
    auto parent = rnd.parents(n);
    for (int v = 1; v < n; ++v) std::cout << ' ' << parent[v] + 1; // p_2 .. p_n，均小于自身编号
    std::cout << "\ngraph:";
    for (auto [u, v] : rnd.graph(n, n + 1, true)) std::cout << ' ' << u + 1 << '-' << v + 1;
    std::cout << '\n';

    cp::Random replay(42);
    std::cout << "replay: " << (replay.integer(5, 8) == n) << '\n';
}
