#pragma once

#include <cstdint>

#include "L1_Peripheral/interrupt.hpp"
#include "utility/status.hpp"
#include "utility/units.hpp"

namespace sjsu
{
class Timer
{
 public:
  // ==============================
  // Interface Definitions
  // ==============================

  /// Actions the timer can perform when the time counter equals one of the
  /// match registers.
  enum class MatchAction : uint8_t
  {
    kInterrupt            = 0b001,
    kRestart              = 0b010,
    kStop                 = 0b100,
    kInterruptRestart     = 0b011,
    kInterruptStop        = 0b101,
    kRestartStop          = 0b110,
    kInterruptRestartStop = 0b111
  };

  // ==============================
  // Interface Methods
  // ==============================

  /// Initialize and enable hardware. This must be called before any other
  /// method in this interface is called.
  ///
  /// @param counter_frequency - the frequency that the timer's count register
  ///        will increment by. If this is set to 1'000'000Hz then the counter
  ///        will increment every microsecond. register will be 10 ms.
  /// @param isr - the ISR that will fire when the condition set by SetTimer
  ///        method is achieved.
  /// @param priority - sets the Timer interrupt's priority level, defaults to
  ///        -1 which uses the platforms default priority.
  virtual Status Initialize(units::frequency::hertz_t counter_frequency,
                            IsrPointer isr   = nullptr,
                            int32_t priority = -1) const = 0;
  /// Set a timer to execute your timer command when the time counter equals the
  /// match register. time in ticks dependent on initialization Functionality is
  /// defined by mode: interrupt, stop, or reset on match.
  ///
  /// @param match_count - when the time counter matches this number perform the
  ///        action in the "action" parameter.
  /// @param action - the action to take when timer counter equals the match
  ///        register.
  /// @param match_register - which match register should be used
  ///        for holding the count for the action.
  virtual void SetMatchBehavior(uint32_t match_count,
                                MatchAction action,
                                uint8_t match_register) const = 0;
  /// @return number of available match registers
  virtual uint8_t GetAvailableMatchRegisters() const = 0;
  /// Get the current count in the count register
  virtual uint32_t GetCount() const = 0;
};
}  // namespace sjsu
