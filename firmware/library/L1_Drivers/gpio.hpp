#pragma once

#include <cstdint>

#include "L0_LowLevel/LPC40xx.h"
#include "L1_Drivers/pin_configure.hpp"

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
     virtual void SetAsInput(void)                                    = 0;
     virtual void SetAsOutput(void)                                   = 0;
     virtual void SetDirection(GpioInterface::PinDirection direction) = 0;
     virtual void SetHigh(void)                                       = 0;
     virtual void SetLow(void)                                        = 0;
     virtual void Set(GpioInterface::PinOutput output = kHigh)        = 0;
     virtual bool ReadPin(void)                                       = 0;
};

class Gpio : public GpioInterface, public PinConfigure
{
 public:
    // Used to point to a certain port located in LPC memory map
    // Defined in gpio.cpp
    static LPC_GPIO_TypeDef * gpio_base[6];

    // For port 0-4, pins 0-31 are available
    // Port 5 only have pins 0-4 available
    constexpr Gpio(uint8_t port_number, uint8_t pin_number)
        : PinConfigure(port_number, pin_number)
    {
    }
    // Sets the GPIO pin direction as input
    void SetAsInput(void) override
    {
        gpio_base[kPort]->DIR &= ~(1 << kPin);
    }
    // Sets the GPIO pin direction as output
    void SetAsOutput(void) override
    {
        gpio_base[kPort]->DIR |= (1 << kPin);
    }
    // Sets the GPIO pin direction as output or input depending on the
    // PinDirection enum parameter
    inline void SetDirection(GpioInterface::PinDirection direction) override
    {
        (direction) ? SetAsOutput() : SetAsInput();
    }
    // Sets the GPIO output pin to high
    void SetHigh(void) override
    {
        gpio_base[kPort]->SET = (1 << kPin);
    }
    // Sets the GPIO output pin to low
    void SetLow(void) override
    {
        gpio_base[kPort]->CLR = (1 << kPin);
    }
    // Sets the GPIO output pin to high or low depending on the PinOutput enum
    // parameter
    void Set(GpioInterface::PinOutput output = kHigh) override
    {
        (output) ? SetHigh() : SetLow();
    }

    // Returns true if input or output pin is high
    bool ReadPin(void) override
    {
        return ((gpio_base[kPort]->PIN >> kPin) & 1);
    }
};
