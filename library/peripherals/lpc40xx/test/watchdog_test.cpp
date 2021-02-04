#include "peripherals/lpc40xx/watchdog.hpp"

#include <cstdint>

#include "platforms/targets/lpc40xx/LPC40xx.h"
#include "testing/testing_frameworks.hpp"
#include "third_party/units/units.h"

namespace sjsu
{
namespace lpc40xx
{
TEST_CASE("Testing lpc40xx watchdog")
{
  constexpr std::chrono::seconds kTimerConstantSeconds = 1s;
  constexpr uint32_t kTimerConstant                    = 125000;
  constexpr uint8_t kTimerMode                         = 0x3;
  constexpr uint32_t kTimerWarning                     = 1023;
  constexpr uint8_t kFeedSequence                      = 0x55;
  constexpr uint32_t kNewTimerValue                    = 150;

  // Create local version of LPC_WDT
  LPC_WDT_TypeDef local_wdt;

  // Clear local wdt registers
  testing::ClearStructure(&local_wdt);

  // Set base registers to local registers
  Watchdog::wdt_base = &local_wdt;

  Watchdog watchdog_test;

  SECTION("Initialization")
  {
    watchdog_test.Initialize(kTimerConstantSeconds);
    CHECK(kTimerConstant == local_wdt.TC);
    CHECK(kTimerMode == local_wdt.MOD);
    CHECK(kTimerWarning == local_wdt.WARNINT);
  }

  SECTION("Send Feed Sequence")
  {
    watchdog_test.FeedSequence();
    CHECK(kFeedSequence == local_wdt.FEED);
  }

  SECTION("Read watchdog timer value")
  {
    local_wdt.TV = kNewTimerValue;
    CHECK(kNewTimerValue == watchdog_test.CheckTimeout());
  }
}
}  // namespace lpc40xx
}  // namespace sjsu
