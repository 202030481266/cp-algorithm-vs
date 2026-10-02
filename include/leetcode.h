#pragma once
// Local driver for LeetCode-style solutions. Only main.cpp's marked class is submitted;
// this header replaces LeetCode's judge: it parses test cases such as [1,2] or
// [3,9,20,null,null,15,7], calls the solution and prints answers in LeetCode format.
// "python tools/cp.py new <LeetCode URL>" generates a main.cpp that uses it.
#include <algorithm>
#include <array>
#include <bitset>
#include <cassert>
#include <cctype>
#include <climits>
#include <cmath>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <deque>
#include <functional>
#include <iomanip>
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
#include <set>
#include <sstream>
#include <stack>
#include <stdexcept>
#include <string>
#include <string_view>
#include <tuple>
#include <type_traits>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>
#if (defined(_MSVC_LANG) && _MSVC_LANG >= 202002L) || __cplusplus >= 202002L
#include <bit>
#include <ranges>
#include <span>
#endif

#include "util/debug.hpp"  // debug(...) on local runs; delete debug calls before submitting.

struct ListNode {
    int val;
    ListNode *next;
    ListNode() : val(0), next(nullptr) {}
    ListNode(int x) : val(x), next(nullptr) {}
    ListNode(int x, ListNode *next) : val(x), next(next) {}
};

struct TreeNode {
    int val;
    TreeNode *left;
    TreeNode *right;
    TreeNode() : val(0), left(nullptr), right(nullptr) {}
    TreeNode(int x) : val(x), left(nullptr), right(nullptr) {}
    TreeNode(int x, TreeNode *left, TreeNode *right) : val(x), left(left), right(right) {}
};

namespace lc {

// Thrown for malformed test data, as opposed to exceptions from the solution itself.
struct InputError : std::runtime_error {
    using std::runtime_error::runtime_error;
};

struct Value {
    enum class Kind { Null, Boolean, Number, String, Array };
    Kind kind = Kind::Null;
    bool boolean = false;
    std::string text;          // Number literal or decoded string.
    std::vector<Value> items;  // Array elements.
};

class Parser {
public:
    explicit Parser(std::string_view text) : text_(text) {}

    Value parse() {
        Value value = parse_value();
        skip_space();
        if (pos_ != text_.size()) fail("unexpected text after the value");
        return value;
    }

private:
    std::string_view text_;
    std::size_t pos_ = 0;

    [[noreturn]] void fail(const std::string& message) const {
        throw InputError(message + " (column " + std::to_string(pos_ + 1) + ")");
    }
    void skip_space() {
        while (pos_ < text_.size() && std::isspace(static_cast<unsigned char>(text_[pos_]))) ++pos_;
    }
    bool consume(char c) {
        skip_space();
        if (pos_ < text_.size() && text_[pos_] == c) {
            ++pos_;
            return true;
        }
        return false;
    }
    bool consume_word(std::string_view word) {
        if (text_.substr(pos_, word.size()) != word) return false;
        pos_ += word.size();
        return true;
    }

    Value parse_value() {
        skip_space();
        if (pos_ == text_.size()) fail("missing value");
        Value value;
        const char c = text_[pos_];
        if (c == '[') {
            ++pos_;
            value.kind = Value::Kind::Array;
            if (consume(']')) return value;
            do value.items.push_back(parse_value()); while (consume(','));
            if (!consume(']')) fail("expected ',' or ']'");
        } else if (c == '"') {
            value.kind = Value::Kind::String;
            value.text = parse_string();
        } else if (consume_word("null")) {
        } else if (consume_word("true")) {
            value.kind = Value::Kind::Boolean;
            value.boolean = true;
        } else if (consume_word("false")) {
            value.kind = Value::Kind::Boolean;
        } else if (c == '-' || c == '+' || std::isdigit(static_cast<unsigned char>(c))) {
            const std::size_t start = pos_++;
            while (pos_ < text_.size() && (std::isdigit(static_cast<unsigned char>(text_[pos_]))
                                           || std::string_view(".eE+-").find(text_[pos_]) != std::string_view::npos)) {
                ++pos_;
            }
            value.kind = Value::Kind::Number;
            value.text = std::string(text_.substr(start, pos_ - start));
        } else {
            fail(std::string("unexpected character '") + c + "'");
        }
        return value;
    }

