// Test for Pin class.
// Using a test by side effect on the LPC_IOCON register
#include "L1_Peripheral/lpc40xx/pin.hpp"

#include "L0_Platform/lpc40xx/LPC40xx.h"
#include "L4_Testing/testing_frameworks.hpp"
#include "utility/debug.hpp"

namespace sjsu::lpc40xx
{
EMIT_ALL_METHODS(Pin);

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

  SECTION("Pin Function")
  {
    SECTION("Valid function code")

    {
      // Setup
      // Source: "UM10562 LPC408x/407x User manual" table 84 page 133
      constexpr uint8_t kPort0Pin0Uart3Txd     = 0b010;
      constexpr uint8_t kPort2Pin5Pwm1Channel6 = 0b001;

      // Exercise
      test_subject00.ConfigureFunction(kPort0Pin0Uart3Txd);
      test_subject25.ConfigureFunction(kPort2Pin5Pwm1Channel6);

      // Verify
      // Check that mapped pin P0.0's first 3 bits are equal to the function
      // U3_TXD
      CHECK(kPort0Pin0Uart3Txd == (local_iocon.P0_0 & 0b111));
      // Check that mapped pin P2.5's first 3 bits are equal to the function
      // PWM1_6
      CHECK(kPort2Pin5Pwm1Channel6 == (local_iocon.P2_5 & 0b111));
    }

    SECTION("Invalid function code")
    {
      // Exercise & Verify
      SJ2_CHECK_EXCEPTION(test_subject00.ConfigureFunction(0b1000),
                          std::errc::invalid_argument);
      SJ2_CHECK_EXCEPTION(test_subject25.ConfigureFunction(0b1111),
                          std::errc::invalid_argument);
    }
  }

  SECTION("Pin Mode")
  {
    // Setup
    // Source: "UM10562 LPC408x/407x User manual" table 83 page 132
    constexpr uint8_t kModePosition = 3;
    constexpr uint32_t kMask        = 0b11 << kModePosition;
    constexpr uint32_t kExpectedForInactive =
        static_cast<uint8_t>(sjsu::Pin::Resistor::kNone) << kModePosition;
    constexpr uint32_t kExpectedForPullDown =
        static_cast<uint8_t>(sjsu::Pin::Resistor::kPullDown) << kModePosition;
    constexpr uint32_t kExpectedForPullUp =
        static_cast<uint8_t>(sjsu::Pin::Resistor::kPullUp) << kModePosition;
    constexpr uint32_t kExpectedForRepeater =
        static_cast<uint8_t>(sjsu::Pin::Resistor::kRepeater) << kModePosition;

    // Exercise - Set as floating
    test_subject00.ConfigureFloating();
    test_subject25.ConfigureFloating();

    // Verify
    CHECK(kExpectedForInactive == (local_iocon.P0_0 & kMask));
    CHECK(kExpectedForInactive == (local_iocon.P2_5 & kMask));

    // Exercise - Set as pull down resistor
    test_subject00.ConfigurePullDown();
    test_subject25.ConfigurePullDown();

    // Verify
    CHECK(kExpectedForPullDown == (local_iocon.P0_0 & kMask));
    CHECK(kExpectedForPullDown == (local_iocon.P2_5 & kMask));

    // Exercise - Set as pull up resistor
    test_subject00.ConfigurePullUp();
    test_subject25.ConfigurePullUp();

    // Verify
    CHECK(kExpectedForPullUp == (local_iocon.P0_0 & kMask));
    CHECK(kExpectedForPullUp == (local_iocon.P2_5 & kMask));

    // Exercise - Set as repeater
    test_subject00.ConfigurePullResistor(sjsu::Pin::Resistor::kRepeater);
    test_subject25.ConfigurePullResistor(sjsu::Pin::Resistor::kRepeater);

    // Verify
    CHECK(kExpectedForRepeater == (local_iocon.P0_0 & kMask));
    CHECK(kExpectedForRepeater == (local_iocon.P2_5 & kMask));
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

  SECTION("Set and Clear Analog Mode")
  {
    // Setup
    // Source: "UM10562 LPC408x/407x User manual" table 83 page 132
    constexpr uint8_t kAdMode = 7;
    constexpr uint32_t kMask  = 0b1 << kAdMode;

    // Exercise
    test_subject00.ConfigureAsAnalogMode(true);
    test_subject25.ConfigureAsAnalogMode(false);

    // Verify
    // Digital filter is set with zero
    CHECK(0 == (local_iocon.P0_0 & kMask));
    CHECK(kMask == (local_iocon.P2_5 & kMask));

    // Exercise
    test_subject00.ConfigureAsAnalogMode(false);
    test_subject25.ConfigureAsAnalogMode(true);

    // Verify
    CHECK(kMask == (local_iocon.P0_0 & kMask));
    CHECK(0 == (local_iocon.P2_5 & kMask));
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

  SECTION("Open Drain")
  {
    // Setup
    // Source: "UM10562 LPC408x/407x User manual" table 83 page 132
    constexpr uint8_t kOpenDrainPosition = 10;
    constexpr uint32_t kMask             = 0b1 << kOpenDrainPosition;
    test_subject00.ConfigureAsOpenDrain(true);
    test_subject25.ConfigureAsOpenDrain(false);

    // Verify
    CHECK(kMask == (local_iocon.P0_0 & kMask));
    CHECK(0 == (local_iocon.P2_5 & kMask));

    // Exercise

    // Exercise
    test_subject00.ConfigureAsOpenDrain(false);
    test_subject25.ConfigureAsOpenDrain(true);

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
