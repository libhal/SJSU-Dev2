#pragma once

#include "L1_Peripheral/i2c.hpp"
#include "L2_HAL/sensors/environment/temperature.hpp"
#include "utility/bit.hpp"
#include "utility/units.hpp"
#include "utility/status.hpp"

namespace sjsu
{
class Si7060 final : public Temperature
{
 public:
  static constexpr uint8_t kDefaultAddress          = 0x31;
  static constexpr uint8_t kIdRegister              = 0xC0;
  static constexpr uint8_t kExpectedSensorId        = 0x14;
  static constexpr uint8_t kOneBurstRegister        = 0xC4;
  static constexpr uint8_t kAutomaticBitRegister    = 0xC5;
  static constexpr uint8_t kMostSignificantRegister = 0xC1;

  explicit Si7060(I2c & i2c, uint8_t address = kDefaultAddress)
      : i2c_(i2c), address_(address)
  {
  }

  Status Initialize() override
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

  Status GetTemperature(units::temperature::celsius_t * temperature) override
  {
    // Note that: 1 << 14 = 2^14 = 16384
    constexpr int32_t kSubtractTemperatureData = 1 << 14;

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
  const I2c & i2c_;
  uint8_t address_;
};
}  // namespace sjsu
