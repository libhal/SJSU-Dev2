#include "L2_HAL/sensors/movement/accelerometer/mma8452q.hpp"

#include <array>

#include "L4_Testing/testing_frameworks.hpp"

namespace sjsu
{
EMIT_ALL_METHODS(Mma8452q);

TEST_CASE("Accelerometer")
{
  Mock<sjsu::I2c> mock_i2c;
  MockProtocol<MemoryAccessProtocol::AddressWidth::kByte1> mock_map;
  Mma8452q test_subject(mock_map, mock_i2c.get());

  // Setup: Set the WhoAmI register to the correct value
  mock_map[Mma8452q::Map::kWhoAmI] = 0x2A;

  SECTION("Initialize")
  {
    // Setup
    Fake(Method(mock_i2c, ModuleInitialize),
         Method(mock_i2c, ConfigureClockRate),
         Method(mock_i2c, ModuleEnable));

    // Exercise
    test_subject.ModuleInitialize();

    // Verify
    Verify(Method(mock_i2c, ModuleInitialize),
           Method(mock_i2c, ConfigureClockRate),
           Method(mock_i2c, ModuleEnable));
  }

  SECTION("Enable")
  {
    SECTION("IsValidDevice() = false")
    {
      // Setup
      // Setup: Change WhoAmI to an inccorrect register value.
      mock_map[Mma8452q::Map::kWhoAmI] = 0x22;

      // Exercise
      // Verify
      SJ2_CHECK_EXCEPTION(test_subject.ModuleEnable(),
                          std::errc::no_such_device);
    }

    SECTION("Success")
    {
      // Setup
      mock_map[Mma8452q::Map::kControlReg1] = 0xFF;

      // Exercise
      test_subject.ModuleEnable();

      // Verify
      uint8_t actual_control = mock_map[Mma8452q::Map::kControlReg1];
      CHECK(0x01 == actual_control);
    }
  }

  SECTION("Enable Various Gravities")
  {
    SECTION("2 standard gravity")
    {
      // Exercise
      test_subject.ConfigureFullScale(2_SG);

      // Verify
      uint8_t actual_config = mock_map[Mma8452q::Map::kDataConfig];
      CHECK((2 >> 2) == actual_config);
    }

    SECTION("4 standard gravity")
    {
      // Exercise
      test_subject.ConfigureFullScale(4_SG);

      // Verify
      uint8_t actual_config = mock_map[Mma8452q::Map::kDataConfig];
      CHECK((4 >> 2) == actual_config);
    }

    SECTION("8 standard gravity")
    {
      // Exercise
      test_subject.ConfigureFullScale(8_SG);

      // Verify
      uint8_t actual_config = mock_map[Mma8452q::Map::kDataConfig];
      CHECK((8 >> 2) == actual_config);
    }
  }

  SECTION("Read")
  {
    // Signed 12-bit value has a maximum
    constexpr int16_t kQuarterMaxAcceleration =
        BitLimits<12, int16_t>::Max() / 4;
    constexpr units::acceleration::meters_per_second_squared_t kExpectedValue =
        0.5_SG;
    constexpr units::acceleration::meters_per_second_squared_t kExpectedZero =
        0_SG;

    SECTION("X has value, Y and Z = 0")
    {
      // Setup
      // Setup: Value
      mock_map[Mma8452q::Map::kXYZStartAddress] =
          std::array<int16_t, 3>{ kQuarterMaxAcceleration << 4, 0, 0 };

      // Exercise
      auto acceleration = test_subject.Read();

      // Verify
      CHECK(acceleration.x.to<float>() ==
            doctest::Approx(kExpectedValue.to<float>()).epsilon(0.01f));
      CHECK(acceleration.y.to<float>() ==
            doctest::Approx(kExpectedZero.to<float>()).epsilon(0.01f));
      CHECK(acceleration.z.to<float>() ==
            doctest::Approx(kExpectedZero.to<float>()).epsilon(0.01f));
    }

    SECTION("Y has value, X and Z = 0")
    {
      // Setup
      // Setup: Value
      mock_map[Mma8452q::Map::kXYZStartAddress] =
          std::array<int16_t, 3>{ 0, kQuarterMaxAcceleration << 4, 0 };

      // Exercise
      auto acceleration = test_subject.Read();

      // Verify
      CHECK(acceleration.x.to<float>() ==
            doctest::Approx(kExpectedZero.to<float>()).epsilon(0.01f));
      CHECK(acceleration.y.to<float>() ==
            doctest::Approx(kExpectedValue.to<float>()).epsilon(0.01f));
      CHECK(acceleration.z.to<float>() ==
            doctest::Approx(kExpectedZero.to<float>()).epsilon(0.01f));
    }

    SECTION("Z has value, X and Y = 0")
    {
      // Setup
      // Setup: Value
      mock_map[Mma8452q::Map::kXYZStartAddress] =
          std::array<int16_t, 3>{ 0, 0, kQuarterMaxAcceleration << 4 };

      // Exercise
      auto acceleration = test_subject.Read();

      // Verify
      CHECK(acceleration.x.to<float>() ==
            doctest::Approx(kExpectedZero.to<float>()).epsilon(0.01f));
      CHECK(acceleration.y.to<float>() ==
            doctest::Approx(kExpectedZero.to<float>()).epsilon(0.01f));
      CHECK(acceleration.z.to<float>() ==
            doctest::Approx(kExpectedValue.to<float>()).epsilon(0.01f));
    }

    SECTION("All Contain Values Z, X and Y")
    {
      // Setup
      // Setup: Value
      mock_map[Mma8452q::Map::kXYZStartAddress] =
          std::array<int16_t, 3>{ kQuarterMaxAcceleration << 4,
                                  kQuarterMaxAcceleration << 4,
                                  kQuarterMaxAcceleration << 4 };

      // Exercise
      auto acceleration = test_subject.Read();

      // Verify
      CHECK(acceleration.x.to<float>() ==
            doctest::Approx(kExpectedValue.to<float>()).epsilon(0.01f));
      CHECK(acceleration.y.to<float>() ==
            doctest::Approx(kExpectedValue.to<float>()).epsilon(0.01f));
      CHECK(acceleration.z.to<float>() ==
            doctest::Approx(kExpectedValue.to<float>()).epsilon(0.01f));
    }
  }
}
}  // namespace sjsu
