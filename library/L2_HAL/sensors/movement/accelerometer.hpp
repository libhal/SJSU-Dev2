#pragma once

#include <cmath>
#include <cstdint>

#include "utility/log.hpp"
#include "utility/status.hpp"
#include "utility/units.hpp"

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
  /// Acceleration along each axis of detection
  struct Acceleration_t
  {
    units::acceleration::meters_per_second_squared_t x;
    units::acceleration::meters_per_second_squared_t y;
    units::acceleration::meters_per_second_squared_t z;

    void Print()
    {
      sjsu::LogInfo("{  x: %.4f m/s^2,  y: %.4f m/s^2,  z: %.4f m/s^2 }",
                    x.to<double>(), y.to<double>(), z.to<double>());
    }
  };

  /// Initialize peripherals required to communicate with device. Must be the
  /// first method called on this driver. After this is called, `Enable()` can
  /// be called to enable the device and begin gathering acceleration data.
  ///
  /// @return Error_t if an error occurred. Typically occurs when I2C peripheral
  ///         fails to initialize.
  virtual Returns<void> Initialize() = 0;

  /// This method must be called before running `Read()`. This function will
  /// configure the device settings as defined through the constructor of this
  /// interface's implementation. Some implementations have more detail or
  /// settings than others. For example, the MMA8452q device allows for the
  /// full-scale value to be changed based on the values supplied to the
  /// constructor.
  ///
  /// @return Error_t if an error occurred. Typically occurs if
  ///         (1) I2C peripheral could not speak to the device due to
  ///             connection issue.
  ///         (2) Device address is incorrect.
  ///         (3) Detected Device ID does not match the driver.
  virtual Returns<void> Enable() = 0;

  /// Accelerometer driver will read each axis of acceleration and convert the
  /// data to m/s^2.
  ///
  /// @return An Acceleration object which contains the acceleration in the
  ///         X, Y, and Z axis.
  virtual Returns<Acceleration_t> Read() = 0;
};
}  // namespace sjsu
