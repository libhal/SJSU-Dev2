#include <cstdint>
#include <limits>

#include "testing/testing_frameworks.hpp"
#include "utility/math/limits.hpp"

namespace sjsu
{
TEST_CASE("Testing Limits Function")
{
  SECTION("int8_t :: 3-bits")
  {
    // Setup
    constexpr auto kExpectedMax = 3;
    constexpr auto kExpectedMin = -4;

    // Exercise
    constexpr auto kMax = BitLimits<3, int8_t>::Max();
    constexpr auto kMin = BitLimits<3, int8_t>::Min();

    // Verify
    CHECK(kMax == kExpectedMax);
    CHECK(kMin == kExpectedMin);
    static_assert(kMax == kExpectedMax);
    static_assert(kMin == kExpectedMin);
  }

  SECTION("int8_t :: 7-bits")
  {
    // Setup
    constexpr auto kExpectedMax = 63;
    constexpr auto kExpectedMin = -64;

    // Exercise
    constexpr auto kMax = BitLimits<7, int8_t>::Max();
    constexpr auto kMin = BitLimits<7, int8_t>::Min();

    // Verify
    CHECK(kMax == kExpectedMax);
    CHECK(kMin == kExpectedMin);
    static_assert(kMax == kExpectedMax);
    static_assert(kMin == kExpectedMin);
  }

  SECTION("int16_t :: 12-bits")
  {
    // Setup
    constexpr auto kExpectedMax = 2047;
    constexpr auto kExpectedMin = -2048;

    // Exercise
    constexpr auto kMax = BitLimits<12, int16_t>::Max();
    constexpr auto kMin = BitLimits<12, int16_t>::Min();

    // Verify
    CHECK(kMax == kExpectedMax);
    CHECK(kMin == kExpectedMin);
    static_assert(kMax == kExpectedMax);
    static_assert(kMin == kExpectedMin);
  }

  SECTION("int32_t :: 24-bits")
  {
    // Setup
    constexpr auto kExpectedMax = 8388607;
    constexpr auto kExpectedMin = -8388608;

    // Exercise
    constexpr auto kMax = BitLimits<24, int32_t>::Max();
    constexpr auto kMin = BitLimits<24, int32_t>::Min();

    // Verify
    CHECK(kMax == kExpectedMax);
    CHECK(kMin == kExpectedMin);
    static_assert(kMax == kExpectedMax);
    static_assert(kMin == kExpectedMin);
  }

  SECTION("int64_t :: 55-bits")
  {
    // Setup
    constexpr int64_t kExpectedMax = 18014398509481983ULL;
    constexpr int64_t kExpectedMin = -18014398509481984ULL;

    // Exercise
    constexpr auto kMax = BitLimits<55, int64_t>::Max();
    constexpr auto kMin = BitLimits<55, int64_t>::Min();

    // Verify
    CHECK(kMax == kExpectedMax);
    CHECK(kMin == kExpectedMin);
    static_assert(kMax == kExpectedMax);
    static_assert(kMin == kExpectedMin);
  }

  SECTION("int64_t :: 64-bits")
  {
    // Setup
    constexpr auto kExpectedMax = std::numeric_limits<int64_t>::max();
    constexpr auto kExpectedMin = std::numeric_limits<int64_t>::min();

    // Exercise
    constexpr auto kMax = BitLimits<64, int64_t>::Max();
    constexpr auto kMin = BitLimits<64, int64_t>::Min();

    // Verify
    CHECK(kMax == kExpectedMax);
    CHECK(kMin == kExpectedMin);
    static_assert(kMax == kExpectedMax);
    static_assert(kMin == kExpectedMin);
  }

  SECTION("uint8_t :: 3-bits")
  {
    // Setup
    constexpr auto kExpectedMax = 3;
    constexpr auto kExpectedMin = -4;

    // Exercise
    constexpr auto kMax = BitLimits<3, int8_t>::Max();
    constexpr auto kMin = BitLimits<3, int8_t>::Min();

    // Verify
    CHECK(kMax == kExpectedMax);
    CHECK(kMin == kExpectedMin);
    static_assert(kMax == kExpectedMax);
    static_assert(kMin == kExpectedMin);
  }

  SECTION("uint8_t :: 7-bits")
  {
    // Setup
    constexpr auto kExpectedMax = 127;
    constexpr auto kExpectedMin = 0;

    // Exercise
    constexpr auto kMax = BitLimits<7, uint8_t>::Max();
    constexpr auto kMin = BitLimits<7, uint8_t>::Min();

    // Verify
    CHECK(kMax == kExpectedMax);
    CHECK(kMin == kExpectedMin);
    static_assert(kMax == kExpectedMax);
    static_assert(kMin == kExpectedMin);
  }

  SECTION("uint16_t :: 12-bits")
  {
    // Setup
    constexpr auto kExpectedMax = 4095;
    constexpr auto kExpectedMin = 0;

    // Exercise
    constexpr auto kMax = BitLimits<12, uint16_t>::Max();
    constexpr auto kMin = BitLimits<12, uint16_t>::Min();

    // Verify
    CHECK(kMax == kExpectedMax);
    CHECK(kMin == kExpectedMin);
    static_assert(kMax == kExpectedMax);
    static_assert(kMin == kExpectedMin);
  }

  SECTION("uint32_t :: 24-bits")
  {
    // Setup
    constexpr auto kExpectedMax = 16777215;
    constexpr auto kExpectedMin = 0;

    // Exercise
    constexpr auto kMax = BitLimits<24, uint32_t>::Max();
    constexpr auto kMin = BitLimits<24, uint32_t>::Min();

    // Verify
    CHECK(kMax == kExpectedMax);
    CHECK(kMin == kExpectedMin);
    static_assert(kMax == kExpectedMax);
    static_assert(kMin == kExpectedMin);
  }

  SECTION("uint64_t :: 55-bits")
  {
    // Setup
    constexpr auto kExpectedMax = 36028797018963967ULL;
    constexpr auto kExpectedMin = 0;

    // Exercise
    constexpr auto kMax = BitLimits<55, uint64_t>::Max();
    constexpr auto kMin = BitLimits<55, uint64_t>::Min();

    // Verify
    CHECK(kMax == kExpectedMax);
    CHECK(kMin == kExpectedMin);
    static_assert(kMax == kExpectedMax);
    static_assert(kMin == kExpectedMin);
  }

  SECTION("uint64_t :: 64-bits")
  {
    // Setup
    constexpr auto kExpectedMax = std::numeric_limits<uint64_t>::max();
    constexpr auto kExpectedMin = std::numeric_limits<uint64_t>::min();

    // Exercise
    constexpr auto kMax = BitLimits<64, uint64_t>::Max();
    constexpr auto kMin = BitLimits<64, uint64_t>::Min();

    // Verify
    CHECK(kMax == kExpectedMax);
    CHECK(kMin == kExpectedMin);
    static_assert(kMax == kExpectedMax);
    static_assert(kMin == kExpectedMin);
  }
}
}  // namespace sjsu
