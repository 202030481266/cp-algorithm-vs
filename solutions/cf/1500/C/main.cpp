#ifdef _MSC_VER
#include "pch.h"
#else
#include <bits/stdc++.h>
#endif
#include <format>
#include <concepts>
#include <ranges>
#include <numeric>
#include <thread>
#include <cstdint>

using namespace std;
using ll = long long;
using ull = unsigned long long;
using uint = unsigned int;
using ld = long double;

template<typename T, typename U = T>
using pr = pair<T, U>;
template<typename T>
using vec = vector<T>;
using pii = pr<int>;
using pll = pr<ll>;
using vi = vec<int>;
using vl = vec<ll>;
using vii = vec<vi>;
using vll = vec<vl>;
using vp = vec<pii>;
using vpp = vec<vp>;

template<typename T>
concept Printable = requires(T x) { cerr << x; };
template<typename T>
concept PairLike = requires(T x) { x.first; x.second; };
template<typename T>
concept Iterable = ranges::range<T> && !PairLike<T> && !is_convertible_v<T, string_view>;

template<typename T> void __print(const T&);
void __print(char x) { cerr << '\'' << x << '\''; }
void __print(bool x) { cerr << (x ? "true" : "false"); }
void __print(const char* x) { cerr << '"' << x << '"'; }
void __print(const string& x) { cerr << '"' << x << '"'; }
void __print(string_view x) { cerr << '"' << x << '"'; }

template<Printable T>
	requires (!PairLike<T> && !Iterable<T>
&& !is_same_v<T, char>
&& !is_same_v<T, bool>
&& !is_same_v<T, string>
&& !is_same_v<T, string_view>
&& !is_convertible_v<T, const char*>)
void __print(const T& x) { cerr << x; }

template<PairLike T>
void __print(const T& x) {
	cerr << '{';
	__print(x.first);
	cerr << ", ";
	__print(x.second);
	cerr << '}';
}

template<typename... Ts>
void __print(const tuple<Ts...>& t) {
	cerr << '(';
	apply([&]<typename... Args>(const Args&... args) {
		size_t n = 0;
		((cerr << (n++ ? ", " : ""), __print(args)), ...);
	}, t);
	cerr << ')';
}

template<Iterable T>
void __print(const T& x) {
	cerr << '{';
	bool first = true;
	for (const auto& i : x) {
		if (!first) cerr << ", ";
		__print(i);
		first = false;
	}
	cerr << '}';
}

template<typename T>
void __print(const optional<T>& x) {
	if (x) { cerr << "Some("; __print(*x); cerr << ')'; }
	else     cerr << "None";
}

void _print() { cerr << "]\n"; }
template<typename T, typename... V>
void _print(T&& t, V&&... v) {
	__print(t);
	if constexpr (sizeof...(v) > 0) cerr << ", ";
	_print(forward<V>(v)...);
}

#ifndef ONLINE_JUDGE
#   define debug(...) \
        cerr << std::format("[{}:{}] [{}] = [", __FILE__, __LINE__, #__VA_ARGS__); \
        _print(__VA_ARGS__)
#else
#   define debug(...)
#endif

inline constexpr int  MAXN = 300005;
inline constexpr int  INF = 0x3f3f3f3f;
inline constexpr ll   LLINF = 0x3f3f3f3f3f3f3f3fLL;
inline constexpr int  MOD_197 = 1'000'000'007;
inline constexpr int  MOD_998 = 998'244'353;
inline constexpr int  MAXB = 30;


//#define DEBUG
//#define MULTI_CASE_INPUT

// 可以证明最后的答案就是一个不同列构成的顺序执行的排序方法
// 反过来想，B肯定符合某一个关键列集合
// 面对这么多的升序列，直接选择第一个会保证后面无解吗？其实不会，因为 c' 是后手执行的。
// 按照这种方法一定可以保证后面是有解的，如果c'不在最终的解里面，那么先执行后面的解再执行c'，结果不变。

void solve() {
	int n, m;
	cin >> n >> m;
	vii A(n, vi(m));
	vii B(n, vi(m));
	for (int i = 0; i < n; ++i) {
		for (int j = 0; j < m; ++j) cin >> A[i][j];
	}
	for (int i = 0; i < n; ++i) {
		for (int j = 0; j < m; ++j) cin >> B[i][j];
	}
	bitset<1503> bs;
	vp a;
	a.emplace_back(0, n);
	vi ans;
	bool change = true;
	while (change) {
		change = false;
		for (int i = 0; i < m; ++i) {
			if (bs[i]) continue;
			bool asc = true;
			for (auto [l, r] : a) {
				for (int j = l; j + 1 < r; ++j) {
					if (B[j][i] > B[j + 1][i]) {
						asc = false;
						break;
					}
				}
				if (!asc) break;
			}
			if (asc) {
				vp tmp;
				for (auto [l, r] : a) {
					int p = l;
					while (p < r) {
						int j = p + 1;
						while (j < r && B[j][i] == B[p][i]) ++j;
						tmp.emplace_back(p, j);
						p = j;
					}
				}
				a = move(tmp);
				bs.set(i);
				change = true;
				break;
			}
		}
	}

}


int main() {
	ios_base::sync_with_stdio(false);
	cin.tie(nullptr);
	int t = 1;
#ifdef MULTI_CASE_INPUT
	cin >> t;
#endif
	while (t--) solve();
}