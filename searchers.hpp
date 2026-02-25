#include <cstdint>
#include <type_traits>
#include <emmintrin.h>

template <class T = int, class index_t = uint8_t, index_t ell = 64>
index_t binary(const T* arr, const T q) {
  static_assert(__builtin_popcountll(ell) == 1);
  static_assert(ell >= 2);
  index_t a = 0;
  index_t b = ell;
  index_t m;
  while (b > a) {
    m = (a + b) / 2;
    if (arr[m] < q)
      a = m + 1;
    else
      b = m;
  }
  return a;
}

template <class T = int, class index_t = uint8_t, index_t ell = 64>
inline index_t templated_binary(const T* arr, const T q) {
  static_assert(__builtin_popcountll(ell) == 1);
  static_assert(ell >= 2);
  if constexpr (ell == 2) {
    if (arr[0] < q) {
      return 1;
    } else {
      return 0;
    }
  } else {
    if (arr[ell / 2 - 1] < q) {
      return ell / 2 + templated_binary<T, index_t, ell / 2>(arr + ell / 2, q);
    } else {
      return templated_binary<T, index_t, ell / 2>(arr, q);
    }
  }
}

template <class T = int, class index_t = uint8_t, index_t ell = 64>
index_t templated_cmov(const T* arr, const T q) {
  static_assert(__builtin_popcountll(ell) == 1);
  static_assert(ell >= 2);
  if constexpr (ell == 2) {
    return arr[0] < q;
  } else {
    index_t offset = (arr[ell / 2 - 1] < q) * (ell / 2);
    index_t res = templated_cmov<T, index_t, ell / 2>(arr + offset, q);
    return res + offset;
  }
}

template <class T = int, class index_t = uint8_t, index_t ell = 64>
index_t templated_sub(const T* arr, const T q) {
  static_assert(__builtin_popcountll(ell) == 1);
  static_assert(std::is_signed<T>::value);
  static_assert(ell >= 2);
  const constexpr index_t mag_bits = sizeof(T)* 8 - 1;
  const constexpr uint64_t MASK = uint64_t(1) << mag_bits;
  if constexpr (ell == 2) {
    return ((arr[0] - q) & MASK) >> (mag_bits);
  } else {
    index_t offset = (((arr[ell / 2 - 1] - q) & MASK) >> mag_bits) * (ell / 2);
    index_t res = templated_sub<T, index_t, ell / 2>(arr + offset, q);
    return res + offset;
  }
}

template <class T = int, class index_t = uint8_t, index_t ell = 64>
index_t branchless_cmov(const T* arr, const T q) {
  static_assert(__builtin_popcountll(ell) == 1);
  static_assert(ell >= 2);
  index_t idx = (ell >> 1) - 1;
  for (index_t i = ell / 2; i > 0; i /= 2) {
    idx ^= ((arr[idx] < q) * i) | (i / 2);
  }
  return idx;
}

template <class T = int, class index_t = uint8_t, index_t ell = 64>
index_t branchless_sub(const T* arr, const T q) {
  static_assert(__builtin_popcount(ell) == 1);
  static_assert(std::is_signed<T>::value);
  static_assert(ell >= 2);
  const constexpr index_t mag_bits = 63;
  const constexpr uint64_t MASK = uint64_t(1) << mag_bits;
  index_t idx = (ell >> 1) - 1;
  int64_t w_q = q;
  for (index_t i = ell / 2; i > 0; i /= 2) {
    idx ^= (((arr[idx] - w_q) & MASK) >> (mag_bits - __builtin_ctz(i))) | (i / 2);
  }
  return idx;
}

template <class T = int, class index_t = uint8_t, index_t ell = 64>
index_t linear_scan(const T* arr, const T q) {
  static_assert(__builtin_popcount(ell) == 1);
  static_assert(ell >= 2);
  for (index_t i = 0; i < ell; i++) {
    if (arr[i] >= q) return i;
  }
  return ell;
}

template <class T = int, class index_t = uint8_t, index_t ell = 64>
index_t linear_scan_cmov(const T* arr, const T q) {
  static_assert(__builtin_popcount(ell) == 1);
  static_assert(ell >= 2);
  index_t res = 0;
  for (index_t idx = 0; idx < ell; idx++) {
    res += arr[idx] < q;
  }
  return res;
}

template <class T = int, class index_t = uint8_t, index_t ell = 64>
index_t linear_scan_sub(const T* arr, const T q) {
  static_assert(__builtin_popcount(ell) == 1);
  static_assert(ell >= 2);
  static_assert(std::is_signed<T>::value);
  const constexpr index_t r_shift = sizeof(T) * 8 - 1;
  index_t res = 0;
  for (index_t idx = 0; idx < ell; idx++) {
    if constexpr (sizeof(T) == 1) {
      res += uint8_t(arr[idx] - q) >> r_shift;
    } else if constexpr (sizeof(T) == 2) {
      res += uint16_t(arr[idx] - q) >> r_shift;
    } else if constexpr (sizeof(T) == 4) {
      res += uint32_t(arr[idx] - q) >> r_shift;
    } else {
      res += uint64_t(arr[idx] - q) >> r_shift;
    }
  }
  return res;
}
