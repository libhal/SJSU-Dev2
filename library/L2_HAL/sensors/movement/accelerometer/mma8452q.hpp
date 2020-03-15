#pragma once

#include <cstdint>

#include "L1_Peripheral/i2c.hpp"
#include "L2_HAL/sensors/movement/accelerometer.hpp"

namespace sjsu
{
/// Driver for the MMA8452Q 3-axis accelerometer
class Mma8452q : public Accelerometer
{
 public:
  /// Map of all of the used device addresses in this driver.
  enum RegisterAddress : uint8_t
  {
    /// Device status register address
    kStatus = 0x00,
    /// Register address of the the first byte of the X axis
    kX = 0x01,
    /// Register address of the the first byte of the Y axis
    kY = 0x03,
    /// Register address of the the first byte of the Z axis
    kZ = 0x05,
    /// Device ID register address
    kWhoAmI = 0x0d,
    /// Device configuration starting address
    kDataConfig = 0x0e
  };

  /// @param i2c - i2c peripheral used to commnicate with device.
  /// @param address - device address.
  explicit constexpr Mma8452q(const I2c & i2c, uint8_t address = 0x1c)
      : i2c_(i2c), accelerometer_address_(address)
  {
  }

  bool Initialize() override
  {
    i2c_.Initialize();
    i2c_.Write(accelerometer_address_, { 0x2A, 0x01 });
    uint8_t who_am_i_received_value;
    uint8_t identity_register = RegisterAddress::kWhoAmI;
    i2c_.WriteThenRead(accelerometer_address_,
                       &identity_register,
                       sizeof(identity_register),
                       &who_am_i_received_value,
                       sizeof(who_am_i_received_value));

    constexpr uint8_t kWhoAmIExpectedValue = 0x2a;
    return (who_am_i_received_value == kWhoAmIExpectedValue);
  }

  int32_t X() const override
  {
    return GetAxisValue(RegisterAddress::kX);
  }

  int32_t Y() const override
  {
    return GetAxisValue(RegisterAddress::kY);
  }

  int32_t Z() const override
  {
    return GetAxisValue(RegisterAddress::kZ);
  }

  int GetFullScaleRange() const override
  {
    static constexpr int kMaxAccelerationScale[4] = { 2, 4, 8, -1 };
    uint8_t full_scale_value;
    i2c_.WriteThenRead(accelerometer_address_,
                       { RegisterAddress::kDataConfig },
                       &full_scale_value,
                       sizeof(full_scale_value));
    full_scale_value &= 0x03;
    int range = kMaxAccelerationScale[full_scale_value];
    return range;
  }
  void SetFullScaleRange(uint8_t range_value) override
  {
    // in units of 9.8 m/s^2 or "g"
    static constexpr uint8_t kSetMaxAccelerationScale[16] = {
      0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00,
      0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
    };

    range_value &= 0x0f;
    i2c_.Write(accelerometer_address_,
               { RegisterAddress::kDataConfig,
                 kSetMaxAccelerationScale[range_value] });
  }

 private:
  int16_t GetAxisValue(uint8_t register_number) const
  {
    constexpr uint16_t kDataOffset = 16;
    constexpr uint8_t kMsbShift    = 8;
    int tilt_reading;
    int16_t axis_tilt;
    uint8_t tilt_val[2];
    i2c_.WriteThenRead(accelerometer_address_,
                       { register_number },
                       tilt_val,
                       sizeof(tilt_val));
    tilt_reading = (tilt_val[0] << kMsbShift) | tilt_val[1];
    axis_tilt    = static_cast<int16_t>(tilt_reading);
    return static_cast<int16_t>(axis_tilt / kDataOffset);
  }

  const I2c & i2c_;
  uint8_t accelerometer_address_;
};
}  // namespace sjsu
