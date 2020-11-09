#pragma once

#include "L1_Peripheral/i2c.hpp"
#include "L2_HAL/sensors/environment/temperature_sensor.hpp"
#include "utility/bit.hpp"
#include "utility/units.hpp"
#include "utility/math/limits.hpp"
#include "utility/error_handling.hpp"
#include "utility/log.hpp"

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

  void ModuleInitialize() override
  {
    if (i2c_.RequiresConfiguration())
    {
      i2c_.Initialize();
      i2c_.ConfigureClockRate();
      i2c_.Enable();
    }
  }

  void ModuleEnable(bool enable = true) override
  {
    if (enable)
    {
      uint8_t temperature_sensor_id_register;

      i2c_.WriteThenRead(address_, { kIdRegister },
                         &temperature_sensor_id_register, 1);

      if (temperature_sensor_id_register != kExpectedSensorId)
      {
        LogDebug("ID = 0x%02X\n", temperature_sensor_id_register);
        throw Exception(std::errc::no_such_device,
                        "Device ID does not match expected device ID 0x14");
      }
    }
    else
    {
      LogInfo("Disable not supported for this driver.");
    }
  }

  units::temperature::celsius_t GetTemperature() override
  {
    constexpr int32_t kSubtractTemperatureData = BitLimits<14, uint32_t>::Max();

    uint8_t most_significant_register;
    uint8_t least_significant_register;

    // The register will enable the device to collect data once
    // and automatically sets the stop bit to 0 (2nd bit).
    i2c_.Write(address_, { kOneBurstRegister, 0x04 });

    // Enable I2C automatic address increment on read (must be done every time)
    i2c_.Write(address_, { kAutomaticBitRegister, 0x01 });

    // These need to be in separate transactions
    i2c_.WriteThenRead(address_, { kMostSignificantRegister },
                       &most_significant_register, 1);
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
    auto temperature =
        units::temperature::celsius_t{ (acquired_temperature / 160.0f) +
                                       55.0f };

    return temperature;
  }

 private:
  /// The I2C peripheral used for communication with the device.
  sjsu::I2c & i2c_;
  /// The device address used for communication.
  uint8_t address_;
};
}  // namespace sjsu
