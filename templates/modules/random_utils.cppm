export module random_utils;

import std;

export template <typename T>
std::vector<T> get_random_data(size_t times, T lower, T upper) {
	static std::random_device rd;
	static std::mt19937 gen(rd());

	std::vector<T> res;
	res.reserve(times);

	if constexpr (std::is_integral_v<T>) {
		std::uniform_int_distribution<T> dist(lower, upper);
		for (size_t i = 0; i < times; ++i) res.push_back(dist(gen));
	}
	else {
		std::uniform_real_distribution<T> dist(lower, upper);
		for (size_t i = 0; i < times; ++i) res.push_back(dist(gen));
	}

	return res;
}

export template <typename T, typename Generator>
std::vector<T> get_custom_data(size_t times, Generator gen_func) {
	std::vector<T> res;
	res.reserve(times);

	for (size_t i = 0; i < times; ++i) res.push_back(gen_func());

	return res;
}

export template <typename T1, typename T2>
std::vector<std::pair<T1, T2>> get_random_pairs(
	size_t times,
	T1 first_lower, T1 first_upper,
	T2 second_lower, T2 second_upper)
{
	auto firsts = get_random_data<T1>(times, first_lower, first_upper);
	auto seconds = get_random_data<T2>(times, second_lower, second_upper);

	std::vector<std::pair<T1, T2>> res;
	res.reserve(times);

	for (size_t i = 0; i < times; ++i) {
		res.emplace_back(firsts[i], seconds[i]);
	}

	return res;
}

export template <typename T>
std::vector<std::pair<T, T>> get_random_pairs(size_t times, T lower, T upper) {
	return get_random_pairs<T, T>(times, lower, upper, lower, upper);
}