#pragma once

#include <cstdint>

#include "L1_Peripheral/interrupt.hpp"
#include "utility/error_handling.hpp"
#include "utility/units.hpp"

namespace sjsu
{
/// Pulse capture library (INCOMPLETE, does not use sjsu::Module yet)
///
/// @ingroup l1_peripheral
class PulseCapture
{
 public:
    /// Structure containing information collected during a capture event
  struct CaptureStatus_t
  {
    /// Timer count at the point in time a capture event was triggered
    uint32_t count;

    /// Capture status flags at the point in time a capture event was triggered
    uint32_t flags;
  };

  /// Interrupt callback that passes capture status information
  using CaptureCallback = std::function<void(PulseCapture::CaptureStatus_t)>;

  /// Define which edges to capture input on
  enum class CaptureEdgeMode : uint8_t
  {
    kNone,
    kRising,
    kFalling,
    kBoth
  };

  /// Initialize timer for capturing
  virtual void Initialize(CaptureCallback isr        = nullptr,
                          int32_t interrupt_priority = -1) const = 0;

  /// Select edge type to capture on
  virtual void ConfigureCapture(CaptureEdgeMode mode) const = 0;

  /// Enable or disable capture interrupts
  virtual void EnableCaptureInterrupt(bool enabled) const = 0;
};
}  // namespace sjsu
