// Test for PinConfigure class.
// Using a test by side effect on the LPC_IOCON register
#include <assert.h>
#include <cstdio>
#include "L0_LowLevel/LPC40xx.h"
#include "L5_Testing/testing_frameworks.hpp"
#include "L1_Drivers/pin_configure.hpp"

TEST_CASE("Testing PinConfigure", "[pin_configure]")
{
    // Simulated local version of LPC_IOCON register to verify register
    // manipulation by side effect of PinConfigure method calls
    LPC_IOCON_TypeDef local_iocon;

    // Substitute the memory mapped LPC_IOCON with the local_iocon test struture
    // Redirects manipulation to the 'local_iocon'
    PinConfigure::pin_map =
        reinterpret_cast<PinConfigure::PinMap_t *>(&local_iocon);

    PinConfigure test_subject00(0, 0);
    PinConfigure test_subject25(2, 5);

    SECTION("Test Pin Function")
    {
        // Source: "UM10562 LPC408x/407x User manual" table 84 page 133
        constexpr uint8_t kP0_0_U3_TXD = 0b010;
        constexpr uint8_t kP2_5_PWM1_6 = 0b001;

        test_subject00.SetPinFunction(kP0_0_U3_TXD);
        test_subject25.SetPinFunction(kP2_5_PWM1_6);
        // Check that mapped pin P0.0's first 3 bits are equal to the function
        // U3_TXD
        CHECK((local_iocon.P0_0 & 0b111) == kP0_0_U3_TXD);
        // Check that mapped pin P2.5's first 3 bits are equal to the function
        // PWM1_6
        CHECK((local_iocon.P2_5 & 0b111) == kP2_5_PWM1_6);
    }
    SECTION("Test Pin Mode")
    {
        // Source: "UM10562 LPC408x/407x User manual" table 83 page 132
        constexpr uint8_t kModePosition = 3;
        constexpr uint32_t kMask        = 0b11 << kModePosition;

        test_subject00.SetPinMode(PinConfigureInterface::kInactive);
        test_subject25.SetPinMode(PinConfigureInterface::kInactive);
        CHECK((local_iocon.P0_0 & kMask) == PinConfigureInterface::kInactive
                                                << kModePosition);
        CHECK((local_iocon.P2_5 & kMask) == PinConfigureInterface::kInactive
                                                << kModePosition);

        test_subject00.SetPinMode(PinConfigureInterface::kPullDown);
        test_subject25.SetPinMode(PinConfigureInterface::kPullDown);

        CHECK((local_iocon.P0_0 & kMask) == PinConfigureInterface::kPullDown
                                                << kModePosition);
        CHECK((local_iocon.P2_5 & kMask) == PinConfigureInterface::kPullDown
                                                << kModePosition);

        test_subject00.SetPinMode(PinConfigureInterface::kPullUp);
        test_subject25.SetPinMode(PinConfigureInterface::kPullUp);

        CHECK((local_iocon.P0_0 & kMask) == PinConfigureInterface::kPullUp
                                                << kModePosition);
        CHECK((local_iocon.P2_5 & kMask) == PinConfigureInterface::kPullUp
                                                << kModePosition);

        test_subject00.SetPinMode(PinConfigureInterface::kRepeater);
        test_subject25.SetPinMode(PinConfigureInterface::kRepeater);

        CHECK((local_iocon.P0_0 & kMask) == PinConfigureInterface::kRepeater
                                                << kModePosition);
        CHECK((local_iocon.P2_5 & kMask) == PinConfigureInterface::kRepeater
                                                << kModePosition);
    }
    SECTION("Test Hysteresis")
    {
        // Source: "UM10562 LPC408x/407x User manual" table 83 page 132
        constexpr uint8_t kHysteresisPosition = 5;
        constexpr uint32_t kMask              = 0b1 << kHysteresisPosition;
        test_subject00.EnableHysteresis(true);
        test_subject25.EnableHysteresis(false);

        CHECK((local_iocon.P0_0 & kMask) == kMask);
        CHECK((local_iocon.P2_5 & kMask) == 0);

        test_subject00.EnableHysteresis(false);
        test_subject25.EnableHysteresis(true);

        CHECK((local_iocon.P0_0 & kMask) == 0);
        CHECK((local_iocon.P2_5 & kMask) == kMask);
    }
    SECTION("Test Set Active level")
    {
        // Source: "UM10562 LPC408x/407x User manual" table 83 page 132
        constexpr uint8_t kInvertPosition = 6;
        constexpr uint32_t kMask          = 0b1 << kInvertPosition;
        test_subject00.SetAsActiveLow(true);
        test_subject25.SetAsActiveLow(false);

        CHECK((local_iocon.P0_0 & kMask) == kMask);
        CHECK((local_iocon.P2_5 & kMask) == 0);

        test_subject00.SetAsActiveLow(false);
        test_subject25.SetAsActiveLow(true);

        CHECK((local_iocon.P0_0 & kMask) == 0);
        CHECK((local_iocon.P2_5 & kMask) == kMask);
    }
    SECTION("Test Fast Mode Set")
    {
        // Source: "UM10562 LPC408x/407x User manual" table 83 page 132
        constexpr uint8_t kSlewPosition = 9;
        constexpr uint32_t kMask        = 0b1 << kSlewPosition;
        test_subject00.EnableFastMode(true);
        test_subject25.EnableFastMode(false);

        CHECK((local_iocon.P0_0 & kMask) == kMask);
        CHECK((local_iocon.P2_5 & kMask) == 0);

        test_subject00.EnableFastMode(false);
        test_subject25.EnableFastMode(true);

        CHECK((local_iocon.P0_0 & kMask) == 0);
        CHECK((local_iocon.P2_5 & kMask) == kMask);
    }
    SECTION("Test Open Drain")
    {
        // Source: "UM10562 LPC408x/407x User manual" table 83 page 132
        constexpr uint8_t kOpenDrainPosition = 10;
        constexpr uint32_t kMask             = 0b1 << kOpenDrainPosition;
        test_subject00.SetAsOpenDrain(true);
        test_subject25.SetAsOpenDrain(false);

        CHECK((local_iocon.P0_0 & kMask) == kMask);
        CHECK((local_iocon.P2_5 & kMask) == 0);

        test_subject00.SetAsOpenDrain(false);
        test_subject25.SetAsOpenDrain(true);

        CHECK((local_iocon.P0_0 & kMask) == 0);
        CHECK((local_iocon.P2_5 & kMask) == kMask);
    }
}
