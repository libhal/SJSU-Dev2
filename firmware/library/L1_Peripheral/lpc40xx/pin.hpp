// Pin abstracts the process of changing the mode and attributes of a
// pin on the LPC40xx series of chips.
//   Usage:
//      Pin P0_0(0, 0);
//      P0_0.SetAsActiveLow();
//      P0_0.SetMode(sjsu::Pin::Mode::pullUp);
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
  enum PinBitMap : uint8_t
  {
    kFunction            = 0,
    kMode                = 3,
    kHysteresis          = 5,
    kInputInvert         = 6,
    kAnalogDigitalMode   = 7,
    kDigitalFilter       = 8,
    kI2cHighSpeed        = 8,
    kSlew                = 9,
    kI2cHighCurrentDrive = 9,
    kOpenDrain           = 10,
    kDacEnable           = 16
  };

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
    *GetPinRegister() = bit::Insert(*GetPinRegister(), function,
                                    util::Value(PinBitMap::kFunction), 3);
  }
  void SetMode(sjsu::Pin::Mode mode) const override
  {
    uint8_t ui_mode   = static_cast<uint8_t>(mode);
    *GetPinRegister() = bit::Insert(*GetPinRegister(), ui_mode,
                                    util::Value(PinBitMap::kMode), 2);
  }
  // Set bit to 0 to enable analog mode
  void SetAsAnalogMode(bool set_as_analog = true) const override
  {
    *GetPinRegister() =
        bit::Insert(*GetPinRegister(), !set_as_analog,
                    util::Value(PinBitMap::kAnalogDigitalMode), 1);
  }

  void SetAsOpenDrain(bool set_as_open_drain = true) const override
  {
    *GetPinRegister() = bit::Insert(*GetPinRegister(), set_as_open_drain,
                                    util::Value(PinBitMap::kOpenDrain), 1);
  }

  void EnableHysteresis(bool enable_hysteresis = true) const
  {
    *GetPinRegister() = bit::Insert(*GetPinRegister(), enable_hysteresis,
                                    util::Value(PinBitMap::kHysteresis), 1);
  }
  void SetAsActiveLow(bool set_as_active_low = true) const
  {
    *GetPinRegister() = bit::Insert(*GetPinRegister(), set_as_active_low,
                                    util::Value(PinBitMap::kInputInvert), 1);
  }
  // Enable by setting bit to 0 to enable digital filter.
  void EnableDigitalFilter(bool enable_digital_filter = true) const
  {
    *GetPinRegister() = bit::Insert(*GetPinRegister(), !enable_digital_filter,
                                    util::Value(PinBitMap::kDigitalFilter), 1);
  }
  void EnableFastMode(bool enable_fast_mode = true) const
  {
    *GetPinRegister() = bit::Insert(*GetPinRegister(), enable_fast_mode,
                                    util::Value(PinBitMap::kSlew), 1);
  }
  // Enable by setting bit to 0 for i2c high speed mode
  void EnableI2cHighSpeedMode(bool enable_high_speed = true) const
  {
    *GetPinRegister() = bit::Insert(*GetPinRegister(), !enable_high_speed,
                                    util::Value(PinBitMap::kI2cHighSpeed), 1);
  }
  void EnableI2cHighCurrentDrive(bool enable_high_current = true) const
  {
    *GetPinRegister() =
        bit::Insert(*GetPinRegister(), enable_high_current,
                    util::Value(PinBitMap::kI2cHighCurrentDrive), 1);
  }
  void EnableDac(bool enable_dac = true) const
  {
    *GetPinRegister() = bit::Insert(*GetPinRegister(), enable_dac,
                                    util::Value(PinBitMap::kDacEnable), 1);
  }

 protected:
  [[gnu::always_inline]] volatile uint32_t * GetPinRegister() const
  {
    return &pin_map->_register[GetPort()][GetPin()];
  }
};
}  // namespace lpc40xx
}  // namespace sjsu
