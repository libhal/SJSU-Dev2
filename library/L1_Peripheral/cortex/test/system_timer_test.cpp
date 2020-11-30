// Test for Pin class.
// Using a test by side effect on the Cortex M4 SysTick register
#include "L1_Peripheral/cortex/system_timer.hpp"

#include "L0_Platform/lpc40xx/LPC40xx.h"
#include "L4_Testing/testing_frameworks.hpp"
#include "config.hpp"

namespace sjsu::cortex
{
EMIT_ALL_METHODS(SystemTimer);

TEST_CASE("Testing ARM Cortex SystemTimer")
{
  cortex::DWT_Type local_dwt = {
    .PCSR = 0,
  };
  cortex::CoreDebug_Type local_core;

  testing::ClearStructure(&local_dwt);
  testing::ClearStructure(&local_core);

  DwtCounter::dwt  = &local_dwt;
  DwtCounter::core = &local_core;

  // Simulated local version of SysTick register to verify register
  // manipulation by side effect of Pin method calls
  // Default figure 552 page 703
  SysTick_Type local_systick = { 0x4, 0, 0, 0x000F'423F };
  // Substitute the memory mapped SysTick with the local_systick test struture
  // Redirects manipulation to the 'local_systick'
  SystemTimer::sys_tick = &local_systick;

  // Set mock for sjsu::SystemController
  constexpr units::frequency::hertz_t kClockFrequency = 10_MHz;

  Mock<SystemController> mock_system_controller;
  When(Method(mock_system_controller, GetClockRate))
      .AlwaysReturn(kClockFrequency);
  sjsu::SystemController::SetPlatformController(&mock_system_controller.get());

  Mock<sjsu::InterruptController> mock_interrupt_controller;
  Fake(Method(mock_interrupt_controller, Enable));
  Fake(Method(mock_interrupt_controller, Disable));
  sjsu::InterruptController::SetPlatformController(
      &mock_interrupt_controller.get());

  using ResourceID = sjsu::SystemController::ResourceID;

  constexpr uint8_t kExpectedPriority = 3;
  SystemTimer test_subject(ResourceID::Define<0>(), kExpectedPriority);

  SECTION("Initialize()")
  {
    // Exercise
    test_subject.ModuleInitialize();

    // Verify
    // Verify: Check that the DWT time was initialized properly
    CHECK(CoreDebug_DEMCR_TRCENA_Msk == local_core.DEMCR);
    CHECK(0 == local_dwt.CYCCNT);
    CHECK(DWT_CTRL_CYCCNTENA_Msk == local_dwt.CTRL);
  }

  SECTION("SetTickFrequency generate desired frequency")
  {
    // Setup
    constexpr auto kFrequency         = 1_kHz;
    constexpr auto kExpectedLoadValue = (kClockFrequency / kFrequency) - 1;
    constexpr auto kClockFrequencyInt = kClockFrequency.to<uint32_t>();
    constexpr auto kExpectedTicksPerMillisecond =
        kClockFrequencyInt / 1000 /* ms/s */;
    constexpr auto kExpectedNanosecondsPerTick =
        (SystemTimer::kFixedPointScaling * 1'000'000'000ns) /
        kClockFrequencyInt;

    // Setup: Set LOAD to zero
    local_systick.LOAD = 0;

    // Exercise
    test_subject.ConfigureTickFrequency(kFrequency);

    // Verify
    CHECK(kExpectedTicksPerMillisecond == SystemTimer::ticks_per_millisecond);
    CHECK(kExpectedNanosecondsPerTick == SystemTimer::nanoseconds_per_tick);
    CHECK(kExpectedLoadValue.to<uint32_t>() == local_systick.LOAD);
  }

  SECTION("Enable()")
  {
    // Setup
    // Source: "UM10562 LPC408x/407x User manual" table 553 page 703
    constexpr uint8_t kEnableMask    = 0b0001;
    constexpr uint8_t kTickIntMask   = 0b0010;
    constexpr uint8_t kClkSourceMask = 0b0100;
    constexpr uint32_t kMask = kEnableMask | kTickIntMask | kClkSourceMask;

    local_systick.LOAD = 1000;
    local_systick.VAL  = 0xBEEF;

    // Exercise
    test_subject.ModuleEnable();

    // Verify
    CHECK(kMask == local_systick.CTRL);
    CHECK(0 == local_systick.VAL);
    Verify(
        Method(mock_interrupt_controller, Enable)
            .Matching([](sjsu::InterruptController::RegistrationInfo_t info) {
              return (info.interrupt_request_number == cortex::SysTick_IRQn) &&
                     (info.priority == kExpectedPriority);
            }));
  }

  SECTION("Enable() should throw exception if LOAD is 0")
  {
    // Setup
    // Source: "UM10562 LPC408x/407x User manual" table 553 page 703
    constexpr uint8_t kClkSourceMask = 0b0100;
    // Default value see above.
    local_systick.CTRL = kClkSourceMask;
    // Setting load value to 0, should return false
    local_systick.LOAD = 0;
    local_systick.VAL  = 0xBEEF;

    // Exercise
    SJ2_CHECK_EXCEPTION(test_subject.ModuleEnable(),
                        std::errc::invalid_argument);

    // Verify
    CHECK(kClkSourceMask == local_systick.CTRL);
    CHECK(0xBEEF == local_systick.VAL);
    Verify(Method(mock_interrupt_controller, Enable)).Never();
  }

  SECTION("Disable Timer should clear all bits")
  {
    // Setup
    // Source: "UM10562 LPC408x/407x User manual" table 553 page 703
    constexpr uint8_t kEnableMask    = 0b0001;
    constexpr uint8_t kTickIntMask   = 0b0010;
    constexpr uint8_t kClkSourceMask = 0b0100;
    constexpr uint32_t kMask = kEnableMask | kTickIntMask | kClkSourceMask;
    local_systick.CTRL       = kMask;
    local_systick.LOAD       = 1000;

    // Exercise
    test_subject.ModuleEnable(false);

    // Verify
    // Verify: The control should not change as unfortunately, the SystemTick
    //         Timer will not come back after being enabled.
    CHECK(kMask == local_systick.CTRL);
  }

  SECTION("ConfigureCallback()")
  {
    // Setup
    bool was_called     = false;
    auto dummy_function = [&was_called](void) { was_called = true; };

    // Exercise
    test_subject.ConfigureCallback(dummy_function);
    test_subject.SystemTimerHandler();

    // Verify
    CHECK(was_called);
  }

  SECTION("GetCount()")
  {
    // Setup
    constexpr uint32_t kDebugCountTicks = 128;
    constexpr auto kMilliseconds        = 1ms;
    SystemTimer::millisecond_count      = kMilliseconds;
    constexpr auto kExpectedUptime = kMilliseconds + (kDebugCountTicks * 100ns);

    // Exercise
    test_subject.Initialize();
    local_dwt.CYCCNT = kDebugCountTicks;
    auto uptime      = test_subject.GetCount();

    // Verify
    CHECK(kExpectedUptime == uptime);
  }

  // Cleanup
  SystemTimer::sys_tick = SysTick;
  DwtCounter::dwt       = DWT;
  DwtCounter::core      = CoreDebug;
}
}  // namespace sjsu::cortex
