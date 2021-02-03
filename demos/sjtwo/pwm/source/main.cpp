#include "peripherals/lpc40xx/gpio.hpp"
#include "peripherals/lpc40xx/pwm.hpp"
#include "utility/log.hpp"
#include "utility/time/time.hpp"

int main()
{
  sjsu::LogInfo("Pwm Application Starting...");

  sjsu::LogInfo("Creating PWM signal on P2.0 (Pwm0).");
  sjsu::lpc40xx::Pwm & pwm = sjsu::lpc40xx::GetPwm<1, 0>();
  pwm.settings.frequency   = 10_kHz;
  pwm.Initialize();

  sjsu::LogInfo(
      "Hookup an oscilloscope to see the signal change frequency and duty "
      "cycle.");
  sjsu::LogInfo(
      "Connect to LED and resistor and see the brightness change with duty "
      "cycle.");

  while (true)
  {
    sjsu::LogInfo("Iterate from 0%% to 100%% duty cycle");
    for (float i = 0; i <= 100; i++)
    {
      pwm.SetDutyCycle(i / 100.0f);
      sjsu::Delay(20ms);
    }

    sjsu::LogInfo("Iterate from 100%% to 50%% duty cycle");
    for (float i = 100; i > 50; i--)
    {
      pwm.SetDutyCycle(i / 100.0f);
      sjsu::Delay(20ms);
    }

    sjsu::LogInfo("Sweep frequency from 10 kHz to 20 kHz.");
    units::frequency::hertz_t frequency = 10_kHz;

    while (frequency < 200_kHz)
    {
      frequency *= 2;

      pwm.settings.frequency = frequency;
      pwm.Initialize();
      pwm.SetDutyCycle(0.5f);

      sjsu::LogInfo("Freq = %f", frequency.to<double>());
      sjsu::Delay(500ms);
    }

    sjsu::LogInfo("Sweep frequency from 200 kHz back to 10 kHz.");
    while (frequency > 10_kHz)
    {
      frequency /= 2;

      pwm.settings.frequency = frequency;
      pwm.Initialize();
      pwm.SetDutyCycle(0.5f);

      sjsu::LogInfo("Freq = %f", frequency.to<double>());

      sjsu::Delay(500ms);
    }
  }

  return 0;
}
