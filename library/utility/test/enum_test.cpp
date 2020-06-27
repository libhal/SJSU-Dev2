#include <cstdint>

#include "L4_Testing/testing_frameworks.hpp"
#include "utility/enum.hpp"

namespace sjsu
{
enum class TestEnumClass : uint8_t
{
  kVal0 = 0b1010,
  kVal1 = 0b0011
};

SJ2_ENABLE_BITMASK_OPERATORS(TestEnumClass);

#define ENUM_TEST_SHORT_OP(op)                                                \
  constexpr uint8_t kVal0     = static_cast<uint8_t>(TestEnumClass::kVal0);   \
  constexpr uint8_t kVal1     = static_cast<uint8_t>(TestEnumClass::kVal1);   \
  constexpr uint8_t kExpected = kVal0 op kVal1;                               \
  static_assert(                                                              \
      kExpected ==                                                            \
          static_cast<uint8_t>(TestEnumClass::kVal0 op TestEnumClass::kVal1), \
      "Enum operator" #op ", does not match expected value!");                \
  CHECK(kExpected ==                                                          \
        static_cast<uint8_t>(TestEnumClass::kVal0 op TestEnumClass::kVal1))

#define ENUM_TEST_SHORT_OP_EQUALS(op, op_eq)                                \
  constexpr uint8_t kVal0     = static_cast<uint8_t>(TestEnumClass::kVal0); \
  constexpr uint8_t kVal1     = static_cast<uint8_t>(TestEnumClass::kVal1); \
  constexpr uint8_t kExpected = kVal0 op kVal1;                             \
  TestEnumClass kActual       = TestEnumClass::kVal0;                       \
  kActual op_eq TestEnumClass::kVal1;                                       \
  CHECK(kExpected == static_cast<uint8_t>(kActual));

TEST_CASE("Testing Enumeration Bit Operators")
{
  SECTION("Enum Class OR")
  {
    ENUM_TEST_SHORT_OP(|);
  }
  SECTION("Enum Class AND")
  {
    ENUM_TEST_SHORT_OP(&);
  }
  SECTION("Enum Class XOR")
  {
    // clang-format off
    ENUM_TEST_SHORT_OP(^);
    // clang-format on
  }
  SECTION("Enum Class INVERT")
  {
    constexpr uint8_t kVal0     = static_cast<uint8_t>(TestEnumClass::kVal0);
    constexpr uint8_t kExpected = ~kVal0;
    static_assert(kExpected == static_cast<uint8_t>(~TestEnumClass::kVal0),
                  "Enum operator~, does not match expected value!");
    CHECK(kExpected == static_cast<uint8_t>(~TestEnumClass::kVal0));
  }
  SECTION("Enum Class OR EQUALS")
  {
    ENUM_TEST_SHORT_OP_EQUALS(|, |=);
  }
  SECTION("Enum Class AND EQUALS")
  {
    ENUM_TEST_SHORT_OP_EQUALS(&, &=);
  }
  SECTION("Enum Class XOR EQUALS")
  {
    // clang-format off
    ENUM_TEST_SHORT_OP_EQUALS(^, ^=);
    // clang-format on
  }
}

TEST_CASE("Testing Enumeration Utilites")
{
  SECTION("Value() should return proper value")
  {
    CHECK(static_cast<uint8_t>(TestEnumClass::kVal0) ==
          Value(TestEnumClass::kVal0));
    CHECK(static_cast<uint8_t>(TestEnumClass::kVal1) ==
          Value(TestEnumClass::kVal1));
  }
}
}  // namespace sjsu
