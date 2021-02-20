#include "peripherals/stm32f10x/pwm.hpp"
#include "utility/log.hpp"

int main()
{
  sjsu::stm32f10x::Pwm & pwm = sjsu::stm32f10x::GetPwmFromPin<'A', 0>();

  pwm.settings.frequency = 10_kHz;
  pwm.Initialize();
  pwm.SetDutyCycle(0.78f);

  return 0;
}
