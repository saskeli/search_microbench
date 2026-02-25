
#include <array>
#include <cstdint>
#include <string>

#include "../googletest/googletest/include/gtest/gtest.h"
#include "../searchers.hpp"

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
      int q_res = typ<int, uint8_t, 8>(arr.data(), q_a[i]);                    \
      ASSERT_EQ(q_res, res[i])                                                 \
          << "(" << array_to_string(arr) << "), " << q_a[i] << " -> " << q_res \
          << ", (" << res[i] << ")";                                           \
    }                                                                          \
  }

BASIC_TEST(binary)
BASIC_TEST(templated_binary)
BASIC_TEST(templated_cmov)
BASIC_TEST(templated_sub)
BASIC_TEST(branchless_cmov)
BASIC_TEST(branchless_sub)
BASIC_TEST(linear_scan)
BASIC_TEST(linear_scan_cmov)
BASIC_TEST(linear_scan_sub)

TEST(Case, debug1) {
  const uint16_t size = 64;
  std::array<int, size> arr = {
      10272454,   68360513,   76690247,   209466417,  235926313,  241515709,
      272702102,  290934651,  304698606,  338471504,  367620617,  373372613,
      404547213,  455104038,  474666992,  598070370,  637865885,  648353503,
      661783701,  679786904,  775872960,  842301111,  856284451,  905725464,
      906426393,  1019536503, 1042336268, 1081607364, 1174419119, 1175147282,
      1357981149, 1373881347, 1407628058, 1408192422, 1457572653, 1463208467,
      1516222419, 1558727304, 1590527846, 1595864830, 1627234529, 1701252276,
      1712145580, 1713538653, 1713919276, 1718589230, 1749605806, 1793167292,
      1823491298, 1886415446, 1945173367, 1961459714, 1966527063, 2005735222,
      2055500373, 2056230259, 2060494293, 2072082348, 2078109053, 2080627695,
      2084332121, 2106917019, 2132196360, 2139884402};
  int q = 592583170;
  int res = 15;
  int actual = binary<int, uint8_t, size>(arr.data(), q);
  ASSERT_EQ(actual, res);
  actual = templated_binary<int, uint8_t, size>(arr.data(), q);
  ASSERT_EQ(actual, res);
  actual = templated_cmov<int, uint8_t, size>(arr.data(), q);
  ASSERT_EQ(actual, res);
  actual = templated_sub<int, uint8_t, size>(arr.data(), q);
  ASSERT_EQ(actual, res);
  actual = linear_scan<int, uint8_t, size>(arr.data(), q);
  ASSERT_EQ(actual, res);
  actual = linear_scan_cmov<int, uint8_t, size>(arr.data(), q);
  ASSERT_EQ(actual, res);
  actual = linear_scan_sub<int, uint8_t, size>(arr.data(), q);
  ASSERT_EQ(actual, res);
  actual = branchless_cmov<int, uint8_t, size>(arr.data(), q);
  ASSERT_EQ(actual, res);
  actual = branchless_sub<int, uint8_t, size>(arr.data(), q);
  ASSERT_EQ(actual, res);
}

TEST(Case, debug2) {
  const uint16_t size = 8;
  std::array<int, size> arr = {1, 8, 8, 8, 8, 8, 8, 12};
  for (int i = 1; i < 13; ++i) {
    int a = binary<int, uint16_t, size>(arr.data(), i);
    int b = templated_binary<int, uint16_t, size>(arr.data(), i);
    ASSERT_EQ(a, b);
    b = templated_cmov<int, uint16_t, size>(arr.data(), i);
    ASSERT_EQ(a, b);
    b = templated_sub<int, uint16_t, size>(arr.data(), i);
    ASSERT_EQ(a, b);
    b = linear_scan<int, uint16_t, size>(arr.data(), i);
    ASSERT_EQ(a, b);
    b = linear_scan_cmov<int, uint16_t, size>(arr.data(), i);
    ASSERT_EQ(a, b);
    b = linear_scan_sub<int, uint16_t, size>(arr.data(), i);
    ASSERT_EQ(a, b);
    b = branchless_cmov<int, uint16_t, size>(arr.data(), i);
    ASSERT_EQ(a, b);
    b = branchless_sub<int, uint16_t, size>(arr.data(), i);
    ASSERT_EQ(a, b);
  }
}

