#include <functional>
#include <iostream>
#include "data_structures/leftist_heap.hpp"

int main() {
    cp::LeftistHeap<int> heaps; // 小根堆，所有堆共用节点池
    int a = -1, b = -1;         // -1 表示空堆
    for (int x : {5, 1, 8}) a = heaps.push(a, x);
    for (int x : {3, 7}) b = heaps.push(b, x);
    std::cout << "top(a)=" << heaps.top(a) << " top(b)=" << heaps.top(b) << '\n';
    int c = heaps.meld(a, b); // 合并后 a、b 不再单独使用
    c = heaps.pop(c);         // 删除最小值 1
    std::cout << "after pop top=" << heaps.top(c) << " node2_value=" << heaps.value(2) << '\n';

    cp::LeftistHeap<int, std::greater<int>> max_heap; // 大根堆
    int m = -1;
    for (int x : {4, 9, 1}) m = max_heap.push(m, x);
    std::cout << "max_top=" << max_heap.top(m) << '\n';
}
