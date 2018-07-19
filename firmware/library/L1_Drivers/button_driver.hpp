#pragma once

#include <cstdint>
#include <cstdio>
#include "L0_LowLevel/LPC40xx.h"
#include "L1_Drivers/pin_configure.hpp"

class ButtonInterface {
 public:
         virtual bool getButtonState()      = 0;
         virtual bool released()            = 0;
         virtual bool pressed()             = 0;
         virtual void defaultButtonSignal() = 0;
         virtual void invertButtonSignal()  = 0;
};
