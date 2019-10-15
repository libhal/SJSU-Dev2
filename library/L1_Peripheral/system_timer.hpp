// SystemTimer abstracts the process of changing enabling and setting
// up the SystemTimer.
#pragma once

#include <cstdint>

#include "L1_Peripheral/interrupt.hpp"
#include "utility/status.hpp"
#include "utility/units.hpp"

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
  virtual int32_t SetTickFrequency(
      units::frequency::hertz_t frequency) const = 0;
  // Start the system timer. Should be done after SetInterrupt and
  // SetTickFrequency have been called.
  virtual Status StartTimer() const = 0;
};
}  // namespace sjsu
