#include "L1_Peripheral/lpc40xx/pwm.hpp"
#include "L2_HAL/audio/buzzer.hpp"
#include "utility/log.hpp"
#include "utility/time.hpp"

int main()
{
    LOG_INFO("Buzzer example starting...");
    LOG_INFO("This example utilizes a Pwm port 2.0(Pwm1)");
    LOG_INFO("The buzzer will start with a frequency of 1000 Hz");
    LOG_INFO("and a volume of 10 percent i.e. 0.1f");
    LOG_INFO("==================================================");
    sjsu::lpc40xx::Pwm pwm_pin(sjsu::lpc40xx::Pwm::Channel::kPwm1);
    sjsu::Buzzer buzzer(pwm_pin);

    constexpr units::frequency::hertz_t kFrequency = 1_kHz;
    constexpr float kVolume = 0.1f;

    LOG_INFO("Initializing the buzzer...");
    buzzer.Initialize();
    sjsu::Delay(1s);

    LOG_INFO("Buzzer will emit sound at 10 percent volume and 1kHz freqeuncy");
    buzzer.Beep(kFrequency, kVolume);
    sjsu::Delay(1s);
    buzzer.Stop();

    LOG_INFO("Volume will be increased to 40 percent and ");
    LOG_INFO("frequency lowered to 750Hz");
    buzzer.Beep(750_Hz, 0.4f);
    sjsu::Delay(1s);
    buzzer.Stop();

    LOG_INFO("Emit sound with default volume (100 percent) ");
    LOG_INFO("and frequency (500Hz)");

    LOG_INFO("if the Beep() function is called without any arguments");

    buzzer.Beep();
    sjsu::Delay(1s);
    buzzer.Stop();

    LOG_INFO("Restart the example anytime by ");
    LOG_INFO("pressing the 'reset' button on controller");

    LOG_INFO("Halting the processor...");
    sjsu::Halt();
    return 0;
}
