#include "L0_LowLevel/LPC40xx.h"
#include "L3_HAL/onboard_led.hpp"
#include "L5_Testing/testing_frameworks.hpp"

TEST_CASE("Testing Onboard LEDs", "[onboardled]")
{
    // Creating an object to be tested. The object is then intialized.
    OnBoardLed led_control_test_unit;

    led_control_test_unit.Initialize();
    // These represent the expected values of led_states_test after each test
    constexpr uint8_t kLedStatesExpectedOn     = 0x01;
    constexpr uint8_t kLedStatesExpectedOff    = 0x00;
    constexpr uint8_t kLedStatesExpectedSet    = 0x02;
    constexpr uint8_t kLedStatesExpectedToggle = 0x06;
    constexpr uint8_t kLedStatesExpectedSetAll = 0x0F;

    SECTION("Turning On and Off the LEDs.")
    {
        led_control_test_unit.On(0);
        CHECK(kLedStatesExpectedOn == led_control_test_unit.GetStates());
        led_control_test_unit.Off(0);
        CHECK(kLedStatesExpectedOff == led_control_test_unit.GetStates());
        led_control_test_unit.Set(1, OnBoardLedInterface::LightState::kOn);
        CHECK(kLedStatesExpectedSet == led_control_test_unit.GetStates());
        led_control_test_unit.Toggle(2);
        CHECK(kLedStatesExpectedToggle == led_control_test_unit.GetStates());
        led_control_test_unit.SetAll(0x0F);
        CHECK(kLedStatesExpectedSetAll == led_control_test_unit.GetStates());

        // This is done to turn off all the LEDs before being in use.
        led_control_test_unit.SetAll(0x00);
    }
}
