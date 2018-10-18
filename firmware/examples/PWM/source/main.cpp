#include "L0_LowLevel/delay.hpp"
#include "L1_Drivers/gpio.hpp"
#include "L1_Drivers/pwm.hpp"
#include "L2_Utilities/debug_print.hpp"

int main(void)
{
  DEBUG_PRINT("Pwm Application Starting...");

  DEBUG_PRINT("Creating Gpio powered LED...P4.28");
  DEBUG_PRINT("This Gpio powered LED represents the maximum brightness");
  DEBUG_PRINT("and is used as reference against the Pwm powered LED");
  DEBUG_PRINT("during its sweep.");
  DEBUG_PRINT("Connect a 470 ohm resistor between the power line and");
  DEBUG_PRINT("the anode of the LED. The cathode is then connected to");
  DEBUG_PRINT("P4.28 and P2.0(Pwm1).");
  Gpio p4_28(4, 28);
  p4_28.SetAsOutput();
  p4_28.SetLow();

  DEBUG_PRINT("Creating Pwm powered LED...P2.0 (Pwm1)");
  Pwm p2_0 = Pwm::CreatePwm<1>();
  // Initialize Pwm at 1,000 hz
  uint32_t frequency = 1000;
  p2_0.Initialize(frequency);
  float duty = 0;

  while (1)
  {
    p2_0.SetFrequency(1000);
    for (int i = 0; i <= 255; i++)
    {
      duty = static_cast<float>(i) / 255.0;
      p2_0.SetDutyCycle(duty);
      Delay(10);
    }

    p2_0.SetDutyCycle(0.5);
    while (frequency < 20000)
    {
      frequency = frequency * 2;
      p2_0.SetFrequency(frequency);
      Delay(500);
    }

    while (frequency > 5)
    {
      frequency = frequency / 2;
      p2_0.SetFrequency(frequency);
      Delay(500);
    }
  }
  return 0;
}
