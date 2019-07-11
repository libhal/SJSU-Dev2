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
  // ==============================
  // Interface Methods
  // ==============================

  // Set the interrupt handler for the system timer
  virtual void SetInterrupt(IsrPointer isr) const = 0;
  // Set frequency of the timer
  virtual uint32_t SetTickFrequency(uint32_t frequency) const = 0;
  // Start the system timer. Should be done after SetInterrupt and
  // SetTickFrequency have been called.
  virtual Status StartTimer() const = 0;
};
}  // namespace sjsu
