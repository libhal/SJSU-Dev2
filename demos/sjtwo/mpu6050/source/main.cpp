#include <cstdint>

#include "L1_Peripheral/lpc40xx/i2c.hpp"
#include "L2_HAL/sensors/movement/accelerometer/mpu6050.hpp"
#include "utility/log.hpp"

int main()
{
  sjsu::LogInfo("Mpu6050 Application Starting...");
  sjsu::LogInfo(
      "This example will access an MPU6050 accelerometer on the I2C bus. If "
      "the accelerometer is accesable, then it will print out the accerometer "
      "readings for each axis");

  sjsu::lpc40xx::I2c i2c(sjsu::lpc40xx::I2c::Bus::kI2c2);
  sjsu::Mpu6050 sensor(i2c, 2_SG);

  sjsu::LogInfo("Initializing accelerometer peripherals...");
  SJ2_RETURN_VALUE_ON_ERROR(sensor.Initialize(), 1);

  sjsu::LogInfo("Enabling accelerometer...");
  SJ2_RETURN_VALUE_ON_ERROR(sensor.Enable(), 2);

  while (true)
  {
    // Read the current acceleration values from the sensor
    auto current_acceleration = SJ2_RETURN_VALUE_ON_ERROR(sensor.Read(), 3);

    // Print the acceleration values
    current_acceleration.Print();

    sjsu::Delay(100ms);
  }
  return 0;
}
