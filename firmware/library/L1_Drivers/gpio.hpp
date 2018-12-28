#pragma once

#include <cstdint>

#include "L0_LowLevel/LPC40xx.h"
#include "L1_Drivers/pin.hpp"

class GpioInterface
{
 public:
  enum Direction : uint8_t
  {
    kInput  = 0,
    kOutput = 1
  };
  enum State : uint8_t
  {
    kLow  = 0,
    kHigh = 1
  };
  virtual void SetAsInput(void)                  = 0;
  virtual void SetAsOutput(void)                 = 0;
  virtual void SetDirection(Direction direction) = 0;
  virtual void SetHigh(void)                     = 0;
  virtual void SetLow(void)                      = 0;
  virtual void Set(State output = kHigh)         = 0;
  virtual void Toggle()                          = 0;
  virtual State ReadState(void)                  = 0;
  virtual bool Read(void)                        = 0;
};
class Gpio : public GpioInterface, public Pin
{
 public:
  // Mode zero is the GPIO function for all pins.
  static constexpr uint8_t kGpioFunction = 0;
  // Table of GPIO ports located in LPC memory map
  inline static LPC_GPIO_TypeDef * gpio_port[6] = {
    LPC_GPIO0, LPC_GPIO1, LPC_GPIO2, LPC_GPIO3, LPC_GPIO4, LPC_GPIO5
  };
  // For port 0-4, pins 0-31 are available. Port 5 only has pins 0-4 available.
  constexpr Gpio(uint8_t port_number, uint8_t pin_number)
      : Pin(port_number, pin_number)
  {
  }
  // Sets the GPIO pin direction as input
  void SetAsInput(void) override
  {
    SetPinFunction(kGpioFunction);
    gpio_port[port_]->DIR &= ~(1 << pin_);
  }
  // Sets the GPIO pin direction as output
  void SetAsOutput(void) override
  {
    SetPinFunction(kGpioFunction);
    gpio_port[port_]->DIR |= (1 << pin_);
  }
  // Sets the GPIO pin direction as output or input depending on the
  // Direction enum parameter
  inline void SetDirection(Direction direction) override
  {
    (direction) ? SetAsOutput() : SetAsInput();
  }
  // Sets the GPIO output pin to high
  void SetHigh(void) override
  {
    gpio_port[port_]->SET = (1 << pin_);
  }
  // Sets the GPIO output pin to low
  void SetLow(void) override
  {
    gpio_port[port_]->CLR = (1 << pin_);
  }
  // Sets the GPIO output pin to high or low depending on the State enum
  // parameter
  void Set(State output = kHigh) override
  {
    (output) ? SetHigh() : SetLow();
  }
  // Toggle the output of a GPIO output pin
  void Toggle() override
  {
    gpio_port[port_]->PIN ^= (1 << pin_);
  }
  // Returns the current State state of the pin
  State ReadState(void) override
  {
    return static_cast<State>((gpio_port[port_]->PIN >> pin_) & 1);
  }
  // Returns true if input or output pin is high
  bool Read(void) override
  {
    return static_cast<bool>((gpio_port[port_]->PIN >> pin_) & 1);
  }
};
