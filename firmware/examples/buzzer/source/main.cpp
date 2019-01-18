#include "L2_HAL/buzzer.hpp"
#include "utility/log.hpp"
#include "utility/time.hpp"

int main(void)
{
    LOG_INFO("Buzzer example starting...");
    LOG_INFO("This example utilizes a Pwm port 2.0(Pwm1)");
    LOG_INFO("The buzzer will start with a frequency of 1000 Hz");
    LOG_INFO("and a volume of 10 percent i.e. 0.1f");
    LOG_INFO("==================================================");

    Buzzer buzzer(1);

    constexpr uint32_t kFrequency = 1000;
    constexpr float kVolume = 0.1f;

    LOG_INFO("Initializing the buzzer...");
    buzzer.Initialize();
    Delay(1000);

    LOG_INFO("Buzzer will emit sound at 10 percent volume and 1kHz freqeuncy");
    buzzer.Beep(kFrequency, kVolume);
    Delay(1000);
    buzzer.Stop();

    LOG_INFO("Volume will be increased to 40 percent and ");
    LOG_INFO("frequency lowered to 750Hz");
    buzzer.Beep(750, 0.4f);
    Delay(1000);
    buzzer.Stop();

    LOG_INFO("Emit sound with default volume (100 percent) ");
    LOG_INFO("and frequency (500Hz)");

    LOG_INFO("if the Beep() function is called without any arguments");

    buzzer.Beep();
    Delay(1000);
    buzzer.Stop();

    LOG_INFO("Restart the example anytime by ");
    LOG_INFO("pressing the 'reset' button on controller");

    LOG_INFO("Halting the processor...");
    Halt();
    return 0;
}
