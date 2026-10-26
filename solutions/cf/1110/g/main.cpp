// C++20+. With local headers, export via: python tools/cp.py export
// CP-STL index: cp-stl/docs/usage/README.md
// Uncomment only what this problem needs; VS and tools/cp.py know these paths.
// #include "data_structures/fenwick.hpp"
// #include "graph/shortest_path.hpp"
// #include "math/combinatorics.hpp"
#include <algorithm>
#include <array>
#include <bit>
#include <bitset>
#include <cassert>
#include <chrono>
#include <cmath>
#include <complex>
#include <concepts>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <deque>
#include <functional>
#include <iostream>
#include <iterator>
#include <limits>
#include <list>
#include <map>
#include <memory>
#include <numeric>
#include <optional>
#include <queue>
#include <random>
#include <ranges>
#include <set>
#include <span>
#include <sstream>
#include <stack>
#include <string>
#include <string_view>
#include <tuple>
#include <type_traits>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <variant>
#include <vector>

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
using minpq = priority_queue<T, vec<T>, greater<T>>;

// Pass ordinary container variables to all/rall; each expands to two iterators.
#define all(x) std::begin(x), std::end(x)
#define rall(x) std::rbegin(x), std::rend(x)
#define sz(x) static_cast<int>(std::size(x))
#define pb push_back
#define eb emplace_back
#define fi first
#define se second
// Integer index loops over [l, r); both bounds are evaluated once.
#define rep(i, l, r) for (int i = static_cast<int>(l), i##_end = static_cast<int>(r); i < i##_end; ++i)
#define per(i, l, r) for (int i = static_cast<int>(r), i##_begin = static_cast<int>(l); i-- > i##_begin; )

template<typename T, typename U>
bool chmin(T& a, const U& b) {
    if (b < a) { a = b; return true; }
    return false;
}
template<typename T, typename U>
bool chmax(T& a, const U& b) {
    if (a < b) { a = b; return true; }
    return false;
}

// Answer output: print(a); print(a, ','); prints a flat range without brackets.
template<ranges::input_range R>
    requires requires(ranges::range_reference_t<R> x) { cout << x; }
void print(R&& a, char separator = ' ', char ending = '\n') {
    bool first = true;
    for (auto&& x : a) {
        if (!first) cout << separator;
        cout << x;
        first = false;
    }
    cout << ending;
}

