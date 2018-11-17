#include <project_config.hpp>

#include <cstdint>

#include "L1_Drivers/i2c.hpp"
#include "L2_HAL/sensors/optical/apds_9960.hpp"
#include "utility/log.hpp"
#include "utility/time.hpp"

int main(void)
{
  Apds9960 sensor;

  printf("Starting Apds9960 Example\n");
  printf("Initializing Gesture Sensor...\n");
  if (sensor.Initialize())
  {
    sensor.EnableGesture();
  }
  else
  {
    printf("Couldn't initialize sensor");
    Halt();
  }

  Apds9960::Gesture value;
  printf("Start collecting gestures:\n");

  while (true)
  {
    value = sensor.GetGesture();
    switch (value)
    {
      case sensor.kSwipeUP: printf("Detected gesture: Swipe Up\n"); break;
      case sensor.kSwipeDOWN: printf("Detected gesture: Swipe Down\n"); break;
      case sensor.kSwipeLEFT: printf("Detected gesture: Swipe Left\n"); break;
      case sensor.kSwipeRIGHT: printf("Detected gesture: Swipe Right\n"); break;
      case sensor.kNEAR: printf("Detected gesture: Near\n"); break;
      case sensor.kFAR: printf("Detected gesture: Far\n"); break;
      default: break;
    }
    Delay(500);
  }
  return 0;
}
