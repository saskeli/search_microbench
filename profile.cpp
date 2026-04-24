#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <iostream>
#include <random>
#include <string>
#include <type_traits>

#include "counters/counters.hpp"
#include "searchers.hpp"

const constexpr int Q_COUNT = 1000000;

template <class dtype>
constexpr dtype max_val() {
  return std::is_signed<dtype>::value
             ? (((dtype(1) << (sizeof(dtype) * 8 - 2)) - 1) << 1) | 1
             : ~dtype(0);
}

template <class dtype, class ityp, uint16_t size, class cb_t, class count_t>
void run_prof(count_t& counter, uint64_t* data, uint64_t seed, cb_t cb) {
  std::array<dtype, size> arr;
  std::mt19937_64 gen(seed);
  dtype m_val = max_val<dtype>();
  std::uniform_int_distribution<dtype> uniform_dist(0, m_val);
  for (size_t i = 0; i < arr.size(); ++i) {
    arr[i] = uniform_dist(gen);
  }
  std::sort(arr.begin(), arr.end());
  std::uniform_int_distribution<dtype> q_dist(arr[0], m_val - 1);
  dtype* q_a = reinterpret_cast<dtype*>(data);
  for (size_t i = 0; i < Q_COUNT; ++i) {
    q_a[i] = q_dist(gen);
  }
  uint64_t checksum = 0;
  counter.clear();
  for (size_t i = 0; i < Q_COUNT; ++i) {
    dtype q = q_a[i];
#ifdef DEPENDENCE_INSERTION
    q += (checksum & 0b1);
#endif
    checksum += cb(arr.data(), q);
  }
  counter.accumulate();
  counter.output_counters(0, Q_COUNT);
  std::cout << "Checksum: " << checksum << std::endl;
}

template <class dtype, class ityp, uint16_t size, class cb_t, class count_t>
void run_fp(count_t& counter, uint64_t* data, uint64_t seed, cb_t cb) {
  uint32_t divisor = 100000000;
  std::mt19937_64 gen(seed);
  std::array<dtype, size> arr;
  std::uniform_int_distribution<uint32_t> dist(0, divisor);
  arr[0] = 0;
  for (ityp i = 1; i < size; ++i) {
    arr[i] = dtype(dist(gen)) / divisor;
  }
  std::sort(arr.begin(), arr.end());
  dtype* q_a = reinterpret_cast<dtype*>(data);
  for (size_t i = 0; i < Q_COUNT; ++i) {
    q_a[i] = dtype(dist(gen)) / divisor;
  }
  uint64_t checksum = 0;
  counter.clear();
  for (size_t i = 0; i < Q_COUNT; ++i) {
    dtype q = q_a[i];
#ifdef DEPENDENCE_INSERTION
    q *= (checksum & 0b1) * 0.99 + (~checksum & 0b1) * 1;
#endif
    checksum += cb(arr.data(), q);
  }
  counter.accumulate();
  counter.output_counters(0, Q_COUNT);
  std::cout << "Checksum: " << checksum << std::endl;
}

template <class T>
struct Decimal {
  T whole;
  T partial;

  bool operator<(const Decimal& rhs) const {
    if (whole == rhs.whole) {
      return partial < rhs.partial;
    }
    return whole < rhs.whole;
  }

