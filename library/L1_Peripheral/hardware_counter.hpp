#pragma once

#include <cstdint>

namespace sjsu
{
/// An abstract interface for controlling a hardware counter, internal or
/// external.
class HardwareCounter
{
 public:
  /// When called, this initializes the peripheral hardware. You must ensure
  /// that the counter is set to zero after this call. NOTE: This must not
  /// enable the counter. In order to start counting based on an external input,
  /// an explicit invocation of Enable() must occur.
  virtual void Initialize() = 0;
  /// Set counter to a specific value .There is no guarantee that counts will
  /// not be missed during the invocation of this call
  ///
  /// @param new_count_value - the value to set the counter to.
  virtual void Set(uint32_t new_count_value) = 0;
  /// Starts counting from clock input. Must be called after Initialize() to
  /// begin counting.
  virtual void Enable() = 0;
  /// Stops counting from clock input. Current count is retained.
  virtual void Disable() = 0;
  /// Get the current count from hardware timer.
  virtual uint32_t GetCount() = 0;
};
}  // namespace sjsu
