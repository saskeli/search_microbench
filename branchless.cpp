#include <stddef.h>

#include <bitset>
#include <cstdint>
#include <iostream>

#ifndef COUNT
#include <chrono>
#endif

#ifdef COUNT
#include "counters.hpp"
#endif

uint8_t binary(int* arr, int q) {
  uint8_t a = 0;
  uint8_t b = 64;
  uint8_t m;
  while (b > a) {
    m = (a + b) / 2;
    if (arr[m] < q)
      a = m + 1;
    else
      b = m;
  }
  return a;
}

template <uint8_t ell = 64>
uint8_t templated_cmov(int* arr, int q) {
  if constexpr (ell == 2) {
    return arr[0] < q;
  }
  uint64_t offset = (arr[ell / 2 - 1] < q) * (ell / 2);
  uint8_t res = templated_cmov<ell / 2>(arr + offset, q);
  return res + offset;
}

uint8_t branchless_cmov(int* arr, int q) {
  uint8_t idx = (uint8_t(1) << 5) - 1;
  idx ^= ((arr[idx] < q) << 5) | (uint8_t(1) << 4);
  idx ^= ((arr[idx] < q) << 4) | (uint8_t(1) << 3);
  idx ^= ((arr[idx] < q) << 3) | (uint8_t(1) << 2);
  idx ^= ((arr[idx] < q) << 2) | (uint8_t(1) << 1);
  idx ^= ((arr[idx] < q) << 1) | uint8_t(1);
  return idx ^ (arr[idx] < q);
}

uint8_t branchless_sub(int* arr, int q) {
  constexpr uint32_t MASK = uint32_t(1) << 31;
  uint8_t idx = (uint8_t(1) << 5) - 1;
  idx ^= (((arr[idx] - q) & MASK) >> 26) | (uint8_t(1) << 4);
  idx ^= (((arr[idx] - q) & MASK) >> 27) | (uint8_t(1) << 3);
  idx ^= (((arr[idx] - q) & MASK) >> 28) | (uint8_t(1) << 2);
  idx ^= (((arr[idx] - q) & MASK) >> 29) | (uint8_t(1) << 1);
  idx ^= (((arr[idx] - q) & MASK) >> 30) | uint8_t(1);
  return idx ^ (((arr[idx] - q) & MASK) >> 31);
}

uint8_t branchless_sub_fix(uint32_t* arr, uint32_t q) {
  constexpr uint32_t MASK = uint32_t(1) << 31;
  uint8_t idx = (uint8_t(1) << 5) - 1;
  uint32_t v1 = arr[idx];
  uint32_t v2 = q;
  uint32_t res = MASK & v1 & v2;
  v1 ^= res;
  v2 ^= res;
  v1 |= (MASK - 1) * (v1 >> 31);
  v1 &= (MASK - 1);
  idx ^= (((v1 - v2) & MASK) >> 26) | (uint8_t(1) << 4);

  v1 = arr[idx];
  v2 = q;
  res = MASK & v1 & v2;
  v1 ^= res;
  v2 ^= res;
  v1 |= (MASK - 1) * (v1 >> 31);
  v1 &= (MASK - 1);
  idx ^= (((v1 - v2) & MASK) >> 27) | (uint8_t(1) << 3);

  v1 = arr[idx];
  v2 = q;
  res = MASK & v1 & v2;
  v1 ^= res;
  v2 ^= res;
  v1 |= (MASK - 1) * (v1 >> 31);
  v1 &= (MASK - 1);
  idx ^= (((v1 - v2) & MASK) >> 28) | (uint8_t(1) << 2);

  v1 = arr[idx];
  v2 = q;
  res = MASK & v1 & v2;
  v1 ^= res;
  v2 ^= res;
  v1 |= (MASK - 1) * (v1 >> 31);
  v1 &= (MASK - 1);
  idx ^= (((v1 - v2) & MASK) >> 29) | (uint8_t(1) << 1);

  v1 = arr[idx];
  v2 = q;
  res = MASK & v1 & v2;
  v1 ^= res;
  v2 ^= res;
  v1 |= (MASK - 1) * (v1 >> 31);
  v1 &= (MASK - 1);
  idx ^= (((v1 - v2) & MASK) >> 30) | uint8_t(1);

  v1 = arr[idx];
  v2 = q;
  res = MASK & v1 & v2;
  v1 ^= res;
  v2 ^= res;
  v1 |= (MASK - 1) * (v1 >> 31);
  v1 &= (MASK - 1);
  return idx ^ (((v1 - v2) & MASK) >> 31);
}

uint8_t linear_scan(int* arr, int q) {
  for (uint8_t i = 0; i < 64; i++) {
    if (arr[i] >= q) return i;
  }
  return 63;
}

uint8_t linear_scan_cmov(int* arr, int q) {
  uint8_t res = 0;
  for (uint8_t idx = 0; idx < 64; idx++) {
    res += arr[idx] < q;
  }
  return res;
}

uint8_t linear_scan_sub(int* arr, int q) {
  uint8_t res = 0;
  for (uint8_t idx = 0; idx < 64; idx++) {
    res += uint32_t(arr[idx] - q) >> 31;
  }
  return res;
}

int main() {
  int arr[64 + 100];
#ifndef COUNT
  using std::chrono::duration_cast;
  using std::chrono::high_resolution_clock;
  using std::chrono::nanoseconds;
#endif

#ifdef COUNT
  Counters<8> count;
#endif

  std::cout
      << "binary\tbranchless_cmov\tbranchless_sub\tbranchless_sub_fix\tlinear_"
         "scan\tlinear_scan_cmov\tlinear_scan_sub\ttemplated_cmov"
      << std::endl;

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
    if (a != b || b != c || c != d || d != e || e != f || f != g || g != h) {
      std::cerr << "Error on line " << j << "\n";
      std::cerr << a << ", " << b << ", " << c << ", " << d << ", " << e << ", "
                << f << ", " << g << ", " << h << std::endl;
      exit(1);
    }
  }

#ifdef COUNT
  for (uint16_t i = 0; i < 8; ++i) {
    std::cout << double(count.get(i)[0]) / (100 * 1000) << "\t";
  }
  std::cout << "Cycles per search\n";
  for (uint16_t i = 0; i < 8; ++i) {
    std::cout << double(count.get(i)[1]) / (100 * 1000) << "\t";
  }
  std::cout << "Instructions per search\n";
  for (uint16_t i = 0; i < 8; ++i) {
    std::cout << double(count.get(i)[2]) / (100 * 1000) << "\t";
  }
  std::cout << "Banch misspredictions per search\n";
  for (uint16_t i = 0; i < 8; ++i) {
    std::cout << double(count.get(i)[3]) / (100 * 1000) << "\t";
  }
  std::cout << "L1 cache misses per search\n";
  for (uint16_t i = 0; i < 8; ++i) {
    auto cn = count.get(i);
    std::cout << double(cn[1]) / cn[0] << "\t";
  }
  std::cout << "IPC\n";
#endif

  return 0;
}
