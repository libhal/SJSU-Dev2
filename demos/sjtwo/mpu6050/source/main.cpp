#include <cstdint>

#include "peripherals/lpc40xx/i2c.hpp"
#include "devices/sensors/movement/accelerometer/mpu6050.hpp"
#include "utility/log.hpp"

int main()
{
  sjsu::LogInfo("Mpu6050 Application Starting...");
  sjsu::LogInfo(
      "This example will access an MPU6050 accelerometer on the I2C bus. If "
      "the accelerometer is accesable, then it will print out the accerometer "
      "readings for each axis");

  sjsu::lpc40xx::I2c & i2c = sjsu::lpc40xx::GetI2c<2>();
  sjsu::Mpu6050 sensor(i2c);

  sjsu::LogInfo("Initializing accelerometer...");
  sensor.Initialize();

  while (true)
  {
    // Read the current acceleration values from the sensor
    auto current_acceleration = sensor.Read();

    // Print the acceleration values
    current_acceleration.Print();

    sjsu::Delay(100ms);
  }
  return 0;
}
