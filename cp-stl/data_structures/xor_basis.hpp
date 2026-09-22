#pragma once
// 使用说明：cp-stl/docs/usage/data_structures/xor_basis.md
// 完整示例：cp-stl/examples/data_structures/xor_basis.cpp
#include <algorithm>
#include <array>
#include <cstdint>
#include <optional>
#include <vector>

namespace cp {
// 完整 uint64_t 值域。允许空子集，0 永远可表示；kth(k) 从 0 开始，去重。
// 插入/查询 O(64)；秩变化后的首次 kth O(64^2)，随后每次 O(64)。
// kth 按需缓存消元结果，无解返回 nullopt，避免把 UINT64_MAX 当错误码。
class XorBasis {
    std::array<std::uint64_t, 64> basis_{};
    int rank_ = 0;
    mutable std::vector<std::uint64_t> ordered_; // 只在首次 kth 时分配；长度同时标记缓存的秩。
public:
    bool insert(std::uint64_t x) {
        for (int i = 63; i >= 0; --i) {
            if (!(x >> i & 1)) continue;
            if (!basis_[i]) { basis_[i] = x; ++rank_; return true; }
            x ^= basis_[i];
        }
        return false;
    }
    bool contains(std::uint64_t x) const {
        for (int i = 63; i >= 0; --i) if (x >> i & 1) x ^= basis_[i];
        return x == 0;
    }
    std::uint64_t max_xor(std::uint64_t initial = 0) const {
        for (int i = 63; i >= 0; --i) initial = std::max(initial, initial ^ basis_[i]);
        return initial;
    }
    std::optional<std::uint64_t> kth(std::uint64_t k) const {
        if (rank_ < 64 && k >= (std::uint64_t{1} << rank_)) return std::nullopt;
        if (int(ordered_.size()) != rank_) {
            auto reduced = basis_;
            ordered_.clear();
            ordered_.reserve(rank_);
            for (int i = 0; i < 64; ++i) {
                if (!reduced[i]) continue;
                for (int j = 0; j < i; ++j) if (reduced[i] >> j & 1) reduced[i] ^= reduced[j];
                ordered_.push_back(reduced[i]);
            }
        }
        std::uint64_t value = 0;
        for (int i = 0; i < rank_; ++i) if (k >> i & 1) value ^= ordered_[i];
        return value;
    }
    int rank() const { return rank_; }
};
} // namespace cp
