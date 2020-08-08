#include <array>

#include "L2_HAL/sensors/movement/accelerometer/mma8452q.hpp"
#include "L4_Testing/testing_frameworks.hpp"

namespace sjsu
{
EMIT_ALL_METHODS(Mma8452q);

TEST_CASE("Accelerometer")
{
  Mock<sjsu::I2c> mock_i2c;
  Fake(Method(mock_i2c, Initialize));

  MockProtocol<MemoryAccessProtocol::AddressWidth::kByte1> mock_map;
  Mma8452q test_subject(mock_map, mock_i2c.get());

  // Setup: Set the WhoAmI register to the correct value
  mock_map[Mma8452q::Map::kWhoAmI] = 0x2A;

  SECTION("Initialize")
  {
    SECTION("Success")
    {
      // Setup
      When(Method(mock_i2c, Initialize)).AlwaysReturn({});

      // Exercise
      auto result = test_subject.Initialize();

      // Verify
      Verify(Method(mock_i2c, Initialize));
      // Verify: Initialize should not have an error
      CHECK(result);
    }

    SECTION("Failure")
    {
      // Setup
      const auto kExpectedStatus = Error(Status::kNotReadyYet, "");
      When(Method(mock_i2c, Initialize)).AlwaysReturn(kExpectedStatus);

      // Exercise
      auto result = test_subject.Initialize();

      // Verify
      Verify(Method(mock_i2c, Initialize));
      // Verify: Initialize should not have an error
      CHECK(!result);
    }
  }

  SECTION("Enable")
  {
    SECTION("IsValidDevice() = false")
    {
      // Setup
      // Setup: Change WhoAmI to an inccorrect register value.
      mock_map[Mma8452q::Map::kWhoAmI] = 0x22;

      // Exercise
      auto result = test_subject.Enable();

      // Verify
      CHECK(!result);
      CHECK(*result.error() == Error_t(Status::kDeviceNotFound));
    }

    SECTION("Success")
    {
      // Setup
      mock_map[Mma8452q::Map::kControlReg1] = 0xFF;
      mock_map[Mma8452q::Map::kDataConfig]  = 0xFF;

      // Exercise
      auto result                     = test_subject.Enable();
      Returns<uint8_t> actual_control = mock_map[Mma8452q::Map::kControlReg1];
      Returns<uint8_t> actual_config  = mock_map[Mma8452q::Map::kDataConfig];

      // Verify
      CHECK(result);
      CHECK(0x01 == actual_control.value());
      CHECK((2 >> 2) == actual_config.value());
    }
  }

  SECTION("Enable Various Gravities")
  {
    SECTION("2 standard gravity")
    {
      // Setup
      Mma8452q gravity_test_subject(mock_map, mock_i2c.get(), 2_SG);

      // Exercise
      auto result                     = gravity_test_subject.Enable();
      Returns<uint8_t> actual_control = mock_map[Mma8452q::Map::kControlReg1];
      Returns<uint8_t> actual_config  = mock_map[Mma8452q::Map::kDataConfig];

      // Verify
      CHECK(result);
      CHECK(0x01 == actual_control.value());
      CHECK((2 >> 2) == actual_config.value());
    }

    SECTION("4 standard gravity")
    {
      // Setup
      Mma8452q gravity_test_subject(mock_map, mock_i2c.get(), 4_SG);

      // Exercise
      auto result            = gravity_test_subject.Enable();
      Returns<uint8_t> actual_control = mock_map[Mma8452q::Map::kControlReg1];
      Returns<uint8_t> actual_config  = mock_map[Mma8452q::Map::kDataConfig];

      // Verify
      CHECK(result);
      CHECK(0x01 == actual_control.value());
      CHECK((4 >> 2) == actual_config.value());
    }

    SECTION("8 standard gravity")
    {
      // Setup
      Mma8452q gravity_test_subject(mock_map, mock_i2c.get(), 8_SG);

      // Exercise
      auto result            = gravity_test_subject.Enable();
      Returns<uint8_t> actual_control = mock_map[Mma8452q::Map::kControlReg1];
      Returns<uint8_t> actual_config  = mock_map[Mma8452q::Map::kDataConfig];

      // Verify
      CHECK(result);
      CHECK(0x01 == actual_control.value());
      CHECK((8 >> 2) == actual_config.value());
    }
  }

  SECTION("Read")
  {
    // Signed 12-bit value has a maximum
    constexpr int16_t kQuaterMaxAcceleration =
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
          std::array<int16_t, 3>{ kQuaterMaxAcceleration << 4, 0, 0 };

      // Exercise
      auto result = test_subject.Read();

      // Verify
      // Verify: Initialize should not have an error
      REQUIRE(result);

      auto acceleration = result.value();

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
          std::array<int16_t, 3>{ 0, kQuaterMaxAcceleration << 4, 0 };

      // Exercise
      auto result = test_subject.Read();

      // Verify
      // Verify: Initialize should not have an error
      REQUIRE(result);

      auto acceleration = result.value();

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
          std::array<int16_t, 3>{ 0, 0, kQuaterMaxAcceleration << 4 };

      // Exercise
      auto result = test_subject.Read();

      // Verify
      // Verify: Initialize should not have an error
      REQUIRE(result);

      auto acceleration = result.value();

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
          std::array<int16_t, 3>{ kQuaterMaxAcceleration << 4,
                                  kQuaterMaxAcceleration << 4,
                                  kQuaterMaxAcceleration << 4 };

      // Exercise
      auto result = test_subject.Read();

      // Verify
      // Verify: Initialize should not have an error
      REQUIRE(result);

      auto acceleration = result.value();

      CHECK(acceleration.x.to<float>() ==
            doctest::Approx(kExpectedValue.to<float>()).epsilon(0.01f));
      CHECK(acceleration.y.to<float>() ==
            doctest::Approx(kExpectedValue.to<float>()).epsilon(0.01f));
      CHECK(acceleration.z.to<float>() ==
            doctest::Approx(kExpectedValue.to<float>()).epsilon(0.01f));
    }

    SECTION("Failure")
    {
      // Setup
      constexpr auto kError = Error_t(Status::kDeviceNotFound);
      mock_map.SetError(kError);

      // Setup: Value
      mock_map[Mma8452q::Map::kXYZStartAddress] =
          std::array<int16_t, 3>{ kQuaterMaxAcceleration << 4,
                                  kQuaterMaxAcceleration << 4,
                                  kQuaterMaxAcceleration << 4 };

      // Exercise
      auto result = test_subject.Read();

      // Verify
      REQUIRE(!result);
      CHECK(*result.error() == kError);
    }
  }
}
}  // namespace sjsu
