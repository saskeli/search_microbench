#include <benchmark/benchmark.h>

#include <algorithm>
#include <array>
#include <cstdint>
#include <random>
#include <string>

#include "searchers.hpp"

#define SEARCH_BENCH(type, ell)                                    \
  void BM_##type##ell(benchmark::State& state) {                   \
    std::array<int, ell> arr;                                      \
    std::array<int, 100000> q_arr;                                 \
    std::mt19937 gen;                                              \
    std::uniform_int_distribution<int> uniform_dist(0, INT32_MAX); \
    uint64_t checksum = 0;                                         \
    int lim = 0;                                                   \
    for (size_t i = 0; i < ell; ++i) {                             \
      arr[i] = uniform_dist(gen);                                  \
      lim = std::max(lim, arr[i]);                                 \
    }                                                              \
    std::uniform_int_distribution<int> q_dist(0, lim - 1);         \
    for (size_t i = 0; i < q_arr.size(); ++i) {                    \
      q_arr[i] = q_dist(gen);                                      \
    }                                                              \
    std::sort(arr.begin(), arr.end());                             \
    for (auto _ : state) {                                         \
      checksum = 0;                                                \
      for (auto q : q_arr) {                                       \
        q += (checksum & 0b1) * 2 - 1;                             \
        checksum += type<ell>(arr.data(), q);                      \
      }                                                            \
    }                                                              \
    state.SetLabel(std::to_string(checksum));                      \
  }                                                                \
  BENCHMARK(BM_##type##ell);

#define SEARCHTYP(typ)  \
  SEARCH_BENCH(typ, 2)  \
  SEARCH_BENCH(typ, 4)  \
  SEARCH_BENCH(typ, 8)  \
  SEARCH_BENCH(typ, 16) \
  SEARCH_BENCH(typ, 32) \
  SEARCH_BENCH(typ, 64) \
  SEARCH_BENCH(typ, 128)

SEARCHTYP(binary)
SEARCHTYP(templated_binary)
SEARCHTYP(templated_cmov)
SEARCHTYP(branchless_cmov)
SEARCHTYP(branchless_sub)
SEARCHTYP(branchless_sub_fix)
SEARCHTYP(linear_scan)
SEARCHTYP(linear_scan_cmov)
SEARCHTYP(linear_scan_sub)

BENCHMARK_MAIN();