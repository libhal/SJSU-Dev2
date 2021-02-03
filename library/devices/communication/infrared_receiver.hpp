#pragma once

#include <functional>

#include "module.hpp"
#include "utility/error_handling.hpp"
#include "utility/infrared_algorithms.hpp"

namespace sjsu
{
/// An abstract interface for communication drivers responsible for receiving
/// infrared (IR) data.
class InfraredReceiver : public Module<>
{
 public:
  /// Callback handler that is invoked when a complete data frame is
  /// successfully received.
  using DataReceivedHandler =
      std::function<void(const infrared::DataFrame_t *)>;

  /// Sets the callback handler that is invoked when a data frame is received.
  ///
  /// @param handler Callback handler to invoke.
  virtual void SetInterruptCallback(DataReceivedHandler handler) = 0;
};
}  // namespace sjsu
