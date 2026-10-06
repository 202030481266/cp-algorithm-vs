#pragma once
// 使用说明：cp-stl/docs/usage/util/random.md
// 完整示例：cp-stl/examples/util/random.cpp
#include <algorithm>
#include <cassert>
#include <chrono>
#include <cstddef>
#include <cstdint>
#include <initializer_list>
#include <numeric>
#include <random>
#include <set>
#include <string>
#include <utility>
#include <vector>

namespace cp {
// 对拍数据生成器。对拍时传固定 seed，失败用同一 seed 重现；正常使用默认随机种子。
// 只用 mt19937_64 的原始输出（标准规定了它的序列），不用标准库的分布和 std::shuffle，
// 所以同一 seed、同一调用顺序在 MSVC、GCC 和不同电脑上生成完全相同的数据。
// 数值范围 [lo,hi] 两端都包含；树和图的点编号为 0..n-1，输出 1-based 时自行加 1。
struct Random {
    std::mt19937_64 engine;
    explicit Random(std::uint64_t seed = std::chrono::steady_clock::now().time_since_epoch().count())
        : engine(seed) {}

    // ---------- 单个数 ----------
    long long integer(long long lo, long long hi) {
        assert(lo <= hi);
        // 在无符号下相减，hi - lo 超出 long long 时也不会溢出。
        return static_cast<long long>(std::uint64_t(lo) + offset(std::uint64_t(hi) - std::uint64_t(lo)));
    }
    // [lo,hi) 内的均匀实数。
    double real(double lo, double hi) { return lo + (hi - lo) * double(engine() >> 11) * 0x1p-53; }
    // 以概率 p 返回 true。
    bool chance(double p) { return real(0, 1) < p; }
    // 等概率返回一个元素，例如 choice(a)、choice(s)、choice({1, 2, 100})。
    template<class Container> typename Container::value_type choice(const Container& a) {
        assert(!a.empty());
        return a[std::size_t(offset(a.size() - 1))];
    }
    template<class T> T choice(std::initializer_list<T> values) { return choice(std::vector<T>(values)); }

    // ---------- 序列 ----------
    std::vector<long long> array(int n, long long lo, long long hi) {
        std::vector<long long> a(n);
        for (auto& x : a) x = integer(lo, hi);
        return a;
    }
    // n 个互不相同的数，顺序随机；要求 n <= hi - lo + 1。O(n log n)，与值域大小无关。
    std::vector<long long> distinct_array(int n, long long lo, long long hi) {
        assert(n >= 0);
        if (n == 0) return {};
        assert(lo <= hi && std::uint64_t(n - 1) <= std::uint64_t(hi) - std::uint64_t(lo));
        // Floyd 抽样：j 依次取值域中最后 n 个偏移，每轮恰好加入一个新数，所有 n 元子集等概率。
        const std::uint64_t range = std::uint64_t(hi) - std::uint64_t(lo);
        std::set<std::uint64_t> chosen;
        for (int i = n - 1; i >= 0; --i) {
            const std::uint64_t j = range - std::uint64_t(i);
            if (!chosen.insert(offset(j)).second) chosen.insert(j);
        }
        std::vector<long long> a;
        a.reserve(n);
        for (std::uint64_t x : chosen) a.push_back(static_cast<long long>(std::uint64_t(lo) + x));
        shuffle(a);
        return a;
    }
    // first, first+1, ..., first+n-1 的随机排列；1-based 排列写 permutation(n, 1)。
    std::vector<int> permutation(int n, int first = 0) {
        assert(n >= 0);
        std::vector<int> p(n);
        std::iota(p.begin(), p.end(), first);
        shuffle(p);
        return p;
    }
    // 把 total 拆成 parts 个有序的数，每个至少 minimum，所有拆法等概率。
    // 例：T 组数据且 n 的总和为 N，写 partition(N, T, 1)。
    std::vector<long long> partition(long long total, int parts, long long minimum = 0) {
        assert(parts >= 1 && total >= minimum * parts);
        // 隔板法：在 free + parts - 1 个位置中选 parts - 1 个放隔板，隔板之间的空位数就是各部分多出的量。
        const long long free = total - minimum * parts, slots = free + parts - 1;
        auto cuts = distinct_array(parts - 1, 1, slots);
        cuts.push_back(0);
        cuts.push_back(slots + 1);
        std::sort(cuts.begin(), cuts.end());
        std::vector<long long> a(parts);
        for (int i = 0; i < parts; ++i) a[i] = minimum + cuts[i + 1] - cuts[i] - 1;
        return a;
    }
    // 满足 lo <= l <= r <= hi 的随机区间 {l, r}，两端都包含。
    std::pair<long long, long long> interval(long long lo, long long hi) {
        long long l = integer(lo, hi), r = integer(lo, hi);
        if (l > r) std::swap(l, r);
        return {l, r};
    }
    // 字符在 [lo,hi] 内的随机串，例如 string(n, 'a', 'c')。
    std::string string(int n, char lo = 'a', char hi = 'z') {
        std::string s(n, lo);
        for (char& c : s) c = static_cast<char>(integer(lo, hi));
        return s;
    }
    // 字符取自 alphabet 的随机串，例如 string(n, "01")、string(n, ".#")。
    std::string string(int n, const std::string& alphabet) {
        std::string s(n, ' ');
        for (char& c : s) c = choice(alphabet);
        return s;
    }
    // 原地打乱 vector、string 等可随机访问的容器，所有排列等概率，O(n)。
    template<class Container> void shuffle(Container& a) {
        // Fisher-Yates：从后往前，把位置 i 与 [0,i] 中随机一个位置交换。
        for (std::ptrdiff_t i = std::ptrdiff_t(a.size()) - 1; i > 0; --i)
            std::iter_swap(a.begin() + i, a.begin() + std::ptrdiff_t(offset(std::uint64_t(i))));
    }

