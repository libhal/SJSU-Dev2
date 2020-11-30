#pragma once

#include <cstdint>
#include <functional>

#include "L1_Peripheral/inactive.hpp"
#include "L1_Peripheral/interrupt.hpp"
#include "utility/error_handling.hpp"
#include "utility/units.hpp"

namespace sjsu
{
/// An abstract interface for timer peripherals. This interface can be used for
/// keeping time, starting and stopping a hardware timer, and causing interrupts
/// when the timer reaches a certain condition. This is not meant to be used for
/// PWM generation, or other advanced feature.
///
/// (INCOMPLETE PERIPHERAL has not migrated to sjsu::Module)
///
/// @ingroup l1_peripheral
class Timer
{
 public:
  // ===========================================================================
  // Interface Definitions
  // ===========================================================================

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

  // ===========================================================================
  // Interface Methods
  // ===========================================================================

  /// Initialize and enable hardware. This must be called before any other
  /// method in this interface is called.
  ///
  /// @param counter_frequency - the frequency that the timer's count register
  ///        will increment by. If this is set to 1'000'000Hz then the counter
  ///        will increment every microsecond. register will be 10 ms.
  /// @param callback - a callback that will be called when the condition set by
  ///        SetTimer method has occurred.
  /// @param priority - sets the Timer interrupt's priority level, defaults to
  ///        -1 which uses the platforms default priority.
  virtual void Initialize(units::frequency::hertz_t counter_frequency,
                          InterruptCallback callback = nullptr,
                          int32_t priority           = -1) const = 0;

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

  /// Starts the timer.
  virtual void Start() const = 0;

  /// Stops the timer.
  virtual void Stop() const = 0;

  /// Resets the timer.
  virtual void Reset() const = 0;
};

/// Template specialization that generates an inactive sjsu::Timer.
template <>
inline sjsu::Timer & GetInactive<sjsu::Timer>()
{
  class InactiveTimer : public sjsu::Timer
  {
   public:
    void Initialize(units::frequency::hertz_t,
                    InterruptCallback,
                    int32_t) const override
    {
    }
    void SetMatchBehavior(uint32_t, MatchAction, uint8_t) const override {}
    uint32_t GetCount() const override
    {
      return 0;
    }
    uint8_t GetAvailableMatchRegisters() const override
    {
      return 3;
    }
    void Start() const override {}
    void Stop() const override {}
    void Reset() const override {}
  };

  static InactiveTimer inactive;
  return inactive;
}
}  // namespace sjsu
