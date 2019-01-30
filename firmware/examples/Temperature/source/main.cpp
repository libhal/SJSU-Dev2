#include "L2_HAL/sensors/temperature/temperature.hpp"
#include "utility/log.hpp"

int main(void)
{
  Temperature temperature_example;
  bool temperature_sensor_initialized = temperature_example.Initialize();
  if (temperature_sensor_initialized)
  {
    double temperature_data = 0;
    LOG_INFO("Temperature Device is ON.\n");
    while (temperature_sensor_initialized)
    {
      temperature_data = static_cast<double>(temperature_example.GetCelsius());
      LOG_INFO("Celsius data: %.2g \n", temperature_data);

      // This will only return Fahrenheit.
      temperature_data = static_cast<double>
                          (temperature_example.GetFahrenheit());
      LOG_INFO("Fahrenheit data: %.2g \n", temperature_data);
    }
  }
  else
  {
    LOG_INFO("Could not initialize temperature device (Si7060)\n\n");
  }
  return 0;
}
