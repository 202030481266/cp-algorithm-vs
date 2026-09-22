#pragma once
// 使用说明：cp-stl/docs/usage/geometry/convex_hull.md
// 完整示例：cp-stl/examples/geometry/convex_hull.cpp
#include <algorithm>
#include <vector>

namespace cp {
// 整数坐标 |x|,|y| <= 1e9；该范围内 long long 足够精确表示叉积。
struct Point {
    long long x, y;
    friend bool operator<(Point a, Point b) { return a.x < b.x || (a.x == b.x && a.y < b.y); }
    friend bool operator==(Point a, Point b) { return a.x == b.x && a.y == b.y; }
};
#if defined(__SIZEOF_INT128__)
using CrossProduct = __int128;
#else
using CrossProduct = long long;
#endif
inline CrossProduct cross(Point a, Point b, Point c) {
    return CrossProduct(b.x - a.x) * (c.y - a.y) - CrossProduct(b.y - a.y) * (c.x - a.x);
}
// Andrew O(n log n)，去重，逆时针，首尾不重复，不保留边上的中间共线点。
// 全部共线时保留两端；0/1 个不同点直接返回。
inline std::vector<Point> convex_hull(std::vector<Point> points) {
    std::sort(points.begin(), points.end());
    points.erase(std::unique(points.begin(), points.end()), points.end());
    if (points.size() <= 1) return points;
    std::vector<Point> hull;
    for (Point p : points) {
        while (hull.size() >= 2 && cross(hull[hull.size() - 2], hull.back(), p) <= 0) hull.pop_back();
        hull.push_back(p);
    }
    std::size_t lower = hull.size();
    for (int i = int(points.size()) - 2; i >= 0; --i) {
        while (hull.size() > lower && cross(hull[hull.size() - 2], hull.back(), points[i]) <= 0) hull.pop_back();
        hull.push_back(points[i]);
    }
    hull.pop_back();
    return hull;
}
} // namespace cp
