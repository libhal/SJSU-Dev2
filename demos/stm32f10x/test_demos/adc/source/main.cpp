#include "L1_Peripheral/stm32f10x/adc.hpp"
#include "utility/log.hpp"
#include "utility/time.hpp"

int main()
{
  sjsu::LogInfo("Starting ADC Application...");

  std::array<sjsu::stm32f10x::Adc, 10> adc_channel = {
    sjsu::stm32f10x::Adc(sjsu::stm32f10x::Adc::Channel::kChannel0),
    sjsu::stm32f10x::Adc(sjsu::stm32f10x::Adc::Channel::kChannel1),
    sjsu::stm32f10x::Adc(sjsu::stm32f10x::Adc::Channel::kChannel2),
    sjsu::stm32f10x::Adc(sjsu::stm32f10x::Adc::Channel::kChannel3),
    sjsu::stm32f10x::Adc(sjsu::stm32f10x::Adc::Channel::kChannel4),
    sjsu::stm32f10x::Adc(sjsu::stm32f10x::Adc::Channel::kChannel5),
    sjsu::stm32f10x::Adc(sjsu::stm32f10x::Adc::Channel::kChannel6),
    sjsu::stm32f10x::Adc(sjsu::stm32f10x::Adc::Channel::kChannel7),
    sjsu::stm32f10x::Adc(sjsu::stm32f10x::Adc::Channel::kChannel8),
    sjsu::stm32f10x::Adc(sjsu::stm32f10x::Adc::Channel::kChannel9),
  };

  for (auto & adc : adc_channel)
  {
    adc.Initialize();
    adc.Enable();
  }

  while (true)
  {
    std::array<units::voltage::volt_t, 10> volt_array = {
      adc_channel[0].Voltage(), adc_channel[1].Voltage(),
      adc_channel[2].Voltage(), adc_channel[3].Voltage(),
      adc_channel[4].Voltage(), adc_channel[5].Voltage(),
      adc_channel[6].Voltage(), adc_channel[7].Voltage(),
      adc_channel[8].Voltage(), adc_channel[9].Voltage(),
    };

    for (int i = 0; auto & volt : volt_array)
    {
      if (volt.to<double>() > 3.0)
      {
        sjsu::LogInfo("[%d] %.1f V", i, volt.to<double>());
      }
      i++;
    }

    sjsu::Delay(500ms);
  }

  return 0;
}