    unsigned parse_hex4() {
        if (pos_ + 4 > text_.size()) fail("incomplete \\u escape");
        unsigned code = 0;
        for (int i = 0; i < 4; ++i) {
            const char c = text_[pos_++];
            code <<= 4;
            if (c >= '0' && c <= '9') code |= static_cast<unsigned>(c - '0');
            else if (c >= 'a' && c <= 'f') code |= static_cast<unsigned>(c - 'a' + 10);
            else if (c >= 'A' && c <= 'F') code |= static_cast<unsigned>(c - 'A' + 10);
            else fail("invalid \\u escape");
        }
        return code;
    }

    static void append_utf8(std::string& out, unsigned code) {
        if (code < 0x80) {
            out += static_cast<char>(code);
        } else if (code < 0x800) {
            out += static_cast<char>(0xC0 | (code >> 6));
            out += static_cast<char>(0x80 | (code & 0x3F));
        } else if (code < 0x10000) {
            out += static_cast<char>(0xE0 | (code >> 12));
            out += static_cast<char>(0x80 | ((code >> 6) & 0x3F));
            out += static_cast<char>(0x80 | (code & 0x3F));
        } else {
            out += static_cast<char>(0xF0 | (code >> 18));
            out += static_cast<char>(0x80 | ((code >> 12) & 0x3F));
            out += static_cast<char>(0x80 | ((code >> 6) & 0x3F));
            out += static_cast<char>(0x80 | (code & 0x3F));
        }
    }

