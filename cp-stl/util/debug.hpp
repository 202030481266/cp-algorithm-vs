#pragma once
// 使用说明：cp-stl/docs/usage/util/debug.md
// 完整示例：cp-stl/examples/util/debug.cpp
#include <iostream>
#include <iterator>
#include <string_view>
#include <tuple>
#include <type_traits>
#include <utility>

namespace cp::debug_detail {
template<class T> struct IsPair : std::false_type {};
template<class A, class B> struct IsPair<std::pair<A, B>> : std::true_type {};
template<class T> struct IsTuple : std::false_type {};
template<class... T> struct IsTuple<std::tuple<T...>> : std::true_type {};
template<class T, class = void> struct IsRange : std::false_type {};
template<class T>
struct IsRange<T, std::void_t<decltype(std::begin(std::declval<const T&>())),
                             decltype(std::end(std::declval<const T&>()))>> : std::true_type {};
template<class T>
void print(const T& value) {
    if constexpr (std::is_same_v<T, bool>) std::cerr << (value ? "true" : "false");
    else if constexpr (std::is_convertible_v<const T&, std::string_view>) std::cerr << '"' << value << '"';
    else if constexpr (IsPair<T>::value) {
        std::cerr << '('; print(value.first); std::cerr << ", "; print(value.second); std::cerr << ')';
    } else if constexpr (IsTuple<T>::value) {
        std::cerr << '(';
        std::apply([](const auto&... items) {
            int index = 0;
            ((std::cerr << (index++ ? ", " : ""), print(items)), ...);
        }, value);
        std::cerr << ')';
    } else if constexpr (IsRange<T>::value) {
        std::cerr << '[';
        bool first = true;
        for (const auto& item : value) {
            if (!first) std::cerr << ", ";
            first = false;
            print(item);
        }
        std::cerr << ']';
    } else std::cerr << value;
}
template<class... T>
void log(const char* names, const T&... values) {
    std::cerr << names << " = ";
    int index = 0;
    ((std::cerr << (index++ ? ", " : ""), print(values)), ...);
    std::cerr << '\n';
}
} // namespace cp::debug_detail

#ifndef debug
#if defined(LOCAL) && !defined(ONLINE_JUDGE) && !defined(NO_DEBUG)
#define debug(...) do { std::cerr << "[line " << __LINE__ << "] "; \
    ::cp::debug_detail::log(#__VA_ARGS__, __VA_ARGS__); } while (false)
#else
#define debug(...) ((void)0)
#endif
#endif // 已有 workspace 的 debug 宏时保留它。
