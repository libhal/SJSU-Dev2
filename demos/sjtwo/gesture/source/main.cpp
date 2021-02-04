#include <cstdint>

#include "peripherals/lpc40xx/i2c.hpp"
// #include "devices/sensors/optical/apds9960.hpp"
#include "utility/log.hpp"
#include "utility/time/time.hpp"

int main(void)
{
  sjsu::LogInfo("This demo is not available");
  return -1;

  /*
  sjsu::lpc40xx::I2c i2c(sjsu::lpc40xx::I2c::Bus::kI2c2);
  sjsu::Apds9960 sensor(i2c);
  i2c.Initialize();

  sjsu::LogInfo("Starting Apds9960 Example\n");
  if (sensor.FindDevice())
  {
    sjsu::LogInfo("Found it!\n");
    sjsu::LogInfo("Initializing Gesture Sensor...\n");
    sensor.Initialize();
    sensor.EnableGesture();
  }
  else
  {
    sjsu::LogInfo("Couldn't initialize sensor");
    sjsu::Halt();
  }

  sjsu::Apds9960::Gesture value;
  sjsu::LogInfo("Start collecting gestures:\n");

  while (true)
  {
    value = sensor.GetGesture();
    switch (value)
    {
      case sensor.kSwipeUP:
        sjsu::LogInfo("Detected gesture: Swipe Up\n");
        break;
      case sensor.kSwipeDOWN:
        sjsu::LogInfo("Detected gesture: Swipe Down\n");
        break;
      case sensor.kSwipeLEFT:
        sjsu::LogInfo("Detected gesture: Swipe Left\n");
        break;
      case sensor.kSwipeRIGHT:
        sjsu::LogInfo("Detected gesture: Swipe Right\n");
        break;
      case sensor.kNEAR: sjsu::LogInfo("Detected gesture: Near\n"); break;
      case sensor.kFAR: sjsu::LogInfo("Detected gesture: Far\n"); break;
      default: break;
    }
    sjsu::Delay(500ms);  // Do other code here
  }
  return 0;
  */
}
