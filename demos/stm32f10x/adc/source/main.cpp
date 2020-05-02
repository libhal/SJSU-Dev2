#include "L1_Peripheral/stm32f10x/adc.hpp"
#include "utility/log.hpp"
#include "utility/time.hpp"

int main()
{
  sjsu::LogInfo("Starting ADC Application...");
  sjsu::LogInfo("Read voltage between 0.0V and 3.3V by connecting it to PA0.");

  sjsu::stm32f10x::Adc adc0(sjsu::stm32f10x::Adc::Channel::kChannel0);
  adc0.Initialize();
  adc0.Enable();

  while (true)
  {
    units::voltage::volt_t voltage = adc0.Voltage();
    sjsu::LogInfo("ADC Channel 0 [PA0] = %.1f V", voltage.to<double>());
    sjsu::Delay(500ms);
  }

  return 0;
}
