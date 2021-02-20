// @ingroup SJSU-Dev2
// @defgroup TFMini driver
// @brief This driver provides the software to use the TFMini, a time of flight
// sensor
// @{
#pragma once

#include <array>
#include <cstdint>
#include <limits>
#include <numeric>
#include <span>

#include "peripherals/uart.hpp"
#include "devices/sensors/distance/distance_sensor.hpp"
#include "utility/error_handling.hpp"
#include "utility/log.hpp"

namespace sjsu
{
/// Distance sensor driver for the TFMini LiDAR module
/// Find link here: https://www.sparkfun.com/products/14588
class TFMini final : public DistanceSensor
{
 public:
  /// Default timeout for reading responses from the TFMini LiDar module.
  /// The device's test latency is 100Hz, which is 10ms per sample, thus a
  /// timeout time of 3x that should give enough time to acquire the sample and
  /// respond back via UART.
  static constexpr std::chrono::nanoseconds kTimeout = 3 * 10ms;

  /// Response packet frame header used for detecting the start of a proper
  /// response packet.
  static constexpr uint8_t kFrameHeader = 0x59;

  /// Used to set the device to long range mode (currently not utilized)
  static constexpr uint8_t kLongDistMode = 0x07;

  /// Sensor signal strength lower bounds
  static constexpr uint8_t kStrengthLowerLimitCap = 80;

  /// Sensor signal strength upper bounds
  static constexpr uint32_t kStrengthUpperBound = 3000;

  /// Device response packet length
  static constexpr uint8_t kDeviceDataLength = 9;

  /// Device command packet length
  static constexpr uint8_t kCommandLength = 8;

  /// Command packet to enable configuration of device
  static constexpr std::array<uint8_t, kCommandLength> kConfigCommand = {
    0x42, 0x57, 0x02, 0x00, 0x00, 0x00, 0x01, 0x02,
  };

  /// Command packet to disable configuration of device
  static constexpr std::array<uint8_t, kCommandLength> kExitConfigCommand = {
    0x42, 0x57, 0x02, 0x00, 0x00, 0x00, 0x00, 0x02
  };

  /// Command packet to set device units to millimeters.
  static constexpr std::array<uint8_t, kCommandLength> kSetDistUnitMM = {
    0x42, 0x57, 0x02, 0x00, 0x00, 0x00, 0x00, 0x1A,
  };

  /// Command packet to allow distance capture on kPromptMeasurementCommand
  /// command.
  static constexpr std::array<uint8_t, kCommandLength>
      kSetExternalTriggerMode = {
        0x42, 0x57, 0x02, 0x00, 0x00, 0x00, 0x00, 0x40
      };

  /// Command packet to request distance measurement data along with signal
  /// strength data.
  static constexpr std::array<uint8_t, kCommandLength>
      kPromptMeasurementCommand = { 0x42, 0x57, 0x02, 0x00,
                                    0x00, 0x00, 0x00, 0x41 };

  /// @param uart - UART peripheral connected to the RX and TX of the TFMini
  ///               module.
  explicit constexpr TFMini(Uart & uart) : uart_(uart) {}

  /// Initialize and enable hardware. This must be called before any other
  /// method in this interface is called.
  /// Sets the baud rate as well as configures the TFMini to have an external
  /// trigger and sets the distance units to mm
  ///
  /// @returns std::errc::io_error if initialization fails
  void ModuleInitialize() override
  {
    static constexpr UartSettings_t kSettings = {
      .baud_rate = 115200,
    };

    uart_.settings = kSettings;
    uart_.Initialize();

    if (!SendCommandAndCheckEcho(kConfigCommand) ||
        !SendCommandAndCheckEcho(kSetExternalTriggerMode) ||
        !SendCommandAndCheckEcho(kSetDistUnitMM) ||
        !SendCommandAndCheckEcho(kExitConfigCommand))
    {
      throw Exception(std::errc::io_error, "Enabling device failed!");
    }
  }

