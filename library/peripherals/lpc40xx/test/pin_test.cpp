// Test for Pin class.
// Using a test by side effect on the LPC_IOCON register
#include "peripherals/lpc40xx/pin.hpp"

#include "platforms/targets/lpc40xx/LPC40xx.h"
#include "testing/testing_frameworks.hpp"
#include "utility/debug.hpp"

namespace sjsu::lpc40xx
{
TEST_CASE("Testing lpc40xx Pin")
{
  // Simulated local version of LPC_IOCON register to verify register
  // manipulation by side effect of Pin method calls
  LPC_IOCON_TypeDef local_iocon;
  testing::ClearStructure(&local_iocon);
  // Substitute the memory mapped LPC_IOCON with the local_iocon test struture
  // Redirects manipulation to the 'local_iocon'
  Pin::pin_map = reinterpret_cast<Pin::PinMap_t *>(&local_iocon);

  Pin test_subject00(0, 0);
  Pin test_subject25(2, 5);

  SECTION("Initialize")
  {
    // Setup
    PinSettings_t settings0 = {};
    PinSettings_t settings1 = {};

    // Source: "UM10562 LPC408x/407x User manual" table 83 page 132
    constexpr auto kResistorMask = bit::MaskFromRange(3, 4);

    // Source: "UM10562 LPC408x/407x User manual" table 83 page 132
    constexpr auto kAnalogMask = bit::MaskFromRange(7);

    // Source: "UM10562 LPC408x/407x User manual" table 83 page 132
    constexpr auto kOpenDrainMask = bit::MaskFromRange(10);

    SECTION("Function Code")
    {
      settings0.function = 0b010;
      settings1.function = 0b001;
    }

    SECTION("Pull Up")
    {
      settings0.PullUp();
      settings1.PullUp();
    }

    SECTION("Pull Down")
    {
      settings0.PullDown();
      settings1.PullDown();
    }

    SECTION("Floating")
    {
      settings0.Floating();
      settings1.Floating();
    }

    SECTION("Analog 1")
    {
      settings0.as_analog = true;
      settings1.as_analog = false;
    }

    SECTION("Analog 2")
    {
      settings0.as_analog = false;
      settings1.as_analog = true;
    }

    SECTION("Open Drain 1")
    {
      settings0.open_drain = true;
      settings1.open_drain = false;
    }

    SECTION("Open Drain 2")
    {
      settings0.open_drain = false;
      settings1.open_drain = true;
    }

    test_subject00.settings = settings0;
    test_subject00.Initialize();
    test_subject25.settings = settings1;
    test_subject25.Initialize();

    // Verify
    // Verify: Function
    constexpr auto kFunctionMask = bit::MaskFromRange(0, 2);
    CHECK(settings0.function == bit::Extract(local_iocon.P0_0, kFunctionMask));
    CHECK(settings1.function == bit::Extract(local_iocon.P2_5, kFunctionMask));

    // Verify: Resistor
    CHECK(Value(settings0.resistor) ==
          bit::Extract(local_iocon.P0_0, kResistorMask));
    CHECK(Value(settings1.resistor) ==
          bit::Extract(local_iocon.P2_5, kResistorMask));

    // Verify: Analog
    CHECK(settings0.as_analog == !bit::Extract(local_iocon.P0_0, kAnalogMask));
    CHECK(settings1.as_analog == !bit::Extract(local_iocon.P2_5, kAnalogMask));

    // Verify: Open Drain
    CHECK(settings0.open_drain ==
          bit::Extract(local_iocon.P0_0, kOpenDrainMask));
    CHECK(settings1.open_drain ==
          bit::Extract(local_iocon.P2_5, kOpenDrainMask));
  }

  SECTION("Invalid function code")
  {
    // Setup
    test_subject00.settings.function = 0b1000;
    test_subject25.settings.function = 0b1111;

    // Exercise & Verify
    SJ2_CHECK_EXCEPTION(test_subject00.Initialize(),
                        std::errc::invalid_argument);
    SJ2_CHECK_EXCEPTION(test_subject25.Initialize(),
                        std::errc::invalid_argument);
  }

  SECTION("Set and clear Hysteresis modes")
  {
    // Setup
    // Source: "UM10562 LPC408x/407x User manual" table 83 page 132
    constexpr uint8_t kHysteresisPosition = 5;
    constexpr uint32_t kMask              = 0b1 << kHysteresisPosition;

    // Exercise
    test_subject00.EnableHysteresis(true);
    test_subject25.EnableHysteresis(false);

    // Verify
    CHECK(kMask == (local_iocon.P0_0 & kMask));
    CHECK(0 == (local_iocon.P2_5 & kMask));

    // Exercise
    test_subject00.EnableHysteresis(false);
    test_subject25.EnableHysteresis(true);

    // Verify
    CHECK(0 == (local_iocon.P0_0 & kMask));
    CHECK(kMask == (local_iocon.P2_5 & kMask));
  }

  SECTION("Set and clear Active level")
  {
    // Setup
    // Source: "UM10562 LPC408x/407x User manual" table 83 page 132
    constexpr uint8_t kInvertPosition = 6;
    constexpr uint32_t kMask          = 0b1 << kInvertPosition;

    // Exercise
    test_subject00.SetAsActiveLow(true);
    test_subject25.SetAsActiveLow(false);

    // Verify
    CHECK(kMask == (local_iocon.P0_0 & kMask));
    CHECK(0 == (local_iocon.P2_5 & kMask));

    // Exercise
    test_subject00.SetAsActiveLow(false);
    test_subject25.SetAsActiveLow(true);

    // Verify
    CHECK(0 == (local_iocon.P0_0 & kMask));
    CHECK(kMask == (local_iocon.P2_5 & kMask));
  }

  SECTION("Enable and Disable Digital Filter")
  {
    // Setup
    // Source: "UM10562 LPC408x/407x User manual" table 83 page 132
    constexpr uint8_t kDigitalFilter = 8;
    constexpr uint32_t kMask         = 0b1 << kDigitalFilter;

    // Exercise
    test_subject00.EnableDigitalFilter(true);
    test_subject25.EnableDigitalFilter(false);

    // Verify
    // Digital filter is set with zero
    CHECK(0 == (local_iocon.P0_0 & kMask));
    CHECK(kMask == (local_iocon.P2_5 & kMask));

    // Exercise
    test_subject00.EnableDigitalFilter(false);
    test_subject25.EnableDigitalFilter(true);

    // Verify
    CHECK(kMask == (local_iocon.P0_0 & kMask));
    CHECK(0 == (local_iocon.P2_5 & kMask));
  }

  SECTION("Fast Mode Set")
  {
    // Setup
    // Source: "UM10562 LPC408x/407x User manual" table 83 page 132
    constexpr uint8_t kSlewPosition = 9;
    constexpr uint32_t kMask        = 0b1 << kSlewPosition;

    // Exercise
    test_subject00.EnableFastMode(true);
    test_subject25.EnableFastMode(false);

    // Verify
    CHECK(kMask == (local_iocon.P0_0 & kMask));
    CHECK(0 == (local_iocon.P2_5 & kMask));

    // Exercise
    test_subject00.EnableFastMode(false);
    test_subject25.EnableFastMode(true);

    // Verify
    CHECK(0 == (local_iocon.P0_0 & kMask));
    CHECK(kMask == (local_iocon.P2_5 & kMask));
  }

  SECTION("Enable and Disable I2c High Speed Mode")
  {
    // Setup
    // Source: "UM10562 LPC408x/407x User manual" table 89 page 141
    constexpr uint8_t kI2cHighSpeedMode = 8;
    constexpr uint32_t kMask            = 0b1 << kI2cHighSpeedMode;
    test_subject00.EnableI2cHighSpeedMode(true);
    test_subject25.EnableI2cHighSpeedMode(false);

    // Verify
    // I2C Highspeed is set with zero
    CHECK(0 == (local_iocon.P0_0 & kMask));
    CHECK(kMask == (local_iocon.P2_5 & kMask));

    // Exercise
    test_subject00.EnableI2cHighSpeedMode(false);
    test_subject25.EnableI2cHighSpeedMode(true);

    // Verify
    CHECK(kMask == (local_iocon.P0_0 & kMask));
    CHECK(0 == (local_iocon.P2_5 & kMask));
  }

  SECTION("Enable and disable high current drive")
  {
    // Setup
    // Source: "UM10562 LPC408x/407x User manual" table 89 page 141
    constexpr uint8_t kHighCurrentDrive = 9;
    constexpr uint32_t kMask            = 0b1 << kHighCurrentDrive;

    // Exercise
    test_subject00.EnableI2cHighCurrentDrive(true);
    test_subject25.EnableI2cHighCurrentDrive(false);

    // Verify
    CHECK(kMask == (local_iocon.P0_0 & kMask));
    CHECK(0 == (local_iocon.P2_5 & kMask));

    // Exercise
    test_subject00.EnableI2cHighCurrentDrive(false);
    test_subject25.EnableI2cHighCurrentDrive(true);

    // Verify
    CHECK(0 == (local_iocon.P0_0 & kMask));
    CHECK(kMask == (local_iocon.P2_5 & kMask));
  }

  SECTION("Enable Dac")
  {
    // Setup
    // Source: "UM10562 LPC408x/407x User manual" table 85 page 138
    constexpr uint8_t kDac   = 16;
    constexpr uint32_t kMask = 0b1 << kDac;

    // Exercise
    test_subject00.EnableDac(true);
    test_subject25.EnableDac(false);

    // Verify
    CHECK(kMask == (local_iocon.P0_0 & kMask));
    CHECK(0 == (local_iocon.P2_5 & kMask));

    // Exercise
    test_subject00.EnableDac(false);
    test_subject25.EnableDac(true);

    // Verify
    CHECK(0 == (local_iocon.P0_0 & kMask));
    CHECK(kMask == (local_iocon.P2_5 & kMask));
  }

  Pin::pin_map = reinterpret_cast<Pin::PinMap_t *>(LPC_IOCON);
}
}  // namespace sjsu::lpc40xx
