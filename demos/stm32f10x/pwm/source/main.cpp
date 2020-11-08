#include "L1_Peripheral/stm32f10x/pwm.hpp"
#include "utility/log.hpp"

int main()
{
  sjsu::stm32f10x::Pwm pwm(sjsu::stm32f10x::Pwm::Channel_t::kA8);

  pwm.Initialize();
  pwm.Enable();
  pwm.ConfigureFrequency(500_Hz);
  pwm.Enable();
  pwm.SetDutyCycle(0.78f);

  return 0;
}
