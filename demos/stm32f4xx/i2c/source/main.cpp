#include <cstdio>

#include "devices/sensors/environment/temperature/si7060.hpp"
#include "devices/sensors/movement/accelerometer/mma8452q.hpp"
#include "devices/sensors/movement/accelerometer/mpu6050.hpp"
#include "peripherals/stm32f4xx/i2c.hpp"
#include "utility/log.hpp"
#include "utility/time/time.hpp"

int main()
{
  sjsu::LogInfo("Starting I2C Application...");

  auto & i2c        = sjsu::stm32f4xx::GetI2c<2>();
  constexpr int app = 1;
  if constexpr (app == 1)
  {
    sjsu::Si7060 si7060(i2c);
    sjsu::Mpu6050 mpu6050(i2c);
    si7060.Initialize();
    mpu6050.Initialize();
    while (true)
    {
      mpu6050.Read().Print();
      auto temperature = si7060.GetTemperature();
      sjsu::LogInfo("temperature = %f", temperature.to<double>());
      sjsu::Delay(250ms);
    }
  }
  else
  {
    i2c.Initialize();
    std::array<uint8_t, 1> address{ 0x0D };
    std::array<uint8_t, 1> id{ 0x00 };

    while (true)
    {
      i2c.WriteThenRead(0x1c, address, id);
      sjsu::LogInfo("MMA8452Q WHO_AM_I = 0x%02X", id[0]);
      sjsu::Delay(250ms);
    }
  }

  return 0;
}
