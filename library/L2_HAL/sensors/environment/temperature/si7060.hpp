#pragma once

#include "L1_Peripheral/i2c.hpp"
#include "L2_HAL/sensors/environment/temperature_sensor.hpp"
#include "utility/bit.hpp"
#include "utility/units.hpp"
#include "utility/status.hpp"

namespace sjsu
{
/// The Si7060 temperature sensor is a device capable of obtaining temperature
/// measurements ranging between  -47.4 °C  to +157.39 °C.
class Si7060 final : public TemperatureSensor
{
 public:
  /// The default I2c address of the device.
  static constexpr uint8_t kDefaultAddress = 0x31;
  /// The address of the 8-bit read-only register containing the device's chip
  /// id (most significant 4-bits) and revision id (least significant 4-bits).
  static constexpr uint8_t kIdRegister = 0xC0;
  /// The expected 8-bit chip id and revision id.
  static constexpr uint8_t kExpectedSensorId = 0x14;
  /// The address of the register to enabled one-burst mode. One-burst mode
  /// pauses the device after performing one temperature conversion.
  static constexpr uint8_t kOneBurstRegister = 0xC4;
  /// The address of the register to enable automatic incrementing of the I2c
  /// register pointer.
  static constexpr uint8_t kAutomaticBitRegister = 0xC5;
  /// The address of the register containing the most significant byte of the
  /// temperature data.
  static constexpr uint8_t kMostSignificantRegister = 0xC1;

  /// The device's I2c device address is factory programmed and can be one of
  /// four values.
  ///
  /// @note To determine the device address, refer to Section 5. Ordering Guide
  ///       of the data-sheet.
  ///
  /// @param i2c The I2C peripheral used for communication with the device.
  /// @param address The device's factory programmed I2c device address.
  explicit constexpr Si7060(sjsu::I2c & i2c, uint8_t address = kDefaultAddress)
      : i2c_(i2c), address_(address)
  {
  }
  /// Initializes the I2C peripheral to enable the device for use.
  ///
  /// @return The initialization status.
  Status Initialize() const override
  {
    Status status;

    status = i2c_.Initialize();
    if (status != Status::kSuccess)
    {
      return status;
    }
    uint8_t temperature_sensor_id_register;
    // is the same as the Base ID (0x14).
    status = i2c_.WriteThenRead(
        address_, { kIdRegister }, &temperature_sensor_id_register, 1);
    if (status != Status::kSuccess)
    {
      return status;
    }
    if (temperature_sensor_id_register != kExpectedSensorId)
    {
      status = Status::kDeviceNotFound;
    }
    return status;
  }
  /// Retrieves the temperature reading from the device.
  ///
  /// @param temperature Output parameter.
  /// @return Returns Status::kSuccess if the temperature measurement was
  ///         successfully obtained.
  Status GetTemperature(
      units::temperature::celsius_t * temperature) const override
  {
    // Note that: 1 << 14 = 2^14 = 16384
    constexpr int32_t kSubtractTemperatureData = (1 << 14);

    uint8_t most_significant_register;
    uint8_t least_significant_register;
    // The register will enable the device to collect data once
    // and automatically sets the stop bit to 0 (2nd bit).
    i2c_.Write(address_, { kOneBurstRegister, 0x04 });
    // Auto increments I2c register address pointer.
    i2c_.Write(address_, { kAutomaticBitRegister, 0x01 });
    i2c_.WriteThenRead(
        address_, { kMostSignificantRegister }, &most_significant_register, 1);
    i2c_.Read(address_, &least_significant_register, 1);

    // The write and read operation sets the most significant bit to one,
    // telling the register that the data has been read.
    // Therefore must clear the most significant bit from above.
    most_significant_register = bit::Clear(most_significant_register, 7);
    // Combine temperature register data
    int32_t temperature_data =
        (most_significant_register << 8) | least_significant_register;
    // The required computation after bit shifting.
    // Formula can be found below, see page 4:
    // datasheets/Temperature-Sensor/si7060-datasheets.pdf
    float acquired_temperature =
        static_cast<float>(temperature_data - kSubtractTemperatureData);
    *temperature =
        units::temperature::celsius_t((acquired_temperature / 160.0f) + 55.0f);

    return Status::kSuccess;
  }

 private:
  /// The I2C peripheral used for communication with the device.
  const sjsu::I2c & i2c_;
  /// The device address used for communication.
  uint8_t address_;
};
}  // namespace sjsu
