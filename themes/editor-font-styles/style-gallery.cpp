// Editor Font Styles: every default style in one file. Comments are italic.
// Standard library names are italic, names declared in this file are bold.
#include <algorithm>
#include <chrono>
#include <iostream>
#include <map>
#include <numeric>
#include <string>
#include <vector>

#define REP(i, n) for (int i = 0; i < (n); ++i)  // a macro defined here is a local symbol

using namespace std;
using ll = long long;

namespace gallery {

constexpr int kLimit = 5;

struct Edge {
    int to, weight;
};

// NOTE: count and max are declared in this file, so they are bold here,
// while std::max further down stays italic.
int count = 0;

ll max(ll a, ll b) { return a > b ? a : b; }

vector<int> build(int n) {
    vector<int> values(n);
    iota(values.begin(), values.end(), 1);
    return values;
}

}  // namespace gallery

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    auto values = gallery::build(gallery::kLimit);
    map<string, int> seen;  // TODO: try unordered_map
    REP(i, static_cast<int>(values.size())) seen[to_string(values[i])] += i;

    vector<gallery::Edge> edges{{1, 7}, {2, 3}};
    sort(edges.begin(), edges.end(), [](const gallery::Edge& lhs, const gallery::Edge& rhs) { return lhs.weight < rhs.weight; });

    ll best = gallery::max(accumulate(values.begin(), values.end(), 0LL), std::max<ll>(3, 4));
    gallery::count += static_cast<int>(edges.size());
    auto start = chrono::steady_clock::now();  // FIXME: time something
    (void)start;
    cout << "best = " << best << ", edges = " << gallery::count << ", seen = " << seen.size() << '\n';
    return 0;
}
