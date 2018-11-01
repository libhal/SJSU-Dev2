#include <project_config.hpp>

#include <cstdint>
#include <iterator>

#include "L2_Utilities/log.hpp"
#include "L2_Utilities/time.hpp"
// #include "L3_HAL/onboard_led.hpp"
#include "L1_Drivers/pin.hpp"
#include "L1_Drivers/pwm.hpp"

int main(void)
{
  DEBUG_PRINT("Staring Hello World Application");
  DEBUG_PRINT("Initializing LEDs...");
  // OnBoardLed leds;
  // leds.Initialize();
  Pwm led3(4);

  Pin led2_pin(1, 26);
  led2_pin.SetPinFunction(0b010);
  Pwm led2(6, led2_pin);

  Pin led1_pin(1, 24);
  led1_pin.SetPinFunction(0b010);
  Pwm led1(5, led1_pin);

  Pin led0_pin(1, 18);
  led0_pin.SetPinFunction(0b010);
  Pwm led0(1, led0_pin);

  led0.Initialize();
  led1.Initialize();
  led2.Initialize();
  led3.Initialize();

  led0.SetFrequency(10000);
  led1.SetFrequency(10000);
  led2.SetFrequency(10000);
  led3.SetFrequency(10000);

  Pwm * pwm_array[] = { &led3, &led2, &led1, &led0  };

  DEBUG_PRINT("LEDs Initialized! %f", 1234.123456);

  while (true)
  {
    for (uint8_t i = 0; i < 16; i++)
    {
      DEBUG_PRINT("Hello World 0x%X\n", i);
      for (uint8_t j = 0; j <= 100; j++)
      {
        for (uint8_t k = 0; k < 4; k++)
        {
          if (i & (1 << k))
          {
            pwm_array[k]->SetDutyCycle(static_cast<float>(100 - j) / 100.0f);
          }
          else
          {
            pwm_array[k]->SetDutyCycle(1.0f);
          }
        }
        Delay(2);
      }
      for(uint8_t j = 100; j > 0; j--)
      {
        for (uint8_t k = 0; k < 4; k++)
        {
          if (i & (1 << k))
          {
            pwm_array[k]->SetDutyCycle(static_cast<float>(100 - j) / 100.0f);
          }
          else
          {
            pwm_array[k]->SetDutyCycle(1.0f);
          }
        }
        Delay(2);
      }
      led0.SetDutyCycle(1.0f);
      led1.SetDutyCycle(1.0f);
      led2.SetDutyCycle(1.0f);
      led3.SetDutyCycle(1.0f);
      // leds.SetAll(i);
      Delay(500);
    }
  }
  return 0;
}
