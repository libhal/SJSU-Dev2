#include "L1_Peripheral/lpc40xx/i2c.hpp"
#include "L2_HAL/sensors/environment/temperature/si7060.hpp"
#include "utility/log.hpp"
#include "utility/time.hpp"

int main()
{
  sjsu::LogInfo("Starting Si7060 Temperature Sensor Demonstration...");
  sjsu::lpc40xx::I2c i2c2(sjsu::lpc40xx::I2c::Bus::kI2c2);
  sjsu::Si7060 temperature_sensor(i2c2);

  temperature_sensor.Initialize();
  temperature_sensor.Enable();

  sjsu::LogInfo("Si7060 Initialized!");

  while (true)
  {
    units::temperature::celsius_t temperature =
        temperature_sensor.GetTemperature();
    sjsu::LogInfo("Board Temperature: %.4f C", temperature.to<double>());
    sjsu::Delay(1s);
  }

  return 0;
}
