#include "L1_Drivers/gpio.hpp"
#include "L1_Drivers/pwm.hpp"
#include "utility/log.hpp"
#include "utility/time.hpp"

int main(void)
{
  LOG_INFO("Pwm Application Starting...");

  LOG_INFO("Creating Gpio powered LED...P4.28");
  LOG_INFO("This Gpio powered LED represents the maximum brightness");
  LOG_INFO("and is used as reference against the Pwm powered LED");
  LOG_INFO("during its sweep.");
  LOG_INFO("Connect a 470 ohm resistor between the power line and");
  LOG_INFO("the anode of the LED. The cathode is then connected to");
  LOG_INFO("P4.28 and P2.0(Pwm1).");
  Gpio p4_28(4, 28);
  p4_28.SetAsOutput();
  p4_28.SetLow();

  LOG_INFO("Creating Pwm powered LED...P2.0 (Pwm1)");
  Pwm p2_0 = Pwm::CreatePwm<1>();
  // Initialize Pwm at 1,000 hz
  uint32_t frequency = 1000;
  p2_0.Initialize(frequency);
  float duty = 0;

  while (true)
  {
    p2_0.SetFrequency(1000);
    for (int i = 0; i <= 255; i++)
    {
      duty = static_cast<float>(i) / 255.0f;
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
