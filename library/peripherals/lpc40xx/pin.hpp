// Pin abstracts the process of changing the mode and attributes of a
// pin on the LPC40xx series of chips.
//   Usage:
//      Pin P0_0(0, 0);
//      P0_0.SetAsActiveLow();
//      P0_0.PullUp();
#pragma once

#include <cstdio>

#include "platforms/targets/lpc40xx/LPC40xx.h"
#include "peripherals/pin.hpp"
#include "utility/math/bit.hpp"
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
  static constexpr bit::Mask kFunction = bit::MaskFromRange(0, 2);

  /// Bitmask for setting resistor mode of pin.
  static constexpr bit::Mask kResistor = bit::MaskFromRange(3, 4);

  /// Bitmask for setting pin hysteresis mode.
  static constexpr bit::Mask kHysteresis = bit::MaskFromRange(5);

  /// Bitmask for setting inputs as active low or active high. This will behave
  /// badly if the pin is set to a mode that is an output or set to analog. See
  /// usermanual for more details.
  static constexpr bit::Mask kInputInvert = bit::MaskFromRange(6);

  /// Bitmask for setting a pin to analog mode.
  static constexpr bit::Mask kAnalogDigitalMode = bit::MaskFromRange(7);

  /// Bitmask for enabling/disabling digital filter. This can be used to
  /// ignore/reject noise, reflections, or signal bounce (from something like a
  /// switch).
  static constexpr bit::Mask kDigitalFilter = bit::MaskFromRange(8);

  /// Bitmask to enable/disable high speed I2C mode
  static constexpr bit::Mask kI2cHighSpeed = bit::MaskFromRange(8);

  /// Bitmask to change the slew rate of signal transitions for outputs for a
  /// pin.
  static constexpr bit::Mask kSlew = bit::MaskFromRange(9);

  /// Bitmask to enable I2C high current drain. This can allow for even faster
  /// I2C communications, as well as allow for more devices on the bus.
  static constexpr bit::Mask kI2cHighCurrentDrive = bit::MaskFromRange(9);

  /// Bitmask to enable/disable open drain mode.
  static constexpr bit::Mask kOpenDrain = bit::MaskFromRange(10);

  /// Bitmask for enabling/disabling digital to analog pin mode.
  static constexpr bit::Mask kDacEnable = bit::MaskFromRange(16);

  /// Pin map table for maping pins and ports to registers.
  struct PinMap_t
  {
    /// Register matrix that maps against the 6 ports and the 32 pins per port
    volatile uint32_t register_matrix[6][32];
  };

  /// A pointer holding the address to the LPC40xx PIN peripheral.
  /// This variable is a dependency injection point for unit testing thus it is
  /// public and mutable. This is needed to perform the "test by side effect"
  /// technique for this class.
  inline static PinMap_t * pin_map = reinterpret_cast<PinMap_t *>(LPC_IOCON);

  /// Construct a pin for the specified port and pin numbers.
  ///
  /// @param port - port number for the pin you want to construct.
  /// @param pin - pin number for the pin you want to construct.
  constexpr Pin(uint8_t port, uint8_t pin) : sjsu::Pin(port, pin) {}

  /// Does nothing, pin hardware is enabled by default.
  void ModuleInitialize() override
  {
    if (settings.function > 0b111)
    {
      throw Exception(
          std::errc::invalid_argument,
          "The function code must be a 3-bit value between 0b000 and 0b111.");
    }

    SetPinRegister(settings.function, kFunction);
    SetPinRegister(Value(settings.resistor), kResistor);
    SetPinRegister(settings.open_drain, kOpenDrain);
    // Invert the bool because the bit must be set to 0 to enable analog mode.
    SetPinRegister(!settings.as_analog, kAnalogDigitalMode);
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

  /// @returns A pointer to the pin's registor in the pin_map matrix.
  [[gnu::always_inline]] volatile uint32_t * PinRegister() const
  {
    return &pin_map->register_matrix[GetPort()][GetPin()];
  }
};

template <int port, int pin_number>
inline Pin & GetPin()
{
  static_assert(
      (port <= 4 && pin_number <= 31) || (port == 5 && pin_number < 4),
      "For ports between 0 and 4, the pin number must be between 0 and 31. For "
      "port 5, the pin number must be equal to or below 4");

  static Pin pin(port, pin_number);
  return pin;
}
}  // namespace lpc40xx
}  // namespace sjsu
