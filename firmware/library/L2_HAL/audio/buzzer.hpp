// Usage:
// Initialize buzzer with pwm channel number
// Buzzer buzzer(1) ;
// buzzer.Beep(500, 0.9) ; // frequency = 500Hz, Volume = 0.9
#pragma once

#include <cstdint>

#include "L1_Peripheral/pwm.hpp"

namespace sjsu
{
class Buzzer
{
 public:
  explicit constexpr Buzzer(Pwm & pwm)
      : pwm_(pwm)
  {
  }

  void Initialize()
  {
    pwm_.Initialize(500);
    Stop();
  }

  void Stop()
  {
    pwm_.SetDutyCycle(0.0);
  }

  void Beep(uint32_t frequency = 500, float volume = 1.0)
  {
    pwm_.SetFrequency(frequency);
    // NOTE: Since the PWM is at its loudest at 50% duty cycle, the maximum PWM
    // is divided by 2.
    pwm_.SetDutyCycle(volume / 2);
  }

  float GetVolume()
  {
    return pwm_.GetDutyCycle();
  }

 private:
  Pwm & pwm_;
};
}  // namespace sjsu
