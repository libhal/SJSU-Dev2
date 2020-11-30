#pragma once

#include "module.hpp"
#include "utility/units.hpp"

namespace sjsu
{
/// @ingroup battery
/// Abstract interface for a state of charge.
class BatteryCharge : public Module
{
 public:
  /// Read the state of battery charge from a device.
  ///
  /// @return charge status of a battery as a float between 0.0 and 1.0.
  virtual float Read() = 0;
};

}  // namespace sjsu
