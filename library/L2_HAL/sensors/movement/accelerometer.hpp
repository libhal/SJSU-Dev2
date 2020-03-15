#pragma once

#include <cmath>
#include <cstdint>

// TODO(#1080): Refactor Accelerometer
namespace sjsu
{
/// @ingroup movement
/// Abstract interface for devices that behave as accelerometers.
/// Accelerometers are devices that can measure acceleration in X, Y, or Z axis.
/// On the earth's surface, if the accelerometer is held still, and it is
/// oriented flat on one of its axes, it will measure approximately 9.8 m/s^2 on
/// that axis.
class Accelerometer
{
 public:
  /// Initialize peripherals required to communicate with device, enables
  /// accelerometer device. After running this function, code can read from the
  /// X, Y, and Z methods as well as configure other settings of the device.
  /// This method must be called before any others are called in this class.
  virtual bool Initialize() = 0;

  /// @return the acceleration on the X axis.
  virtual int32_t X() const = 0;

  /// @return the acceleration on the Y axis.
  virtual int32_t Y() const = 0;

  /// @return the acceleration on the Z axis.
  virtual int32_t Z() const = 0;

  /// @return the scale multiplier of gravity. If this returns 1, then the
  /// maximum returned acceleration will be 9.8 m/s^2. If it is 2, then the
  /// maximum acceleration value would be 2 time that.
  virtual int GetFullScaleRange() const = 0;

  /// Set the full scale range
  /// @param range_value - full scale multiplier. If set to 2 then the maximum
  /// value of any axis will represent 2 x 9.8m/s^2. If it was set to 1 then the
  /// maximum value of any axis will represent 9.8m/s^2.
  virtual void SetFullScaleRange(uint8_t range_value) = 0;

  /// Computes the pitch orientation of the accelerometer
  ///
  /// @return the degrees of pitch from -90 to 90 degrees as a float.
  float Pitch() const
  {
    static constexpr float kRadiansToDegree = 180.0f / 3.14f;
    float x                                 = static_cast<float>(X());
    float y                                 = static_cast<float>(Y());
    float z                                 = static_cast<float>(Z());
    float pitch_numerator                   = x * -1.0f;
    float pitch_denominator                 = sqrtf((y * y) + (z * z));
    float pitch = atan2f(pitch_numerator, pitch_denominator) * kRadiansToDegree;
    return pitch;
  }

  /// Computes the roll orientation of the device.
  ///
  /// @return the degrees of roll in -90 to 90 degrees
  float Roll() const
  {
    static constexpr float kRadiansToDegree = 180.0f / 3.14f;
    float y                                 = static_cast<float>(Y());
    float z                                 = static_cast<float>(Z());
    return (atan2f(y, z) * kRadiansToDegree);
  }
};
}  // namespace sjsu
