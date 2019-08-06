#include <cstdint>

#include "L1_Peripheral/lpc40xx/i2c.hpp"
#include "L2_HAL/sensors/optical/apds9960.hpp"
#include "utility/log.hpp"
#include "utility/time.hpp"

int main(void)
{
  sjsu::lpc40xx::I2c i2c(sjsu::lpc40xx::I2c::Bus::kI2c2);
  sjsu::Apds9960 sensor(i2c);
  i2c.Initialize();

  LOG_INFO("Starting Apds9960 Example\n");
  if (sensor.FindDevice())
  {
    LOG_INFO("Found it!\n");
    LOG_INFO("Initializing Gesture Sensor...\n");
    sensor.Initialize();
    sensor.EnableGesture();
  }
  else
  {
    LOG_INFO("Couldn't initialize sensor");
    sjsu::Halt();
  }

  sjsu::Apds9960::Gesture value;
  LOG_INFO("Start collecting gestures:\n");

  while (true)
  {
    value = sensor.GetGesture();
    switch (value)
    {
      case sensor.kSwipeUP:
        LOG_INFO("Detected gesture: Swipe Up\n");
      break;
      case sensor.kSwipeDOWN:
        LOG_INFO("Detected gesture: Swipe Down\n");
      break;
      case sensor.kSwipeLEFT:
        LOG_INFO("Detected gesture: Swipe Left\n");
      break;
      case sensor.kSwipeRIGHT:
        LOG_INFO("Detected gesture: Swipe Right\n");
      break;
      case sensor.kNEAR:
        LOG_INFO("Detected gesture: Near\n");
      break;
      case sensor.kFAR:
        LOG_INFO("Detected gesture: Far\n");
      break;
      default: break;
    }
    sjsu::Delay(500ms);  // Do other code here
  }
  return 0;
}
