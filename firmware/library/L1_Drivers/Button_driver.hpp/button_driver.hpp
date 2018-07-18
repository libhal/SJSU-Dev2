#pragma once

#include <cstdint>

#include "L0_LowLevel/LPC40xx.h"
// #include "L1_Drivers/gpio.hpp"
#include "L1_Drivers/pin_configure.hpp"

class ButtonInterface
{
 public:
         virtual void Init()                = 0;
         virtual bool GetButtonState()      = 0;
         virtual bool Released()            = 0;
         virtual bool Pressed()             = 0;
         virtual void InvertButtonSignal(bool enable_invert_signal = true)  = 0;
};

class ButtonDriver : public ButtonInterface
{
 public:
       static uint8_t button;
       explicit constexpr ButtonDriver(uint8_t num)
           : btn_1_(btn_1_pin_),
             btn_2_(btn_2_pin_),
             btn_3_(btn_3_pin_),
             btn_4_(btn_4_pin_),
//           gpio_1_(gpio_p1_9_),
//           gpio_2_(gpio_p1_10_),
//           gpio_3_(gpio_p1_14_),
//           gpio_4_(gpio_p1_15_),
             btn_1_pin_(PinConfigure::CreatePinConfigure<1, 9>()),
             btn_2_pin_(PinConfigure::CreatePinConfigure<1, 10>()),
             btn_3_pin_(PinConfigure::CreatePinConfigure<1, 14>()),
             btn_4_pin_(PinConfigure::CreatePinConfigure<1, 15>())
       {
             button = num;
       }

void Init() override {
  /**PIN CONFIGURATION: PIN MODE and FUNCTION SET**/
  btn_1_pin_.SetPinMode(PinConfigureInterface::PinMode::kInactive);
  btn_1_pin_.SetPinFunction(0);

  btn_2_pin_.SetPinMode(PinConfigureInterface::PinMode::kInactive);
  btn_2_pin_.SetPinFunction(0);

  btn_3_pin_.SetPinMode(PinConfigureInterface::PinMode::kInactive);
  btn_3_pin_.SetPinFunction(0);

  btn_4_pin_.SetPinMode(PinConfigureInterface::PinMode::kInactive);
  btn_4_pin_.SetPinFunction(0);

/**SET Button PIN direction as input**/
/*
  gpio_p1_9_.SetAsInput();
  gpio_p1_10_.SetAsInput();
  gpio_p1_14_.SetAsInput();
  gpio_p1_15_.SetAsInput();
*/
}

bool GetButtonState() override {
  if (button == 1) {
    if (LPC_GPIO1->PIN & (1 << 9)) {
      return true;
    } else {
        return false;
    }
  } else if (button == 2) {
      if (LPC_GPIO1->PIN & (1 << 10)) {
        return true;
      } else {
          return false;
      }
  } else if (button == 3) {
      if (LPC_GPIO1->PIN & (1 << 14)) {
        return true;
      } else {
          return false;
      }
  } else if (button == 4) {
      if (LPC_GPIO1->PIN & (1 << 15)) {
        return true;
      } else {
          return false;
      }
  } else {
      return false;
  }
}

bool Released() override {
  if (GetButtonState()) {
     return false;
  } else {
     return true;
  }
}

bool Pressed() override {
  if (GetButtonState()) {
    return true;
  } else {
    return false;
  }
}

void InvertButtonSignal(bool enable_invert_signal = true) override {
  if (enable_invert_signal == false) {
    if (button == 1) {
      btn_1_pin_.SetAsActiveLow(false);
    } else if (button == 2) {
        btn_2_pin_.SetAsActiveLow(false);
    } else if (button == 3) {
        btn_3_pin_.SetAsActiveLow(false);
    } else if (button == 4) {
        btn_4_pin_.SetAsActiveLow(false);
    }
  } else {
    if (button == 1) {
        btn_1_pin_.SetAsActiveLow(true);
    } else if (button == 2) {
        btn_2_pin_.SetAsActiveLow(true);
    } else if (button == 3) {
        btn_3_pin_.SetAsActiveLow(true);
    } else if (button == 4) {
        btn_4_pin_.SetAsActiveLow(true);
    }
  }
}

 private:
    const PinConfigureInterface & btn_1_;
    const PinConfigureInterface & btn_2_;
    const PinConfigureInterface & btn_3_;
    const PinConfigureInterface & btn_4_;
//    const GpioInterface & gpio_1_;
//    const GpioInterface & gpio_2_;
//    const GpioInterface & gpio_3_;
//    const GpioInterface & gpio_4_;

    PinConfigure btn_1_pin_;
    PinConfigure btn_2_pin_;
    PinConfigure btn_3_pin_;
    PinConfigure btn_4_pin_;
//  GpioConfigure gpio_p1_9_;
//  GpioConfigure gpio_p1_10_;
//  GpioConfigure gpio_p1_14_;
//  GpioConfigure gpio_p1_15_;
};
