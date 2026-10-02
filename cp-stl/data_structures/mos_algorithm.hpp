#pragma once
// 使用说明：cp-stl/docs/usage/data_structures/mos_algorithm.md
// 完整示例：cp-stl/examples/data_structures/mos_algorithm.cpp
#include <algorithm>
#include <array>
#include <cassert>
#include <cmath>
#include <utility>
#include <vector>

namespace cp {
// 莫队算法（Mo's algorithm）：离线回答区间询问 [l,r)，0 <= l <= r <= n。
// 先登记全部询问（构造时传入，或逐个 add_query），再选一种 run：
//   run           普通莫队，端点每移动一步调用一次加入 / 删除，端点总移动 O(n sqrt q)；
//   run_rollback  回滚莫队，只加入不删除、靠快照撤销，用于删除困难的统计（如区间众数）。
// 当前区间到达某个询问时调用 answer(询问编号)，编号就是登记顺序（从 0 开始）。
class MosAlgorithm {
    int n_;
    std::vector<std::pair<int, int>> queries_;
    int block_size() const { return std::max(1, int(n_ / std::sqrt(double(std::max(query_count(), 1))))); }
    // 按 (左端点所在块, 右端点) 排序；alternate 为真时奇数块的右端点倒序，减少右端点来回移动。
    // 排序键预先算好，比在比较函数里做除法快约一倍。
    std::vector<int> sorted_ids(bool alternate) const {
        int q = query_count(), block = block_size();
        std::vector<std::pair<unsigned long long, int>> keyed(q);
        for (int i = 0; i < q; ++i) {
            auto [l, r] = queries_[i];
            unsigned long long b = unsigned(l / block);
            keyed[i] = {b << 32 | unsigned(alternate && (b & 1) ? n_ - r : r), i};
        }
        std::sort(keyed.begin(), keyed.end());
        std::vector<int> ids(q);
        for (int i = 0; i < q; ++i) ids[i] = keyed[i].second;
        return ids;
    }
public:
    explicit MosAlgorithm(int n, const std::vector<std::pair<int, int>>& queries = {}) : n_(n) {
        assert(n >= 0);
        queries_.reserve(queries.size());
        for (auto [l, r] : queries) add_query(l, r);
    }
    // 登记询问 [l,r)，返回询问编号。
    int add_query(int l, int r) {
        assert(0 <= l && l <= r && r <= n_);
        queries_.emplace_back(l, r);
        return int(queries_.size()) - 1;
    }
    int query_count() const { return int(queries_.size()); }
    // 普通莫队。当前区间 [cl,cr) 先扩张再收缩，任何时刻 cl <= cr：
    // add_left(i)/add_right(i) 加入左端/右端的新元素 i，remove_left(i)/remove_right(i) 删除端点元素 i。
    template<class AddLeft, class AddRight, class RemoveLeft, class RemoveRight, class Answer>
    void run(AddLeft add_left, AddRight add_right, RemoveLeft remove_left, RemoveRight remove_right,
             Answer answer) const {
        int cl = 0, cr = 0;
        for (int id : sorted_ids(true)) {
            auto [l, r] = queries_[id];
            while (cl > l) add_left(--cl);
            while (cr < r) add_right(cr++);
            while (cl < l) remove_left(cl++);
            while (cr > r) remove_right(--cr);
            answer(id);
        }
    }
    // 加入、删除与方向无关时的简写：add(i)、remove(i)。
    template<class Add, class Remove, class Answer>
    void run(Add add, Remove remove, Answer answer) const { run(add, add, remove, remove, answer); }
    // 回滚莫队：add_left/add_right 加入元素；snapshot() 返回当前状态，rollback(s) 撤销到该状态。
    // 开始时统计必须为空，结束后恢复为空。同一块内右端点只增不减，左半部分每次从块尾重新加入，
    // 共 O(n sqrt q) 次加入。
    template<class AddLeft, class AddRight, class Snapshot, class Rollback, class Answer>
    void run_rollback(AddLeft add_left, AddRight add_right, Snapshot snapshot, Rollback rollback,
                      Answer answer) const {
        int q = query_count(), block = block_size();
        std::vector<int> ids = sorted_ids(false);
        auto empty = snapshot();
        for (int i = 0; i < q;) {
            int current_block = queries_[ids[i]].first / block;
            int block_end = std::min(n_, (current_block + 1) * block), cr = block_end;
            rollback(empty);
            for (; i < q && queries_[ids[i]].first / block == current_block; ++i) {
                int id = ids[i];
                auto [l, r] = queries_[id];
                if (r <= block_end) { // 短询问在块内：按 r 排序保证它们排在长询问之前，此时统计为空。
                    auto state = snapshot();
                    for (int p = l; p < r; ++p) add_right(p);
                    answer(id);
                    rollback(state);
                    continue;
                }
                while (cr < r) add_right(cr++);
                auto state = snapshot();
                for (int p = block_end - 1; p >= l; --p) add_left(p);
                answer(id);
                rollback(state);
            }
        }
        rollback(empty);
    }
};

// 带修莫队（Mo's algorithm with updates）：按输入顺序登记修改和询问，每个询问看到在它之前登记的全部修改。
// 修改的内容由调用方保存；run 中 apply_update(k, l, r) 在当前区间为 [l,r) 时切换第 k 次修改，
// 同一次修改第二次调用表示撤销（通常写成：位置在 [l,r) 内就先删旧值，交换数组值与修改值，再加新值）。
// 块长约 (n^2 * 修改数 / q)^(1/3)，n、q、修改数同阶时总复杂度约 O(n^(5/3))。
class MosAlgorithmWithUpdates {
    int n_, update_count_ = 0;
    std::vector<std::array<int, 3>> queries_; // {l, r, 已生效的修改数}
public:
    explicit MosAlgorithmWithUpdates(int n) : n_(n) { assert(n >= 0); }
    // 登记一次修改，返回修改编号（从 0 开始）。
    int add_update() { return update_count_++; }
    // 登记询问 [l,r)，返回询问编号（从 0 开始）。
    int add_query(int l, int r) {
        assert(0 <= l && l <= r && r <= n_);
        queries_.push_back({l, r, update_count_});
        return int(queries_.size()) - 1;
    }
    int query_count() const { return int(queries_.size()); }
    int update_count() const { return update_count_; }
    // 当前区间先扩张再收缩，再调整修改；结束前撤销全部修改，调用方的数组和修改列表恢复原样。
    template<class Add, class Remove, class ApplyUpdate, class Answer>
    void run(Add add, Remove remove, ApplyUpdate apply_update, Answer answer) const {
        int q = query_count();
        double cube = double(n_) * n_ * std::max(update_count_, 1) / std::max(q, 1);
        int block = std::max({1, int(std::cbrt(cube)), int(n_ / std::sqrt(double(std::max(q, 1))))});
        std::vector<std::array<int, 4>> keyed(q); // (左端点块, 右端点块, 修改数, 编号)
        for (int i = 0; i < q; ++i) keyed[i] = {queries_[i][0] / block, queries_[i][1] / block, queries_[i][2], i};
        std::sort(keyed.begin(), keyed.end());
        int cl = 0, cr = 0, ct = 0; // 当前区间 [cl,cr)，已生效 ct 次修改。
        for (const auto& key : keyed) {
            int id = key[3];
            auto [l, r, t] = queries_[id];
            while (cl > l) add(--cl);
            while (cr < r) add(cr++);
            while (cl < l) remove(cl++);
            while (cr > r) remove(--cr);
            while (ct < t) apply_update(ct++, cl, cr);
            while (ct > t) apply_update(--ct, cl, cr);
            answer(id);
        }
        while (ct > 0) apply_update(--ct, cl, cr);
    }
};
} // namespace cp
