#include <iostream>
#include <vector>
#include "data_structures/lazy_segment_tree.hpp"

// 自定义 Info/Tag：01 序列区间取反，查询 1 的个数（必须写在函数外）。
struct Flip {
    bool on = false;
    void apply(const Flip& newer) { on ^= newer.on; }
};
struct Ones {
    int ones = 0, length = 0;
    void apply(const Flip& t) { if (t.on) ones = length - ones; }
    friend Ones operator+(const Ones& a, const Ones& b) { return {a.ones + b.ones, a.length + b.length}; }
};

int main() {
    cp::RangeAddSum seg(std::vector<long long>{1, 2, 3, 4});
    std::cout << "before=" << seg.sum(0, 4) << '\n';
    seg.add(1, 4, 5); // 1,7,8,9
    std::cout << "sum(1,3)=" << seg.sum(1, 3) << '\n';
    seg.add(0, 2, -2); // -1,5,8,9
    std::cout << "after=" << seg.sum(0, 4) << '\n';
    seg.add(2, 2, 100);
    std::cout << "empty=" << seg.sum(2, 2) << '\n';

    // 预置的区间赋值 + 区间加，查询和、最小值、最大值。
    std::vector<long long> a{5, 1, 4, 2, 3};
    cp::LazySegmentTree<cp::SumMinMax, cp::AssignAdd> tree(std::vector<cp::SumMinMax>(a.begin(), a.end()));
    tree.apply(1, 4, cp::AssignAdd::add(10));   // 5,11,14,12,3
    tree.apply(0, 2, cp::AssignAdd::assign(7)); // 7,7,14,12,3
    auto info = tree.prod(0, 5);
    std::cout << "sum=" << info.sum << " min=" << info.minimum << " max=" << info.maximum << '\n';
    int r = tree.max_right(0, [](const cp::SumMinMax& s) { return s.maximum < 13; });
    std::cout << "prefix_max<13 r=" << r << '\n';

    cp::LazySegmentTree<Ones, Flip> lights(6, Ones{0, 1}); // 6 盏灯全灭
    lights.apply(1, 4, Flip{true});                         // 011100
    lights.apply(2, 6, Flip{true});                         // 010011
    std::cout << "on=" << lights.prod(0, 6).ones << " on[0,3)=" << lights.prod(0, 3).ones << '\n';
}
