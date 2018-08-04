#include <cstdint>
#include "L0_LowLevel/LPC40xx.h"
#include "L0_LowLevel/startup.hpp"
#include "L1_Drivers/i2c.hpp"
#include "L5_Testing/testing_frameworks.hpp"

TEST_CASE("Testing I2C", "[i2c]")
{
    // Create a local register
    LPC_I2C_TypeDef local_i2c;
    // Clear local i2c registers
    memset(&local_i2c, 0, sizeof(local_i2c));
    // Setting i2c register to local_i2c
    I2c::i2c[static_cast<uint8_t>(I2c::Port::kI2c0)] = &local_i2c;

    I2c test_subject(I2c::Port::kI2c0);

    SECTION("Initialize")
    {
        // Source: "UM10562 LPC408x/407x User manual" table 84 page 443
        // constexpr uint8_t kPort0Pin0Uart3Txd = 0b010;
        // constexpr uint8_t kPort2Pin5Pwm1Channel6 = 0b001;

        // test_subject.Initialize();
    }
    SECTION("Read Setup")
    {
    }
    SECTION("Write Setup")
    {
    }
    SECTION("Write and Read Setup")
    {
    }
    SECTION("I2C State Machine")
    {
    }
    I2c::i2c[static_cast<uint8_t>(I2c::Port::kI2c0)] = LPC_I2C0;
}
