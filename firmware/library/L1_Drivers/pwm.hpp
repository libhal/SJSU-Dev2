#pragma once

#include <cstdint>

class PwmInterface
{
 public:
    virtual void Initialize(uint32_t frequencyHz = 1'000) = 0;
    virtual void SetDutyCycle(float dutyCycle) = 0;
    virtual float GetDutyCycle() = 0;
    virtual void SetFrequency(uint32_t frequencyHz) = 0;
    virtual uint32_t GetFrequency() = 0;
};
