// Pin abstracts the process of changing the mode and attributes of a
// pin on the LPC40xx series of chips.
//   Usage:
//      Pin P0_0(0, 0);
//      P0_0.SetAsActiveLow();
//      P0_0.SetPull(sjsu::Pin::Resistor::pullUp);
#pragma once

#include <cstdio>

#include "L1_Peripheral/pin.hpp"

#include "L0_Platform/lpc40xx/LPC40xx.h"
#include "utility/bit.hpp"
#include "utility/enum.hpp"
#include "utility/macros.hpp"

namespace sjsu
{
namespace lpc40xx
{
/// Pin control driver for the LPC40xx and LPC17xx microcontrollers.
class Pin final : public sjsu::Pin
{
 public:
  // Source: "UM10562 LPC408x/407x User manual" table 83 page 132
  /// Bitmask for setting pin mux function code.
  static constexpr bit::Mask kFunction = bit::CreateMaskFromRange(0, 2);
  /// Bitmask for setting resistor mode of pin.
  static constexpr bit::Mask kResistor = bit::CreateMaskFromRange(3, 4);
  /// Bitmask for setting pin hysteresis mode.
  static constexpr bit::Mask kHysteresis = bit::CreateMaskFromRange(5);
  /// Bitmask for setting inputs as active low or active high. This will behave
  /// badly if the pin is set to a mode that is an output or set to analog. See
  /// usermanual for more details.
  static constexpr bit::Mask kInputInvert = bit::CreateMaskFromRange(6);
  /// Bitmask for setting a pin to analog mode.
  static constexpr bit::Mask kAnalogDigitalMode = bit::CreateMaskFromRange(7);
  /// Bitmask for enabling/disabling digital filter. This can be used to
  /// ignore/reject noise, reflections, or signal bounce (from something like a
  /// switch).
  static constexpr bit::Mask kDigitalFilter = bit::CreateMaskFromRange(8);
  /// Bitmask to enable/disable high speed I2C mode
  static constexpr bit::Mask kI2cHighSpeed = bit::CreateMaskFromRange(8);
  /// Bitmask to change the slew rate of signal transitions for outputs for a
  /// pin.
  static constexpr bit::Mask kSlew = bit::CreateMaskFromRange(9);
  /// Bitmask to enable I2C high current drain. This can allow for even faster
  /// I2C communications, as well as allow for more devices on the bus.
  static constexpr bit::Mask kI2cHighCurrentDrive = bit::CreateMaskFromRange(9);
  /// Bitmask to enable/disable open drain mode.
  static constexpr bit::Mask kOpenDrain = bit::CreateMaskFromRange(10);
  /// Bitmask for enabling/disabling digital to analog pin mode.
  static constexpr bit::Mask kDacEnable = bit::CreateMaskFromRange(16);
  /// Pin map table for maping pins and ports to registers.
  struct [[gnu::packed]] PinMap_t
  {
    /// Register matrix that maps against the 6 ports and the 32 pins per port
    volatile uint32_t register_matrix[6][32];
  };
  /// A pointer holding the address to the LPC40xx PIN peripheral.
  /// This variable is a dependency injection point for unit testing thus it is
  /// public and mutable. This is needed to perform the "test by side effect"
  /// technique for this class.
  inline static PinMap_t * pin_map = reinterpret_cast<PinMap_t *>(LPC_IOCON);
  /// Compile time Pin factory that test the port and pin variables to make sure
  /// they are within bounds of the pin map register.
  template <unsigned port_, unsigned pin_>
  static constexpr sjsu::lpc40xx::Pin CreatePin()
  {
    static_assert(port_ <= 5, "Port must be between 0 and 5");
    static_assert(pin_ <= 31, "Pin must be between 0 and 31");
    static_assert(port_ < 5 || (port_ == 5 && pin_ <= 4),
                  "For port 5, the pin number must be equal to or below 4");
    return Pin(port_, pin_);
  }

  /// Construct a pin for the specified port and pin numbers.
  ///
  /// @param port - port number for the pin you want to construct.
  /// @param pin - pin number for the pin you want to construct.
  constexpr Pin(uint8_t port, uint8_t pin) : sjsu::Pin(port, pin) {}

  void SetPinFunction(uint8_t function) const override
  {
    SetPinRegister(function, kFunction);
  }
  void SetPull(Resistor resistor) const override
  {
    SetPinRegister(static_cast<uint8_t>(resistor), kResistor);
  }
  void SetAsAnalogMode(bool set_as_analog = true) const override
  {
    // Invert the bool because the bit must be set to 0 to enable analog mode.
    SetPinRegister(!set_as_analog, kAnalogDigitalMode);
  }
  void SetAsOpenDrain(bool set_as_open_drain = true) const override
  {
    SetPinRegister(set_as_open_drain, kOpenDrain);
  }
  /// Enable pin hysteresis. This allows the pin to remember which state is was
  /// previously when it was driven.
  ///
  /// @param enable_hysteresis - set to false to disable this mode.
  void EnableHysteresis(bool enable_hysteresis = true) const
  {
    SetPinRegister(enable_hysteresis, kHysteresis);
  }
  /// Set the pin to be active low. Only works for input pin functions.
  /// Undefined behavior, possibly dangerous, if used with output pin function.
  ///
  /// @param set_as_active_low - set to false to disable this mode.
  void SetAsActiveLow(bool set_as_active_low = true) const
  {
    SetPinRegister(set_as_active_low, kInputInvert);
  }
  /// Enable by setting bit to 0 to enable digital filter.
  ///
  /// @param enable_digital_filter - set to false to disable this mode.
  void EnableDigitalFilter(bool enable_digital_filter = true) const
  {
    SetPinRegister(!enable_digital_filter, kDigitalFilter);
  }
  /// Enable fast IO mode this pin.
  ///
  /// @param enable_fast_mode - set to false to disable this mode.
  void EnableFastMode(bool enable_fast_mode = true) const
  {
    SetPinRegister(enable_fast_mode, kSlew);
  }
  /// Enable I2C high speed mode for this pin.
  ///
  /// @param enable_high_speed - set to false to disable this mode.
  void EnableI2cHighSpeedMode(bool enable_high_speed = true) const
  {
    SetPinRegister(!enable_high_speed, kI2cHighSpeed);
  }
  /// Enable i2c high current drive mode on pin.
  ///
  /// @param enable_high_current - set to false to disable this mode.
  void EnableI2cHighCurrentDrive(bool enable_high_current = true) const
  {
    SetPinRegister(enable_high_current, kI2cHighCurrentDrive);
  }
  /// Enable digital-to-analog mode on pin.
  ///
  /// @param enable_dac - set to false to disable this mode.
  void EnableDac(bool enable_dac = true) const
  {
    SetPinRegister(enable_dac, kDacEnable);
  }

 protected:
  /// Does the work of changing the contents of the pin register.
  ///
  /// @param data - the contents to load into the register
  /// @param mask - indicates which bits to set to data
  void SetPinRegister(uint8_t data, bit::Mask mask) const
  {
    *PinRegister() = bit::Insert(*PinRegister(), data, mask);
  }
  /// @returns a pointer to the pin's registor in the pin_map matrix.
  [[gnu::always_inline]] volatile uint32_t * PinRegister() const
  {
    return &pin_map->register_matrix[GetPort()][GetPin()];
  }
};
}  // namespace lpc40xx
}  // namespace sjsu