TEST(Case, debug3) {
  const constexpr uint16_t size = 512;
  std::array<int8_t, size> arr = {
      0,   0,   0,   0,   1,   1,   2,   2,   2,   2,   2,   2,   2,   3,   3,
      3,   3,   3,   4,   4,   4,   5,   5,   5,   6,   6,   6,   7,   7,   8,
      8,   8,   8,   9,   9,   9,   10,  10,  10,  10,  11,  11,  11,  11,  11,
      11,  11,  11,  11,  12,  12,  12,  12,  12,  12,  13,  13,  13,  14,  14,
      14,  14,  14,  14,  14,  15,  15,  15,  15,  16,  16,  16,  16,  16,  16,
      16,  16,  17,  17,  17,  17,  17,  18,  18,  19,  19,  19,  20,  20,  20,
      21,  21,  21,  21,  23,  23,  24,  24,  25,  25,  25,  25,  25,  25,  25,
      25,  26,  26,  26,  27,  27,  28,  28,  28,  28,  28,  29,  29,  30,  30,
      30,  30,  30,  30,  31,  31,  31,  31,  31,  32,  32,  32,  32,  32,  32,
      32,  32,  32,  33,  34,  34,  35,  35,  35,  35,  35,  35,  36,  36,  36,
      36,  37,  38,  38,  38,  38,  38,  38,  39,  39,  39,  39,  39,  40,  40,
      40,  40,  40,  40,  40,  41,  41,  41,  42,  42,  43,  43,  44,  44,  44,
      45,  45,  45,  45,  46,  46,  46,  46,  46,  46,  46,  46,  47,  47,  47,
      47,  47,  47,  47,  49,  49,  50,  50,  51,  51,  51,  51,  51,  51,  52,
      53,  53,  53,  54,  54,  54,  54,  54,  55,  55,  55,  56,  58,  58,  58,
      58,  58,  59,  59,  59,  59,  59,  59,  60,  60,  60,  61,  61,  61,  62,
      62,  62,  62,  62,  62,  63,  63,  63,  64,  64,  64,  64,  64,  64,  65,
      65,  66,  66,  66,  66,  66,  66,  66,  66,  66,  67,  67,  67,  67,  67,
      68,  68,  68,  69,  69,  70,  70,  70,  70,  70,  71,  71,  71,  71,  71,
      72,  72,  72,  72,  73,  73,  73,  74,  74,  74,  74,  74,  74,  75,  75,
      75,  75,  75,  75,  75,  75,  75,  76,  76,  76,  76,  76,  76,  77,  77,
      77,  77,  77,  78,  78,  78,  79,  79,  79,  79,  79,  80,  80,  80,  80,
      81,  81,  82,  82,  83,  83,  83,  83,  83,  84,  84,  85,  85,  85,  85,
      86,  86,  87,  87,  87,  88,  88,  88,  88,  88,  88,  89,  89,  89,  89,
      90,  91,  91,  91,  91,  91,  92,  92,  92,  92,  92,  92,  93,  94,  94,
      94,  94,  95,  95,  95,  95,  95,  95,  95,  95,  96,  96,  96,  96,  97,
      97,  98,  98,  98,  99,  99,  99,  99,  99,  99,  99,  100, 100, 100, 100,
      101, 101, 101, 101, 101, 102, 102, 102, 102, 103, 103, 103, 103, 103, 103,
      104, 104, 104, 105, 105, 106, 106, 106, 106, 106, 106, 106, 106, 107, 107,
      107, 108, 109, 109, 109, 109, 110, 110, 110, 111, 111, 111, 111, 112, 112,
      112, 112, 112, 112, 113, 113, 113, 113, 114, 114, 115, 115, 115, 115, 116,
      116, 116, 116, 116, 116, 116, 116, 117, 117, 118, 118, 118, 119, 119, 119,
      119, 120, 120, 121, 121, 121, 121, 122, 122, 122, 122, 122, 123, 123, 123,
      124, 124, 124, 124, 124, 125, 125, 125, 125, 126, 126, 126, 126, 126, 127,
      127, 127};
  int8_t q = 68;
  uint16_t res = 270;
  int actual = binary<int8_t, uint16_t, size>(arr.data(), q);
  ASSERT_EQ(actual, res);
  actual = templated_binary<int8_t, uint16_t, size>(arr.data(), q);
  ASSERT_EQ(actual, res);
  actual = templated_cmov<int8_t, uint16_t, size>(arr.data(), q);
  ASSERT_EQ(actual, res);
  actual = templated_sub<int8_t, uint16_t, size>(arr.data(), q);
  ASSERT_EQ(actual, res);
  actual = linear_scan<int8_t, uint16_t, size>(arr.data(), q);
  ASSERT_EQ(actual, res);
  actual = linear_scan_cmov<int8_t, uint16_t, size>(arr.data(), q);
  ASSERT_EQ(actual, res);
  actual = linear_scan_sub<int8_t, uint16_t, size>(arr.data(), q);
  ASSERT_EQ(actual, res);
  actual = branchless_cmov<int8_t, uint16_t, size>(arr.data(), q);
  ASSERT_EQ(actual, res);
  actual = branchless_sub<int8_t, uint16_t, size>(arr.data(), q);
  ASSERT_EQ(actual, res);
}