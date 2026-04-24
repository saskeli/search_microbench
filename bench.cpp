#include <benchmark/benchmark.h>

#include <algorithm>
#include <array>
#include <cstdint>
#include <random>
#include <string>
#include <type_traits>
#include <iostream>

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
  std::uniform_int_distribution<T> q_dist(arr[0], max_val<T>() - 1);
  std::array<T, 100000> q_arr;
  bool checksum_set = false;
  for (auto _ : state) {
    state.PauseTiming();
    for (size_t i = 0; i < q_arr.size(); ++i) {
      q_arr[i] = q_dist(gen);
    }
    uint64_t checksum = 0;
    state.ResumeTiming();
    for (auto q : q_arr) {
#ifdef DEPENDENCE_INSERTION
      q += checksum & 0b1;
#endif
      checksum += q_f(arr, q);
    }
    state.PauseTiming();
    if (not checksum_set) {
      state.SetLabel(std::to_string(checksum));
      checksum_set = true;
    }
    state.ResumeTiming();
  }
}

#define TYPED_BENCH(typ, dtyp, ityp, ell)                                 \
  void BM_##typ##dtyp##ityp##ell(benchmark::State& state) {               \
    run_bench<dtyp, ityp, ell>(state, [](const auto& arr, const auto q) { \
      return typ<dtyp, ityp, ell>(arr.data(), q);                         \
    });                                                                   \
  }                                                                       \
  BENCHMARK(BM_##typ##dtyp##ityp##ell);

#define SEARCH_TYPED_BENCH(typ, dtyp)    \
  TYPED_BENCH(typ, dtyp, uint8_t, 2)     \
  TYPED_BENCH(typ, dtyp, uint8_t, 4)     \
  TYPED_BENCH(typ, dtyp, uint8_t, 8)     \
  TYPED_BENCH(typ, dtyp, uint8_t, 16)    \
  TYPED_BENCH(typ, dtyp, uint8_t, 32)    \
  TYPED_BENCH(typ, dtyp, uint8_t, 64)    \
  TYPED_BENCH(typ, dtyp, uint8_t, 128)   \
  TYPED_BENCH(typ, dtyp, uint16_t, 256)  \
  TYPED_BENCH(typ, dtyp, uint16_t, 512)  \
  TYPED_BENCH(typ, dtyp, uint16_t, 1024) \
  TYPED_BENCH(typ, dtyp, uint16_t, 2048) \
  TYPED_BENCH(typ, dtyp, uint16_t, 4096) \
  TYPED_BENCH(typ, dtyp, uint16_t, 8192)

#define SEARCH_DTYPED_BENCH(typ)   \
  SEARCH_TYPED_BENCH(typ, int8_t)  \
  SEARCH_TYPED_BENCH(typ, int16_t) \
  SEARCH_TYPED_BENCH(typ, int32_t) \
  SEARCH_TYPED_BENCH(typ, int64_t)

SEARCH_DTYPED_BENCH(binary)
SEARCH_DTYPED_BENCH(templated_binary)
SEARCH_DTYPED_BENCH(templated_cmov)
SEARCH_DTYPED_BENCH(branchless_cmov)
SEARCH_DTYPED_BENCH(linear_scan)
SEARCH_DTYPED_BENCH(linear_scan_cmov)

template <class T, class index_t, index_t ell>
void run_fp(benchmark::State& state, auto q_f) {
  uint32_t divisor = 100000000;
  std::mt19937_64 gen;
  std::array<T, ell> arr;
  std::uniform_int_distribution<uint32_t> dist(0, divisor);
  arr[0] = 0;
  for (index_t i = 1; i < ell; ++i) {
    arr[i] = T(dist(gen)) / divisor;
  }
  std::sort(arr.begin(), arr.end());
  std::array<T, 100000> q_arr;
  bool checksum_set = false;
  for (auto _ : state) {
    state.PauseTiming();
    for (size_t i = 0; i < q_arr.size(); ++i) {
      q_arr[i] = T(dist(gen)) / divisor;
    }
    uint64_t checksum = 0;
    state.ResumeTiming();
    for (auto q : q_arr) {
#ifdef DEPENDENCE_INSERTION
      q *= (checksum & 0b1) * 0.99 + (~checksum & 0b1) * 1;
#endif
      checksum += q_f(arr, q);
    }
    state.PauseTiming();
    if (not checksum_set) {
      state.SetLabel(std::to_string(checksum));
      checksum_set = true;
    }
    state.ResumeTiming();
  }
}

#define TYPED_FP(typ, dtyp, ityp, ell)                                 \
  void FP_##typ##dtyp##ityp##ell(benchmark::State& state) {            \
    run_fp<dtyp, ityp, ell>(state, [](const auto& arr, const auto q) { \
      return typ<dtyp, ityp, ell>(arr.data(), q);                      \
    });                                                                \
  }                                                                    \
  BENCHMARK(FP_##typ##dtyp##ityp##ell);

#define SEARCH_TYPED_FP(typ, dtyp)    \
  TYPED_FP(typ, dtyp, uint8_t, 2)     \
  TYPED_FP(typ, dtyp, uint8_t, 4)     \
  TYPED_FP(typ, dtyp, uint8_t, 8)     \
  TYPED_FP(typ, dtyp, uint8_t, 16)    \
  TYPED_FP(typ, dtyp, uint8_t, 32)    \
  TYPED_FP(typ, dtyp, uint8_t, 64)    \
  TYPED_FP(typ, dtyp, uint8_t, 128)   \
  TYPED_FP(typ, dtyp, uint16_t, 256)  \
  TYPED_FP(typ, dtyp, uint16_t, 512)  \
  TYPED_FP(typ, dtyp, uint16_t, 1024) \
  TYPED_FP(typ, dtyp, uint16_t, 2048) \
  TYPED_FP(typ, dtyp, uint16_t, 4096) \
  TYPED_FP(typ, dtyp, uint16_t, 8192)

#define SEARCH_DTYPED_FP(typ) \
  SEARCH_TYPED_FP(typ, float) \
  SEARCH_TYPED_FP(typ, double)

SEARCH_DTYPED_FP(binary)
SEARCH_DTYPED_FP(templated_binary)
SEARCH_DTYPED_FP(templated_cmov)
SEARCH_DTYPED_FP(branchless_cmov)
SEARCH_DTYPED_FP(linear_scan)
SEARCH_DTYPED_FP(linear_scan_cmov)

template<class T>
struct Decimal{
  T whole;
  T partial;

  bool operator<(const Decimal& rhs) const {
    if (whole == rhs.whole) {
      return partial < rhs.partial;
    }
    return whole < rhs.whole;
  }

  bool operator>(const Decimal& rhs) const {
    return rhs < *this;
  }

  bool operator==(const Decimal& rhs) const {
    return whole == rhs.whole && partial == rhs.partial;
  }

  bool operator<=(const Decimal& rhs) const {
    if (whole == rhs.whole) {
      return partial <= rhs.partial;
    }
    return whole < rhs.whole;
  }

  bool operator>=(const Decimal& rhs) const {
    if (whole == rhs.whole) {
      return partial >= rhs.partial;
    }
    return whole > rhs.whole;
  }
};

template <class T, class index_t, index_t ell>
void run_dec(benchmark::State& state, auto q_f) {
  std::mt19937_64 gen;
  std::array<Decimal<T>, ell> arr;
  std::uniform_int_distribution<T> dist(0, max_val<T>());
  arr[0] = {0, 0};
  for (index_t i = 1; i < ell; ++i) {
    arr[i] = {dist(gen), dist(gen)};
  }
  std::sort(arr.begin(), arr.end());
  std::array<Decimal<T>, 100000> q_arr;
  bool checksum_set = false;
  for (auto _ : state) {
    state.PauseTiming();
    for (size_t i = 0; i < q_arr.size(); ++i) {
      q_arr[i] = {dist(gen), dist(gen)};
    }
    uint64_t checksum = 0;
    state.ResumeTiming();
    for (auto q : q_arr) {
#ifdef DEPENDENCE_INSERTION
      q.partial += checksum & 0b1;
#endif
      checksum += q_f(arr, q);
    }
    state.PauseTiming();
    if (not checksum_set) {
      state.SetLabel(std::to_string(checksum));
      checksum_set = true;
    }
    state.ResumeTiming();
  }
}

#define TYPED_DEC(typ, dtyp, ityp, ell)                                 \
  void DEC_##typ##dtyp##ityp##ell(benchmark::State& state) {            \
    run_dec<dtyp, ityp, ell>(state, [](const auto& arr, const auto q) { \
      return typ<Decimal<dtyp>, ityp, ell>(arr.data(), q);                      \
    });                                                                \
  }                                                                    \
  BENCHMARK(DEC_##typ##dtyp##ityp##ell);

#define SEARCH_TYPED_DEC(typ, dtyp)    \
  TYPED_DEC(typ, dtyp, uint8_t, 2)     \
  TYPED_DEC(typ, dtyp, uint8_t, 4)     \
  TYPED_DEC(typ, dtyp, uint8_t, 8)     \
  TYPED_DEC(typ, dtyp, uint8_t, 16)    \
  TYPED_DEC(typ, dtyp, uint8_t, 32)    \
  TYPED_DEC(typ, dtyp, uint8_t, 64)    \
  TYPED_DEC(typ, dtyp, uint8_t, 128)   \
  TYPED_DEC(typ, dtyp, uint16_t, 256)  \
  TYPED_DEC(typ, dtyp, uint16_t, 512)  \
  TYPED_DEC(typ, dtyp, uint16_t, 1024) \
  TYPED_DEC(typ, dtyp, uint16_t, 2048) \
  TYPED_DEC(typ, dtyp, uint16_t, 4096) \
  TYPED_DEC(typ, dtyp, uint16_t, 8192)

#define SEARCH_DTYPED_DEC(typ) \
  SEARCH_TYPED_DEC(typ, uint8_t) \
  SEARCH_TYPED_DEC(typ, uint16_t) \
  SEARCH_TYPED_DEC(typ, uint32_t) \
  SEARCH_TYPED_DEC(typ, uint64_t) \

SEARCH_DTYPED_DEC(binary)
SEARCH_DTYPED_DEC(templated_binary)
SEARCH_DTYPED_DEC(templated_cmov)
SEARCH_DTYPED_DEC(branchless_cmov)
SEARCH_DTYPED_DEC(linear_scan)
SEARCH_DTYPED_DEC(linear_scan_cmov)

BENCHMARK_MAIN();