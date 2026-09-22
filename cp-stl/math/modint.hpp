#pragma once
// 使用说明：cp-stl/docs/usage/math/modint.md
// 完整示例：cp-stl/examples/math/modint.cpp
#include <cassert>
#include <istream>
#include <ostream>
#include "number_theory.hpp"

namespace cp {
template<int Mod>
class ModInt {
    static_assert(Mod > 1, "modulus must be greater than one");
    int value_ = 0;
public:
    static constexpr int mod() { return Mod; }
    ModInt(long long x = 0) : value_(int(x % Mod)) { if (value_ < 0) value_ += Mod; }
    int val() const { return value_; }
    ModInt& operator+=(ModInt b) {
        // 无符号加法覆盖 Mod 接近 INT_MAX 时的两个模值之和。
        unsigned sum = static_cast<unsigned>(value_) + static_cast<unsigned>(b.value_);
        if (sum >= static_cast<unsigned>(Mod)) sum -= static_cast<unsigned>(Mod);
        value_ = int(sum);
        return *this;
    }
    ModInt& operator-=(ModInt b) { value_ -= b.value_; if (value_ < 0) value_ += Mod; return *this; }
    ModInt& operator*=(ModInt b) { value_ = int(1LL * value_ * b.value_ % Mod); return *this; }
    ModInt& operator/=(ModInt b) { return *this *= b.inv(); }
    ModInt operator-() const { return ModInt(-value_); }
    ModInt pow(long long exponent) const {
        assert(exponent >= 0);
        ModInt base = *this, result = 1;
        while (exponent) {
            if (exponent & 1) result *= base;
            exponent >>= 1;
            if (exponent) base *= base;
        }
        return result;
    }
    ModInt inv() const {
        auto value = inverse_mod(value_, Mod);
        assert(value.has_value()); // 除数与模数必须互质。
        return ModInt(value.value());
    }
    friend ModInt operator+(ModInt a, ModInt b) { return a += b; }
    friend ModInt operator-(ModInt a, ModInt b) { return a -= b; }
    friend ModInt operator*(ModInt a, ModInt b) { return a *= b; }
    friend ModInt operator/(ModInt a, ModInt b) { return a /= b; }
    friend bool operator==(ModInt a, ModInt b) { return a.value_ == b.value_; }
    friend bool operator!=(ModInt a, ModInt b) { return !(a == b); }
    friend std::ostream& operator<<(std::ostream& out, ModInt a) { return out << a.val(); }
    friend std::istream& operator>>(std::istream& in, ModInt& a) {
        long long value;
        if (in >> value) a = ModInt(value);
        return in;
    }
};
using Mint = ModInt<998244353>;
using Mint1000000007 = ModInt<1000000007>;
} // namespace cp