    std::string parse_string() {
        ++pos_;  // Opening quote.
        std::string out;
        while (true) {
            if (pos_ == text_.size()) fail("unterminated string");
            const char c = text_[pos_++];
            if (c == '"') return out;
            if (c != '\\') {
                out += c;
                continue;
            }
            if (pos_ == text_.size()) fail("unterminated escape");
            switch (const char e = text_[pos_++]) {
            case '"': case '\\': case '/': out += e; break;
            case 'b': out += '\b'; break;
            case 'f': out += '\f'; break;
            case 'n': out += '\n'; break;
            case 'r': out += '\r'; break;
            case 't': out += '\t'; break;
            case 'u': {
                unsigned code = parse_hex4();
                if (code >= 0xD800 && code <= 0xDBFF && text_.substr(pos_, 2) == "\\u") {
                    pos_ += 2;
                    const unsigned low = parse_hex4();
                    code = 0x10000 + ((code - 0xD800) << 10) + (low - 0xDC00);
                }
                append_utf8(out, code);
                break;
            }
            default: fail(std::string("invalid escape \\") + e);
            }
        }
    }
};

namespace detail {
template<class T> struct IsVector : std::false_type {};
template<class T, class A> struct IsVector<std::vector<T, A>> : std::true_type {};
template<class> inline constexpr bool always_false = false;

inline std::string describe(const Value& value) {
    switch (value.kind) {
    case Value::Kind::Null: return "null";
    case Value::Kind::Boolean: return value.boolean ? "true" : "false";
    case Value::Kind::Number: return value.text;
    case Value::Kind::String: return '"' + value.text + '"';
    case Value::Kind::Array: return "an array of " + std::to_string(value.items.size()) + " items";
    }
    return "?";
}

inline void expect(const Value& value, Value::Kind kind, const char* what) {
    if (value.kind != kind) throw InputError(std::string("expected ") + what + ", got " + describe(value));
}

template<class T>
T to_integer(const Value& value) {
    expect(value, Value::Kind::Number, "an integer");
    const std::string& text = value.text;
    bool fits = text.find_first_of(".eE") == std::string::npos;
    if (fits) {
        try {
            std::size_t used = 0;
            if constexpr (std::is_signed_v<T>) {
                const long long x = std::stoll(text, &used);
                fits = used == text.size() && x >= static_cast<long long>(std::numeric_limits<T>::min())
                       && x <= static_cast<long long>(std::numeric_limits<T>::max());
                if (fits) return static_cast<T>(x);
            } else {
                const unsigned long long x = std::stoull(text, &used);
                fits = used == text.size() && text[0] != '-' && x <= std::numeric_limits<T>::max();
                if (fits) return static_cast<T>(x);
            }
        } catch (const std::logic_error&) {
            fits = false;
        }
    }
    throw InputError("number " + text + " does not fit the parameter type");
}
}  // namespace detail

// Converts parsed JSON to a parameter type. vector, ListNode* and TreeNode* nest freely.
template<class T>
T from_value(const Value& value) {
    using Kind = Value::Kind;
    if constexpr (std::is_same_v<T, bool>) {
        detail::expect(value, Kind::Boolean, "true or false");
        return value.boolean;
    } else if constexpr (std::is_same_v<T, char>) {
        detail::expect(value, Kind::String, "a one-character string");
        if (value.text.size() != 1) throw InputError("expected one character, got \"" + value.text + '"');
        return value.text[0];
    } else if constexpr (std::is_integral_v<T>) {
        return detail::to_integer<T>(value);
    } else if constexpr (std::is_floating_point_v<T>) {
        detail::expect(value, Kind::Number, "a number");
        try {
            return static_cast<T>(std::stod(value.text));
        } catch (const std::logic_error&) {
            throw InputError("invalid number " + value.text);
        }
    } else if constexpr (std::is_same_v<T, std::string>) {
        detail::expect(value, Kind::String, "a string");
        return value.text;
    } else if constexpr (std::is_same_v<T, ListNode*>) {
        detail::expect(value, Kind::Array, "a list such as [1,2,3]");
        ListNode head;
        ListNode* tail = &head;
        for (const Value& item : value.items) tail = tail->next = new ListNode(from_value<int>(item));
        return head.next;
    } else if constexpr (std::is_same_v<T, TreeNode*>) {
        detail::expect(value, Kind::Array, "a tree such as [1,null,2]");
        const auto& items = value.items;
        if (items.empty() || items[0].kind == Kind::Null) return nullptr;
        TreeNode* root = new TreeNode(from_value<int>(items[0]));
        std::queue<TreeNode*> parents;
        parents.push(root);
        std::size_t next = 1;
        while (!parents.empty() && next < items.size()) {
            TreeNode* parent = parents.front();
            parents.pop();
            for (TreeNode** child : {&parent->left, &parent->right}) {
                if (next < items.size() && items[next].kind != Kind::Null) {
                    *child = new TreeNode(from_value<int>(items[next]));
                    parents.push(*child);
                }
                ++next;
            }
        }
        return root;
    } else if constexpr (detail::IsVector<T>::value) {
        detail::expect(value, Kind::Array, "an array");
        T result;
        result.reserve(value.items.size());
        for (const Value& item : value.items) result.push_back(from_value<typename T::value_type>(item));
        return result;
    } else {
        static_assert(detail::always_false<T>, "lc: unsupported parameter type; read this argument yourself");
    }
}

inline void write_string(std::ostream& out, std::string_view text) {
    out << '"';
    for (const char c : text) {
        switch (c) {
        case '"': out << "\\\""; break;
        case '\\': out << "\\\\"; break;
        case '\n': out << "\\n"; break;
        case '\r': out << "\\r"; break;
        case '\t': out << "\\t"; break;
        default: out << c;
        }
    }
    out << '"';
}

// Prints like LeetCode: [1,2], "text", true, 2.50000, null.
template<class T>
void write(std::ostream& out, const T& value) {
    if constexpr (std::is_same_v<T, bool>) {
        out << (value ? "true" : "false");
    } else if constexpr (std::is_same_v<T, char>) {
        write_string(out, std::string_view(&value, 1));
    } else if constexpr (std::is_integral_v<T>) {
        out << +value;
    } else if constexpr (std::is_floating_point_v<T>) {
        std::ostringstream number;
        number << std::fixed << std::setprecision(5) << value;
        out << number.str();
    } else if constexpr (std::is_convertible_v<const T&, std::string_view>) {
        write_string(out, std::string_view(value));
    } else if constexpr (std::is_same_v<T, ListNode*>) {
        out << '[';
        std::size_t count = 0;
        for (const ListNode* node = value; node; node = node->next) {
            if (++count > 1'000'000) throw std::runtime_error("lc: returned list is too long or has a cycle");
            out << (count > 1 ? "," : "") << node->val;
        }
        out << ']';
    } else if constexpr (std::is_same_v<T, TreeNode*>) {
        std::vector<const TreeNode*> order;
        std::queue<const TreeNode*> pending;
        pending.push(value);
        while (!pending.empty()) {
            const TreeNode* node = pending.front();
            pending.pop();
            order.push_back(node);
            if (node) {
                pending.push(node->left);
                pending.push(node->right);
            }
        }
        while (!order.empty() && !order.back()) order.pop_back();
        out << '[';
        for (std::size_t i = 0; i < order.size(); ++i) {
            if (i) out << ',';
            if (order[i]) out << order[i]->val;
            else out << "null";
        }
        out << ']';
    } else if constexpr (detail::IsVector<T>::value) {
        out << '[';
        for (std::size_t i = 0; i < value.size(); ++i) {
            if (i) out << ',';
            if constexpr (std::is_same_v<typename T::value_type, bool>) write(out, static_cast<bool>(value[i]));
            else write(out, value[i]);
        }
        out << ']';
    } else {
        static_assert(detail::always_false<T>, "lc: unsupported return type; print the answer yourself");
    }
}

template<class T>
std::string to_json(const T& value) {
    std::ostringstream out;
    write(out, value);
    return out.str();
}

// GCC -O2 falsely reports -Warray-bounds when calling a member pointer on an empty Solution.
#if defined(__GNUC__) && !defined(__clang__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Warray-bounds"
#endif

namespace detail {
inline void open_local_files() {
#ifdef LOCAL_FILE
    if (!std::freopen("data/input.txt", "r", stdin)) {
        std::perror("data/input.txt");
        std::exit(1);
    }
    if (!std::freopen("data/output.txt", "w", stdout)) {
        std::perror("data/output.txt");
        std::exit(1);
    }
#endif
}

// Reads the next non-blank line, so blank lines between examples are allowed.
inline bool next_line(std::istream& in, std::string& line, int& number) {
    while (std::getline(in, line)) {
        if (++number == 1 && line.rfind("\xEF\xBB\xBF", 0) == 0) line.erase(0, 3);
        if (!line.empty() && line.back() == '\r') line.pop_back();
        if (line.find_first_not_of(" \t") != std::string::npos) return true;
    }
    return false;
}

template<std::size_t I, class Tuple>
bool read_argument(std::istream& in, Tuple& args, int& line_number) {
    std::string line;
    if (!next_line(in, line, line_number)) {
        if constexpr (I == 0) return false;
        else throw InputError("incomplete test case: expected " + std::to_string(std::tuple_size_v<Tuple>)
                         + " lines, one per parameter");
    }
    std::get<I>(args) = from_value<std::tuple_element_t<I, Tuple>>(Parser(line).parse());
    return true;
}

template<class Tuple, std::size_t... I>
bool read_arguments(std::istream& in, Tuple& args, int& line_number, std::index_sequence<I...>) {
    bool complete = true;
    ((complete = complete && read_argument<I>(in, args, line_number)), ...);
    return complete;
}

template<class... A, std::size_t... I>
std::tuple<std::decay_t<A>...> parse_call(const Value& args, std::index_sequence<I...>) {
    expect(args, Value::Kind::Array, "an argument list");
    if (args.items.size() != sizeof...(A)) {
        throw InputError("expected " + std::to_string(sizeof...(A)) + " arguments, got "
                         + std::to_string(args.items.size()));
    }
    return std::tuple<std::decay_t<A>...>(from_value<std::decay_t<A>>(args.items[I])...);
}

// Param < 0: print the return value. Param >= 0: print that argument after the call
// (in-place problems). Prefix: print "k, name = [first k elements,_,_]".
template<int Param, bool Prefix, class C, class R, class... A, class F>
int run_solution(F method, [[maybe_unused]] const char* name) {
    open_local_files();
    int line_number = 0;
    try {
        while (true) {
            std::tuple<std::decay_t<A>...> args;
            if (!read_arguments(std::cin, args, line_number, std::index_sequence_for<A...>{})) break;
            C solution;
            auto call = [&]() -> decltype(auto) {
                return std::apply([&](auto&... a) -> decltype(auto) { return (solution.*method)(a...); }, args);
            };
            if constexpr (Param >= 0 && Prefix) {
                const long long kept = static_cast<long long>(call());
                const auto& array = std::get<Param>(args);
                std::cout << kept << ", " << name << " = [";
                for (std::size_t i = 0; i < array.size(); ++i) {
                    if (i) std::cout << ',';
                    if (static_cast<long long>(i) < kept) write(std::cout, array[i]);
                    else std::cout << '_';
                }
                std::cout << ']';
            } else if constexpr (Param >= 0) {
                call();
                write(std::cout, std::get<Param>(args));
            } else if constexpr (std::is_void_v<R>) {
                call();
                std::cout << "null";
            } else {
                write(std::cout, call());
            }
            std::cout << std::endl;  // Keep earlier answers if a later case crashes.
            if constexpr (sizeof...(A) == 0) break;
        }
    } catch (const InputError& error) {
        std::cerr << "lc: input line " << line_number << ": " << error.what() << '\n';
        return 1;
    }
    return 0;
}

template<class C, class R, class... A, class F>
std::function<std::string(C&, const Value&)> bind_method(F method) {
    return [method](C& object, const Value& args) {
        auto parsed = parse_call<A...>(args, std::index_sequence_for<A...>{});
        if constexpr (std::is_void_v<R>) {
            std::apply([&](auto&... a) { (object.*method)(a...); }, parsed);
            return std::string("null");
        } else {
            return to_json(std::apply([&](auto&... a) -> decltype(auto) { return (object.*method)(a...); }, parsed));
        }
    };
}
}  // namespace detail

#if defined(__GNUC__) && !defined(__clang__)
#pragma GCC diagnostic pop
#endif

// Function problems: each test case is one line per parameter, as in LeetCode's test panel.
template<int Param = -1, bool Prefix = false, class C, class R, class... A>
int run_solution(R (C::*method)(A...), const char* name = "") {
    return detail::run_solution<Param, Prefix, C, R, A...>(method, name);
}
template<int Param = -1, bool Prefix = false, class C, class R, class... A>
int run_solution(R (C::*method)(A...) const, const char* name = "") {
    return detail::run_solution<Param, Prefix, C, R, A...>(method, name);
}

// Design problems: each test case is two lines, the operation names and their arguments.
template<class C>
using Method = std::function<std::string(C&, const Value&)>;
template<class C>
using Factory = std::function<std::unique_ptr<C>(const Value&)>;

template<class C, class R, class... A>
Method<C> method(R (C::*f)(A...)) { return detail::bind_method<C, R, A...>(f); }
template<class C, class R, class... A>
Method<C> method(R (C::*f)(A...) const) { return detail::bind_method<C, R, A...>(f); }

template<class C, class... A>
Factory<C> constructor() {
    return [](const Value& args) {
        auto parsed = detail::parse_call<A...>(args, std::index_sequence_for<A...>{});
        return std::apply([](auto&... a) { return std::make_unique<C>(a...); }, parsed);
    };
}

template<class C>
int run_design(Factory<C> make, const std::map<std::string, Method<C>>& methods) {
    detail::open_local_files();
    int line_number = 0;
    std::string names_line, args_line;
    try {
        while (detail::next_line(std::cin, names_line, line_number)) {
            if (!detail::next_line(std::cin, args_line, line_number)) {
                throw InputError("missing the argument line after the operation names");
            }
            const Value names = Parser(names_line).parse();
            const Value calls = Parser(args_line).parse();
            detail::expect(names, Value::Kind::Array, "a list of operation names");
            detail::expect(calls, Value::Kind::Array, "a list of argument lists");
            if (names.items.empty() || names.items.size() != calls.items.size()) {
                throw InputError("operation names and argument lists must be non-empty and equally long");
            }
            std::unique_ptr<C> object = make(calls.items[0]);
            std::string answer = "[null";
            for (std::size_t i = 1; i < names.items.size(); ++i) {
                const std::string name = from_value<std::string>(names.items[i]);
                const auto found = methods.find(name);
                if (found == methods.end()) throw InputError("unknown operation \"" + name + '"');
                answer += ',' + found->second(*object, calls.items[i]);
            }
            std::cout << answer << ']' << std::endl;
        }
    } catch (const InputError& error) {
        std::cerr << "lc: input line " << line_number << ": " << error.what() << '\n';
        return 1;
    }
    return 0;
}

}  // namespace lc

using namespace std;  // LeetCode's C++ environment does the same.
