export module ops;

import std;

export struct Min {
	template<typename T>
	T operator()(T a, T b) const { return std::min(a, b); }
};

export struct Max {
	template<typename T>
	T operator()(T a, T b) const { return std::max(a, b); }
};

export struct Gcd {
	template<typename T>
	T operator()(T a, T b) const { return std::gcd(a, b); }
};

export struct BitAnd {
	template<typename T>
	T operator()(T a, T b) const { return a & b; }
};

export struct BitOr {
	template<typename T>
	T operator()(T a, T b) const { return a | b; }
};