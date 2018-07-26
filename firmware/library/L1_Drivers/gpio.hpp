#pragma once

#include <cstdint>

class GpioInterface
{
 public:
    enum PinDirection : uint8_t
    {
        kInput  = 0,
        kOutput = 1
    };
    enum PinOutput : uint8_t
    {
        kLow  = 0,
        kHigh = 1
    };
    virtual void SetAsInput(void)                         = 0;
    virtual void SetAsOutput(void)                        = 0;
    virtual void SetDirection(PinDirection direction)     = 0;
    virtual void SetHigh(void)                            = 0;
    virtual void SetLow(void)                             = 0;
    virtual void Set(PinOutput output = PinOutput::kHigh) = 0;
    virtual bool ReadPin(void)                            = 0;
};
