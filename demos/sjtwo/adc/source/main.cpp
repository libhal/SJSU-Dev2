#include <cstdint>

#include "L1_Peripheral/example.hpp"
#include "L1_Peripheral/inactive.hpp"
#include "L1_Peripheral/lpc40xx/adc.hpp"
#include "utility/log.hpp"
#include "utility/map.hpp"
#include "utility/time.hpp"

int main()
{
  LOG_INFO("Analog-to-Ditial Application Starting...");

  LOG_INFO("Creating ADC object and selecting ADC channel 4 & 5");
  LOG_INFO("ADC channel 4 is connected to pin P1.30");
  sjsu::lpc40xx::Adc adc4(sjsu::lpc40xx::Adc::Channel::kChannel4);
  LOG_INFO("ADC channel 5 is connected to pin P1.31");
  sjsu::lpc40xx::Adc adc5(sjsu::lpc40xx::Adc::Channel::kChannel5);

  LOG_INFO(
      "If you leave a channel disconnected, then the pin will be in a floating "
      "state, and in this state, the voltage read from this pin will be "
      "random.");

  LOG_INFO("Initializing ADC ...");
  adc5.Initialize();
  adc4.Initialize();
  LOG_INFO("Initializing ADC Complete!");

  LOG_INFO(
      "Apply a voltage from 0 to 3.3V (BE SUPER SURE not to accidently apply "
      "more then 3.3V or you WILL damage your board).");

  while (true)
  {
    // Now that conversion is complete, read the value like so.
    uint32_t adc_digital_value[2];
    float voltage[2];

    // For the LPC40xx with a 12-bit ADC, lowest and highest values are 0 to
    // 1023, where as the lowest and highest voltages are between 0 and 3.3V

    // Now that conversion is complete, read the value like so.
    adc_digital_value[0] = adc5.Read();
    adc_digital_value[1] = adc4.Read();
    // For the LPC40xx with a 12-bit ADC, lowest and highest values are 0 to
    // 1023, where as the lowest and highest voltages are between 0 and 3.3V
    voltage[0] = sjsu::Map(adc_digital_value[0], 0, 4095, 0.0f, 3.3f);
    voltage[1] = sjsu::Map(adc_digital_value[1], 0, 4095, 0.0f, 3.3f);

    LOG_INFO("voltage[0] = %f V (%lu) :: voltage[1] = %f V (%lu)",
             static_cast<double>(voltage[0]),
             adc_digital_value[0],
             static_cast<double>(voltage[1]),
             adc_digital_value[1]);
    sjsu::Delay(250ms);
  }
  return 0;
}
