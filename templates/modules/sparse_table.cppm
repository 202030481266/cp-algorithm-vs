export module sparse_table;

import std;

export template<typename T>
concept Element = std::copyable<T>;

export template<typename Op, typename T>
concept IdempotentOp = requires(Op op, T a, T b) {
	{ op(a, b) } -> std::convertible_to<T>;
};

export template<Element T, typename Op>
requires IdempotentOp<Op, T>
class SparseTable {
public:
	SparseTable() = default;

	explicit SparseTable(std::vector<T> const& a, Op op = {})
		: n_(a.size()), op_(op) 
	{
		int LOG = std::bit_width(static_cast<unsigned>(n_));
		table_.assign(LOG, std::vector<T>(n_));
		table_[0] = a;
		for (int j = 1; j < LOG; ++j) {
			for (int i = 0; i + (1 << j) <= n_; ++i) {
				table_[j][i] = op_(table_[j - 1][i], table_[j - 1][i + (1 << j - 1)]);
			}
		}
		log_.resize(n_ + 1);
		log_[1] = 0;
		for (int i = 2; i <= n_; ++i) log_[i] = log_[i / 2] + 1;
	}

	T query(int l, int r) const {
		int k = log_[r - l + 1];
		return op_(table_[k][l], table_[k][r - (1 << k) + 1]);
	}

	int size() const {
		return n_;
	}

private:
	int n_ = 0;
	Op op_;
	std::vector<std::vector<T>> table_;
	std::vector<int> log_;
};
