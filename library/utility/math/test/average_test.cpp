#include <cstdint>

#include "testing/testing_frameworks.hpp"
#include "utility/math/average.hpp"

namespace sjsu
{
#define APPROX_EQUALITY(expected, actual, resolution) \
  CHECK(-resolution < (actual - expected)); \
  CHECK((actual - expected) < resolution);

#define COMPILETIME_STATIC_ASSERT(expected, actual, resolution) \
  static_assert(-resolution < (actual-expected)); \
  static_assert(resolution > (actual-expected));

TEST_CASE("Testing Average Function")
{
  // Static_assert is used to make sure that the functions work at compile
  // time.
  constexpr float kResolution = 0.001f;

  constexpr float kExpectedZero = 6;
  constexpr float kAvgZero = Average({6, 6, 6, 6});
  APPROX_EQUALITY(kExpectedZero, kAvgZero, kResolution);
  COMPILETIME_STATIC_ASSERT(kExpectedZero, kAvgZero, kResolution);

  constexpr float kExpectedOne = 6.34775f;
  constexpr float kAvgOne = Average({6.12f, 6.45f, 6.366f, 6.455f});
  APPROX_EQUALITY(kExpectedOne, kAvgOne, kResolution);
  COMPILETIME_STATIC_ASSERT(kExpectedOne, kAvgOne, kResolution);

  constexpr float kExpectedTwo = 2.09325f;
  constexpr float kAvgTwo = Average({0.6f, 0.003f, 1.77f, 6.0f});
  APPROX_EQUALITY(kExpectedTwo, kAvgTwo, kResolution);
  COMPILETIME_STATIC_ASSERT(kExpectedTwo, kAvgTwo, kResolution);

  const float kArray[] = {1.1f, 3.4f, 5.2f};
  const float kExpectedThree = 3.233f;
  const float kAvgThree = Average(kArray, std::size(kArray));
  APPROX_EQUALITY(kExpectedThree, kAvgThree, kResolution);

  const float kAvgFour = Average(kArray);
  APPROX_EQUALITY(kExpectedThree, kAvgFour, kResolution);

  constexpr float kArray1[] = {100, 450, 200, 67000, 80000};
  constexpr float kExpectedFive = 29550;
  constexpr float kAvgFive = Average(kArray1);
  APPROX_EQUALITY(kExpectedFive, kAvgFive, kResolution);
  COMPILETIME_STATIC_ASSERT(kExpectedFive, kAvgFive, kResolution);

  const float kArray2[] = {1.689f, 450, 2.666f, 6700, 8000};
  const float kExpectedSix = 3030.871f;
  const float kAvgSix = Average(kArray2, std::size(kArray2));
  APPROX_EQUALITY(kExpectedSix, kAvgSix, kResolution);
}
}  // namespace sjsu
