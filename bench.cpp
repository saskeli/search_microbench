#include <benchmark/benchmark.h>

#include <algorithm>
#include <array>
#include <cstdint>
#include <random>
#include <string>
#include <type_traits>

#include "searchers.hpp"

#define SEARCH_BENCH(type, dtyp, ityp, ell)                                    \
  void BM_##type##dtyp##ityp##ell(benchmark::State& state) {                   \
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
  BENCHMARK(BM_##type##dtyp##ityp##ell);

#define SEARCHTYP(typ)  \
  SEARCH_BENCH(typ, int32_t, uint8_t, 2)  \
  SEARCH_BENCH(typ, int32_t, uint8_t, 4)  \
  SEARCH_BENCH(typ, int32_t, uint8_t, 8)  \
  SEARCH_BENCH(typ, int32_t, uint8_t, 16) \
  SEARCH_BENCH(typ, int32_t, uint8_t, 32) \
  SEARCH_BENCH(typ, int32_t, uint8_t, 64) \
  SEARCH_BENCH(typ, int32_t, uint8_t, 128)

SEARCHTYP(branchless_sub)
SEARCHTYP(branchless_sub_fix)
SEARCHTYP(linear_scan_sub)

template <class T, class index_t, index_t ell>
void run_bench(benchmark::State& state, auto q_f) {
  std::mt19937_64 gen;
  std::array<T, ell> arr;
  std::uniform_int_distribution<T> dist(
      0, std::is_signed<T>::value ? (~T(0)) >> 1 : ~T(0));
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
  TYPED_BENCH(typ, dtyp, uint16_t, 1024)

#define SEARCH_DTYPED_BENCH(typ)      \
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

BENCHMARK_MAIN();