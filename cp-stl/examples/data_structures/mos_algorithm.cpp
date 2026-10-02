#include <algorithm>
#include <iostream>
#include <utility>
#include <vector>
#include "data_structures/mos_algorithm.hpp"

int main() {
    std::vector<int> a{1, 2, 1, 3, 2, 1};
    cp::MosAlgorithm mo(int(a.size()), {{0, 3}, {1, 5}, {2, 6}}); // 区间 [l,r)，可以构造时一次给出
    mo.add_query(0, 6);                                            // 也可以逐个登记，返回询问编号 3
    int q = mo.query_count();

    // 普通莫队：区间内不同数的个数。
    std::vector<int> cnt(4), distinct_answer(q);
    int distinct = 0;
    mo.run([&](int i) { if (cnt[a[i]]++ == 0) ++distinct; },
           [&](int i) { if (--cnt[a[i]] == 0) --distinct; },
           [&](int id) { distinct_answer[id] = distinct; });

    // 回滚莫队：区间众数的出现次数（只加不删，用操作栈撤销）。
    std::vector<int> freq(4), history, best_history, mode_answer(q);
    int best = 0;
    auto add = [&](int i) {
        history.push_back(a[i]);
        best_history.push_back(best);
        best = std::max(best, ++freq[a[i]]);
    };
    mo.run_rollback(add, add,
                    [&] { return int(history.size()); },
                    [&](int size) {
                        while (int(history.size()) > size) {
                            --freq[history.back()];
                            best = best_history.back();
                            history.pop_back();
                            best_history.pop_back();
                        }
                    },
                    [&](int id) { mode_answer[id] = best; });
    for (int id = 0; id < q; ++id)
        std::cout << "query " << id << " distinct=" << distinct_answer[id] << " mode_count=" << mode_answer[id] << '\n';

    // 带修莫队：按输入顺序登记修改和询问，每个询问看到在它之前登记的全部修改。
    std::vector<int> color{1, 2, 1, 3};
    std::vector<std::pair<int, int>> changes; // 第 k 次修改：(位置, 新值)，由调用方保存
    cp::MosAlgorithmWithUpdates timed(int(color.size()));
    timed.add_query(0, 4);         // 询问 0：1 2 1 3
    timed.add_update();            // 修改 0：color[1] = 3
    changes.emplace_back(1, 3);
    timed.add_query(0, 4);         // 询问 1：1 3 1 3
    timed.add_update();            // 修改 1：color[0] = 2
    changes.emplace_back(0, 2);
    timed.add_query(0, 3);         // 询问 2：2 3 1
    std::vector<int> count(4), timed_answer(timed.query_count());
    int kinds = 0;
    auto add_color = [&](int i) { if (count[color[i]]++ == 0) ++kinds; };
    auto remove_color = [&](int i) { if (--count[color[i]] == 0) --kinds; };
    timed.run(add_color, remove_color,
              [&](int k, int l, int r) { // 切换第 k 次修改：与数组中的值交换，再调用一次就是撤销
                  auto& [pos, value] = changes[k];
                  bool inside = l <= pos && pos < r;
                  if (inside) remove_color(pos);
                  std::swap(color[pos], value);
                  if (inside) add_color(pos);
              },
              [&](int id) { timed_answer[id] = kinds; });
    std::cout << "with updates:";
    for (int x : timed_answer) std::cout << ' ' << x;
    std::cout << "\nrestored:";
    for (int x : color) std::cout << ' ' << x; // run 结束前撤销全部修改，数组恢复原样
    std::cout << '\n';
}
