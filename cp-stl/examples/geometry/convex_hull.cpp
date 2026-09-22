#include <iostream>
#include <vector>
#include "geometry/convex_hull.hpp"

int main() {
    std::vector<cp::Point> points{
        {0, 0}, {2, 0}, {2, 2}, {0, 2}, {1, 1}, {1, 0}, {0, 0}
    };
    auto hull = cp::convex_hull(points);
    std::cout << "hull:";
    for (auto p : hull) std::cout << " (" << p.x << ',' << p.y << ')';
    std::cout << '\n';
    cp::CrossProduct twice_area = 0;
    for (int i = 1; i + 1 < int(hull.size()); ++i)
        twice_area += cp::cross(hull[0], hull[i], hull[i + 1]);
    std::cout << "twice_area=" << static_cast<long long>(twice_area) << '\n';
    std::cout << std::boolalpha << "ccw=" << (cp::cross({0, 0}, {2, 0}, {1, 1}) > 0) << '\n';
    std::cout << "collinear_vertices=" << cp::convex_hull({{0, 0}, {1, 1}, {2, 2}}).size() << '\n';
}
