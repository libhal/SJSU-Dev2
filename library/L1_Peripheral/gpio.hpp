#pragma once

#include <cstdint>

#include "L1_Peripheral/interrupt.hpp"
#include "L1_Peripheral/lpc40xx/pin.hpp"
#include "utility/status.hpp"

namespace sjsu
{
class Gpio
{
 public:
  // ==============================
  // Interface Defintions
  // ==============================

  enum Direction : uint8_t
  {
    kInput  = 0,
    kOutput = 1
  };
  enum State : uint8_t
  {
    kLow  = 0,
    kHigh = 1
  };
  enum class Edge : uint8_t
  {
    kEdgeRising  = 0,
    kEdgeFalling = 1,
    kEdgeBoth    = 2
  };

  // ==============================
  // Interface Methods
  // ==============================

  /// Set pin as an output or an input
  virtual void SetDirection(Direction direction) const = 0;
  /// Set the pin state as HIGH voltage or LOW voltage
  virtual void Set(State output) const = 0;
  /// Toggles pin state. If the pin is HIGH, after this call it will be LOW
  /// and vise versa.
  virtual void Toggle() const = 0;
  /// @return the state of the pin, note that this method does not consider
  ///         whether or not the active level is high or low. Simply returns the
  ///         state as depicted in memory
  virtual bool Read() const = 0;
  /// @return underlying pin object
  virtual const sjsu::Pin & GetPin() const = 0;
  /// Attach an interrupt call to a pin
  ///
  /// @param function - the function to execute when the interrupt condition
  ///        occurs
  /// @param edge - the pin condition that will trigger the interrupt
  virtual void AttachInterrupt(IsrPointer function, Edge edge) const = 0;
  /// Remove interrupt call from pin and deactivate interrupts for this pin
  virtual void DetachInterrupt() const = 0;

  // ==============================
  // Utility Methods
  // ==============================

  void SetHigh() const
  {
    Set(State::kHigh);
  }
  void SetLow() const
  {
    Set(State::kLow);
  }
  void SetAsInput() const
  {
    SetDirection(Direction::kInput);
  }
  void SetAsOutput() const
  {
    SetDirection(Direction::kOutput);
  }
};
}  // namespace sjsu
