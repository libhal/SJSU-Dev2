#pragma once

#include <cstdint>

#include "peripherals/gpio.hpp"
#include "module.hpp"
#include "utility/log.hpp"

namespace sjsu
{
/// ParallelBus is an abstraction for a set of parallel digital input/output
/// pins that can be used to communicate over a parallel bus, read switch
/// states, or possibly control control LEDs.
class ParallelBus : public Module<>
{
 public:
  // ===========================================================================
  // Interface Methods
  // ===========================================================================

  /// Write the contents of the data parameter the parallel bus.
  /// Each bit of the data uint32_t is assigned to each corrissponding bit of
  /// the parallel bus from the 0th bit to the Nth, where N is the width of the
  /// bus.
  ///
  /// Lets assume a bit width of 5 bits.
  ///
  ///    parallel_bus.Write(0b1'1001);
  ///
  /// Becomes:
  ///                  /- [4] 1
  ///                 /-- [3] 1
  ///      0b1'1001 ----- [2] 0
  ///                 \-- [1] 0
  ///                  \- [0] 1
  ///
  /// Where each [x] is a pin.
  ///
  /// @param data - value to set the parallel bus to.
  virtual void Write(uint32_t data) = 0;

  /// Read the state of the parallel pins in the bus and return them as a
  /// uint32_t. NOTE: if the pins are set to output, this function is expected
  /// to return the state of the output pins. This should not change the
  /// direction of the pins from OUTPUT to INPUT.
  virtual uint32_t Read() = 0;

  /// Returns the number of parallel pins that make up this parallel bus.
  virtual size_t BusWidth() = 0;

  /// Set the direction of the parallel bus pins.
  ///
  /// @param direction - The direction to set the parallel bus to.
  virtual void SetDirection(sjsu::Gpio::Direction direction) = 0;

  /// Set the pins of the parallel bus as open drain (or open collector).
  /// Default version of this will halt the system if called, as most
  /// implementations are not expected to have an open drain capability.
  ///
  /// @param set_as_open_drain - if true, set output of parallel bus pins to
  /// open drain. Otherwise, set pin as push-pull.
  virtual void ConfigureAsOpenDrain(bool set_as_open_drain = true)
  {
    if (set_as_open_drain)
    {
      throw sjsu::Exception(
          std::errc::not_supported,
          "ConfigureAsOpenDrain() is not available for this parallel bus "
          "implementation.");
    }
  }

  // ===========================================================================
  // Helper Functions
  // ===========================================================================

  /// Utility method for setting all pins to output.
  void SetAsOutput()
  {
    SetDirection(sjsu::Gpio::Direction::kOutput);
  }

  /// Utility method for setting all pins to input.
  void SetAsInput()
  {
    SetDirection(sjsu::Gpio::Direction::kInput);
  }
};
}  // namespace sjsu
