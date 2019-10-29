#pragma once

#include "L2_HAL/sensors/environment/temperature.hpp"

#include "L1_Peripheral/i2c.hpp"
#include "utility/bit.hpp"
#include "utility/log.hpp"

namespace sjsu
{
class Tmp102 final : public Temperature
{
 public:
  /// Device addresses based on the connection of the address pin, A0.
  /// For example, if A0 is physically connected to ground, then the device
  /// address is configured to be 0b100'1000.
  struct DeviceAddress  // NOLINT
  {
   public:
    static constexpr uint8_t kGround      = 0b100'1000;
    static constexpr uint8_t kVoltageHigh = 0b100'1001;
    static constexpr uint8_t kSda         = 0b100'1010;
    static constexpr uint8_t kScl         = 0b100'1011;
  };

  struct RegisterAddress  // NOLINT
  {
   public:
    static constexpr uint8_t kTemperature   = 0x00;
    static constexpr uint8_t kConfiguration = 0x01;
  };

  static constexpr uint8_t kOneShotShutdownMode = 0x81;
  /// Max time for the device to complete one temperature conversion.
  static constexpr std::chrono::milliseconds kConversionTimeout = 30ms;

  explicit constexpr Tmp102(
      I2c & i2c, uint8_t device_address = sjsu::Tmp102::DeviceAddress::kGround)
      : i2c_(i2c), kDeviceAddress(device_address)
  {
  }

  Status Initialize() override
  {
    return i2c_.Initialize();
  }

  Status GetTemperature(units::temperature::celsius_t * temperature) override
  {
    OneShotShutdown();
    constexpr uint8_t kBufferLength = 2;
    uint8_t temperature_buffer[kBufferLength];
    // Note: The MSB is received first in the buffer.
    i2c_.WriteThenRead(kDeviceAddress,
                       { RegisterAddress::kTemperature },
                       &temperature_buffer[0],
                       kBufferLength,
                       kConversionTimeout);
    // The temperature value is at bits [15:3].
    const int32_t kTemperatureData =
        (temperature_buffer[0] << 4) | (temperature_buffer[1] >> 4);
    constexpr float kResolution = 0.0625f;
    *temperature                = units::temperature::celsius_t(
        static_cast<float>(kTemperatureData) * kResolution);
    return Status::kSuccess;
  }

 private:
  void OneShotShutdown() const
  {
    i2c_.Write(kDeviceAddress,
               { RegisterAddress::kConfiguration, kOneShotShutdownMode });
  }

  const I2c & i2c_;
  const uint8_t kDeviceAddress;
};
}  // namespace sjsu
