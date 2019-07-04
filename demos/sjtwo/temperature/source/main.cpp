#include "L1_Peripheral/lpc40xx/i2c.hpp"
#include "L2_HAL/sensors/environment/temperature/si7060.hpp"
#include "utility/log.hpp"
#include "utility/time.hpp"

int main()
{
  sjsu::lpc40xx::I2c i2c2(sjsu::lpc40xx::I2c::Bus::kI2c2);
  sjsu::Si7060 temperature_sensor(i2c2);

  bool temperature_sensor_initialized = temperature_sensor.Initialize();

  if (!temperature_sensor_initialized)
  {
    LOG_ERROR("Could not initialize temperature device (Si7060)");
    sjsu::Halt();
  }

  LOG_INFO("Temperature Device is ON.");

  while (temperature_sensor_initialized)
  {
    double temperature_data =
        static_cast<double>(temperature_sensor.GetCelsius());
    LOG_INFO("Board Temperature: %.4f C", temperature_data);
    sjsu::Delay(1000);
  }
  return 0;
}
