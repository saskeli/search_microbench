
#include <array>
#include <cstdint>
#include <string>

#include "../googletest/googletest/include/gtest/gtest.h"
#include "../searchers.hpp"

template <uint8_t ell>
uint8_t binary_int(const int* arr, const int q)  {
  return binary<int, uint8_t, ell>(arr, q);
}

template <uint8_t ell>
uint8_t templated_binary_int(const int* arr, const int q) {
  return templated_binary<int, uint8_t, ell>(arr, q);
}

template <uint8_t ell>
uint8_t templated_cmov_int(const int* arr, const int q) {
  return templated_cmov<int, uint8_t, ell>(arr, q);
}

template <uint8_t ell>
uint8_t branchless_cmov_int(const int* arr, const int q) {
  return branchless_cmov<int, uint8_t, ell>(arr, q);
}

template <uint8_t ell>
uint8_t linear_scan_int(const int* arr, const int q) {
  return linear_scan<int, uint8_t, ell>(arr, q);
}

template <uint8_t ell>
uint8_t linear_scan_cmov_int(const int* arr, const int q) {
  return linear_scan_cmov<int, uint8_t, ell>(arr, q);
}

template <class A>
std::string array_to_string(const A& a) {
  std::string ret = "";
  for (size_t i = 0; i < a.size(); ++i) {
    ret.append(std::to_string(a[i]));
    if (i < a.size() - 1) {
      ret.append(", ");
    }
  }
  return ret;
}

#define BASIC_TEST(typ)                                                        \
  TEST(Basic, typ) {                                                           \
    std::array<int, 8> arr = {0, 8, 13, 14, 27, 27, 30, 33};                   \
    std::array<int, 8> q_a = {1, 4, 9, 13, 14, 27, 29, 31};                    \
    std::array<int, 8> res = {1, 1, 2, 2, 3, 4, 6, 7};                         \
    for (size_t i = 0; i < arr.size(); ++i) {                                  \
      int q_res = typ<8>(arr.data(), q_a[i]);                                  \
      ASSERT_EQ(q_res, res[i])                                                 \
          << "(" << array_to_string(arr) << "), " << q_a[i] << " -> " << q_res \
          << ", (" << res[i] << ")";                                           \
    }                                                                          \
  }

BASIC_TEST(binary_int)
BASIC_TEST(templated_binary_int)
BASIC_TEST(templated_cmov_int)
BASIC_TEST(branchless_cmov_int)
BASIC_TEST(branchless_sub)
BASIC_TEST(branchless_sub_fix)
BASIC_TEST(linear_scan_int)
BASIC_TEST(linear_scan_cmov_int)
BASIC_TEST(linear_scan_sub)

TEST(Case, debug1) {
  const uint16_t size = 64;
  std::array<int, size> arr = {
      10272454,   68360513,   76690247,   209466417,  235926313,  
      241515709,  272702102,  290934651,  304698606,  338471504,  
      367620617,  373372613,  404547213,  455104038,  474666992,  
      598070370,  637865885,  648353503,  661783701,  679786904,  
      775872960,  842301111,  856284451,  905725464,  906426393,  
      1019536503, 1042336268, 1081607364, 1174419119, 1175147282,
      1357981149, 1373881347, 1407628058, 1408192422, 1457572653, 
      1463208467, 1516222419, 1558727304, 1590527846, 1595864830, 
      1627234529, 1701252276, 1712145580, 1713538653, 1713919276, 
      1718589230, 1749605806, 1793167292, 1823491298, 1886415446, 
      1945173367, 1961459714, 1966527063, 2005735222, 2055500373, 
      2056230259, 2060494293, 2072082348, 2078109053, 2080627695,
      2084332121, 2106917019, 2132196360, 2139884402};
  int q = 592583170;
  int res = 15;
  ASSERT_EQ(binary_int<size>(arr.data(), q), res);
  ASSERT_EQ(templated_binary_int<size>(arr.data(), q), res);
  ASSERT_EQ(templated_cmov_int<size>(arr.data(), q), res);
  ASSERT_EQ(linear_scan_int<size>(arr.data(), q), res);
  ASSERT_EQ(linear_scan_cmov_int<size>(arr.data(), q), res);
  ASSERT_EQ(linear_scan_sub<size>(arr.data(), q), res);
  
  ASSERT_EQ(branchless_cmov_int<size>(arr.data(), q), res);
  ASSERT_EQ(branchless_sub<size>(arr.data(), q), res);
  ASSERT_EQ(branchless_sub_fix<size>(arr.data(), q), res);
}
