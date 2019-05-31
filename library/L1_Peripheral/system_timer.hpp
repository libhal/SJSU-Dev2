// SystemTimer abstracts the process of changing enabling and setting
// up the SystemTimer.
#pragma once

#include <cstring>

#include "config.hpp"

#include "L0_Platform/interrupt.hpp"
#include "L0_Platform/lpc40xx/LPC40xx.h"
#include "L1_Peripheral/lpc40xx/system_controller.hpp"
#include "utility/macros.hpp"
#include "utility/status.hpp"

namespace sjsu
{
class SystemTimer
{
 public:
  virtual void SetInterrupt(IsrPointer isr) const             = 0;
  virtual Status StartTimer() const                           = 0;
  virtual uint32_t SetTickFrequency(uint32_t frequency) const = 0;
};
}  // namespace sjsu
