#include "config.hpp"
#include "testing/testing_frameworks.hpp"

namespace sjsu
{
TEST_CASE("config.hpp test")
{
  SECTION("Example usage of SJ2_DECLARE_CONSTANT")
  {
#define SJ2_CALIBRATION_TRIM 115
    SJ2_DECLARE_CONSTANT(CALIBRATION_TRIM, int, kCalibrationTrim);
    static_assert(-100'000 <= kCalibrationTrim && kCalibrationTrim <= 120'000,
                  "SJ2_SYSTEM_CLOCK can only be between 1 Hz and 4000 Mhz");
#undef SJ2_CALIBRATION_TRIM
  }
}
}  // namespace sjsu
