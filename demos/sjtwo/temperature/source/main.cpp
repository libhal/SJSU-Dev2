#include "L1_Peripheral/lpc40xx/i2c.hpp"
#include "L2_HAL/sensors/environment/temperature/si7060.hpp"
#include "utility/log.hpp"

int main()
{
  sjsu::lpc40xx::I2c i2c2(sjsu::lpc40xx::I2c::Bus::kI2c2);
  sjsu::Si7060 temperature_sensor(i2c2);

  bool temperature_sensor_initialized = temperature_sensor.Initialize();

  if (!temperature_sensor_initialized)
  {
    LOG_INFO("Could not initialize temperature device (Si7060)\n\n");
    sjsu::Halt();
  }

  LOG_INFO("Temperature Device is ON.\n");

  while (temperature_sensor_initialized)
  {
    double temperature_data;

    temperature_data = static_cast<double>(temperature_sensor.GetCelsius());
    LOG_INFO("Celsius data: %.2g \n", temperature_data);

    // This will only return Fahrenheit.
    temperature_data = static_cast<double>(temperature_sensor.GetFahrenheit());
    LOG_INFO("Fahrenheit data: %.2g \n", temperature_data);
  }
  return 0;
}
