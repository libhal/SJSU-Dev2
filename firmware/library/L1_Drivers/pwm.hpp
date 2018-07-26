#pragma once

#include <cstdint>

class PwmInterface
{
 public:
    virtual void Initialize(uint32_t frequency_hz = 1'000) = 0;
    virtual void SetDutyCycle(float duty_cycle) = 0;
    virtual float GetDutyCycle() = 0;
    virtual void SetFrequency(uint32_t frequency_hz) = 0;
    virtual uint32_t GetFrequency() = 0;
};