namespace cp_debug {
template<typename T>
struct IsOptional : false_type {};
template<typename T>
struct IsOptional<optional<T>> : true_type {};
template<typename T>
struct IsVariant : false_type {};
template<typename... T>
struct IsVariant<variant<T...>> : true_type {};

template<typename T>
concept PairLike = requires(T x) { x.first; x.second; };
template<typename T>
concept StackLike = requires(T x) { x.top(); x.pop(); x.empty(); };
template<typename T>
concept QueueLike = requires(T x) { x.front(); x.pop(); x.empty(); };

inline void text(string_view value, char quote = '"') {
    cerr << quote;
    for (char c : value) {
        switch (c) {
        case '\\': cerr << "\\\\"; break;
        case '\n': cerr << "\\n"; break;
        case '\r': cerr << "\\r"; break;
        case '\t': cerr << "\\t"; break;
        case '\0': cerr << "\\0"; break;
        default:
            if (c == quote) cerr << '\\';
            cerr << c;
        }
    }
    cerr << quote;
}

// One recursive entry point avoids overload-order problems with nested types.
template<typename T>
void write(T&& value) {
    using U = remove_cvref_t<T>;
    if constexpr (is_same_v<U, bool> || is_same_v<U, vector<bool>::reference>) {
        cerr << (value ? "true" : "false");
    } else if constexpr (is_same_v<U, char>) {
        text(string_view(&value, 1), '\'');
    } else if constexpr (is_array_v<U> && is_same_v<remove_cv_t<remove_extent_t<U>>, char>) {
        const auto last = find(begin(value), end(value), '\0');
        text(string_view(value, static_cast<size_t>(last - begin(value))));
    } else if constexpr (is_same_v<U, string> || is_same_v<U, string_view>) {
        text(value);
    } else if constexpr (is_same_v<U, char*> || is_same_v<U, const char*>) {
        if (value) text(value);
        else cerr << "nullptr";
    } else if constexpr (IsOptional<U>::value) {
        if (value) { cerr << "Some("; write(*value); cerr << ')'; }
        else cerr << "None";
    } else if constexpr (IsVariant<U>::value) {
        if (value.valueless_by_exception()) cerr << "Variant(valueless)";
        else {
            cerr << "Variant(";
            visit([](auto&& x) { write(std::forward<decltype(x)>(x)); }, value);
            cerr << ')';
        }
    } else if constexpr (is_same_v<U, monostate>) {
        cerr << "monostate";
    } else if constexpr (PairLike<T>) {
        cerr << '{'; write(value.first); cerr << ", "; write(value.second); cerr << '}';
    } else if constexpr (StackLike<U> || QueueLike<U>) {
        // Show pop order using a copy; the original adaptor is unchanged.
        auto copy = value;
        cerr << '{';
        bool first = true;
        while (!copy.empty()) {
            if (!first) cerr << ", ";
            if constexpr (StackLike<U>) write(copy.top());
            else write(copy.front());
            copy.pop();
            first = false;
        }
        cerr << '}';
    } else if constexpr (ranges::input_range<T>) {
        cerr << '{';
        bool first = true;
        for (auto&& x : value) {
            if (!first) cerr << ", ";
            write(std::forward<decltype(x)>(x));
            first = false;
        }
        cerr << '}';
    } else if constexpr (requires { typename tuple_size<U>::type; }) {
        cerr << '(';
        apply([](auto&&... x) {
            [[maybe_unused]] size_t index = 0;
            ((cerr << (index++ ? ", " : ""), write(std::forward<decltype(x)>(x))), ...);
        }, value);
        cerr << ')';
    } else if constexpr (requires { cerr << value; }) {
        cerr << value;  // Numbers, bitset, complex, and custom operator<<.
    } else {
        static_assert(is_void_v<U>, "debug: provide operator<<(ostream&, const T&) for this type");
    }
}

inline void header(const char* file, int line, const char* names) {
    string_view filename(file);
    const auto slash = filename.find_last_of("/\\");
    if (slash != string_view::npos) filename.remove_prefix(slash + 1);
    cerr << '[' << filename << ':' << line << "] [" << names << "] = [";
}

template<typename... T>
void values(T&&... value) {
    [[maybe_unused]] size_t index = 0;
    ((cerr << (index++ ? ", " : ""), write(std::forward<T>(value))), ...);
}

template<ranges::input_range R>
void matrix(const char* file, int line, const char* name, R&& rows) {
    header(file, line, name);
    cerr << '\n';
    for (auto&& row : rows) {
        cerr << "  ";
        write(std::forward<decltype(row)>(row));
        cerr << '\n';
    }
    cerr << "]\n";
}
}  // namespace cp_debug

// Uncomment before the macro definitions below when needed.
// #define MULTI_CASE_INPUT
// #define NO_DEBUG
// LOCAL_FILE is set in VS project properties for local file debugging.

#if !defined(ONLINE_JUDGE) && !defined(NO_DEBUG)
#define debug(...) do { \
    cp_debug::header(__FILE__, __LINE__, "" #__VA_ARGS__); \
    cp_debug::values(__VA_ARGS__); \
    std::cerr << "]\n"; \
} while (false)
#define debug_matrix(...) do { cp_debug::matrix(__FILE__, __LINE__, "" #__VA_ARGS__, (__VA_ARGS__)); } while (false)
#else
#define debug(...) ((void)0)
#define debug_matrix(...) ((void)0)
#endif

