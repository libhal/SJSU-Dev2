#include <cstdint>

#include "testing/testing_frameworks.hpp"
#include "utility/math/map.hpp"

namespace sjsu
{
TEST_CASE("Testing Numeric Range remapping utility function")
{
  SECTION("Mapping with number literals")
  {
    CHECK(5 == Map(75, 0, 100, -10, 10));
    CHECK(-10 == Map(0, 0, 100, -10, 0));
    CHECK(255 == Map(127, -128, 127, 0, 255));
  }
  SECTION("Mapping with variable literals")
  {
    int8_t test     = 125;
    uint8_t min     = 100;
    uint8_t max     = 200;
    int16_t new_min = -100;
    int16_t new_max = 100;
    CHECK(-50 == Map(test, min, max, new_min, new_max));
  }
  SECTION("Mapping from int to float")
  {
    constexpr float kExpectedValueFloat   = 1;
    constexpr double kExpectedValueDouble = 1;
    float actual_value_float              = Map(310, 0, 1024, 0.0f, 3.3f);
    float float_error = actual_value_float - kExpectedValueFloat;
    CHECK(-0.1f <= float_error);
    CHECK(float_error <= 0.1f);

    double actual_value_double = Map(310, 0, 1024, 0.0, 3.3);
    double double_error        = actual_value_double - kExpectedValueDouble;
    CHECK(-0.1 <= double_error);
    CHECK(double_error <= 0.1);
  }
}
}  // namespace sjsu
