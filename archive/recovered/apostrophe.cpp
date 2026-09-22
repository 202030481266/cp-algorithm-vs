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

// 看着其实并不算很难：
/*
	aabcada -> a a a a
				b c d
	then we can add c at most 6 times, and every time we calculate the sequence we should recover.
	1 times:  (recover 0)
		a a a a
		 b c d c
	2/3 times: (recover 1)
		a a a d a
		 b c c c
	4/5 times: (recover 2)
		Here is a wrong case:
		a a c a d a
		 b c c c c 
		Because the c will make conflict, we have to treat it as another new add c
		a a b a d a
		 c c c c c c
	6 times:
		When the total times of c comes to 7, the status is stable
		a a b a d a
	   c c c c c c c

	通过上面的一个操作，本质上可以归结为两个操作：
	1. 反向传播
	比如添加4个c，aabcada + cccc，那么我们前面的匹配过程就是：
	ada + ccc，但是又碰到了一个c，还需往前配两个不是c，所以是 ab，因此我们把 abada 放到上面，下面放 5 个 c
		a a b a d a
		 c c c c c
	2. 前向传播
	对于饱和状态，没有办法通过反向传播容纳，比如：
		a a b a d a
	   c c c c c c c
	这个时候需要添加 更多的 c，只能是去贪心匹配后面的不等于c的过程。比如 aabcadacccc + ccceccefdd
	那么后面的匹配就是 ccc + e -> cc, cc + cc -> cccc, cccc + efdd -> matched
		a a b a d a e e f d d
	   c c c c c c c c c c c c

	这里涉及到操作是线性的，那么有没有可以预知操作之后所有的结果呢？因为可以看到这里的操作的本质上是相消，出现次数是很关键的。
	所以这里只需要将其分块，每一个块保证其中最大的出现字符等于其他字符出现之和，然后讲这些块拼接即可。
	由于拼接的时候我们总是可以二选一，所以总是可以找到方案将其拼起来。

	但是一个最大的问题是如何分块才是对的？比如 b3 c3 a4，从左到右和从右到左都是不对的
*/

void solve() {
	int n;
	cin >> n;
	vi arr(n);
	for (int& v : arr) cin >> v;
	int cnt = 0, cur = -1;
	vi pos;
	vp ed;
	for (int i = 0; i < n; ++i) {
		if (cur == -1) {
			cur = arr[i];
			cnt = 1;
		}
		else if (arr[i] == cur) ++cnt;
		else {
			--cnt;
			if (cnt == 0) {
				pos.push_back(i);
				cur = -1;
			}
		}
	}
	if (cnt > 1) {
		cout << "NO" << '\n';
		return;
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