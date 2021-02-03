#include "peripherals/lpc40xx/i2c.hpp"
#include "devices/sensors/environment/temperature/si7060.hpp"
#include "utility/log.hpp"
#include "utility/time/time.hpp"

int main()
{
  sjsu::LogInfo("Starting Si7060 Temperature Sensor Demonstration...");
  sjsu::lpc40xx::I2c & i2c2 = sjsu::lpc40xx::GetI2c<2>();
  sjsu::Si7060 temperature_sensor(i2c2);

  temperature_sensor.Initialize();
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
