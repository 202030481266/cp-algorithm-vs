#pragma once
// 使用说明：cp-stl/docs/usage/geometry/rectangle_union.md
// 完整示例：cp-stl/examples/geometry/rectangle_union.cpp
#include <algorithm>
#include <cassert>
#include <vector>

namespace cp {
// 轴平行矩形，左下角 (x1,y1)、右上角 (x2,y2)，要求 x1 <= x2、y1 <= y2；退化矩形不影响面积。
struct Rectangle { long long x1, y1, x2, y2; };

namespace rectangle_detail {
// 扫描线用的线段树：维护离散化后的 y 区间被覆盖的总长度。覆盖次数只增减成对出现，不需要下传标记。
class CoverTree {
    int n_;
    const std::vector<long long>& ys_;
    std::vector<int> cover_;
    std::vector<long long> length_;
    void pull(int p, int l, int r) {
        if (cover_[p]) length_[p] = ys_[r] - ys_[l];
        else length_[p] = r - l == 1 ? 0 : length_[2 * p] + length_[2 * p + 1];
    }
    void update(int p, int l, int r, int ql, int qr, int delta) {
        if (ql <= l && r <= qr) { cover_[p] += delta; pull(p, l, r); return; }
        int m = (l + r) / 2;
        if (ql < m) update(2 * p, l, m, ql, qr, delta);
        if (m < qr) update(2 * p + 1, m, r, ql, qr, delta);
        pull(p, l, r);
    }
public:
    explicit CoverTree(const std::vector<long long>& ys)
        : n_(std::max(1, int(ys.size()) - 1)), ys_(ys), cover_(4 * n_), length_(4 * n_) {}
    // 基本区间 [l,r)（对应 ys[l]..ys[r]）的覆盖次数加 delta。
    void update(int l, int r, int delta) { if (l < r) update(1, 0, n_, l, r, delta); }
    long long length() const { return length_[1]; }
};
struct Event { long long x, y1, y2; int delta; };

// 沿 x 扫描：每个事件后回调 visit(覆盖长度变化量的绝对值, 当前覆盖长度, 下一个事件的 x - 当前 x)。
template<class Visit>
void sweep(std::vector<Event> events, Visit visit) {
    std::vector<long long> ys;
    for (const auto& e : events) { ys.push_back(e.y1); ys.push_back(e.y2); }
    std::sort(ys.begin(), ys.end());
    ys.erase(std::unique(ys.begin(), ys.end()), ys.end());
    // 同一 x 上先加入后删除，避免相邻矩形的公共边被重复计算。
    std::sort(events.begin(), events.end(), [](const Event& a, const Event& b) {
        return a.x != b.x ? a.x < b.x : a.delta > b.delta;
    });
    CoverTree tree(ys);
    for (std::size_t i = 0; i < events.size(); ++i) {
        const Event& e = events[i];
        long long before = tree.length();
        int l = int(std::lower_bound(ys.begin(), ys.end(), e.y1) - ys.begin());
        int r = int(std::lower_bound(ys.begin(), ys.end(), e.y2) - ys.begin());
        tree.update(l, r, e.delta);
        long long after = tree.length();
        long long gap = i + 1 < events.size() ? events[i + 1].x - e.x : 0;
        visit(after > before ? after - before : before - after, after, gap);
    }
}
} // namespace rectangle_detail

// 矩形面积并：扫描线 + 线段树，O(n log n)。结果必须在 long long 范围内。
inline long long rectangle_union_area(const std::vector<Rectangle>& rects) {
    std::vector<rectangle_detail::Event> events;
    for (const auto& r : rects) {
        assert(r.x1 <= r.x2 && r.y1 <= r.y2);
        if (r.x1 == r.x2 || r.y1 == r.y2) continue;
        events.push_back({r.x1, r.y1, r.y2, 1});
        events.push_back({r.x2, r.y1, r.y2, -1});
    }
    long long area = 0;
    rectangle_detail::sweep(events, [&](long long, long long covered, long long gap) { area += covered * gap; });
    return area;
}

// 矩形周长并（并集轮廓的总长度）：分别沿 x、y 扫描，累加每个事件前后覆盖长度的变化量。O(n log n)。
inline long long rectangle_union_perimeter(const std::vector<Rectangle>& rects) {
    std::vector<rectangle_detail::Event> along_x, along_y;
    for (const auto& r : rects) {
        assert(r.x1 <= r.x2 && r.y1 <= r.y2);
        if (r.x1 == r.x2 || r.y1 == r.y2) continue;
        along_x.push_back({r.x1, r.y1, r.y2, 1});
        along_x.push_back({r.x2, r.y1, r.y2, -1});
        along_y.push_back({r.y1, r.x1, r.x2, 1});
        along_y.push_back({r.y2, r.x1, r.x2, -1});
    }
    long long perimeter = 0;
    auto add_change = [&](long long change, long long, long long) { perimeter += change; };
    rectangle_detail::sweep(along_x, add_change);
    rectangle_detail::sweep(along_y, add_change);
    return perimeter;
}
} // namespace cp
