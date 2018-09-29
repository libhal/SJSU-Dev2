#pragma once

#include <cstdint>

#include "L0_LowLevel/LPC40xx.h"
#include "L1_Drivers/gpio.hpp"
#include "L1_Drivers/pin_configure.hpp"

class ButtonInterface
{
 public:
         virtual void Initialize()                                          = 0;
         virtual bool GetButtonState()                                      = 0;
         virtual bool Released()                                            = 0;
         virtual bool Pressed()                                             = 0;
         virtual void InvertButtonSignal(bool enable_invert_signal = true)  = 0;
};

class ButtonDriver : public ButtonInterface
{
 public:
       static constexpr uint8_t kFunction0 = 0b000;
       static constexpr uint8_t kInput       = 0b0;
       static constexpr uint8_t kPortOne  = 1;
       static constexpr uint8_t kPinNine  = 9;

       constexpr ButtonDriver(uint8_t port_num_, uint8_t pin_num_)
           : port_(port_num_),
             pin_(pin_num_),
             gpio_(gpio_pin_),
             pincon_(pincon_pin_),
             gpio_pin_(port_, pin_),
             pincon_pin_(port_, pin_)
       {
       }

       constexpr ButtonDriver(const GpioInterface & gpio,
                              const PinConfigureInterface & pincon)
           : port_(5),
             pin_(4),
             gpio_(gpio),
             pincon_(pincon),
             gpio_pin_(kPortOne, kPinNine),
             pincon_pin_(PinConfigure::CreateInactivePin())
       {
       }

       ~ButtonDriver() {}

void Initialize() override {
      pincon_pin_.SetPinMode(PinConfigureInterface::kInactive);
      pincon_pin_.SetPinFunction(kFunction0);
      gpio_pin_.SetAsInput();
}

bool Read() override {
    return (gpio_pin_.ReadPin()) ? true : false;
}

bool Released() override {
    return (Read()) ? true : false;
}

bool Pressed() override {
    return (Read()) ? false : true;
}

void InvertButtonSignal(bool enable_invert_signal = true) override {
  if (enable_invert_signal == false) {
        pincon_pin_.SetAsActiveLow(false);
  } else {
        pincon_pin_.SetAsActiveLow(true);
  }
}

 private:
    const GpioInterface & gpio_;
    const PinConfigureInterface  & pincon_;
    Gpio gpio_pin_;
    PinConfigure pincon_pin_;

    uint8_t port_;
    uint8_t pin_;
};
