#pragma once

#include <cstdint>
#include <cmath>

#include "L1_Peripheral/i2c.hpp"
#include "L2_HAL/sensors/movement/accelerometer.hpp"

namespace sjsu
{
class Mma8452q : public Accelerometer
{
 public:
  static constexpr uint16_t kDataOffset         = 16;
  static constexpr float kRadiansToDegree       = 180.0f / 3.14f;
  static constexpr uint8_t kWhoAmIExpectedValue = 0x2a;
  static constexpr uint8_t kMsbShift            = 8;

  // in units of 9.8 m/s^2 or "g"
  static constexpr int kMaxAccelerationScale[4]         = { 2, 4, 8, -1 };
  static constexpr uint8_t kSetMaxAccelerationScale[16] = {
    0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00,
    0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
  };

  enum RegisterMap : uint8_t
  {
    kStatus     = 0x00,
    kX          = 0x01,
    kY          = 0x03,
    kZ          = 0x05,
    kWhoAmI     = 0x0d,
    kDataConfig = 0x0e
  };

  explicit constexpr Mma8452q(const I2c & i2c, uint8_t address = 0x1c)
      : i2c_(i2c),
        accelerometer_address_(address)
  {
  }
  bool Initialize() override
  {
    i2c_.Initialize();
    i2c_.Write(accelerometer_address_, { 0x2A, 0x01 });
    uint8_t who_am_i_received_value;
    uint8_t identity_register = RegisterMap::kWhoAmI;
    i2c_.WriteThenRead(accelerometer_address_, &identity_register,
                        sizeof(identity_register), &who_am_i_received_value,
                        sizeof(who_am_i_received_value));
    return (who_am_i_received_value == kWhoAmIExpectedValue);
  }
  int16_t GetAxisValue(uint8_t register_number) const
  {
    int tilt_reading;
    int16_t axis_tilt;
    uint8_t tilt_val[2];
    i2c_.WriteThenRead(accelerometer_address_, { register_number }, tilt_val,
                        sizeof(tilt_val));
    tilt_reading = (tilt_val[0] << kMsbShift) | tilt_val[1];
    axis_tilt    = static_cast<int16_t>(tilt_reading);
    return static_cast<int16_t>(axis_tilt / kDataOffset);
  }
  int16_t X() const override
  {
    return GetAxisValue(RegisterMap::kX);
  }
  int16_t Y() const override
  {
    return GetAxisValue(RegisterMap::kY);
  }
  int16_t Z() const override
  {
    return GetAxisValue(RegisterMap::kZ);
  }
  float Pitch() const override
  {
    float x                 = static_cast<float>(X());
    float y                 = static_cast<float>(Y());
    float z                 = static_cast<float>(Z());
    float pitch_numerator   = x * -1.0f;
    float pitch_denominator = sqrtf((y * y) + (z * z));
    float pitch = atan2f(pitch_numerator, pitch_denominator) * kRadiansToDegree;
    return pitch;
  }
  float Roll() const override
  {
    float y = static_cast<float>(Y());
    float z = static_cast<float>(Z());
    return (atan2f(y, z) * kRadiansToDegree);
  }
  int GetFullScaleRange() const override
  {
    uint8_t config_reg = RegisterMap::kDataConfig;
    uint8_t full_scale_value;
    i2c_.WriteThenRead(accelerometer_address_, { config_reg },
                        &full_scale_value, sizeof(full_scale_value));
    full_scale_value &= 0x03;
    int range = kMaxAccelerationScale[full_scale_value];
    return range;
  }
  void SetFullScaleRange(uint8_t range_value) override
  {
    range_value &= 0x0f;
    uint8_t config_reg                 = RegisterMap::kDataConfig;
    uint8_t send_range                 = kSetMaxAccelerationScale[range_value];
    uint8_t full_scale_write_buffer[2] = { config_reg, send_range };
    i2c_.Write(accelerometer_address_, full_scale_write_buffer,
                sizeof(full_scale_write_buffer));
  }

 private:
  const I2c & i2c_;
  uint8_t accelerometer_address_;
};
}  // namespace sjsu
