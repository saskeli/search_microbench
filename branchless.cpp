#include <bitset>
#include <cstddef>
#include <cstdint>
#include <iostream>
#include <vector>
#include <string>

#ifndef COUNT
#include <chrono>
#endif

#ifdef COUNT
#include "counters/counters.hpp"
#endif

#include "searchers.hpp"

int main() {
  int arr[64 + 100];
#ifndef COUNT
  using std::chrono::duration_cast;
  using std::chrono::high_resolution_clock;
  using std::chrono::nanoseconds;
#endif

  std::vector<std::string> types = {"binary",          "branchless_cmov",
                                    "branchless_sub",  "branchless_sub_fix",
                                    "linear_scan",     "linear_scan_cmov",
                                    "linear_scan_sub", "templated_cmov",
                                    "templated_binary"};
#ifdef COUNT
  count::Default<9> count;
#else
  for (auto t : types) {
    std::cout << t << "\t";
  }
  std::cout << std::endl;
#endif
  for (size_t j = 0; j < 1000; j++) {
    for (int i = 0; i < 64 + 100; i++) {
      std::cin >> arr[i];
    }

    uint32_t a = 0;
#ifndef COUNT
    auto t1 = high_resolution_clock::now();
#endif
#ifdef COUNT
    count.reset();
#endif
    for (int qi = 64; qi < 64 + 100; qi++) {
      a += binary(arr, arr[qi]);
    }
#ifdef COUNT
    count.accumulate(0);
#endif
#ifndef COUNT
    auto t2 = high_resolution_clock::now();
    std::cout << (double)duration_cast<nanoseconds>(t2 - t1).count() / 100
              << "\t";
#endif

    uint32_t b = 0;
#ifndef COUNT
    t1 = high_resolution_clock::now();
#endif
#ifdef COUNT
    count.reset();
#endif
    for (int qi = 64; qi < 64 + 100; qi++) {
      b += branchless_cmov(arr, arr[qi]);
    }
#ifdef COUNT
    count.accumulate(1);
#endif
#ifndef COUNT
    t2 = high_resolution_clock::now();
    std::cout << (double)duration_cast<nanoseconds>(t2 - t1).count() / 100
              << "\t";
#endif
    uint32_t c = 0;
#ifndef COUNT
    t1 = high_resolution_clock::now();
#endif
#ifdef COUNT
    count.reset();
#endif
    for (int qi = 64; qi < 64 + 100; qi++) {
      c += branchless_sub(arr, arr[qi]);
    }
#ifdef COUNT
    count.accumulate(2);
#endif
#ifndef COUNT
    t2 = high_resolution_clock::now();
    std::cout << (double)duration_cast<nanoseconds>(t2 - t1).count() / 100
              << "\t";
#endif

    uint32_t d = 0;
#ifndef COUNT
    t1 = high_resolution_clock::now();
#endif
#ifdef COUNT
    count.reset();
#endif
    for (int qi = 64; qi < 64 + 100; qi++) {
      d += branchless_sub_fix(reinterpret_cast<uint32_t*>(arr), arr[qi]);
    }
#ifdef COUNT
    count.accumulate(3);
#endif
#ifndef COUNT
    t2 = high_resolution_clock::now();
    std::cout << (double)duration_cast<nanoseconds>(t2 - t1).count() / 100
              << "\t";
#endif
    uint32_t e = 0;
#ifndef COUNT
    t1 = high_resolution_clock::now();
#endif
#ifdef COUNT
    count.reset();
#endif
    for (int qi = 64; qi < 64 + 100; qi++) {
      e += linear_scan(arr, arr[qi]);
    }
#ifdef COUNT
    count.accumulate(4);
#endif
#ifndef COUNT
    t2 = high_resolution_clock::now();
    std::cout << (double)duration_cast<nanoseconds>(t2 - t1).count() / 100
              << "\t";
#endif
    uint32_t f = 0;
#ifndef COUNT
    t1 = high_resolution_clock::now();
#endif
#ifdef COUNT
    count.reset();
#endif
    for (int qi = 64; qi < 64 + 100; qi++) {
      f += linear_scan_cmov(arr, arr[qi]);
    }
#ifdef COUNT
    count.accumulate(5);
#endif
#ifndef COUNT
    t2 = high_resolution_clock::now();
    std::cout << (double)duration_cast<nanoseconds>(t2 - t1).count() / 100
              << "\t";
#endif
    uint32_t g = 0;
#ifndef COUNT
    t1 = high_resolution_clock::now();
#endif
#ifdef COUNT
    count.reset();
#endif
    for (int qi = 64; qi < 64 + 100; qi++) {
      g += linear_scan_sub(arr, arr[qi]);
    }
#ifdef COUNT
    count.accumulate(6);
#endif
#ifndef COUNT
    t2 = high_resolution_clock::now();
    std::cout << (double)duration_cast<nanoseconds>(t2 - t1).count() / 100
              << "\t";
#endif
    uint32_t h = 0;
#ifndef COUNT
    t1 = high_resolution_clock::now();
#endif
#ifdef COUNT
    count.reset();
#endif
    for (int qi = 64; qi < 64 + 100; qi++) {
      h += templated_cmov(arr, arr[qi]);
    }
#ifdef COUNT
    count.accumulate(7);
#endif
#ifndef COUNT
    t2 = high_resolution_clock::now();
    std::cout << double(duration_cast<nanoseconds>(t2 - t1).count()) / 100
              << std::endl;
#endif
    uint32_t i = 0;
#ifndef COUNT
    t1 = high_resolution_clock::now();
#endif
#ifdef COUNT
    count.reset();
#endif
    for (int qi = 64; qi < 64 + 100; qi++) {
      i += templated_binary(arr, arr[qi]);
    }
#ifdef COUNT
    count.accumulate(8);
#endif
#ifndef COUNT
    t2 = high_resolution_clock::now();
    std::cout << double(duration_cast<nanoseconds>(t2 - t1).count()) / 100
              << std::endl;
#endif
    if (a != b || b != c || c != d || d != e || e != f || f != g || g != h ||
        h != i) {
      std::cerr << "Error on line " << j << "\n";
      std::cerr << a << ", " << b << ", " << c << ", " << d << ", " << e << ", "
                << f << ", " << g << ", " << h << ", " << i << std::endl;
      exit(1);
    }
  }

#ifdef COUNT
  for (uint16_t i = 0; i < 9; ++i) {
    std::cout << types[i] << "\n";
    count.output_counters(i, 100 * 1000);
  }
#endif

  return 0;
}
