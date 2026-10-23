// VS Code Dark Modern — C++ color gallery for Visual Studio
// Open this file in a C++ project with IntelliSense enabled.
#include <chrono>
#include <iostream>
#include <map>
#include <sstream>
#include <string>
#include <vector>

#define DM_TRACE(expr) (std::cout << #expr << " = " << (expr) << '\n')

namespace modern {

enum class Level { low, medium, high };

template <typename T>
concept Summable = requires(T a, T b) { a + b; };

template <Summable T>
class Accumulator {
public:
    explicit Accumulator(T start) : total_(start) {}

    Accumulator& add(T value) {
        total_ = total_ + value;
        ++count_;
        return *this;
    }

    [[nodiscard]] T total() const { return total_; }
    static int instances() { return created_; }

private:
    T total_;
    int count_ = 0;
    static inline int created_ = 1;
};

struct Point {
    int x{}, y{};
    Point operator+(const Point& other) const { return {x + other.x, y + other.y}; }
};

} // namespace modern

constexpr long long operator""_k(unsigned long long value) { return value * 1000; }

#if 0
// Inactive region: VS Code renders it at 55% opacity.
static void legacy_path() {}
#endif

// Overloaded operators (>>, [], +, <<) use the function yellow in VX2762, as in VS Code,
// and the operator gray in Vivid. Built-in operators such as 1 << a and | are always gray.
bool operator_color_demo() {
    std::istringstream input("3 4");
    int a = 0, b = 0;
    input >> a >> b;                   // member operator>>

    std::vector<int> cells(4);
    cells[a] = b;                      // member operator[]
    const int mask = (1 << a) | b;     // built-in << and |

    modern::Point p{1, 2}, q{a, b};
    const modern::Point r = p + q;     // member operator+

    std::ostringstream out;
    out << cells[a] << '\t' << r.x + r.y << '\n';
    return out.str() == "4\t10\n" && mask == 12;
}

int main() {
    using namespace std::literals;
    if (!operator_color_demo()) return 2;

    modern::Accumulator<long long> acc(0);
    for (int i = 1; i <= 4; ++i) {
        if (i % 2 == 0) continue;
        acc.add(i * 2_k);
    }

    auto* level = new modern::Level(modern::Level::high);
    std::map<std::string, int> score{{"alpha"s, 1}, {"beta"s, 2}};
    const auto timeout = 250ms;

    switch (*level) {
    case modern::Level::high:
        score["alpha"] += static_cast<int>(timeout.count());
        break;
    default:
        break;
    }
    delete level;

    DM_TRACE(acc.total());
    std::cout << "alpha: " << score["alpha"] << ", instances: "
              << modern::Accumulator<long long>::instances() << "\n";
    return acc.total() == 8000 && score["alpha"] == 251 ? 0 : 1;
}