    // ---------- 树与图：返回边表 {u, v}，边的顺序和端点先后都已打乱 ----------
    // 随机有根树的父亲数组：p[0] = -1，其余 p[i] < i。
    // 在完全图上随机游走，每到达一个新点就保留来时的边（Aldous-Broder），得到的树在 n^(n-2) 棵有标号树中等概率；
    // 再按首次到达的顺序重新编号，所以父亲编号总比自己小。树形与 tree(n) 同分布，深度约 √n，期望 O(n log n)。
    std::vector<int> parents(int n) {
        assert(n >= 1);
        std::vector<int> id(n, -1), parent(n, -1);
        int current = int(offset(n - 1)), found = 1;
        id[current] = 0;
        while (found < n) {
            const int next = int(offset(n - 1));
            if (id[next] < 0) {
                id[next] = found;
                parent[found++] = id[current];
            }
            current = next;
        }
        return parent;
    }
    // 等概率的 n 点有标号无根树，n - 1 条边。
    std::vector<std::pair<int, int>> tree(int n) {
        const auto parent = parents(n);
        const auto label = permutation(n);
        std::vector<std::pair<int, int>> edges;
        for (int v = 1; v < n; ++v) edges.emplace_back(label[parent[v]], label[v]);
        scramble(edges);
        return edges;
    }
    // 链：随机顺序的 n 个点首尾相连，直径为 n - 1。
    std::vector<std::pair<int, int>> path_tree(int n) {
        const auto order = permutation(n);
        std::vector<std::pair<int, int>> edges;
        for (int i = 1; i < n; ++i) edges.emplace_back(order[i - 1], order[i]);
        scramble(edges);
        return edges;
    }
    // 菊花：随机选一个中心连向其余所有点。
    std::vector<std::pair<int, int>> star_tree(int n) {
        assert(n >= 1);
        const int center = int(offset(n - 1));
        std::vector<std::pair<int, int>> edges;
        for (int v = 0; v < n; ++v) if (v != center) edges.emplace_back(center, v);
        scramble(edges);
        return edges;
    }
    // m 条边的随机简单无向图（无自环、无重边）；connected 为真时先放一棵随机树，要求 m >= n - 1。
    // 稀疏时随机抽边并去重；m 超过可能边数的一半时改为枚举全部点对再抽，两种情况都是期望 O(m log m)。
    std::vector<std::pair<int, int>> graph(int n, int m, bool connected = false) {
        const long long total = 1LL * n * (n - 1) / 2;
        assert(n >= 1 && 0 <= m && m <= total && (!connected || m >= n - 1));
        auto key = [](int u, int v) { return std::make_pair(std::min(u, v), std::max(u, v)); };
        std::vector<std::pair<int, int>> edges;
        if (connected) edges = tree(n);
        std::set<std::pair<int, int>> used;
        for (auto [u, v] : edges) used.insert(key(u, v));
        if (2LL * m > total) {
            std::vector<std::pair<int, int>> rest;
            for (int u = 0; u < n; ++u)
                for (int v = u + 1; v < n; ++v)
                    if (!used.count({u, v})) rest.emplace_back(u, v);
            shuffle(rest);
            edges.insert(edges.end(), rest.begin(), rest.begin() + (m - int(edges.size())));
        } else {
            while (int(edges.size()) < m) {
                const int u = int(offset(n - 1)), v = int(offset(n - 1));
                if (u != v && used.insert(key(u, v)).second) edges.emplace_back(u, v);
            }
        }
        scramble(edges);
        return edges;
    }

private:
    // [0,range] 内的均匀整数。拒绝最小的 2^64 mod (range+1) 个原始值，剩下的个数是 range+1 的倍数，取模后无偏。
    std::uint64_t offset(std::uint64_t range) {
        if (range == UINT64_MAX) return engine();
        const std::uint64_t bound = range + 1, rejected = (0 - bound) % bound;
        std::uint64_t x = engine();
        while (x < rejected) x = engine();
        return x % bound;
    }
    // 打乱边的顺序和端点先后，避免解法依赖生成方式（例如总是父亲在前、编号小的在前）。
    void scramble(std::vector<std::pair<int, int>>& edges) {
        for (auto& [u, v] : edges) if (offset(1)) std::swap(u, v);
        shuffle(edges);
    }
};
} // namespace cp
