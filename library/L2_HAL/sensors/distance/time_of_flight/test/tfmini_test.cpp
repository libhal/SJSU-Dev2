// @ingroup SJSU-Dev2
// @defgroup TFMini unit tests
// @brief This file contains the unit tests to validate the TFMini device driver
// @{
#include "L2_HAL/sensors/distance/time_of_flight/tfmini.hpp"
#include "L4_Testing/testing_frameworks.hpp"
#include "utility/log.hpp"
#include "utility/bit.hpp"
#include "utility/units.hpp"

namespace sjsu
{
EMIT_ALL_METHODS(TFMini);

namespace
{
template <size_t length>
auto MockReadImplementation(size_t & read_count,
                            const std::array<uint8_t, length> & list)
{
  return [&list, &read_count](void * data_ptr, size_t size) -> size_t {
    uint8_t * data = reinterpret_cast<uint8_t *>(data_ptr);
    for (size_t i = 0; i < size; i++)
    {
      data[i] = list[read_count++];
    }
    return size;
  };
}
}  // namespace

TEST_CASE("Testing TFMini")
{
  Mock<Uart> mock_uart;
  Fake(Method(mock_uart, Initialize),
       ConstOverloadedMethod(mock_uart, Write, void(const void *, size_t)));

  Uart & uart = mock_uart.get();
  TFMini test(uart);

  SECTION("Initialize()")
  {
    constexpr uint32_t kBaudRate = 115200;

    size_t read_count                                        = 0;
    const std::array<uint8_t, 8 * 6> kExpectedInitializeData = {
      0x42, 0x57, 0x02, 0x01, 0x00, 0x00, 0x01, 0x02,  // Successful Ack
      0x42, 0x57, 0x02, 0x01, 0x00, 0x00, 0x01, 0x02,  // Successful Ack
      0x42, 0x57, 0x02, 0x01, 0x00, 0x00, 0x01, 0x02,  // Successful Ack
      0x42, 0x57, 0x02, 0x01, 0x00, 0x00, 0x01, 0x02,  // Successful Ack
      0x42, 0x57, 0x02, 0x01, 0x00, 0x00, 0x01, 0x02,  // Successful Ack
      0x42, 0x57, 0x02, 0x01, 0x00, 0x00, 0x00, 0x02,  // Exit Config
    };

    auto init_read_callback =
        MockReadImplementation(read_count, kExpectedInitializeData);

    When(Method(mock_uart, HasData)).AlwaysReturn(true);
    When(ConstOverloadedMethod(mock_uart, Read, size_t(void *, size_t)))
        .AlwaysDo(init_read_callback);

    // Exercise
    test.Initialize();

    // Verify:
    // Verify: Check that uart initialization uses the correct Baud rate
    Verify(Method(mock_uart, Initialize).Using(kBaudRate));
    // Verify: Check the entering of Config mode
    Verify(ConstOverloadedMethod(mock_uart, Write, void(const void *, size_t))
               .Using(TFMini::kConfigCommand, TFMini::kCommandLength));
    // Verify: Check setting of external trigger mode
    Verify(ConstOverloadedMethod(mock_uart, Write, void(const void *, size_t))
               .Using(TFMini::kSetExternalTriggerMode, TFMini::kCommandLength));
    // Verify: Check Dist units are set to millimeters
    Verify(ConstOverloadedMethod(mock_uart, Write, void(const void *, size_t))
               .Using(TFMini::kSetDistUnitMM, TFMini::kCommandLength));
    // Verify: Check that config mode was exited
    Verify(ConstOverloadedMethod(mock_uart, Write, void(const void *, size_t))
               .Using(TFMini::kExitConfigCommand, TFMini::kCommandLength));
    // Verify: Check that we wrote commands exactly 4 different times
    Verify(ConstOverloadedMethod(mock_uart, Write, void(const void *, size_t)))
        .Exactly(4);
  }

  SECTION("GetDistance()")
  {
    // Setup
    static constexpr units::length::millimeter_t kExpectedDistance = 291_mm;
    size_t read_count                                              = 0;
    constexpr uint8_t kLowerByte =
        bit::Extract(kExpectedDistance.to<uint32_t>(), 0, 8);
    constexpr uint8_t kHigherByte =
        bit::Extract(kExpectedDistance.to<uint32_t>(), 8, 8);
    // =========================================================================
    // | Byte0-1 | Byte 2 | Byte 3 | Byte 4  | Byte 5  | Byte6 | Byte7| Byte8  |
    // |  0x59   | Dist_L | Dist_H | Stren_L | Stren_H | Mode  | 0x00 |Checksum|
    // =========================================================================
    std::array<uint8_t, TFMini::kDeviceDataLength> device_result = {
      0x59, 0x59, kLowerByte, kHigherByte, 0xDC, 0x05, 0x02, 0x00,
    };

    // Setup: Generate the checksum
    device_result.end()[-1] =
        std::accumulate(device_result.begin(), &device_result.end()[-2], 0);

    // Setup: Assume that we always have data available
    When(Method(mock_uart, HasData)).AlwaysReturn(true);

    auto dist_read_callback = MockReadImplementation(read_count, device_result);
    When(ConstOverloadedMethod(mock_uart, Read, size_t(void *, size_t)))
        .AlwaysDo(dist_read_callback);

    SECTION("Success")
    {
      // Exercise
      auto distance = test.GetDistance();

      // Verify
      CHECK(distance == kExpectedDistance);
      Verify(
          ConstOverloadedMethod(mock_uart, Write, void(const void *, size_t))
              .Using(TFMini::kPromptMeasurementCommand, TFMini::kCommandLength))
          .Exactly(1);
    }

    SECTION("BusError")
    {
      // Setup: throw one of the bytes off from the checksum
      device_result[4] = 0x34;

      // Exercise
      // Verify
      SJ2_CHECK_EXCEPTION(test.GetDistance(), std::errc::io_error);
    }

    SECTION("Device Not Found")
    {
      SECTION("First byte is invalid")
      {
        device_result[0] = 0x00;
      }

      SECTION("Second byte is invalid")
      {
        device_result[1] = 0x00;
      }

      // Exercise
      SJ2_CHECK_EXCEPTION(test.GetDistance(), std::errc::no_such_device);
    }

    // Verify
    Verify(
        ConstOverloadedMethod(mock_uart, Write, void(const void *, size_t))
            .Using(TFMini::kPromptMeasurementCommand, TFMini::kCommandLength));
    Verify(ConstOverloadedMethod(mock_uart, Write, void(const void *, size_t)))
        .Exactly(1);
  }

  SECTION("GetSignalStrengthPercent()")
  {
    constexpr uint16_t kExpectedStrength = 0x05DC;
    constexpr float kExpectedStrengthRange =
        static_cast<float>(kExpectedStrength) / TFMini::kStrengthUpperBound;

    constexpr uint8_t kLowerByte  = bit::Extract(kExpectedStrength, 1, 8);
    constexpr uint8_t kHigherByte = bit::Extract(kExpectedStrength, 9, 8);

    std::array<uint8_t, TFMini::kDeviceDataLength> device_result = {
      0x59, 0x59, kLowerByte, kHigherByte, 0xDC, 0x05, 0x02, 0x00,
    };

    // Setup: Generate the checksum
    device_result.end()[-1] =
        std::accumulate(device_result.begin(), &device_result.end()[-2], 0);

    // Setup: Assume that we always have data available
    When(Method(mock_uart, HasData)).AlwaysReturn(true);

    size_t read_count      = 0;
    auto strength_callback = MockReadImplementation(read_count, device_result);
    When(ConstOverloadedMethod(mock_uart, Read, size_t(void *, size_t)))
        .AlwaysDo(strength_callback);

    constexpr float kEpsilon = 0.001f;

    SECTION("Proper Operation")
    {
      // Exercise
      auto strength = test.GetSignalStrengthPercent();

      // Verify
      CHECK(strength ==
            doctest::Approx(kExpectedStrengthRange).epsilon(kEpsilon));
    }

    SECTION("Invalid Device Header")
    {
      // Setup
      SECTION("First byte is invalid")
      {
        device_result[0] = 0x00;
      }

      SECTION("Second byte is invalid")
      {
        device_result[1] = 0x00;
      }

      // Exercise
      // Verify
      SJ2_CHECK_EXCEPTION(test.GetSignalStrengthPercent(),
                          std::errc::no_such_device);
    }

    SECTION("Invalid Checksum")
    {
      // Setup
      // Setup: throw one of the bytes off from the checksum
      device_result[4] = 0x34;

      // Exercise
      // Verify
      SJ2_CHECK_EXCEPTION(test.GetSignalStrengthPercent(), std::errc::io_error);
    }

    Verify(
        ConstOverloadedMethod(mock_uart, Write, void(const void *, size_t))
            .Using(TFMini::kPromptMeasurementCommand, TFMini::kCommandLength));
    Verify(ConstOverloadedMethod(mock_uart, Write, void(const void *, size_t)))
        .Exactly(1);
  }

  SECTION("SetMinSignalThreshhold()")
  {
    // Setup
    std::array<uint8_t, TFMini::kCommandLength * 4> device_result = {
      0x42, 0x57, 0x02, 0x01, 0x00, 0x00, 0x01, 0x02,  // kSuccessfulAck
      0x42, 0x57, 0x02, 0x01, 0x00, 0x00, 0x01, 0x02,  // kSuccessfulAck
      0x42, 0x57, 0x02, 0x01, 0x00, 0x00, 0x00, 0x02,  // kExitConfig
    };

    // Setup: Assume that we always have data available
    When(Method(mock_uart, HasData)).AlwaysReturn(true);

    size_t read_count      = 0;
    auto strength_callback = MockReadImplementation(read_count, device_result);
    When(ConstOverloadedMethod(mock_uart, Read, size_t(void *, size_t)))
        .AlwaysDo(strength_callback);

    SECTION("Successfully return using edge case 0")
    {
      // Exercise
      test.SetMinSignalThreshhold(0);
    }

    SECTION("Successfully return in proper usage")
    {
      // Exercise
      test.SetMinSignalThreshhold(50);
    }

    SECTION("Successfully return using edge case: Above the threshold cap")
    {
      // Exercise
      test.SetMinSignalThreshhold(100);
    }

    // TODO(#1052): Missing verification for
    //              SendCommandAndCheckEcho(updated_min_threshold_command)

    // Verify
    // Verify: that the command to enter config has been used
    Verify(ConstOverloadedMethod(mock_uart, Write, void(const void *, size_t))
               .Using(TFMini::kConfigCommand, TFMini::kCommandLength));
    // Verify: that the command to exit config has been used
    Verify(ConstOverloadedMethod(mock_uart, Write, void(const void *, size_t))
               .Using(TFMini::kExitConfigCommand, TFMini::kCommandLength));
    // Verify: that we wrote commands exactly 3 different times
    Verify(ConstOverloadedMethod(mock_uart, Write, void(const void *, size_t)))
        .Exactly(3);
  }
}
}  // namespace sjsu
