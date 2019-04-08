// Usage:
// Initialize buzzer with pwm channel number
// Buzzer buzzer(1) ;
// buzzer.Beep(500, 0.9) ; // frequency = 500Hz, Volume = 0.9
#pragma once

#include <cstdint>
#include "L1_Drivers/pwm.hpp"

class BuzzerInterface
{
 public:
  virtual void Initialize() = 0;
  virtual void Stop()       = 0;
  // Produce a tone at the supplied frequency and volume
  virtual void Beep(uint32_t frequency, float volume) = 0;
};

class Buzzer : public BuzzerInterface
{
 public:
  explicit constexpr Buzzer(const Pwm::Channel_t & channel)
      : pwm_(&pwm_pin_), pwm_pin_(channel)
  {
  }

  explicit Buzzer(PwmInterface * pwm_pin_select)
      : pwm_(pwm_pin_select), pwm_pin_(Pwm::Channel::kPwm0)
  {
  }

  void Initialize() override
  {
    pwm_->Initialize(500);
    Stop();
  }

  void Stop() override
  {
    pwm_->SetDutyCycle(0.0);
  }

  void Beep(uint32_t frequency = 500, float volume = 1.0) override
  {
    pwm_->SetFrequency(frequency);
    // NOTE: Since the PWM is at its loudest at 50% duty cycle, the maximum PWM
    // is divided by 2.
    pwm_->SetDutyCycle(volume/2);
  }

  float GetVolume()
  {
    return pwm_->GetDutyCycle();
  }

 private:
  PwmInterface * pwm_;
  Pwm pwm_pin_;
};
