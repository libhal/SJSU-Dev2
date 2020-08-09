#include "L1_Peripheral/lpc40xx/i2c.hpp"
#include "L2_HAL/sensors/environment/temperature/si7060.hpp"
#include "utility/log.hpp"
#include "utility/time.hpp"

int main()
{
  sjsu::LogInfo("Starting Si7060 Temperature Sensor Demonstration...");
  sjsu::lpc40xx::I2c i2c2(sjsu::lpc40xx::I2c::Bus::kI2c2);
  sjsu::Si7060 temperature_sensor(i2c2);

  SJ2_RETURN_VALUE_ON_ERROR(temperature_sensor.Initialize(), -1);
  SJ2_RETURN_VALUE_ON_ERROR(temperature_sensor.Enable(), -2);

  sjsu::LogInfo("Si7060 Initialized!");

  while (true)
  {
    units::temperature::celsius_t temperature =
        SJ2_RETURN_VALUE_ON_ERROR(temperature_sensor.GetTemperature(), -3);
    sjsu::LogInfo("Board Temperature: %.4f C",
                  static_cast<double>(temperature.value()));
    sjsu::Delay(1s);
  }

  return 0;
}
