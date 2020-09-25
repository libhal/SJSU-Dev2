#pragma once

#include "L1_Peripheral/i2c.hpp"
#include "L2_HAL/sensors/environment/temperature_sensor.hpp"
#include "utility/log.hpp"

namespace sjsu
{
/// The Tmp102 temperature sensor is a device that utilizes I2C for
/// communication and is capable of measuring temperatures ranging between -40˚C
/// and +125˚C.
class Tmp102 final : public TemperatureSensor
{
 public:
  /// Device addresses based on the connection of the address pin, A0.
  /// For example, if A0 is physically connected to ground, then the device
  /// address is configured to be 0b100'1000.
  struct DeviceAddress  // NOLINT
  {
   public:
    /// The device address when A0 is connected to GND.
    static constexpr uint8_t kGround = 0b100'1000;
    /// The device address when A0 is connected to V+.
    static constexpr uint8_t kVoltageHigh = 0b100'1001;
    /// The device address when A0 is connected to SDA.
    static constexpr uint8_t kSda = 0b100'1010;
    /// The device address when A0 is connected to SCL.
    static constexpr uint8_t kScl = 0b100'1011;
  };

  /// Register addresses of the device used to perform read/write operations.
  struct RegisterAddress  // NOLINT
  {
   public:
    /// The address of the read-only register containing the temperature data.
    static constexpr uint8_t kTemperature = 0x00;
    /// The address of the register used to configure the device.
    static constexpr uint8_t kConfiguration = 0x01;
  };

  /// The command to enable one-shot shutdown mode.
  static constexpr uint8_t kOneShotShutdownMode = 0x81;

  /// Max time for the device to complete one temperature conversion.
  static constexpr std::chrono::milliseconds kConversionTimeout = 30ms;

  /// @param i2c The I2C peripheral used for communication with the device.
  /// @param device_address The device address of the sensor. The addres is
  ///                       configured by physically modifying the connection of
  ///                       the P0 pin.
  explicit constexpr Tmp102(
      sjsu::I2c & i2c,
      uint8_t device_address = sjsu::Tmp102::DeviceAddress::kGround)
      : i2c_(i2c), kDeviceAddress(device_address)
  {
  }

  void ModuleInitialize() override
  {
    if (i2c_.RequiresConfiguration())
    {
      i2c_.Initialize();
      i2c_.ConfigureClockRate();
      i2c_.Enable();
    }
  }

  void ModuleEnable(bool = true) override {}

  units::temperature::celsius_t GetTemperature() override
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

    return units::temperature::celsius_t(static_cast<float>(kTemperatureData) *
                                         kResolution);
  }

 private:
  /// Sets the device to use one-shot shutdown mode. This allows power to be
  /// conserved by putting the device in the shutdown state once a reading is
  /// obtained.
  void OneShotShutdown()
  {
    i2c_.Write(kDeviceAddress,
               { RegisterAddress::kConfiguration, kOneShotShutdownMode });
  }

  /// The I2C peripheral used for communication with the device.
  sjsu::I2c & i2c_;
  /// The configurable device address used for communication.
  const uint8_t kDeviceAddress;
};
}  // namespace sjsu
