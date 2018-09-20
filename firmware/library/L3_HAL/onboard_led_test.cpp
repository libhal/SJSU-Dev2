#include "L0_LowLevel/LPC40xx.h"
#include "L3_HAL/onboard_led.hpp"
#include "L5_Testing/testing_frameworks.hpp"

TEST_CASE("Testing Onboard LEDs", "[onboardled]")
{
    // Creating an object to be tested. The object is then intialized.
    LPC_IOCON_TypeDef local_iocon;
    memset(&local_iocon, 0, sizeof(local_iocon));
    Pin::pin_map = reinterpret_cast<Pin::PinMap_t *>(&local_iocon);
    LPC_GPIO_TypeDef led_local_port[1];
    memset(&led_local_port, 0, sizeof(led_local_port));

    Gpio::gpio_base[1] = &led_local_port[1];

    Gpio p01_01(1, 1);
    Gpio p01_08(1, 8);
    Gpio p01_10(1, 10);
    Gpio p01_15(1, 15);

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
