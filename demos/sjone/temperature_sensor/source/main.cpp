#include "peripherals/lpc17xx/i2c.hpp"
#include "peripherals/lpc17xx/system_controller.hpp"
#include "devices/sensors/environment/temperature/tmp102.hpp"
#include "utility/log.hpp"

int main()
{
  sjsu::LogInfo("Starting LPC176x/5x Temperature Sensor Example...");

  sjsu::lpc40xx::I2c & i2c = sjsu::lpc40xx::GetI2c<2>();
  sjsu::Tmp102 temperature_sensor(i2c);

  temperature_sensor.Initialize();

  while (true)
  {
    units::temperature::celsius_t temperature =
        temperature_sensor.GetTemperature();
    sjsu::LogInfo("Temperature: %.2f C", temperature.to<double>());
    sjsu::Delay(1s);
  }

  return 0;
}
