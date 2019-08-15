#include <cstdint>
#include <limits>

#include "L4_Testing/testing_frameworks.hpp"
#include "utility/stopwatch.hpp"

namespace sjsu
{
// Blank namespace: Within scope of this file,
// anything inside this file cannot see this scope
// so same variable names within the namespaces won't conflict
namespace
{
// 1. Global setup
FAKE_VALUE_FUNC(uint32_t, tick);
FAKE_VALUE_FUNC(uint32_t, calibrate_tick);

uint32_t calibration_result = 0;
uint32_t CalibrateTicksHandler()
{
  calibration_result += 100;
  return calibration_result;
}
}  // namespace

TEST_CASE("Testing StopWatch Utilities", "[stopwatch]")
{
  // 2. Defining expected
  StopWatch test_sw(tick);

  SECTION("CurrentTicks")
  {
    // 2. Defining expected
    constexpr uint32_t kExpected0 = 123;
    constexpr uint32_t kExpected1 = 12'000'231;

    // 3. Setup
    // Minimize human error when typing numbers
    tick_fake.return_val = kExpected0;
    CHECK(kExpected0 == test_sw.CurrentTicks());

    tick_fake.return_val = kExpected1;
    CHECK(kExpected1 == test_sw.CurrentTicks());

    // Template specialize, return maximum value at compile time
    tick_fake.return_val = std::numeric_limits<uint32_t>::max();
    CHECK(std::numeric_limits<uint32_t>::max() == test_sw.CurrentTicks());
  }

  SECTION("Start and Stop")
  {
    constexpr uint32_t kStartTicks0    = 0;
    constexpr uint32_t kStopTicks0     = 200;
    constexpr uint32_t kExpectedDelta0 = kStopTicks0 - kStartTicks0;

    tick_fake.return_val = kStartTicks0;
    test_sw.Start();
    tick_fake.return_val    = kStopTicks0;
    uint32_t actual_delta_0 = test_sw.Stop();
    CHECK(kExpectedDelta0 == actual_delta_0);

    constexpr uint32_t kStartTicks1    = 3'435;
    constexpr uint32_t kStopTicks1     = 96'059'321;
    constexpr uint32_t kExpectedDelta1 = kStopTicks1 - kStartTicks1;

    tick_fake.return_val = kStartTicks1;
    test_sw.Start();
    tick_fake.return_val   = kStopTicks1;
    uint32_t actual_delta1 = test_sw.Stop();
    CHECK(kExpectedDelta1 == actual_delta1);

    // Checks for edge case when ticks overflows
    constexpr uint32_t kStartTicks2 =
        std::numeric_limits<uint32_t>::max() - 200;
    constexpr uint32_t kStopTicks2     = 300;
    constexpr uint32_t kExpectedDelta2 = kStopTicks2 - kStartTicks2;

    tick_fake.return_val = kStartTicks2;
    test_sw.Start();
    tick_fake.return_val   = kStopTicks2;
    uint32_t actual_delta2 = test_sw.Stop();
    CHECK(kExpectedDelta2 == actual_delta2);
  }

  SECTION("Calibrate")
  {
    constexpr uint32_t kExpectedDelta0         = 500;
    constexpr uint32_t kAddToCalibrationResult = 500;

    calibrate_tick_fake.custom_fake = CalibrateTicksHandler;

    StopWatch test_calibrate_sw(calibrate_tick);

    test_calibrate_sw.Calibrate();
    test_calibrate_sw.Start();
    calibration_result += kAddToCalibrationResult;
    uint32_t actual_delta0 = test_calibrate_sw.Stop();
    CHECK(actual_delta0 == kExpectedDelta0);
  }
}
}  // namespace sjsu
