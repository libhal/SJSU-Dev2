#pragma once

#include <functional>

#include "utility/infrared_algorithms.hpp"
#include "utility/error_handling.hpp"

namespace sjsu
{
/// An abstract interface for communication drivers responsible for receiving
/// infrared (IR) data.
class InfraredReceiver
{
 public:
  /// Callback handler that is invoked when a complete data frame is
  /// successfully received.
  using DataReceivedHandler =
      std::function<void(const infrared::DataFrame_t *)>;

  /// Initialize and enable hardware. This must be called before any other
  /// method in this interface is called.
  virtual void Initialize() = 0;

  /// Sets the callback handler that is invoked when a data frame is received.
  ///
  /// @param handler Callback handler to invoke.
  virtual void SetInterruptCallback(DataReceivedHandler handler) = 0;
};
}  // namespace sjsu
