#include "L1_Peripheral/lpc40xx/i2c.hpp"
#include "L2_HAL/sensors/environment/temperature/si7060.hpp"
#include "utility/log.hpp"
#include "utility/time.hpp"

int main()
{
  LOG_INFO("Starting Si7060 Temperature Sensor Demonstration...");
  sjsu::lpc40xx::I2c i2c2(sjsu::lpc40xx::I2c::Bus::kI2c2);
  sjsu::Si7060 temperature_sensor(i2c2);

  sjsu::Status temperature_device_status = temperature_sensor.Initialize();
  if (temperature_device_status != sjsu::Status::kSuccess)
  {
    LOG_ERROR("Could not initialize temperature device (Si7060)");
    sjsu::Halt();
  }

  LOG_INFO("Si7060 Initialized!");

  while (true)
  {
    units::temperature::celsius_t temperature;
    temperature_sensor.GetTemperature(&temperature);
    LOG_INFO("Board Temperature: %.4f C",
             static_cast<double>(temperature.value()));
    sjsu::Delay(1s);
  }
  return 0;
}
