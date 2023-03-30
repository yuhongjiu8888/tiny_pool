#include <deque>
#include <iostream>
#include <queue>
#include <vector>

#include "tiny_pool.h"

int main() {
  std::cout << "Memory pool test start....." << std::endl;

  std::vector<int, tiny_allocator<int>> vec{11, 1,  12, 3,  2,
                                            14, 33, 21, 42, 17};
  // vec.push_back(11);
  // vec.push_back(1);
  // vec.push_back(12);
  // vec.push_back(3);
  // vec.push_back(2);
  // vec.push_back(14);
  // vec.push_back(33);
  // vec.push_back(21);
  // vec.push_back(42);
  // vec.push_back(17);
  vec.emplace_back(18);

  for (auto v : vec) {
    std::cout << v << ",";
  }
  std::cout << "\n";

  std::deque<double, tiny_allocator<double>> deq;
  for (int i = 0; i < 100000; i++) {
    deq.push_back(i);
  }
  for (auto v : deq) {
    std::cout << v << ",";
  }
  std::cout << "\n";

  std::queue<double> q;
  q.push(1.0);

  tiny_mempool::instance().report();

  std::cout << "Memory pool test end ....." << std::endl;
  return 0;
}
