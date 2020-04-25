#include <cstdint>

#include "L1_Peripheral/lpc40xx/i2c.hpp"
#include "L2_HAL/sensors/movement/accelerometer/mma8452q.hpp"
#include "utility/log.hpp"

int main()
{
  sjsu::LogInfo("MMA8452q Application Starting...");

  sjsu::lpc40xx::I2c i2c(sjsu::lpc40xx::I2c::Bus::kI2c2);
  sjsu::Mma8452q sensor(i2c);

  sjsu::LogInfo("Initializing accelerometer peripherals...");
  SJ2_RETURN_VALUE_ON_ERROR(sensor.Initialize(), 1);

  sjsu::LogInfo("Enabling accelerometer...");
  SJ2_RETURN_VALUE_ON_ERROR(sensor.Enable(), 1);

  while (true)
  {
    // Read the current acceleration from the sensor
    auto current_acceleration = SJ2_RETURN_VALUE_ON_ERROR(sensor.Read(), 2);

    // Print the acceleration values
    current_acceleration.Print();

    sjsu::Delay(100ms);
  }
  return 0;
}
