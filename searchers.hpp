#include <bitset>
#include <cstdint>

template <uint8_t ell = 64>
uint8_t binary(int* arr, int q) {
  static_assert(__builtin_popcount(ell) == 1);
  static_assert(ell <= 128);
  static_assert(ell >= 2);
  uint8_t a = 0;
  uint8_t b = ell;
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
uint8_t templated_binary(int* arr, int q) {
  static_assert(__builtin_popcount(ell) == 1);
  static_assert(ell <= 128);
  static_assert(ell >= 2);
  if constexpr (ell == 2) {
    if (arr[0] < q) {
      return 1;
    } else {
      return 0;
    }
  } else {
    if (arr[ell / 2 - 1] < q) {
      return ell / 2 + templated_binary<ell / 2>(arr + ell / 2, q);
    } else {
      return templated_binary<ell / 2>(arr, q);
    }
  }
}

template <uint8_t ell = 64>
uint8_t templated_cmov(int* arr, int q) {
  static_assert(__builtin_popcount(ell) == 1);
  static_assert(ell <= 128);
  static_assert(ell >= 2);
  if constexpr (ell == 2) {
    return arr[0] < q;
  } else {
    uint64_t offset = (arr[ell / 2 - 1] < q) * (ell / 2);
    uint8_t res = templated_cmov<ell / 2>(arr + offset, q);
    return res + offset;
  }
}

template <uint8_t ell = 64>
uint8_t branchless_cmov(int* arr, int q) {
  static_assert(__builtin_popcount(ell) == 1);
  static_assert(ell <= 128);
  static_assert(ell >= 2);
  uint8_t idx = (ell >> 1) - 1;
  for (uint8_t i = ell / 2; i > 0; i /= 2) {
    idx ^= ((arr[idx] < q) * i) | (i / 2);
  }
  return idx;
}

template <uint8_t ell = 64>
uint8_t branchless_sub(int* arr, int q) {
  static_assert(__builtin_popcount(ell) == 1);
  static_assert(ell <= 128);
  static_assert(ell >= 2);
  constexpr uint32_t MASK = uint32_t(1) << 31;
  uint8_t idx = (ell >> 1) - 1;
  for (uint8_t i = ell / 2; i > 0; i /= 2) {
    idx ^= (((arr[idx] - q) & MASK) >> (31 - __builtin_ctz(i))) | (i / 2);
  }
  return idx;
}

template <uint8_t ell = 64>
uint8_t branchless_sub_fix(int* arr, uint32_t q) {
  static_assert(__builtin_popcount(ell) == 1);
  static_assert(ell <= 128);
  static_assert(ell >= 2);
  constexpr uint32_t MASK = uint32_t(1) << 31;
  uint8_t idx = (ell >> 1) - 1;
  uint32_t v1;
  uint32_t v2;
  uint32_t res;
  for (uint8_t i = ell / 2; i > 0; i /= 2) {
    v1 = arr[idx];
    v2 = q;
    res = MASK & v1 & v2;
    v1 ^= res;
    v2 ^= res;
    v1 |= (MASK - 1) * (v1 >> 31);
    v1 &= (MASK - 1);
    idx ^= (((v1 - v2) & MASK) >> (31 - __builtin_ctz(i))) | (i / 2);
  }
  return idx;
}

template <uint64_t ell = 64>
uint8_t linear_scan(int* arr, int q) {
  static_assert(__builtin_popcount(ell) == 1);
  static_assert(ell <= 128);
  static_assert(ell >= 2);
  for (uint8_t i = 0; i < ell; i++) {
    if (arr[i] >= q) return i;
  }
  return 64;
}

template <uint64_t ell = 64>
uint8_t linear_scan_cmov(int* arr, int q) {
  static_assert(__builtin_popcount(ell) == 1);
  static_assert(ell <= 128);
  static_assert(ell >= 2);
  uint8_t res = 0;
  for (uint8_t idx = 0; idx < ell; idx++) {
    res += arr[idx] < q;
  }
  return res;
}

template <uint64_t ell = 64>
uint8_t linear_scan_sub(int* arr, int q) {
  static_assert(__builtin_popcount(ell) == 1);
  static_assert(ell <= 128);
  static_assert(ell >= 2);
  uint8_t res = 0;
  for (uint8_t idx = 0; idx < ell; idx++) {
    res += uint32_t(arr[idx] - q) >> 31;
  }
  return res;
}
