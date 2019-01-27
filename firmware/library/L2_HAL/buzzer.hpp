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
  // beep continuously at a certain volume &
  // at a specified frequency
  virtual void Beep(uint32_t frequency, float volume) = 0;
};

class Buzzer : public BuzzerInterface
{
 public:
  explicit constexpr Buzzer(uint8_t pin_select)
  : pwm_(&pwm_pin_), pwm_pin_(pin_select)
  {}

  explicit Buzzer(PwmInterface * pwm_pin_select)
  : pwm_(pwm_pin_select), pwm_pin_(Pwm::CreatePwm<1>())
  {}

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
    pwm_->SetDutyCycle(volume);
  }

    uint32_t GetFrequency()
    {
        return pwm_->GetFrequency();
    }

    float GetVolume()
    {
        return pwm_->GetDutyCycle();
    }

 private:
  PwmInterface * pwm_;
  Pwm pwm_pin_;
};
