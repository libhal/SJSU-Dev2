#include "L0_LowLevel/LPC40xx.h"
#include "L1_Drivers/gpio.hpp"
#include "L5_Testing/testing_frameworks.hpp"

TEST_CASE("Testing Gpio", "[gpio]")
{
    // Declared constants that are to be used within the different sections
    // of this unit test
    constexpr uint8_t kPin0 = 0;
    constexpr uint8_t kPin7 = 7;

    // Initialized local LPC_GPIO_TypeDef objects with 0 to observe how the Gpio
    // class manipulates the data in the registers
    LPC_GPIO_TypeDef local_gpio_port[2] =
    {
        {0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0}
    };

    // Only GPIO port 1 & 2 will be used in this unit test
    Gpio::gpio_base[0] = &local_gpio_port[0];
    Gpio::gpio_base[1] = &local_gpio_port[1];

    // Pins that are to be used in the unit test
    Gpio p0_00(0, 0);
    Gpio p1_07(1, 7);

    SECTION("Set as Output and Input")
    {
        // Source: "UM10562 LPC408x/407x User manual" table 96 page 148
        constexpr uint8_t kInputSet = 0b0;
        constexpr uint8_t kOutputSet = 0b1;

        p0_00.SetAsInput();
        p1_07.SetAsOutput();

        // Check bit 0 of local_gpio_port[0].DIR (port 0 pin 0)
        // to see if it is cleared
        CHECK(((local_gpio_port[0].DIR >> kPin0) & 1) == kInputSet);
        // Check bit 7 of local_gpio_port[1].DIR (port 1 pin 7)
        // to see if it is set
        CHECK(((local_gpio_port[1].DIR >> kPin7) & 1) == kOutputSet);

        p0_00.SetDirection(GpioInterface::kOutput);
        p1_07.SetDirection(GpioInterface::kInput);

        // Check bit 0 of local_gpio_port[0].DIR (port 0 pin 0)
        // to see if it is set
        CHECK(((local_gpio_port[0].DIR >> kPin0) & 1) == kOutputSet);
        // Check bit 7 of local_gpio_port[1].DIR (port 1 pin 7)
        // to see if it is cleared
        CHECK(((local_gpio_port[1].DIR >> kPin7) & 1) == kInputSet);
    }
    SECTION("Set High")
    {
        // Source: "UM10562 LPC408x/407x User manual" table 99 page 149
        constexpr uint8_t kHighSet = 0b1;

        p0_00.SetHigh();
        p1_07.Set(GpioInterface::kHigh);

        // Check bit 0 of local_gpio_port[0].SET (port 0 pin 0)
        // to see if it is set
        CHECK(((local_gpio_port[0].SET >> kPin0) & 1) == kHighSet);
        // Check bit 7 of local_gpio_port[1].SET (port 1 pin 7)
        // to see if it is set
        CHECK(((local_gpio_port[1].SET >> kPin7) & 1) == kHighSet);
    }
    SECTION("Set Low")
    {
        // Source: "UM10562 LPC408x/407x User manual" table 100 page 150
        constexpr uint8_t kLowSet = 0b1;

        p0_00.SetLow();
        p1_07.Set(GpioInterface::kLow);

        // Check bit 0 of local_gpio_port[0].CLR (port 0 pin 0)
        // to see if it is set
        CHECK(((local_gpio_port[0].CLR >> kPin0) & 1) == kLowSet);
        // Check bit 7 of local_gpio_port[1].CLR (port 1 pin 7)
        // to see if it is set
        CHECK(((local_gpio_port[1].CLR >> kPin7) & 1) == kLowSet);
    }
    SECTION("Read Pin")
    {
        // Source: "UM10562 LPC408x/407x User manual" section 8.5.1.3 page 148
        constexpr uint8_t kLowRead = 0b0;
        constexpr uint8_t kHighRead = 0b1;

        // Clearing bit 0 of local_gpio_port[0].PIN (port 0 pin 0) in order to
        // read the pin value through the ReadPin method
        local_gpio_port[0].PIN &= ~(1 << kPin0);
        // Setting bit 7 of local_gpio_port[1].PIN (port 1 pin 7) in order to
        // read the pin value through the ReadPin method
        local_gpio_port[1].PIN |= (1 << kPin7);

        CHECK(p0_00.ReadPin() == kLowRead);
        CHECK(p1_07.ReadPin() == kHighRead);
    }
    Gpio::gpio_base[0] = LPC_GPIO0;
    Gpio::gpio_base[1] = LPC_GPIO1;
    Gpio::gpio_base[2] = LPC_GPIO2;
    Gpio::gpio_base[3] = LPC_GPIO3;
    Gpio::gpio_base[4] = LPC_GPIO4;
    Gpio::gpio_base[5] = LPC_GPIO5;
}
