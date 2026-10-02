#include <iostream>
#include <vector>
#include "geometry/rectangle_union.hpp"

int main() {
    // 两个 2x2 正方形部分重叠，另有一个独立的 1x2 矩形。
    std::vector<cp::Rectangle> rects{{0, 0, 2, 2}, {1, 1, 3, 3}, {5, 5, 6, 7}};
    std::cout << "area=" << cp::rectangle_union_area(rects) << '\n';
    std::cout << "perimeter=" << cp::rectangle_union_perimeter(rects) << '\n';
}
