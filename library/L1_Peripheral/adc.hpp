#pragma once

#include <cstdint>

#include "utility/status.hpp"

namespace sjsu
{
class Adc
{
 public:
  /// Initialize and enable hardware. This must be called before any other
  /// method in this interface is called.
  virtual Status Initialize() const = 0;
  /// Start conversion process and loop until the ADC conversion is complete.
  /// When this returns, you can read the actual value from the Read() method.
  ///
  /// Some implementations of ADC have additional functionality that allows them
  /// to skip the conversion step. For example the LPC40xx series of chips have
  /// burst mode, which causes the hardware to continuous perform conversion
  /// without the need of the user code to run this method. With Burst mode
  /// enabled, Read() will always return the latest value.
  virtual void Conversion() const = 0;
  /// Read the analog signal's value.
  /// The number active bits depends on the ADC being used and be known by
  /// running the GetActiveBits().
  ///
  /// @return Returns the digital representation of the analog.
  virtual uint32_t Read() const = 0;
  /// @return true if a conversion has finished. The exact behaviour of this
  ///         method is not known if Conversion() was not called before calling
  ///         this method.
  virtual bool HasConversionFinished() const = 0;
  /// @return number of active bits for the ADC.
  virtual uint8_t GetActiveBits() const = 0;
};
}  // namespace sjsu
