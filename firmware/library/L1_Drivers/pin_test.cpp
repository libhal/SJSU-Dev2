// Test for Pin class.
// Using a test by side effect on the LPC_IOCON register
#include "L0_LowLevel/LPC40xx.h"
#include "L1_Drivers/pin.hpp"
#include "L5_Testing/testing_frameworks.hpp"

TEST_CASE("Testing Pin", "[pin_configure]")
{
  // Simulated local version of LPC_IOCON register to verify register
  // manipulation by side effect of Pin method calls
  LPC_IOCON_TypeDef local_iocon;
  memset(&local_iocon, 0, sizeof(local_iocon));
  // Substitute the memory mapped LPC_IOCON with the local_iocon test struture
  // Redirects manipulation to the 'local_iocon'
  Pin::pin_map = reinterpret_cast<Pin::PinMap_t *>(&local_iocon);

  Pin test_subject00(0, 0);
  Pin test_subject25(2, 5);

  SECTION("Pin Function")
  {
    // Source: "UM10562 LPC408x/407x User manual" table 84 page 133
    constexpr uint8_t kPort0Pin0Uart3Txd     = 0b010;
    constexpr uint8_t kPort2Pin5Pwm1Channel6 = 0b001;

    test_subject00.SetPinFunction(kPort0Pin0Uart3Txd);
    test_subject25.SetPinFunction(kPort2Pin5Pwm1Channel6);
    // Check that mapped pin P0.0's first 3 bits are equal to the function
    // U3_TXD
    CHECK(kPort0Pin0Uart3Txd == (local_iocon.P0_0 & 0b111));
    // Check that mapped pin P2.5's first 3 bits are equal to the function
    // PWM1_6
    CHECK(kPort2Pin5Pwm1Channel6 == (local_iocon.P2_5 & 0b111));
  }
  SECTION("Pin Mode")
  {
    // Source: "UM10562 LPC408x/407x User manual" table 83 page 132
    constexpr uint8_t kModePosition = 3;
    constexpr uint32_t kMask        = 0b11 << kModePosition;
    constexpr uint32_t kExpectedForInactive =
        static_cast<uint8_t>(PinInterface::Mode::kInactive) << kModePosition;
    constexpr uint32_t kExpectedForPullDown =
        static_cast<uint8_t>(PinInterface::Mode::kPullDown) << kModePosition;
    constexpr uint32_t kExpectedForPullUp =
        static_cast<uint8_t>(PinInterface::Mode::kPullUp) << kModePosition;
    constexpr uint32_t kExpectedForRepeater =
        static_cast<uint8_t>(PinInterface::Mode::kRepeater) << kModePosition;

    test_subject00.SetMode(PinInterface::Mode::kInactive);
    test_subject25.SetMode(PinInterface::Mode::kInactive);
    CHECK(kExpectedForInactive == (local_iocon.P0_0 & kMask));
    CHECK(kExpectedForInactive == (local_iocon.P2_5 & kMask));

    test_subject00.SetMode(PinInterface::Mode::kPullDown);
    test_subject25.SetMode(PinInterface::Mode::kPullDown);

    CHECK(kExpectedForPullDown == (local_iocon.P0_0 & kMask));
    CHECK(kExpectedForPullDown == (local_iocon.P2_5 & kMask));

    test_subject00.SetMode(PinInterface::Mode::kPullUp);
    test_subject25.SetMode(PinInterface::Mode::kPullUp);

    CHECK(kExpectedForPullUp == (local_iocon.P0_0 & kMask));
    CHECK(kExpectedForPullUp == (local_iocon.P2_5 & kMask));

    test_subject00.SetMode(PinInterface::Mode::kRepeater);
    test_subject25.SetMode(PinInterface::Mode::kRepeater);

    CHECK(kExpectedForRepeater == (local_iocon.P0_0 & kMask));
    CHECK(kExpectedForRepeater == (local_iocon.P2_5 & kMask));
  }
  SECTION("Set and clear Hysteresis modes")
  {
    // Source: "UM10562 LPC408x/407x User manual" table 83 page 132
    constexpr uint8_t kHysteresisPosition = 5;
    constexpr uint32_t kMask              = 0b1 << kHysteresisPosition;
    test_subject00.EnableHysteresis(true);
    test_subject25.EnableHysteresis(false);

    CHECK(kMask == (local_iocon.P0_0 & kMask));
    CHECK(0 == (local_iocon.P2_5 & kMask));

    test_subject00.EnableHysteresis(false);
    test_subject25.EnableHysteresis(true);

    CHECK(0 == (local_iocon.P0_0 & kMask));
    CHECK(kMask == (local_iocon.P2_5 & kMask));
  }
  SECTION("Set and clear Active level")
  {
    // Source: "UM10562 LPC408x/407x User manual" table 83 page 132
    constexpr uint8_t kInvertPosition = 6;
    constexpr uint32_t kMask          = 0b1 << kInvertPosition;
    test_subject00.SetAsActiveLow(true);
    test_subject25.SetAsActiveLow(false);

    CHECK(kMask == (local_iocon.P0_0 & kMask));
    CHECK(0 == (local_iocon.P2_5 & kMask));

    test_subject00.SetAsActiveLow(false);
    test_subject25.SetAsActiveLow(true);

    CHECK(0 == (local_iocon.P0_0 & kMask));
    CHECK(kMask == (local_iocon.P2_5 & kMask));
  }
  SECTION("Set and Clear Analog Mode")
  {
    // Source: "UM10562 LPC408x/407x User manual" table 83 page 132
    constexpr uint8_t kAdMode = 7;
    constexpr uint32_t kMask  = 0b1 << kAdMode;
    test_subject00.SetAsAnalogMode(true);
    test_subject25.SetAsAnalogMode(false);

    // Digital filter is set with zero
    CHECK(0 == (local_iocon.P0_0 & kMask));
    CHECK(kMask == (local_iocon.P2_5 & kMask));

    test_subject00.SetAsAnalogMode(false);
    test_subject25.SetAsAnalogMode(true);

    CHECK(kMask == (local_iocon.P0_0 & kMask));
    CHECK(0 == (local_iocon.P2_5 & kMask));
  }
  SECTION("Enable and Disable Digital Filter")
  {
    // Source: "UM10562 LPC408x/407x User manual" table 83 page 132
    constexpr uint8_t kDigitalFilter = 8;
    constexpr uint32_t kMask         = 0b1 << kDigitalFilter;
    test_subject00.EnableDigitalFilter(true);
    test_subject25.EnableDigitalFilter(false);

    // Digital filter is set with zero
    CHECK(0 == (local_iocon.P0_0 & kMask));
    CHECK(kMask == (local_iocon.P2_5 & kMask));

    test_subject00.EnableDigitalFilter(false);
    test_subject25.EnableDigitalFilter(true);

    CHECK(kMask == (local_iocon.P0_0 & kMask));
    CHECK(0 == (local_iocon.P2_5 & kMask));
  }
  SECTION("Fast Mode Set")
  {
    // Source: "UM10562 LPC408x/407x User manual" table 83 page 132
    constexpr uint8_t kSlewPosition = 9;
    constexpr uint32_t kMask        = 0b1 << kSlewPosition;
    test_subject00.EnableFastMode(true);
    test_subject25.EnableFastMode(false);

    CHECK(kMask == (local_iocon.P0_0 & kMask));
    CHECK(0 == (local_iocon.P2_5 & kMask));

    test_subject00.EnableFastMode(false);
    test_subject25.EnableFastMode(true);

    CHECK(0 == (local_iocon.P0_0 & kMask));
    CHECK(kMask == (local_iocon.P2_5 & kMask));
  }
  SECTION("Enable and Disable I2c High Speed Mode")
  {
    // Source: "UM10562 LPC408x/407x User manual" table 89 page 141
    constexpr uint8_t kI2cHighSpeedMode = 8;
    constexpr uint32_t kMask            = 0b1 << kI2cHighSpeedMode;
    test_subject00.EnableI2cHighSpeedMode(true);
    test_subject25.EnableI2cHighSpeedMode(false);
    // I2C Highspeed is set with zero
    CHECK(0 == (local_iocon.P0_0 & kMask));
    CHECK(kMask == (local_iocon.P2_5 & kMask));

    test_subject00.EnableI2cHighSpeedMode(false);
    test_subject25.EnableI2cHighSpeedMode(true);

    CHECK(kMask == (local_iocon.P0_0 & kMask));
    CHECK(0 == (local_iocon.P2_5 & kMask));
  }
  SECTION("Enable and disable high current drive")
  {
    // Source: "UM10562 LPC408x/407x User manual" table 89 page 141
    constexpr uint8_t kHighCurrentDrive = 9;
    constexpr uint32_t kMask            = 0b1 << kHighCurrentDrive;
    test_subject00.EnableI2cHighCurrentDrive(true);
    test_subject25.EnableI2cHighCurrentDrive(false);

    CHECK(kMask == (local_iocon.P0_0 & kMask));
    CHECK(0 == (local_iocon.P2_5 & kMask));

    test_subject00.EnableI2cHighCurrentDrive(false);
    test_subject25.EnableI2cHighCurrentDrive(true);

    CHECK(0 == (local_iocon.P0_0 & kMask));
    CHECK(kMask == (local_iocon.P2_5 & kMask));
  }
  SECTION("Open Drain")
  {
    // Source: "UM10562 LPC408x/407x User manual" table 83 page 132
    constexpr uint8_t kOpenDrainPosition = 10;
    constexpr uint32_t kMask             = 0b1 << kOpenDrainPosition;
    test_subject00.SetAsOpenDrain(true);
    test_subject25.SetAsOpenDrain(false);

    CHECK(kMask == (local_iocon.P0_0 & kMask));
    CHECK(0 == (local_iocon.P2_5 & kMask));

    test_subject00.SetAsOpenDrain(false);
    test_subject25.SetAsOpenDrain(true);

    CHECK(0 == (local_iocon.P0_0 & kMask));
    CHECK(kMask == (local_iocon.P2_5 & kMask));
  }
  SECTION("Enable Dac")
  {
    // Source: "UM10562 LPC408x/407x User manual" table 85 page 138
    constexpr uint8_t kDac   = 16;
    constexpr uint32_t kMask = 0b1 << kDac;
    test_subject00.EnableDac(true);
    test_subject25.EnableDac(false);

    CHECK(kMask == (local_iocon.P0_0 & kMask));
    CHECK(0 == (local_iocon.P2_5 & kMask));

    test_subject00.EnableDac(false);
    test_subject25.EnableDac(true);

    CHECK(0 == (local_iocon.P0_0 & kMask));
    CHECK(kMask == (local_iocon.P2_5 & kMask));
  }
  Pin::pin_map = reinterpret_cast<Pin::PinMap_t *>(LPC_IOCON);
}
