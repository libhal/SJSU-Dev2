#pragma once

#include <cmath>
#include <cstdint>

#include "module.hpp"
#include "utility/error_handling.hpp"
#include "utility/log.hpp"
#include "utility/math/units.hpp"

namespace sjsu
{
/// Generic settings for a standard Accelerometer device
struct AccelerometerSettings_t
{
  /// Set the maximum absolute acceleration that can be read by the
  /// accelerometer. NOT calling this before calling Enable() will result in the
  /// default full scale being used. Please consult the datasheet to see if this
  /// fits your application.
  ///
  /// In general accelerometers have a fixed number of bits that respresents te
  /// precisions of their measurements. Setting a smaller maximum full scale
  /// results in a higher precision measurement, but means that the highest
  /// measureable acceleration is decreased.
  ///
  /// Vice-versa for setting a higher maximum scale. Precision is sacraficed,
  /// but the maximum acceleration is larger.
  ///
  /// Its a trade off that is very application specific. In general most
  /// orientation measurements will require 2Gs of precision to account for
  /// accelerations caused by translation rather than simply rotation.
  units::acceleration::standard_gravity_t gravity = 2_SG;
};

/// @ingroup movement
/// Abstract interface for devices that behave as accelerometers.
/// Accelerometers are devices that can measure acceleration in X, Y, or Z axis.
/// On the earth's surface, if the accelerometer is held still, and it is
/// oriented flat on one of its axes, it will measure approximately 9.8 m/s^2 on
/// that axis.
class Accelerometer : public Module<AccelerometerSettings_t>
{
 public:
  /// Acceleration along each axis of detection
  struct Acceleration_t
  {
    /// Acceleration in the x axis
    units::acceleration::meters_per_second_squared_t x;
    /// Acceleration in the y axis
    units::acceleration::meters_per_second_squared_t y;
    /// Acceleration in the z axis
    units::acceleration::meters_per_second_squared_t z;

    /// Print the acceleration of this object.
    void Print()
    {
      sjsu::LogInfo("{  x: %.4f m/s^2,  y: %.4f m/s^2,  z: %.4f m/s^2 }",
                    x.to<double>(),
                    y.to<double>(),
                    z.to<double>());
    }
  };

  /// Accelerometer driver will read each axis of acceleration and convert the
  /// data to m/s^2.
  ///
  /// @return An Acceleration object which contains the acceleration in the
  ///         X, Y, and Z axis.
  virtual Acceleration_t Read() = 0;
};
}  // namespace sjsu