inline constexpr int MAXN = 300005;
inline constexpr int INF = 0x3f3f3f3f;
inline constexpr ll LLINF = 0x3f3f3f3f3f3f3f3fLL;
inline constexpr int MOD_197 = 1'000'000'007;
inline constexpr int MOD_998 = 998'244'353;
inline constexpr int MAXB = 30;

#define MULTI_CASE_INPUT

void solve() {
    int n;
    cin >> n;
    vp edge(n - 1);
    for (auto& [u, v] : edge) cin >> u >> v;
    string s;
    cin >> s;
    int cnt = 0;
    for (char c : s) {
        if (c == 'W') {
            ++cnt;
            if (cnt > 2) {
                cout << "White" << '\n';
                return;
            }
        }
    }
    vi b(n + 1);
    vii g(n + 1);
    for (auto [u, v] : edge) {
        ++b[u];
        ++b[v];
        g[u].pb(v);
        g[v].pb(u);
    }
    vi c(n + 1), d(n + 1);
    rep(i, 1, n + 1) {
        int x = 0, y = 0;
        for (int v : g[i]) {
            if (b[v] > x) {
                y = x;
                x = b[v];
            }
            else if (b[v] > y) y = b[v];
        }
        c[i] = x;
        d[i] = y;
    }
    int mx = *max_element(all(b));
    if (mx > 3) {
        cout << "White" << '\n';
        return;
    }
    vi dep(n + 1);
    auto dfs = [&](auto&& self, int u, int fa) -> void {
        dep[u] = dep[fa] ^ 1;
        for (int v : g[u]) {
            if (v != fa) {
                self(self, v, u);
            }
        }
    };
    dfs(dfs, 1, 0);
    if (cnt == 0) {
        if (mx == 3) {
            vi tmp;
            rep(i, 1, n + 1) {
                if (b[i] == 3) {
                    tmp.pb(i);
                    if (d[i] > 1) {
                        cout << "White" << '\n';
                        return;
                    }
                }
            }
            assert(sz(tmp) <= 2);
            if (sz(tmp) == 2) {
                if (dep[tmp[0]] == dep[tmp[1]]) {
                    cout << "White" << '\n';
                    return;
                }
            }
            cout << "Draw" << '\n';
        }
        else {
            cout << "Draw" << '\n';
        }
        return;
    }
    if (cnt == 1) {
        int x = 0;
        rep(i, 0, n) {
            if (s[i] == 'W') {
                x = i + 1;
                break;
            }
        }
        if (b[x] > 1) {
            if (n > 3) {
                cout << "White" << '\n';
            }
            else cout << "Draw" << '\n';
        }
        else { // b[x] == 1
            rep(i, 1, n + 1) {
                if (b[i] == 3) {
                    if (dep[x] ^ dep[i]) {
                        cout << "White" << '\n';
                        return;
                    }
                    else {
                        if (d[i] > 1) {
                            cout << "White" << '\n';
                            return;
                        }
                    }
                }
            }
            cout << "Draw" << '\n';
        }
    }
    else {
        int x = 0, y = 0;
        rep(i, 0, n) {
            if (s[i] == 'W') {
                if (!x) x = i + 1;
                else if (!y) y = i + 1;
                else break;
            }
        }
        if (b[x] > 1 || b[y] > 1) {
            if (n >= 3) cout << "White" << '\n';
            else cout << "Draw" << '\n';
        }
        else {
            if (dep[x] == dep[y] || mx == 3) {
                cout << "White" << '\n';
            }
            else {
                cout << "Draw" << '\n';
            }
        }
    }
}

int main() {
#ifdef LOCAL_FILE
    if (!freopen("data/input.txt", "r", stdin)) {
        perror("data/input.txt");
        return 1;
    }
    if (!freopen("data/output.txt", "w", stdout)) {
        perror("data/output.txt");
        return 1;
    }
#endif
    ios_base::sync_with_stdio(false);
    cin.tie(nullptr);

    int t = 1;
#ifdef MULTI_CASE_INPUT
    if (!(cin >> t) || t < 0) return 0;
#endif
    while (t--) solve();
    return 0;
}
