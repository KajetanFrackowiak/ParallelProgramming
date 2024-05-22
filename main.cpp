// Static work distribution with ~equal job length
// By: Nick from CoffeeBeforeArch

#include <tbb/parallel_for.h>
#include <CL/sycl.hpp>

#include <algorithm>
#include <chrono>
#include <iterator>
#include <random>
#include <vector>
#include <iostream>

int main() {

   sycl::queue q;

    // Submit a SYCL command group
    q.submit([&](sycl::handler& h) {
        // Accessor to global memory
        sycl::stream out(1024, 256, h);

        // Kernel to print "Hello, World!"
        h.single_task<class hello_world>([=]() {
            out << "Hello, World!\n";
        });
    });

  // Create a random number generator
  std::random_device rd;
  std::mt19937 mt(rd());

  // Create 1 distribution
  std::uniform_int_distribution bin(20, 30);

  // Calculate the number elements per bin
  int num_work_items = 1 << 18;

  // Create work items
  std::vector<int> work_items;
  std::generate_n(std::back_inserter(work_items), num_work_items,
                  [&] { return bin(mt); });

  // Process all elements in a parallel_for loop
  tbb::parallel_for(
      tbb::blocked_range<int>(0, num_work_items),
      [&](tbb::blocked_range<int> r) {
        for (int i = r.begin(); i < r.end(); i++) {
          std::this_thread::sleep_for(std::chrono::microseconds(work_items[i]));
        }
      },
      tbb::static_partitioner());

  // std::cout << "Hello, World!";
  return 0;
}
