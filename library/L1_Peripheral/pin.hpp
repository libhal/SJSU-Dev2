#pragma once

#include <cstdint>

#include "inactive.hpp"
#include "module.hpp"
#include "utility/error_handling.hpp"

namespace sjsu
{
/// Abstraction interface for physical electrical "pins".
///
/// Pins are a type of electrical contact. Contacts are metal surfaces which can
/// be pressed or connected to another contact or pin in order to create an
/// electric circuit.
///
/// The pins of a microcontrollers typically can support more then one function
/// or mode. This could be allowing the pin to sense analog voltages, another
/// could be a mode for communicating to other devices.
///
/// Not only can pin's functions be changed but their properties can also be
/// changed along side these functions. For example, many controllers have
/// internal pull up (resistor connected to high voltage) or pull down (resistor
/// connected to ground) resistors to keep the state of the pin at a known
/// voltage when the pin is not connected to anything.
///
/// This interface represents a common set of functions that most controllers
/// and systems support.
/// @ingroup l1_peripheral
class Pin : public Module
{
 public:
  // ===========================================================================
  // Interface Defintions
  // ===========================================================================

  /// Defines the set of internal resistance connections to a pin. Read specific
  /// enumeration constant comments/documentation to understand more about what
  /// each one does.
  enum class Resistor : uint8_t
  {
    /// Disable resistor pull. If the pin is setup as high-z (input mode) and
    /// not connected to anything then the pin will be floating. Its value will
    /// not undefined.
    kNone = 0,

    /// Connect pin to ground using weak (high resistance) resistor.
    kPullDown,

    /// Connect pin to controller digital voltage (VCC) using a weak (high
    /// resistance) resistor.
    kPullUp,

    /// Connect pin to a pull up or pull down resistor based what the pin's
    /// previous state was.
    ///
    /// For example: if the pin was connected to a HIGH
    /// voltage for a moment, then then was removed from this source (so now the
    /// pin is disconnected or floating), then the pin will remember that
    /// voltage state and enable the pull-up resistor.
    ///
    /// Connecting a LOW voltage source to the pin, will enable the pull-down
    /// resistor.
    kRepeater
  };

  /// Set internal port and pin values.
  constexpr Pin(uint8_t port, uint8_t pin) : port_(port), pin_(pin) {}

  // ===========================================================================
  // Interface Methods
  // ===========================================================================

  // ---------------------------------------------------------------------------
  // Configuration Methods
  // ---------------------------------------------------------------------------

  /// Set the pin's function using a function code.
  /// The function code is very specific to the controller being used.
  ///
  /// But an example could be the LPC4078 chip's P0.0. It has the following
  /// functions:
  ///
  ///    0. GPIO (General purpose input or output) pin
  ///    1. CANBUS port 1 Read
  ///    2. UART port 3 transmitter
  ///    3. I2C port 1 serial data
  ///    4. and UART port 0 transmitter
  ///
  /// Each function listed above is listed with its function code. If you pass
  /// the value 4 into the ConfigureFunction function, for P0.0, it will set
  /// that pin to the UART port 0 transmitter function. After that, if you have
  /// properly enabled the UART hardware, when you attempt to send data through
  /// the uart0 port, it will show up on the pin.
  ///
  /// Please consult the user manual for the chip you are using to figure out
  /// which function codes correspond to specific functions.
  ///
  /// Generally, this method is only used laterally in the L1 layer. This is due
  /// to the fact that other L1s need to use this library to setup their
  /// external pins, but also due to the fact that a Hardware abstraction or
  /// above should not have to concern itself with function codes, thus it is
  /// the job of the L1 peripheral that uses this pin to manage its own function
  /// code usage.
  ///
  /// @param function - pin function code
  /// @throw sjsu::Exception - std::errc::invalid_argument the function code is
  /// not valid.
  virtual void ConfigureFunction(uint8_t function) = 0;

  /// Set pin's resistor pull, setting ot either no resistor pull, pull down,
  /// pull up and repeater.
  ///
  /// @param resistor - which resistor setup you would like.
  /// @throw sjsu::Exception - with std::errc::not_supported if the pull option
  /// is not supported by the MCU.
  virtual void ConfigurePullResistor(Resistor resistor) = 0;

  /// Set pin to open drain mode
  ///
  /// @param set_as_open_drain If false, disable open drain feature, pin
  ///        becomes push-pull (a.k.a totem pole).
  /// @throw sjsu::Exception - with std::errc::not_supported if open drain is
  /// not supported by the MCU.
  virtual void ConfigureAsOpenDrain(bool set_as_open_drain = true) = 0;

  /// Set pin as analog mode
  ///
  /// @param set_as_analog If false, disable analog mode for pin
  /// @throw sjsu::Exception - with std::errc::not_supported if analog mode is
  /// not supported by the MCU.
  virtual void ConfigureAsAnalogMode(bool set_as_analog = true) = 0;

  // ---------------------------------------------------------------------------
  // Usage Methods
  // ---------------------------------------------------------------------------

  // ===========================================================================
  // Utility Methods
  // ===========================================================================

  /// Configure the pin to use the internal pull up resistor
  void ConfigurePullUp()
  {
    return ConfigurePullResistor(Resistor::kPullUp);
  }

  /// Configure the pin to use the internal pull up resistor
  void ConfigurePullDown()
  {
    return ConfigurePullResistor(Resistor::kPullDown);
  }

  /// Configure the pin to not use a pull resistor
  void ConfigureFloating()
  {
    return ConfigurePullResistor(Resistor::kNone);
  }

  /// Getter method for the pin's port.
  ///
  /// @returns The pin's port.
  uint8_t GetPort() const
  {
    return port_;
  }

  /// Getter method for the pin's pin.
  ///
  /// @returns The pin's pin.
  uint8_t GetPin() const
  {
    return pin_;
  }

 protected:
  /// Assigned port
  uint8_t port_;
  /// Assigned pin within the above port
  uint8_t pin_;
};

/// Template specialization that generates an inactive sjsu::Pin.
template <>
inline sjsu::Pin & GetInactive<sjsu::Pin>()
{
  class InactivePin : public sjsu::Pin
  {
   public:
    InactivePin() : sjsu::Pin(0, 0) {}
    void ModuleInitialize() override {}
    void ModuleEnable(bool = true) override {}
    void ConfigureFunction(uint8_t) override {}
    void ConfigurePullResistor(Resistor) override {}
    void ConfigureAsOpenDrain(bool) override {}
    void ConfigureAsAnalogMode(bool) override {}
  };

  static InactivePin inactive;
  return inactive;
}
}  // namespace sjsu
