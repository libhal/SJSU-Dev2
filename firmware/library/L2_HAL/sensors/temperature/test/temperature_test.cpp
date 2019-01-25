#include "L1_Drivers/i2c.hpp"
#include "L2_HAL/sensors/temperature/temperature.hpp"
#include "L4_Testing/testing_frameworks.hpp"

EMIT_ALL_METHODS(Temperature);

TEST_CASE("Testing temperature sensor", "[temperature]")
{
  Mock<I2cInterface> mock_i2c;

  I2cInterface & test_i2c = mock_i2c.get();
  Fake(Method(mock_i2c, Initialize), Method(mock_i2c, WriteThenRead),
       Method(mock_i2c, Write), Method(mock_i2c, Read));

  Temperature test(&test_i2c);

  SECTION("Check Initialization")
  {
    constexpr uint8_t kTestRegister = 0xC0;
    constexpr uint8_t kI2cReturn = '\0';
    bool check_initialization = test.Initialize();
    Verify(Method(mock_i2c, Initialize),
          Method(mock_i2c, WriteThenRead).Using(0x31, { kTestRegister },
                 &kI2cReturn, 1));
    CHECK(check_initialization == 0);
  }

  SECTION("Check Celsuis")
  {
    constexpr uint8_t kTestRegister = 0xC1;
    constexpr uint8_t kReturn1;
    constexpr uint8_t kReturn2;
    float check_celsius = test.GetCelsius();
    Verify(Method(mock_i2c, Write).Using(0x31, { 0xC4, 0x04 }, 2),
          Method(mock_i2c, Write).Using(0x31, { 0xC5, 0x01 }, 2),
          Method(mock_i2c, WriteThenRead).Using(0x31, { kTestRegister },
                &kReturn1, 1),
          Method(mock_i2c, Read).Using(0x31, &kReturn2, 1));
    CHECK(-47.4 <= check_celsius <= 157.39);
  }

  SECTION("Check Fahrenheit")
  {
    constexpr uint8_t kTestRegister = 0xC1;
    constexpr uint8_t kReturn1;
    constexpr uint8_t kReturn2;
    float check_fahrenheit = test.GetFahranheit();
    Verify(Method(mock_i2c, Write).Using(0x31, { 0xC4, 0x04 }, 2),
          Method(mock_i2c, Write).Using(0x31, { 0xC5, 0x01 }, 2),
          Method(mock_i2c, WriteThenRead).Using(0x31, { kTestRegister },
                &return1, 1),
          Method(mock_i2c, Read).Using(0x31, &return2, 1));
    CHECK(-53.5 <= check_fahrenheit <= 315.3);
  }
}
