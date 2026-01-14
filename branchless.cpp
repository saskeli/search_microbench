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
  dtype lim = arr[arr.size() - 1];
  std::uniform_int_distribution<dtype> q_dist(0, lim - 2);
  dtype* q_a = reinterpret_cast<dtype*>(data);
  for (size_t i = 0; i < Q_COUNT; ++i) {
    q_a[i] = q_dist(gen);
  }
  uint64_t checksum = 0;
  counter.clear();
  for (size_t i = 0; i < Q_COUNT; ++i) {
    checksum += cb(arr.data(), q_a[i]);
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

#define SEARCH_DTYPED_PROF(typ)    \
  SEARCH_TYPED_PROF(typ, int8_t)   \
  SEARCH_TYPED_PROF(typ, int16_t)  \
  SEARCH_TYPED_PROF(typ, int32_t)  \
  SEARCH_TYPED_PROF(typ, int64_t)  \
  SEARCH_TYPED_PROF(typ, uint8_t)  \
  SEARCH_TYPED_PROF(typ, uint16_t) \
  SEARCH_TYPED_PROF(typ, uint32_t) \
  SEARCH_TYPED_PROF(typ, uint64_t)

int main(int argc, char const* argv[]) {
  uint64_t seed = 1337;
  if (argc > 1) {
    seed = std::stoull(argv[1]);
  }
  uint64_t* data = (uint64_t*)malloc(Q_COUNT * sizeof(uint64_t));
  count::Default counter;
  SEARCH_DTYPED_PROF(binary)
  SEARCH_DTYPED_PROF(templated_binary)
  SEARCH_DTYPED_PROF(templated_cmov)
  SEARCH_DTYPED_PROF(branchless_cmov)
  SEARCH_DTYPED_PROF(linear_scan)
  SEARCH_DTYPED_PROF(linear_scan_cmov)

  SEARCH_TYPED_PROF(branchless_sub, int8_t)
  SEARCH_TYPED_PROF(branchless_sub, int16_t)
  SEARCH_TYPED_PROF(branchless_sub, int32_t)
  SEARCH_TYPED_PROF(branchless_sub, int64_t)
  SEARCH_TYPED_PROF(linear_scan_sub, int8_t)
  SEARCH_TYPED_PROF(linear_scan_sub, int16_t)
  SEARCH_TYPED_PROF(linear_scan_sub, int32_t)
  SEARCH_TYPED_PROF(linear_scan_sub, int64_t)
  SEARCH_TYPED_PROF(templated_sub, int8_t)
  SEARCH_TYPED_PROF(templated_sub, int16_t)
  SEARCH_TYPED_PROF(templated_sub, int32_t)
  SEARCH_TYPED_PROF(templated_sub, int64_t)
  return 0;
}