  /// Obtain the distance to the object directly in front of the sensor
  ///
  /// @return the distance an object is from the TFMini in mm.
  ///         If the signal strength is lower than the min threshold, or if the
  ///         device is not recognized, the distance will be unreliable and set
  ///         to std::numeric_limits<uint32_t>::max(). If the actual distance is
  ///         greater than 12000mm, the distance value caps at 12000
  units::length::millimeter_t GetDistance() override
  {
    std::array<uint8_t, kDeviceDataLength> device_data = { 0 };

    uart_.Write(kPromptMeasurementCommand);
    uart_.Read(device_data, kTimeout);

    VerifyData(device_data);

    uint32_t dist = device_data[3] << 8 | device_data[2];
    return units::length::millimeter_t(dist);
  }

  /// Obtain the strength of the light pulse the sensor emits.
  ///
  /// @return the strength of the light pulse, calculated by
  ///         strength_val / 3000. Recommended that readings are reliable if in
  ///         range .7%-67%, lower limit is adjustable -> SetMinSignalThreshhold
  float GetSignalStrengthPercent() override
  {
    std::array<uint8_t, kDeviceDataLength> device_data = { 0 };

    uart_.Write(kPromptMeasurementCommand);
    uart_.Read(device_data, kTimeout);

    VerifyData(device_data);

    uint32_t strength_bytes = device_data[5] << 8 | device_data[4];
    return (static_cast<float>(strength_bytes) / kStrengthUpperBound);
  }

  /// Update the signal strength's lower limit valid range.
  ///
  /// @param lower_threshold - the value to set the lower threshold as.
  ///        default value is 20, caps at 80. Decrease the value to increase the
  ///        measurement range, increase value to improve reliability.
  void SetMinSignalThreshhold(uint8_t lower_threshold = 20)
  {
    constexpr std::array<uint8_t, kCommandLength> kUpdateMinThresholdCommand = {
      0x42, 0x57, 0x02, 0x00, 0xEE, 0x00, 0x00, 0x20
    };
    constexpr uint8_t kThresholdByte = 4;
    std::array<uint8_t, kCommandLength> updated_min_threshold_command;

    uint8_t low_limit = lower_threshold;

    if (low_limit > kStrengthLowerLimitCap)
    {
      low_limit = kStrengthLowerLimitCap;
    }

    updated_min_threshold_command                 = kUpdateMinThresholdCommand;
    updated_min_threshold_command[kThresholdByte] = low_limit;

    if (!SendCommandAndCheckEcho(kConfigCommand) ||
        !SendCommandAndCheckEcho(updated_min_threshold_command) ||
        !SendCommandAndCheckEcho(kExitConfigCommand))
    {
      throw Exception(std::errc::io_error,
                      "Setting minimum threshold values failed.");
    }

    min_threshold_ = lower_threshold;
  }

 private:
  void VerifyData(std::span<uint8_t> data)
  {
    if (data[0] != kFrameHeader || data[1] != kFrameHeader)
    {
      SJ2_PRINT_VARIABLE(data[0], "%u");
      SJ2_PRINT_VARIABLE(data[1], "%u");
      throw Exception(std::errc::no_such_device, "Frame header is incorrect");
    }

    uint8_t checksum = std::accumulate(data.data(), &data.end()[-2], 0);

    if (checksum != data[8])
    {
      SJ2_PRINT_VARIABLE(checksum, "%u");
      SJ2_PRINT_VARIABLE(data[8], "%u");
      throw Exception(std::errc::io_error, "Checksum came back incorrect");
    }
  }

  bool SendCommandAndCheckEcho(std::span<const uint8_t> command) const
  {
    static constexpr std::array<uint8_t, kCommandLength> kEchoSuccess = {
      0x42, 0x57, 0x02, 0x01, 0x00, 0x00, 0x01, 0x02,
    };
    static constexpr std::array<uint8_t, kCommandLength> kConfigExit = {
      0x42, 0x57, 0x02, 0x01, 0x00, 0x00, 0x00, 0x02,
    };

    std::array<uint8_t, kCommandLength> echo;

    uart_.Write(command);
    uart_.Read(echo, kTimeout);

    return (kEchoSuccess == echo) || (kConfigExit == echo);
  }

  Uart & uart_;
  uint8_t min_threshold_ = 20;
};
}  // namespace sjsu
