#pragma once

#ifndef PCH_H
#define PCH_H

// C++ 标准库

// 1. 输入输出
#include <iostream>
#include <cstdio>
#include <fstream>
#include <sstream>

// 2. 容器
#include <vector>
#include <list>
#include <deque>
#include <stack>
#include <queue>
#include <map>
#include <set>
#include <unordered_map>
#include <unordered_set>
#include <tuple>

// 3. 算法
#include <algorithm>
#include <numeric>

// 4. 字符串处理
#include <string>
#include <cstring> // C-style string functions

// 5. 数学运算
#include <cmath>
#include <complex>

// 6. 工具
#include <utility> // for std::pair, std::move
#include <memory>  // for std::unique_ptr, std::shared_ptr
#include <functional>
#include <stdexcept>

// 7. 时间
#include <chrono>
#include <ctime>

// 8. 其他
#include <limits>
#include <bitset>
#include <cassert>

// Explicit headers used by the current C++20/23 template.
#include <bit>
#include <concepts>
#include <cstdint>
#include <format>
#include <optional>
#include <random>
#include <ranges>
#include <string_view>
#include <thread>
#include <type_traits>

#endif //PCH_H