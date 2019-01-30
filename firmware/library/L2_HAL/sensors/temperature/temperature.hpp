#pragma once

#include "L1_Drivers/i2c.hpp"
#include "utility/bit.hpp"

class TemperatureInterface
{
 public:
  virtual bool Initialize()     = 0;
  virtual float GetCelsius()    = 0;
  virtual float GetFahrenheit() = 0;
};

class Temperature : public TemperatureInterface
{
 public:
  static constexpr uint8_t kDeviceAddress           = 0x31;
  static constexpr uint8_t kIdRegister              = 0xC0;
  static constexpr uint8_t kExpectedSensorId        = 0x14;
  static constexpr uint8_t kOneBurstRegister        = 0xC4;
  static constexpr uint8_t kAutonicBitRegister      = 0xC5;
  static constexpr uint8_t kMostSignificantRegister = 0xC1;

  constexpr Temperature() : i2c_(&lpc40xx_i2c_) {}

  explicit Temperature(I2cInterface * lpc40xx_i2c_select)
      : i2c_(lpc40xx_i2c_select)
  {
  }

  bool Initialize() override
  {
    i2c_->Initialize();
    uint8_t temperature_sensor_id_register;
    // is the same as the Base ID (0x14).
    i2c_->WriteThenRead(kDeviceAddress, { kIdRegister },
                        &temperature_sensor_id_register, 1);
    return (temperature_sensor_id_register == kExpectedSensorId);
  }

  float GetCelsius() override
  {
    uint8_t most_significant_register;
    uint8_t least_significant_register;
    // The register will enable the device to collect data once
    // and automatically sets the stop bit to 0 (2nd bit).
    i2c_->Write(kDeviceAddress, { kOneBurstRegister, 0x04 }, 2);
    // Auto increments I2c register address pointer.
    i2c_->Write(kDeviceAddress, { kAutonicBitRegister, 0x01 }, 2);
    i2c_->WriteThenRead(kDeviceAddress, { kMostSignificantRegister },
                        &most_significant_register, 1);
    i2c_->Read(kDeviceAddress, &least_significant_register, 1);

    uint32_t temperature_data = 0;
    // Shift all bits to the 15th bit.
    temperature_data =
        bit::Insert(temperature_data, most_significant_register, 8, 16);
    // The write and read operation sets the most significant bit to one,
    // telling the register that the data has been read.
    // Therefore must clear the most significant bit from above.
    temperature_data = bit::Clear(temperature_data, 15);
    temperature_data =
        bit::Insert(temperature_data, least_significant_register, 0, 8);
    // The required computation after bit shifting.
    // Formula can be found below, see page 4:
    // datasheets/Temperature-Sensor/si7060-datasheets.pdf
    // Note that: 1 << 14 = 2^14 = 16384
    uint16_t new_temperature_data = static_cast<uint16_t>(temperature_data);
    constexpr float kSubtractTemperatureData = static_cast<float>(1 << 14);
    return (((new_temperature_data - kSubtractTemperatureData) / 160.0f)
            + 55.0f);
  }

  float GetFahrenheit() override
  {
    // General formula used to convert Celsius to Fahrenheit
    return (((GetCelsius() * 9.0f) / 5.0f) - 32.0f);
  }

 private:
  I2cInterface * i2c_;
  I2c lpc40xx_i2c_;
};
