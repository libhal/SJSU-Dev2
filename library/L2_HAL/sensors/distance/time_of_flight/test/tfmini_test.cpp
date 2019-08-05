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
TEST_CASE("Testing tfmini", "[tfmini]")
{
  Mock<Uart> mock_uart;
  Fake(Method(mock_uart, Initialize),
       ConstOverloadedMethod(mock_uart, Write, void(const uint8_t *, size_t)));

  Uart & uart = mock_uart.get();
  TFMini test(uart);

  SECTION("Check Initialize")
  {
    constexpr uint32_t kBaudRate = 115200;
    // Assuming Uart Initialization is successful
    When(Method(mock_uart, Initialize)).Return(sjsu::Status::kSuccess);
    auto init_read_callback =
        [](uint8_t * data,
           size_t size,
           std::chrono::microseconds timeout) -> sjsu::Status {
      static constexpr uint8_t kSuccessfulAck[8] = { 0x42, 0x57, 0x02, 0x01,
                                                     0x00, 0x00, 0x01, 0x02 };
      static constexpr uint8_t kExitConfig[8]    = { 0x42, 0x57, 0x02, 0x01,
                                                  0x00, 0x00, 0x00, 0x02 };
      static uint8_t count                       = 0;
      count++;

      CHECK(timeout == TFMini::kTimeout);
      // Each Write prompts an Acknowledge sequence sent by the device
      // The driver first Writes to enter Config mode then send two
      //   config commands
      // --> Prompts 3 Acks from Device
      // The fourth Write exits Config mode, which prompts an Exit Ack
      if (count != 4)
      {
        for (size_t iterator = 0; iterator < size; iterator++)
        {
          data[iterator] = kSuccessfulAck[iterator];
        }
      }
      else
      {
        for (size_t iterator = 0; iterator < size; iterator++)
        {
          data[iterator] = kExitConfig[iterator];
        }
      }
      return sjsu::Status::kSuccess;
    };
    When(ConstOverloadedMethod(
             mock_uart,
             Read,
             sjsu::Status(uint8_t *, size_t, std::chrono::microseconds)))
        .AlwaysDo(init_read_callback);
    CHECK(test.Initialize() == sjsu::Status::kSuccess);
    // Check that uart initialization uses the correct Baud rate
    Verify(Method(mock_uart, Initialize).Using(kBaudRate));
    // Check the entering of Config mode
    Verify(
        ConstOverloadedMethod(mock_uart, Write, void(const uint8_t *, size_t))
            .Using(TFMini::kConfigCommand, TFMini::kCommandLength));
    // Check setting of external trigger mode
    Verify(
        ConstOverloadedMethod(mock_uart, Write, void(const uint8_t *, size_t))
            .Using(TFMini::kSetExternalTriggerMode, TFMini::kCommandLength));
    // Check Dist units are set to milimeters
    Verify(
        ConstOverloadedMethod(mock_uart, Write, void(const uint8_t *, size_t))
            .Using(TFMini::kSetDistUnitMM, TFMini::kCommandLength));
    // Check that config mode was exited
    Verify(
        ConstOverloadedMethod(mock_uart, Write, void(const uint8_t *, size_t))
            .Using(TFMini::kExitConfigCommand, TFMini::kCommandLength));
    // Check that we wrote commands exactly 4 different times
    Verify(
        ConstOverloadedMethod(mock_uart, Write, void(const uint8_t *, size_t)))
        .Exactly(4);
  }

  SECTION("Check GetDistance")
  {
    Status get_dist_status;
    units::length::millimeter_t distance_check                     = 0_mm;
    static constexpr units::length::millimeter_t kExpectedDistance = 291_mm;
    auto dist_read_callback =
        [](uint8_t * data,
           size_t size,
           std::chrono::microseconds timeout) -> sjsu::Status {
      static constexpr uint8_t kReadData[8] = {
        0x59,
        0x59,
        bit::Extract(kExpectedDistance.to<uint32_t>(), 0, 8),
        bit::Extract(kExpectedDistance.to<uint32_t>(), 8, 8),
        0xDC,
        0x05,
        0x02,
        0x00
      };
      uint8_t checksum     = 0;
      static uint8_t count = 0;
      count++;

      CHECK(timeout == TFMini::kTimeout);
      for (int i = 0; i < 8; i++)
      {
        checksum += kReadData[i];
      }
      // =============================================================
      // |Byte0-1|Byte2 | Byte3 | Byte4 | Byte5 |Byte6|Byte7| Byte8  |
      // | 0x59  |Dist_L| Dist_H|Stren_L|Stren_H|Mode |0x00 |Checksum|
      // =============================================================
      // First test checks that expected values work.
      // Second test observes change in the device header
      // Third test observes an error in the checksum
      for (size_t iterator = 0; iterator < (size - 1); iterator++)
      {
        data[iterator] = kReadData[iterator];
      }
      if (count == 2)
      {
        data[0]        = 0x00;
        data[size - 1] = static_cast<uint8_t>(checksum - kReadData[0]);
      }
      else if (count == 3)
      {
        data[size - 1] = 0x00;
      }
      else
      {
        data[size - 1] = static_cast<uint8_t>(checksum);
      }
      return sjsu::Status::kSuccess;
    };
    When(ConstOverloadedMethod(
             mock_uart,
             Read,
             sjsu::Status(uint8_t *, size_t, std::chrono::microseconds)))
        .AlwaysDo(dist_read_callback);

    // Checking Proper Operation
    get_dist_status = test.GetDistance(&distance_check);
    CHECK(get_dist_status == sjsu::Status::kSuccess);
    CHECK(distance_check == kExpectedDistance);
    Verify(
        ConstOverloadedMethod(mock_uart, Write, void(const uint8_t *, size_t))
            .Using(TFMini::kPromptMeasurementCommand, TFMini::kCommandLength));
    Verify(
        ConstOverloadedMethod(mock_uart, Write, void(const uint8_t *, size_t)))
        .Exactly(1);

    // Checking Invalid Device Header
    get_dist_status = test.GetDistance(&distance_check);
    CHECK(get_dist_status == sjsu::Status::kDeviceNotFound);
    CHECK(distance_check ==
          std::numeric_limits<units::length::millimeter_t>::max());

    // Checking Invalid Checksum
    get_dist_status = test.GetDistance(&distance_check);
    CHECK(get_dist_status == sjsu::Status::kBusError);
    CHECK(distance_check ==
          std::numeric_limits<units::length::millimeter_t>::max());
  }

  SECTION("Check GetSignalStrengthPercent")
  {
    Status get_stren_status;
    float strength_check                 = 0;
    constexpr uint16_t kExpectedStrength = 0x05DC;
    constexpr float kExpectedStrengthRange =
        static_cast<float>(kExpectedStrength / TFMini::kStrengthUpperBound);
    constexpr float kExpectedError = -1.0f;
    auto stren_read_callback =
        [](uint8_t * data,
           size_t size,
           std::chrono::microseconds timeout) -> sjsu::Status {
      static constexpr uint8_t kReadData[8] = {
        0x59,
        0x59,
        0x23,
        0x01,
        bit::Extract(kExpectedStrength, 1, 8),
        bit::Extract(kExpectedStrength, 9, 8),
        0x02,
        0x00
      };
      uint8_t checksum     = 0;
      static uint8_t count = 0;
      count++;

      CHECK(timeout == TFMini::kTimeout);
      for (int i = 0; i < 8; i++)
      {
        checksum += kReadData[i];
      }
      // =============================================================
      // |Byte0-1|Byte2 | Byte3 | Byte4 | Byte5 |Byte6|Byte7| Byte8  |
      // | 0x59  |Dist_L| Dist_H|Stren_L|Stren_H|Mode |0x00 |Checksum|
      // =============================================================
      // First call checks that expected values work.
      // Second call observes change in the device header
      // Third call observes an error in the checksum
      for (size_t iterator = 0; iterator < (size - 1); iterator++)
      {
        data[iterator] = kReadData[iterator];
      }
      if (count == 2)
      {
        data[0]        = 0x00;
        data[size - 1] = static_cast<uint8_t>(checksum - kReadData[0]);
      }
      else if (count == 3)
      {
        data[size - 1] = 0x00;
      }
      else
      {
        data[size - 1] = static_cast<uint8_t>(checksum);
      }
      return sjsu::Status::kSuccess;
    };
    When(ConstOverloadedMethod(
             mock_uart,
             Read,
             sjsu::Status(uint8_t *, size_t, std::chrono::microseconds)))
        .AlwaysDo(stren_read_callback);

    // Checking Proper Operation
    get_stren_status = test.GetSignalStrengthPercent(&strength_check);
    CHECK(get_stren_status == sjsu::Status::kSuccess);
    CHECK(strength_check < (kExpectedStrengthRange + 0.001f));
    CHECK(strength_check > (kExpectedStrengthRange - 0.001f));
    Verify(
        ConstOverloadedMethod(mock_uart, Write, void(const uint8_t *, size_t))
            .Using(TFMini::kPromptMeasurementCommand, TFMini::kCommandLength));
    Verify(
        ConstOverloadedMethod(mock_uart, Write, void(const uint8_t *, size_t)))
        .Exactly(1);

    // Checking Invalid Device Header
    get_stren_status = test.GetSignalStrengthPercent(&strength_check);
    CHECK(get_stren_status == sjsu::Status::kDeviceNotFound);
    CHECK(strength_check < (kExpectedError + 0.001f));
    CHECK(strength_check > (kExpectedError - 0.001f));

    // Checking Invalid Checksum
    get_stren_status = test.GetSignalStrengthPercent(&strength_check);
    CHECK(get_stren_status == sjsu::Status::kBusError);
    CHECK(strength_check < (kExpectedError + 0.001f));
    CHECK(strength_check > (kExpectedError - 0.001f));
  }

  SECTION("Check SetMinSignalThreshhold")
  {
    uint8_t test_numbers[3] = { 0, 50, 100 };
    auto min_threshold_update =
        [](uint8_t * data,
           size_t size,
           std::chrono::microseconds timeout) -> sjsu::Status {
      static constexpr uint8_t kSuccessfulAck[8] = { 0x42, 0x57, 0x02, 0x01,
                                                     0x00, 0x00, 0x01, 0x02 };
      static constexpr uint8_t kExitConfig[8]    = { 0x42, 0x57, 0x02, 0x01,
                                                  0x00, 0x00, 0x00, 0x02 };
      static uint8_t count                       = 0;
      count++;

      CHECK(timeout == TFMini::kTimeout);
      // Each Write prompts an Acknowledge sequence sent by the device
      // The driver first Writes to enter Config mode then send one
      //   additional config commands
      // --> Prompts 2 Acks from Device
      // The third Write exits Config mode, which prompts an Exit Ack
      if (count != 3)
      {
        for (size_t iterator = 0; iterator < size; iterator++)
        {
          data[iterator] = kSuccessfulAck[iterator];
        }
      }
      else
      {
        for (size_t iterator = 0; iterator < size; iterator++)
        {
          data[iterator] = kExitConfig[iterator];
        }
      }
      return sjsu::Status::kSuccess;
    };
    When(ConstOverloadedMethod(
             mock_uart,
             Read,
             sjsu::Status(uint8_t *, size_t, std::chrono::microseconds)))
        .AlwaysDo(min_threshold_update);

    // Verify successful return using edge case 0
    CHECK(test.SetMinSignalThreshhold(test_numbers[0]) ==
          sjsu::Status::kSuccess);
    // Check that the command to enter config has been used
    Verify(
        ConstOverloadedMethod(mock_uart, Write, void(const uint8_t *, size_t))
            .Using(TFMini::kConfigCommand, TFMini::kCommandLength));
    // Check that the command to exit config has been used
    Verify(
        ConstOverloadedMethod(mock_uart, Write, void(const uint8_t *, size_t))
            .Using(TFMini::kExitConfigCommand, TFMini::kCommandLength));
    // Check that we wrote commands exactly 3 different times
    Verify(
        ConstOverloadedMethod(mock_uart, Write, void(const uint8_t *, size_t)))
        .Exactly(3);
    // Verify successful return in proper usage
    CHECK(test.SetMinSignalThreshhold(test_numbers[1]) ==
          sjsu::Status::kSuccess);
    // Verify successful return using edge case: Above the threshold cap
    CHECK(test.SetMinSignalThreshhold(test_numbers[2]) ==
          sjsu::Status::kSuccess);
  }
}
}  // namespace sjsu
