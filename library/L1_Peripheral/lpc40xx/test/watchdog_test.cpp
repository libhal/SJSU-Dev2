#include <cstdint>

#include "L0_Platform/lpc40xx/LPC40xx.h"
#include "L1_Peripheral/lpc40xx/watchdog.hpp"
#include "L4_Testing/testing_frameworks.hpp"

namespace sjsu
{
namespace lpc40xx
{
EMIT_ALL_METHODS(Watchdog);

TEST_CASE("Testing lpc40xx watchdog", "[lpc40xx-watchdog]")
{
    constexpr uint32_t kTimerConstant = 0x02'FFFF;
    constexpr uint8_t  kTimerMode     = 0x3;
    constexpr uint32_t kTimerWindow   = 0xFF'FFFF;
    constexpr uint32_t kTimerWarning  = 0b11'1111'1111;
    constexpr uint8_t  kFeedSequence  = 0x55;
    constexpr uint32_t kTimerValue    = 0;

    // Create local version of LPC_WDT
    LPC_WDT_TypeDef local_wdt;

    // Clear local wdt registers
    memset(&local_wdt, 0, sizeof(local_wdt));

    // Set base registers to local registers
    Watchdog::wdt_base = &local_wdt;

    Watchdog watchdog_test;
    watchdog_test.Initialize(kTimerConstant, kTimerWindow, kTimerWarning);
    watchdog_test.FeedSequence();
    watchdog_test.CheckTimeout();

    SECTION("Initialization")
    {
        CHECK(kTimerConstant == local_wdt.TC);
        CHECK(kTimerMode == local_wdt.MOD);
        CHECK(kTimerWindow == local_wdt.WINDOW);
        CHECK(kTimerWarning == local_wdt.WARNINT);
    }

    SECTION("Send Feed Sequence")
    {
        CHECK(kFeedSequence == local_wdt.FEED);
    }
    SECTION("Read watchdog timer value")
    {
        CHECK(kTimerValue == local_wdt.TV);
    }
}
}  // namespace lpc40xx
}  // namespace sjsu
