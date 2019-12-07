#include "L1_Peripheral/lpc17xx/i2c.hpp"
#include "L1_Peripheral/lpc17xx/system_controller.hpp"
#include "L2_HAL/sensors/environment/temperature/tmp102.hpp"
#include "utility/log.hpp"

int main()
{
  LOG_INFO("Starting LPC176x/5x Temperature Sensor Example...");

  sjsu::lpc17xx::SystemController system_controller;
  system_controller.SetPeripheralClockDivider(
      sjsu::lpc17xx::SystemController::Peripherals::kI2c2, 4);
  sjsu::SystemController::SetPlatformController(&system_controller);

  sjsu::lpc17xx::I2c i2c2(sjsu::lpc17xx::I2cBus::kI2c2);
  sjsu::Tmp102 temperature_sensor(i2c2, sjsu::Tmp102::DeviceAddress::kGround);
  units::temperature::celsius_t temperature;

  SJ2_ASSERT_FATAL(temperature_sensor.Initialize() == sjsu::Status::kSuccess,
                   "Failed to initialize Tmp102 Temperature Sensor!");

  while (1)
  {
    temperature_sensor.GetTemperature(&temperature);
    LOG_INFO("Temperature: %.2f C", temperature.to<double>());
    sjsu::Delay(1s);
  }

  return 0;
}