  bool operator>(const Decimal& rhs) const { return rhs < *this; }

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

template <class dtype, class ityp, uint16_t size, class cb_t, class count_t>
void run_dec(count_t& counter, uint64_t* data, uint64_t seed, cb_t cb) {
  std::mt19937_64 gen(seed);
  std::array<Decimal<dtype>, size> arr;
  std::uniform_int_distribution<dtype> dist(0, max_val<dtype>());
  arr[0] = {0, 0};
  for (ityp i = 1; i < size; ++i) {
    arr[i] = {dist(gen), dist(gen)};
  }
  std::sort(arr.begin(), arr.end());
  Decimal<dtype>* q_a = reinterpret_cast<Decimal<dtype>*>(data);
  for (size_t i = 0; i < Q_COUNT; ++i) {
    q_a[i] = {dist(gen), dist(gen)};
  }
  uint64_t checksum = 0;
  counter.clear();
  for (size_t i = 0; i < Q_COUNT; ++i) {
    Decimal<dtype> q = q_a[i];
#ifdef DEPENDENCE_INSERTION
    q.partial += checksum & 0b1;
#endif
    checksum += cb(arr.data(), q);
  }
  counter.accumulate();
  counter.output_counters(0, Q_COUNT);
  std::cout << "Checksum: " << checksum << std::endl;
}

#define TYPED_PROF(typ, dtyp, ityp, ell)                                     \
  std::cout                                                                  \
      << "\n====================== Profiling run ====================\n";    \
  std::cout << "Type: " << #typ << "\nData type: " << #dtyp                  \
            << "\nIndex type: " << #ityp << "\nSize: " << #ell << std::endl; \
  run_prof<dtyp, ityp, ell>(counter, data, seed,                             \
                            [](const auto& arr, const auto q) {              \
                              return typ<dtyp, ityp, ell>(arr, q);           \
                            });

#define SEARCH_TYPED_PROF(typ, dtyp)    \
  TYPED_PROF(typ, dtyp, uint8_t, 2)     \
  TYPED_PROF(typ, dtyp, uint8_t, 4)     \
  TYPED_PROF(typ, dtyp, uint8_t, 8)     \
  TYPED_PROF(typ, dtyp, uint8_t, 16)    \
  TYPED_PROF(typ, dtyp, uint8_t, 32)    \
  TYPED_PROF(typ, dtyp, uint8_t, 64)    \
  TYPED_PROF(typ, dtyp, uint8_t, 128)   \
  TYPED_PROF(typ, dtyp, uint16_t, 256)  \
  TYPED_PROF(typ, dtyp, uint16_t, 512)  \
  TYPED_PROF(typ, dtyp, uint16_t, 1024) \
  TYPED_PROF(typ, dtyp, uint16_t, 2048) \
  TYPED_PROF(typ, dtyp, uint16_t, 4096) \
  TYPED_PROF(typ, dtyp, uint16_t, 8192)

#define SEARCH_DTYPED_PROF(typ)   \
  SEARCH_TYPED_PROF(typ, int8_t)  \
  SEARCH_TYPED_PROF(typ, int16_t) \
  SEARCH_TYPED_PROF(typ, int32_t) \
  SEARCH_TYPED_PROF(typ, int64_t)

#define TYPED_FP(typ, dtyp, ityp, ell)                                       \
  std::cout                                                                  \
      << "\n====================== Profiling run ====================\n";    \
  std::cout << "Type: " << #typ << "\nData type: " << #dtyp                  \
            << "\nIndex type: " << #ityp << "\nSize: " << #ell << std::endl; \
  run_fp<dtyp, ityp, ell>(counter, data, seed,                               \
                          [](const auto& arr, const auto q) {                \
                            return typ<dtyp, ityp, ell>(arr, q);             \
                          });

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

#define TYPED_DEC(typ, dtyp, ityp, ell)                                      \
  std::cout                                                                  \
      << "\n====================== Profiling run ====================\n";    \
  std::cout << "Type: " << #typ << "\nData type: Decimal-" << #dtyp          \
            << "\nIndex type: " << #ityp << "\nSize: " << #ell << std::endl; \
  run_dec<dtyp, ityp, ell>(counter, data, seed,                              \
                           [](const auto& arr, const auto q) {               \
                             return typ<Decimal<dtyp>, ityp, ell>(arr, q);   \
                           });

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

#define SEARCH_DTYPED_DEC(typ)    \
  SEARCH_TYPED_DEC(typ, uint8_t)  \
  SEARCH_TYPED_DEC(typ, uint16_t) \
  SEARCH_TYPED_DEC(typ, uint32_t) \
  SEARCH_TYPED_DEC(typ, uint64_t)

int main(int argc, char const* argv[]) {
  uint64_t seed = 1337;
  if (argc > 1) {
    seed = std::stoull(argv[1]);
  }
  uint64_t* data = (uint64_t*)malloc(2 * Q_COUNT * sizeof(uint64_t));
  count::Counters<false, 1, count::Counter::instructions,
                  count::Counter::branches, count::Counter::branch_miss,
                  count::Counter::L1D_miss, count::Counter::IPC>
      counter;
  SEARCH_DTYPED_PROF(binary)
  SEARCH_DTYPED_PROF(templated_binary)
  SEARCH_DTYPED_PROF(templated_cmov)
  SEARCH_DTYPED_PROF(branchless_cmov)
  SEARCH_DTYPED_PROF(linear_scan)
  SEARCH_DTYPED_PROF(linear_scan_cmov)

  SEARCH_DTYPED_FP(binary)
  SEARCH_DTYPED_FP(templated_binary)
  SEARCH_DTYPED_FP(templated_cmov)
  SEARCH_DTYPED_FP(branchless_cmov)
  SEARCH_DTYPED_FP(linear_scan)
  SEARCH_DTYPED_FP(linear_scan_cmov)

  SEARCH_DTYPED_DEC(binary)
  SEARCH_DTYPED_DEC(templated_binary)
  SEARCH_DTYPED_DEC(templated_cmov)
  SEARCH_DTYPED_DEC(branchless_cmov)
  SEARCH_DTYPED_DEC(linear_scan)
  SEARCH_DTYPED_DEC(linear_scan_cmov)

  return 0;
}
