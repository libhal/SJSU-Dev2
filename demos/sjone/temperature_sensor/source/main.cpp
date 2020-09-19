#include "L1_Peripheral/lpc17xx/i2c.hpp"
#include "L1_Peripheral/lpc17xx/system_controller.hpp"
#include "L2_HAL/sensors/environment/temperature/tmp102.hpp"
#include "utility/log.hpp"

int main()
{
  sjsu::LogInfo("Starting LPC176x/5x Temperature Sensor Example...");

  sjsu::lpc17xx::I2c i2c2(sjsu::lpc17xx::I2cBus::kI2c2);
  sjsu::Tmp102 temperature_sensor(i2c2);

  temperature_sensor.Initialize();
  temperature_sensor.Enable();

  while (true)
  {
    units::temperature::celsius_t temperature =
        temperature_sensor.GetTemperature();
    sjsu::LogInfo("Temperature: %.2f C", temperature.to<double>());
    sjsu::Delay(1s);
  }

  return 0;
}
