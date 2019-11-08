#pragma once

namespace sjsu
{
/// Abstraction Interface for a coulomb counter. This device can give us
/// information about a connected battery's power level.
class CoulombCounter
{
 public:
  /// Initialize and enable hardware. This must be called before any other
  /// method in this interface is called.
  virtual void Initialize() const = 0;
  /// Returns the battery's milliamp hours at a given ppint in time.
  virtual float GetBatterymAh() const = 0;
};
}  // namespace sjsu
