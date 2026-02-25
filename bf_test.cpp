#include <algorithm>
#include <array>
#include <cstdint>
#include <iostream>
#include <random>
#include <type_traits>

#include "searchers.hpp"

std::array<std::string, 8> searcher_names = {
    "templated_binary", "templated_cmov", "branchless_cmov", "linear_scan",
    "linear_scan_cmov", "branchless_sub", "linear_scan_sub"};

template <class dtype>
constexpr dtype max_val() {
  return std::is_signed<dtype>::value
             ? (((dtype(1) << (sizeof(dtype) * 8 - 2)) - 1) << 1) | 1
             : ~dtype(0);
}

template <class dtype, uint16_t size>
void run_test(const std::string dtype_name) {
  std::array<dtype, size> arr;
  std::array<dtype, 100000> q_a;
  std::mt19937_64 gen;
  dtype m_val = max_val<dtype>();
  const constexpr size_t type_c = std::is_signed<dtype>::value ? 7 : 5;
  std::array<uint16_t, type_c> results;
  std::uniform_int_distribution<dtype> uniform_dist(0, m_val);
  for (size_t i = 0; i < arr.size(); ++i) {
    arr[i] = uniform_dist(gen);
  }
  std::sort(arr.begin(), arr.end());
  dtype lim = arr[arr.size() - 1];
  std::uniform_int_distribution<dtype> q_dist(0, lim - 2);
  for (size_t i = 0; i < q_a.size(); ++i) {
    q_a[i] = q_dist(gen);
  }
  for (auto q : q_a) {
    uint16_t bin_res = binary<dtype, uint16_t, size>(arr.data(), q);
    results[0] = templated_binary<dtype, uint16_t, size>(arr.data(), q);
    results[1] = templated_cmov<dtype, uint16_t, size>(arr.data(), q);
    results[2] = branchless_cmov<dtype, uint16_t, size>(arr.data(), q);
    results[3] = linear_scan<dtype, uint16_t, size>(arr.data(), q);
    results[4] = linear_scan_cmov<dtype, uint16_t, size>(arr.data(), q);

    if constexpr (std::is_signed<dtype>::value) {
      results[5] = branchless_sub<dtype, uint16_t, size>(arr.data(), q);
      results[6] = linear_scan_sub<dtype, uint16_t, size>(arr.data(), q);
    }
    for (size_t i = 0; i < type_c; ++i) {
      if (bin_res != results[i]) {
        std::cout << "std::array<" << dtype_name << ", " << size << "> arr = {";
        bool first = true;
        for (auto e : arr) {
          if (first) {
            first = false;
          } else {
            std::cout << ", ";
          }
          std::cout << (std::is_signed<dtype>::value ? int64_t(e)
                                                     : uint64_t(e));
        }
        std::cout << "};" << std::endl;
        std::cout << dtype_name << " q = "
                  << (std::is_signed<dtype>::value ? int64_t(q) : uint64_t(q))
                  << ";" << std::endl;
        std::cout << "uint16_t res = " << bin_res << ";" << std::endl;
        std::cerr << "\n"
                  << searcher_names[i] << ": "
                  << (std::is_signed<dtype>::value ? int64_t(q) : uint64_t(q))
                  << " -> " << results[i] << std::endl;
        exit(1);
      }
    }
  }
}

#define TEST_S(dtyp, si) run_test<dtyp, si>(#dtyp);

#define TEST_D(dtyp) \
  TEST_S(dtyp, 2)    \
  TEST_S(dtyp, 4)    \
  TEST_S(dtyp, 8)    \
  TEST_S(dtyp, 16)   \
  TEST_S(dtyp, 32)   \
  TEST_S(dtyp, 64)   \
  TEST_S(dtyp, 128)  \
  TEST_S(dtyp, 256)  \
  TEST_S(dtyp, 512)  \
  TEST_S(dtyp, 1024)

#define R_TEST()   \
  TEST_D(uint8_t)  \
  TEST_D(uint32_t) \
  TEST_D(uint64_t) \
  TEST_D(int8_t)   \
  TEST_D(int16_t)  \
  TEST_D(int32_t)  \
  TEST_D(int64_t)

int main() {
  uint64_t epoch = 0;
  while (true) {
    std::cerr << epoch++ << "\r";
    R_TEST();
  }
}