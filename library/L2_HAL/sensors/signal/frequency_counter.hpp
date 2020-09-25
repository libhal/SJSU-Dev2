#pragma once

#include "L1_Peripheral/hardware_counter.hpp"
#include "module.hpp"
#include "utility/time.hpp"
#include "utility/units.hpp"

namespace sjsu
{
/// FrequencyCounter can measure the frequency of a signal based on a hardware
/// counter.
class FrequencyCounter : public Module
{
 public:
  // ===========================================================================
  // Interface Methods
  // ===========================================================================

  // ---------------------------------------------------------------------------
  // Configuration Methods
  // ---------------------------------------------------------------------------

  // ---------------------------------------------------------------------------
  // Usage Methods
  // ---------------------------------------------------------------------------

  /// @param counter - A hardware counter implementation that will be used to
  /// derive a frequency from.
  explicit FrequencyCounter(HardwareCounter * counter) : counter_(counter) {}

  /// Resets the frequency counter.
  virtual void Reset()
  {
    previous_count_ = counter_->GetCount();
    previous_time_  = Uptime();
  }

  /// Returns the approximate frequency of the hardware counter by measuring the
  /// time between calls of this function.
  ///
  /// This function cannot be called too frequently as it will cause issues with
  /// the measurement. For example, if you have a signal that has a maximum
  /// frequency of 1kHz, then the fastest you can call this method is 2ms
  /// between each call. This is important, as calling this any faster will
  /// result in erroneous results.
  ///
  /// The rule is this, you must wait 2x the maximum period of the input signal
  /// you expect in order to get reliable results. For more information as to
  /// why this is, see whats called the "Nyquist Frequency".
  /// https://en.wikipedia.org/wiki/Nyquist_frequency
  virtual units::frequency::hertz_t GetFrequency()
  {
    uint32_t current_count = counter_->GetCount();
    auto current_uptime    = Uptime();

    uint32_t count_delta                 = current_count - previous_count_;
    units::time::nanosecond_t time_delta = current_uptime - previous_time_;
    units::frequency::hertz_t result     = count_delta / time_delta;
    Reset();
    return result;
  }

  /// Initializes counter hardware. Will NOT start counting at this point. Will
  /// also set the counting direction to count up.
  void ModuleInitialize() override
  {
    counter_->Initialize();
    counter_->SetDirection(HardwareCounter::Direction::kUp);
  }

  /// Will enable the hardware counter and start the time measurement that will
  /// be used to measure the approximate frequency of the hardware counter.
  void ModuleEnable(bool enable = true) override
  {
    counter_->Enable(enable);
    previous_time_ = Uptime();
  }

 private:
  HardwareCounter * counter_;
  uint32_t previous_count_                = 0;
  std::chrono::nanoseconds previous_time_ = 0ns;
};
}  // namespace sjsu
