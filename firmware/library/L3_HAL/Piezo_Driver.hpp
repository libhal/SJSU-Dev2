#pragma once
#include <cstdint>

class BuzzerInterface
{
 public:
    virtual void Initialize() = 0;
    virtual void Stop() = 0;
    virtual void Beep(uint32_t frequency, uint32_t volume) = 0;
};

