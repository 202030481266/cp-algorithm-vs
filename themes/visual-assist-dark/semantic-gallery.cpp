// Visual Assist Dark — C++ semantic color gallery
// Open this file in a C++ project with IntelliSense enabled.
#include <algorithm>
#include <array>
#include <concepts>
#include <iostream>
#include <numeric>
#include <sstream>
#include <string_view>
#include <vector>

#define VA_TRACE(value) (std::cout << #value << " = " << (value) << '\n')

namespace amber {

enum class State { idle, running, complete };

template <typename T>
concept Arithmetic = std::integral<T> || std::floating_point<T>;

template <Arithmetic T>
class Fenwick {
public:
    explicit Fenwick(int size) : tree_(size + 1, T{}) {}

    void add(int index, T delta) {
        for (++index; index < static_cast<int>(tree_.size()); index += index & -index)
            tree_[index] += delta;
    }

    [[nodiscard]] T prefix_sum(int end) const {
        T result{};
        for (; end > 0; end -= end & -end)
            result += tree_[end];
        return result;
    }

    static constexpr std::string_view name() { return "Fenwick tree"; }

private:
    std::vector<T> tree_;
};

struct Node {
    int index{};
    long long weight{};
    State state = State::idle;
    bool operator<(const Node& other) const { return weight < other.weight; }
};

using Counter = Fenwick<long long>;
constexpr int kSampleCount = 6;
inline int visits = 0;

} // namespace amber

// Overloaded and built-in operators should all remain neutral gray.
// The native preset uses bold khaki for local variables and parameters.
// System-library italics require source-aware classification from VA
// or another editor extension.
[[nodiscard]] bool operator_color_demo() {
    std::istringstream input("2 7");
    int index = 0, value = 0;
    input >> index >> value;                // member operator>>

    std::vector<int> counts(3);
    counts[index] = value;                  // member operator[]
    int plain[3]{};
    plain[index] = value;                   // built-in subscript
    const int bitmask = (1 << index) >> 1;  // built-in shifts

    std::ostringstream output;
    output << counts[index] << ':' << plain[index];
    return output.str() == "7:7" && bitmask == 2;
}

int main() {
    if (!operator_color_demo()) return 2;

    amber::Counter tree(amber::kSampleCount);
    const std::array values{3, 1, 4, 1, 5, 9};
    for (int i = 0; i < static_cast<int>(values.size()); ++i)
        tree.add(i, values[i]);

    auto twice = [](long long value) { return value * 2; };
    const auto total = tree.prefix_sum(amber::kSampleCount);
    amber::Node root{0, twice(total), amber::State::complete};
    const auto [index, weight, state] = root;
    ++amber::visits;
    VA_TRACE(total);
    std::cout << amber::Counter::name() << ": " << weight << "\n";
    return total == 23 && weight == 46 ? 0 : 1;
}
