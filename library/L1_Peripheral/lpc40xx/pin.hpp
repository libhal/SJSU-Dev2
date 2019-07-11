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
class Pin final : public sjsu::Pin
{
 public:
  // Source: "UM10562 LPC408x/407x User manual" table 83 page 132
  static constexpr bit::Mask kFunction    = bit::CreateMaskFromRange(0, 2);
  static constexpr bit::Mask kResistor    = bit::CreateMaskFromRange(3, 4);
  static constexpr bit::Mask kHysteresis  = bit::CreateMaskFromRange(5);
  static constexpr bit::Mask kInputInvert = bit::CreateMaskFromRange(6);
  static constexpr bit::Mask kAnalogDigitalMode   = bit::CreateMaskFromRange(7);
  static constexpr bit::Mask kDigitalFilter       = bit::CreateMaskFromRange(8);
  static constexpr bit::Mask kI2cHighSpeed        = bit::CreateMaskFromRange(8);
  static constexpr bit::Mask kSlew                = bit::CreateMaskFromRange(9);
  static constexpr bit::Mask kI2cHighCurrentDrive = bit::CreateMaskFromRange(9);
  static constexpr bit::Mask kOpenDrain = bit::CreateMaskFromRange(10);
  static constexpr bit::Mask kDacEnable = bit::CreateMaskFromRange(16);

  struct [[gnu::packed]] PinMap_t
  {
    using pin_config_register_t = volatile uint32_t;
    pin_config_register_t _register[6][32];
  };

  inline static PinMap_t * pin_map = reinterpret_cast<PinMap_t *>(LPC_IOCON);
  // Compile time Pin factory that test the port and pin variables to make sure
  // they are within bounds of the pin_config_register.
  template <unsigned port_, unsigned pin_>
  static constexpr sjsu::lpc40xx::Pin CreatePin()
  {
    static_assert(port_ <= 5, "Port must be between 0 and 5");
    static_assert(pin_ <= 31, "Pin must be between 0 and 31");
    static_assert(port_ < 5 || (port_ == 5 && pin_ <= 4),
                  "For port 5, the pin number must be equal to or below 4");
    return Pin(port_, pin_);
  }
  // Pin P5.4 is not featured on the LPC4078, so manipulating its bits has
  // no effect.
  static constexpr sjsu::lpc40xx::Pin CreateInactivePin()
  {
    return Pin(5, 4);
  }
  constexpr Pin(uint8_t port, uint8_t pin) : sjsu::Pin(port, pin) {}

  void SetPinFunction(uint8_t function) const override
  {
    SetPinRegister(function, kFunction);
  }
  void SetPull(Resistor resistor) const override
  {
    SetPinRegister(static_cast<uint8_t>(resistor), kResistor);
  }
  // Set bit to 0 to enable analog mode
  void SetAsAnalogMode(bool set_as_analog = true) const override
  {
    SetPinRegister(!set_as_analog, kAnalogDigitalMode);
  }

  void SetAsOpenDrain(bool set_as_open_drain = true) const override
  {
    SetPinRegister(set_as_open_drain, kOpenDrain);
  }

  void EnableHysteresis(bool enable_hysteresis = true) const
  {
    SetPinRegister(enable_hysteresis, kHysteresis);
  }
  void SetAsActiveLow(bool set_as_active_low = true) const
  {
    SetPinRegister(set_as_active_low, kInputInvert);
  }
  // Enable by setting bit to 0 to enable digital filter.
  void EnableDigitalFilter(bool enable_digital_filter = true) const
  {
    SetPinRegister(!enable_digital_filter, kDigitalFilter);
  }
  void EnableFastMode(bool enable_fast_mode = true) const
  {
    SetPinRegister(enable_fast_mode, kSlew);
  }
  // Enable by setting bit to 0 for i2c high speed mode
  void EnableI2cHighSpeedMode(bool enable_high_speed = true) const
  {
    SetPinRegister(!enable_high_speed, kI2cHighSpeed);
  }
  void EnableI2cHighCurrentDrive(bool enable_high_current = true) const
  {
    SetPinRegister(enable_high_current, kI2cHighCurrentDrive);
  }
  void EnableDac(bool enable_dac = true) const
  {
    SetPinRegister(enable_dac, kDacEnable);
  }

 protected:
  void SetPinRegister(uint8_t data, bit::Mask mask) const
  {
    *PinRegister() = bit::Insert(*PinRegister(), data, mask);
  }
  [[gnu::always_inline]] volatile uint32_t * PinRegister() const
  {
    return &pin_map->_register[GetPort()][GetPin()];
  }
};
}  // namespace lpc40xx
}  // namespace sjsu
