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
  explicit Buzzer(uint8_t select_pin) : pwm_channel_(select_pin) {}

  void Initialize() override
  {
    pwm_channel_.Initialize(500);
    Stop();
  }

  void Stop() override
  {
    pwm_channel_.SetDutyCycle(0.0);
  }

  void Beep(uint32_t frequency = 500, float volume = 1.0) override
  {
    pwm_channel_.SetFrequency(frequency);
    pwm_channel_.SetDutyCycle(volume);
  }

 private:
  Pwm pwm_channel_;
};
