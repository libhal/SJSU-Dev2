#include <array>
#include <cstdint>

#include "L1_Peripheral/lpc40xx/adc.hpp"
#include "utility/log.hpp"
#include "utility/map.hpp"
#include "utility/time.hpp"

int main()
{
  sjsu::LogInfo("ADC Application Starting...");

  sjsu::LogInfo("Creating ADC object and selecting ADC channel 4 & 5");
  sjsu::LogInfo("ADC channel 4 is connected to pin P1.30");
  sjsu::lpc40xx::Adc adc4(sjsu::lpc40xx::Adc::Channel::kChannel4);

  sjsu::LogInfo("ADC channel 5 is connected to pin P1.31");
  sjsu::lpc40xx::Adc adc5(sjsu::lpc40xx::Adc::Channel::kChannel5);

  sjsu::LogInfo(
      "If you leave a channel disconnected, then the pin will be in a floating "
      "state, and in this state, the voltage read from this pin will be "
      "random.");

  sjsu::LogInfo("Initializing ADCs ...");
  adc5.Initialize();
  adc4.Initialize();
  sjsu::LogInfo("Initializing ADCs Complete!");

  sjsu::LogInfo("Enabling ADCs ...");
  adc5.Enable();
  adc4.Enable();
  sjsu::LogInfo("Enabling ADCs Complete!");

  sjsu::LogInfo("Apply voltage from 0 to 3.3V. DO NOT GO BEYOND THIS LIMIT!");

  while (true)
  {
    // For the LPC40xx with a 12-bit ADC, lowest and highest values are 0 to
    // 4095, where as the lowest and highest voltages are between 0 and 3.3V

    // Using the Read() API, you can get a value from the 0 to 4095.
    uint32_t digital_value = adc4.Read();

    // Now we convert the digital_value into a float from 0.0 and 3.3 to get the
    // voltage.
    float voltage =
        sjsu::Map(digital_value, 0, adc4.GetMaximumValue(), 0.0f, 3.3f);

    // OR

    // A more convenient way to do this is to simply grab the voltage from the
    // ADC. The method will handle the conversion from digital value to the ADCs
    // actual voltage.
    units::voltage::volt_t adc_voltage = adc5.Voltage();

    sjsu::LogInfo("ADC4 = %.5f V (%04lu) :: ADC5 voltage = %.5f V",
                  static_cast<double>(voltage),
                  digital_value,
                  adc_voltage.to<double>());

    sjsu::Delay(250ms);
  }

  return 0;
}
