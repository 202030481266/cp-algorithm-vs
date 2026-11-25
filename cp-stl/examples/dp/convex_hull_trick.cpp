#include <iostream>
#include <vector>
#include "dp/convex_hull_trick.hpp"

int main() {
    // 把数组分成若干连续段，每段的代价是 (段内和)^2 + C，求最小总代价。
    // dp[i] = s_i^2 + C + min_j { (−2·s_j)·s_i + dp[j] + s_j^2 }：
    // 直线的斜率 −2·s_j 单调递减，查询点 s_i 单调递增。
    std::vector<long long> a{3, 1, 4, 1, 5, 9, 2, 6};
    const long long C = 20;
    int n = int(a.size());
    std::vector<long long> s(n + 1), dp(n + 1);
    for (int i = 0; i < n; ++i) s[i + 1] = s[i] + a[i];
    cp::MonotoneCHT hull;
    hull.add(0, 0); // j = 0
    for (int i = 1; i <= n; ++i) {
        dp[i] = s[i] * s[i] + C + hull.query_increasing(s[i]);
        hull.add(-2 * s[i], dp[i] + s[i] * s[i]);
    }
    std::cout << "monotone: " << dp[n] << '\n';

    // 李超线段树：直线可以按任意顺序加入，只要事先给出所有查询的横坐标。
    cp::LiChaoTree<long long> tree(s);
    std::vector<long long> f(n + 1);
    tree.add_line(0, 0);
    for (int i = 1; i <= n; ++i) {
        f[i] = s[i] * s[i] + C + tree.query(s[i]);
        tree.add_line(-2 * s[i], f[i] + s[i] * s[i]);
    }
    std::cout << "li chao: " << f[n] << '\n';

    // 线段只在自己的横坐标范围内参与取最小值。
    cp::LiChaoTree<long long> segments({0, 1, 2, 3, 4, 5});
    segments.add_segment(1, 0, 0, 5);  // y = x，x 在 [0,5]
    segments.add_segment(-1, 4, 1, 3); // y = 4 − x，x 在 [1,3]
    segments.add_line(0, 3);           // y = 3
    std::cout << "segments:";
    for (long long x = 0; x <= 5; ++x) std::cout << ' ' << segments.query(x);
    std::cout << '\n';
}
