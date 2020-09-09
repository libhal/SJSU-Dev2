#include "L1_Peripheral/lpc40xx/pwm.hpp"
#include "L2_HAL/audio/buzzer.hpp"
#include "utility/log.hpp"
#include "utility/time.hpp"

int main()
{
  sjsu::LogInfo("Buzzer example starting...");
  sjsu::LogInfo("This example utilizes a Pwm port 2.0(Pwm1)");
  sjsu::LogInfo("The buzzer will start with a frequency of 1000 Hz");
  sjsu::LogInfo("and a volume of 10 percent i.e. 0.1f");
  sjsu::LogInfo("==================================================");

  sjsu::lpc40xx::Pwm pwm_pin(sjsu::lpc40xx::Pwm::Channel::kPwm1);
  sjsu::Buzzer buzzer(pwm_pin);

  constexpr units::frequency::hertz_t kFrequency = 1_kHz;
  constexpr float kVolume                        = 0.1f;

  sjsu::LogInfo("Initializing the buzzer...");
  buzzer.Initialize();
  sjsu::Delay(1s);

  sjsu::LogInfo(
      "Buzzer will emit sound at 10 percent volume and 1kHz frequency");
  buzzer.Beep(kFrequency, kVolume);
  sjsu::Delay(1s);
  buzzer.Stop();

  sjsu::LogInfo("Volume will be increased to 40 percent and ");
  sjsu::LogInfo("frequency lowered to 750Hz");
  buzzer.Beep(750_Hz, 0.4f);
  sjsu::Delay(1s);
  buzzer.Stop();

  sjsu::LogInfo("Emit sound with default volume (100 percent) ");
  sjsu::LogInfo("and frequency (500Hz)");

  sjsu::LogInfo("if the Beep() function is called without any arguments");

  buzzer.Beep();
  sjsu::Delay(1s);
  buzzer.Stop();

  sjsu::LogInfo(
      "Restart the example anytime by pressing the 'reset' button on "
      "controller");

  return 0;
}
