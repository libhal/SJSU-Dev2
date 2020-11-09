#pragma once

#include "module.hpp"
#include "utility/units.hpp"
#include "utility/error_handling.hpp"

namespace sjsu
{
/// Abstraction Interface for a coulomb counter. This device can give us
/// information about a connected battery's power level.
class CoulombCounter : public Module
{
 public:
  /// Returns the cumulative amount of charge that has passed through the
  /// coulomb counter.
  virtual units::charge::microampere_hour_t GetCharge() = 0;
};
}  // namespace sjsu
