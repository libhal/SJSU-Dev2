#include <cstdint>

#include "L1_Peripheral/lpc40xx/adc.hpp"
#include "utility/log.hpp"
#include "utility/map.hpp"
#include "utility/time.hpp"

int main()
{
  LOG_INFO("Analog-to-Ditial Application Starting...");

  LOG_INFO("Creating Adc object and selecting ADC channel 0");
  LOG_INFO("ADC channel 0 is connected to pin P0.23");
  sjsu::lpc40xx::Adc adc0(sjsu::lpc40xx::Adc::Channel::kChannel0);
  LOG_INFO("Initializing ADC ...");
  adc0.Initialize();
  LOG_INFO("Initializing ADC Complete!");
  LOG_INFO("Enabling ADC to burst mode!");
  LOG_INFO(
      "Burst mode is useful in that you do not need to turn on conversion. You "
      "simply need to run the Read() method!");
  adc0.BurstMode(true);

  LOG_INFO(
      "Apply a voltage from 0 to 3.3V (BE SUPER SURE not to accidently apply "
      "more then 3.3V or you WILL damage your board).");

  while (true)
  {
    uint16_t adc_digital_value = adc0.Read();
    // For the LPC40xx with a 12-bit ADC, lowest and highest values are 0 to
    // 1023, where as the lowest and highest voltages are between 0 and 3.3V
    float voltage = sjsu::Map(adc_digital_value, 0, 1023, 0.0f, 3.3f);
    LOG_INFO("Voltage on pin P0.23 = %f, raw value = %u\n",
             static_cast<double>(voltage), adc_digital_value);
    sjsu::Delay(250);
  }
  return 0;
}
