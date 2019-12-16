#pragma once

#include "utility/units.hpp"

namespace sjsu
{
/// Abstraction Interface for a coulomb counter. This device can give us
/// information about a connected battery's power level.
class CoulombCounter
{
 public:
  /// Initialize and enable hardware. This must be called before any other
  /// method in this interface is called.
  virtual void Initialize() = 0;
  /// Returns the cumulative amount of charge that has passed through the
  /// coulomb counter.
  virtual units::charge::milliampere_hour_t GetCharge() const = 0;
  /// Default virtual destructor
  virtual ~CoulombCounter() = default;
};
}  // namespace sjsu
