#pragma once

#include <cstdint>
#include "utility/log.hpp"
#include "L1_Peripheral/i2c.hpp"

// Change these address values later

namespace sjsu
{
namespace robotics
{
class MagneticEncoder
{
 public:
  static constexpr uint8_t kMagneticEncoderAddress   = 0x36;
  static constexpr uint8_t kRawAngleHighRegister     = 0x0E;
  static constexpr uint8_t kRawAngleLowRegister      = 0x0F;
  static constexpr uint8_t kMsbShift                 = 8;
  static constexpr units::angle::degree_t kZeroAngle = 0_deg;
  explicit constexpr MagneticEncoder(sjsu::I2c & i2c)
      : current_angle_(kZeroAngle), i2c_(i2c)
  {
  }
  void Initialize()
  {
    i2c_.Initialize();
    // idk what goes here figure the address out later
    i2c_.Write(kMagneticEncoderAddress, { 0x1 });
  }

  units::angle::degree_t GetAngle() const
  {
    // i have no idea how to read with i2c, gotta read datasheet here
    int tilt_reading;
    int16_t axis_tilt;
    LOG_INFO("before: %d", axis_tilt);
    uint8_t tilt_val[2];
    i2c_.WriteThenRead(kMagneticEncoderAddress,
                       { kRawAngleLowRegister },
                       tilt_val,
                       sizeof(tilt_val));
    tilt_reading = (tilt_val[0] << kMsbShift) | tilt_val[1];
    axis_tilt    = static_cast<int16_t>(tilt_reading);
    LOG_INFO("after: %d %d", axis_tilt, tilt_val[1]);
    units::angle::degree_t angle(axis_tilt);
    return angle;
  }
  void SetZero()
  {
    // do some i2c here to set the current angle to zero.
    current_angle_ = kZeroAngle;
  }

 private:
  units::angle::degree_t current_angle_;
  const sjsu::I2c & i2c_;
};
}  // namespace robotics
}  // namespace sjsu
