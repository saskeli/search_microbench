#include <benchmark/benchmark.h>

#include <algorithm>
#include <array>
#include <cstdint>
#include <random>
#include <string>
#include <type_traits>

#include "searchers.hpp"

template <class dtype>
constexpr dtype max_val() {
  return std::is_signed<dtype>::value
             ? (((dtype(1) << (sizeof(dtype) * 8 - 2)) - 1) << 1) | 1
             : ~dtype(0);
}

template <class T, class index_t, index_t ell>
void run_bench(benchmark::State& state, auto q_f) {
  std::mt19937_64 gen;
  std::array<T, ell> arr;
  std::uniform_int_distribution<T> dist(0, max_val<T>());
  for (index_t i = 0; i < ell; ++i) {
    arr[i] = dist(gen);
  }
  std::sort(arr.begin(), arr.end());
  std::array<T, 100000> q_arr;
  std::uniform_int_distribution<T> q_dist(0, arr[arr.size() - 1] - 2);
  for (size_t i = 0; i < q_arr.size(); ++i) {
    q_arr[i] = q_dist(gen);
  }
  uint64_t checksum = 0;
  for (auto _ : state) {
    checksum = 0;
    for (auto q : q_arr) {
      q += checksum & 0b1;
      checksum += q_f(arr, q);
    }
  }
  state.SetLabel(std::to_string(checksum));
}

#define TYPED_BENCH(typ, dtyp, ityp, ell)                                 \
  void BM_##typ##dtyp##ityp##ell(benchmark::State& state) {               \
    run_bench<dtyp, ityp, ell>(state, [](const auto& arr, const auto q) { \
      return typ<dtyp, ityp, ell>(arr.data(), q);                         \
    });                                                                   \
  }                                                                       \
  BENCHMARK(BM_##typ##dtyp##ityp##ell);

#define SEARCH_TYPED_BENCH(typ, dtyp)   \
  TYPED_BENCH(typ, dtyp, uint8_t, 2)    \
  TYPED_BENCH(typ, dtyp, uint8_t, 4)    \
  TYPED_BENCH(typ, dtyp, uint8_t, 8)    \
  TYPED_BENCH(typ, dtyp, uint8_t, 16)   \
  TYPED_BENCH(typ, dtyp, uint8_t, 32)   \
  TYPED_BENCH(typ, dtyp, uint8_t, 64)   \
  TYPED_BENCH(typ, dtyp, uint8_t, 128)  \
  TYPED_BENCH(typ, dtyp, uint16_t, 256) \
  TYPED_BENCH(typ, dtyp, uint16_t, 512) \
  TYPED_BENCH(typ, dtyp, uint16_t, 1024) \
  TYPED_BENCH(typ, dtyp, uint16_t, 2048) \
  TYPED_BENCH(typ, dtyp, uint16_t, 4096) \
  TYPED_BENCH(typ, dtyp, uint16_t, 8192)

#define SEARCH_DTYPED_BENCH(typ)    \
  SEARCH_TYPED_BENCH(typ, int8_t)   \
  SEARCH_TYPED_BENCH(typ, int16_t)  \
  SEARCH_TYPED_BENCH(typ, int32_t)  \
  SEARCH_TYPED_BENCH(typ, int64_t)  \
  SEARCH_TYPED_BENCH(typ, uint8_t)  \
  SEARCH_TYPED_BENCH(typ, uint16_t) \
  SEARCH_TYPED_BENCH(typ, uint32_t) \
  SEARCH_TYPED_BENCH(typ, uint64_t)

SEARCH_DTYPED_BENCH(binary)
SEARCH_DTYPED_BENCH(templated_binary)
SEARCH_DTYPED_BENCH(templated_cmov)
SEARCH_DTYPED_BENCH(branchless_cmov)
SEARCH_DTYPED_BENCH(linear_scan)
SEARCH_DTYPED_BENCH(linear_scan_cmov)

SEARCH_TYPED_BENCH(branchless_sub, int8_t)
SEARCH_TYPED_BENCH(branchless_sub, int16_t)
SEARCH_TYPED_BENCH(branchless_sub, int32_t)
SEARCH_TYPED_BENCH(branchless_sub, int64_t)
SEARCH_TYPED_BENCH(linear_scan_sub, int8_t)
SEARCH_TYPED_BENCH(linear_scan_sub, int16_t)
SEARCH_TYPED_BENCH(linear_scan_sub, int32_t)
SEARCH_TYPED_BENCH(linear_scan_sub, int64_t)
SEARCH_TYPED_BENCH(templated_sub, int8_t)
SEARCH_TYPED_BENCH(templated_sub, int16_t)
SEARCH_TYPED_BENCH(templated_sub, int32_t)
SEARCH_TYPED_BENCH(templated_sub, int64_t)

BENCHMARK_MAIN();