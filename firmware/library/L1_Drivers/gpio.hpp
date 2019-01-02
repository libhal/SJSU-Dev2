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
  virtual void SetAsInput()                      = 0;
  virtual void SetAsOutput()                     = 0;
  virtual void SetDirection(Direction direction) = 0;
  virtual void SetHigh()                         = 0;
  virtual void SetLow()                          = 0;
  virtual void Set(State output = kHigh)         = 0;
  virtual void Toggle()                          = 0;
  virtual State ReadState()                      = 0;
  virtual bool Read()                            = 0;
  virtual PinInterface & GetPin()                = 0;
};
class Gpio final : public GpioInterface
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
      : pin_(&lpc40xx_pin_), lpc40xx_pin_(port_number, pin_number)
  {
  }
  // For port 0-4, pins 0-31 are available. Port 5 only has pins 0-4 available.
  constexpr explicit Gpio(PinInterface * pin)
      : pin_(pin), lpc40xx_pin_(Pin::CreateInactivePin())
  {
  }
  // Sets the GPIO pin direction as input
  void SetAsInput(void) override
  {
    pin_->SetPinFunction(kGpioFunction);
    gpio_port[pin_->GetPort()]->DIR &= ~(1 << pin_->GetPin());
  }
  // Sets the GPIO pin direction as output
  void SetAsOutput(void) override
  {
    pin_->SetPinFunction(kGpioFunction);
    gpio_port[pin_->GetPort()]->DIR |= (1 << pin_->GetPin());
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
    gpio_port[pin_->GetPort()]->SET = (1 << pin_->GetPin());
  }
  // Sets the GPIO output pin to low
  void SetLow(void) override
  {
    gpio_port[pin_->GetPort()]->CLR = (1 << pin_->GetPin());
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
    gpio_port[pin_->GetPort()]->PIN ^= (1 << pin_->GetPin());
  }
  // Returns the current State state of the pin
  State ReadState(void) override
  {
    bool state = (gpio_port[pin_->GetPort()]->PIN >> pin_->GetPin()) & 1;
    return static_cast<State>(state);
  }
  // Returns true if input or output pin is high
  bool Read(void) override
  {
    return (gpio_port[pin_->GetPort()]->PIN >> pin_->GetPin()) & 1;
  }
  PinInterface & GetPin() override
  {
    return *pin_;
  }

 private:
  PinInterface * pin_;
  Pin lpc40xx_pin_;
};
