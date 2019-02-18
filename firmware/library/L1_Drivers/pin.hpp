// Pin abstracts the process of changing the mode and attributes of a
// pin on the LPC40xx series of chips.
//   Usage:
//      Pin P0_0(0, 0);
//      P0_0.SetAsActiveLow();
//      P0_0.SetMode(PinInterface::Mode::pullUp);
#pragma once

#include <cstdio>

#include "L0_LowLevel/LPC40xx.h"
#include "utility/macros.hpp"
#include "utility/bit.hpp"

class PinInterface
{
 public:
  // Source: "UM10562 LPC408x/407x User manual" table 83 page 132
  enum class Mode : uint8_t
  {
    kInactive = 0,
    kPullDown,
    kPullUp,
    kRepeater
  };
  virtual void SetPinFunction(uint8_t function)                           = 0;
  virtual void SetMode(PinInterface::Mode mode)                           = 0;
  virtual void EnableHysteresis(bool enable_hysteresis = true)            = 0;
  virtual void SetAsActiveLow(bool set_as_active_low = true)              = 0;
  virtual void SetAsAnalogMode(bool set_as_analog = true)                 = 0;
  virtual void EnableDigitalFilter(bool enable_digital_filter = true)     = 0;
  virtual void EnableFastMode(bool enable_fast_mode = true)               = 0;
  virtual void EnableI2cHighSpeedMode(bool enable_high_speed = true)      = 0;
  virtual void EnableI2cHighCurrentDrive(bool enable_high_current = true) = 0;
  virtual void SetAsOpenDrain(bool set_as_open_drain = true)              = 0;
  virtual void EnableDac(bool enable_dac = true)                          = 0;
  virtual uint8_t GetPort() const                                         = 0;
  virtual uint8_t GetPin() const                                          = 0;
};

class Pin : public PinInterface
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

  SJ2_PACKED(struct) PinMap_t
  {
    using pin_config_register_t = uint32_t;
    pin_config_register_t _register[6][32];
  };

  inline static PinMap_t * pin_map = reinterpret_cast<PinMap_t *>(LPC_IOCON);
  /// Compile time Pin factory that test the port and pin variables to make sure
  /// they are within bounds of the pin_config_register.
  template <unsigned port_, unsigned pin_>
  static constexpr Pin CreatePin()
  {
    static_assert(port_ <= 5, "Port must be between 0 and 5");
    static_assert(pin_ <= 31, "Pin must be between 0 and 31");
    static_assert(port_ < 5 || (port_ == 5 && pin_ <= 4),
                  "For port 5, the pin number must be equal to or below 4");
    return Pin(port_, pin_);
  }
  /// Pin P5.4 is not featured on the LPC4078, so manipulating its bits has
  /// no effect.
  static constexpr Pin CreateInactivePin()
  {
    return Pin(5, 4);
  }
  constexpr Pin(uint8_t port_number, uint8_t pin_number)
      : port_(port_number), pin_(pin_number)
  {
  }
  /// Select the function that this pin will be. Pins can be more than general
  /// purpose Input and Output devices. Some have the ability to output analog
  /// signals and some allow serial data to be sent and recieved from a pin.
  ///
  /// @param function - pass the 3-bit value to select the function for this
  ///                   pin
  void SetPinFunction(uint8_t function) override
  {
    pin_map->_register[port_][pin_] =
        bit::Insert(pin_map->_register[port_][pin_],
                 function  PinBitMap::kFunction,& 0b111, 3);
  }
  /// Set the mode of the pin which can be pull-up resistor, pull down resistor,
  /// neither, and repeater mode.
  void SetMode(PinInterface::Mode mode) override
  {
    uint8_t ui_mode                 = static_cast<uint8_t>(mode);
    pin_map->_register[port_][pin_] = bit::Insert(
        pin_map->_register[port_][pin_], ui_mode  PinBitMap::kMode,& 0b11, 2);
  }
  /// Enable hysteresis on the pin, which will allow
  ///
  /// @param enable_hysteresis - if false, disable hystersis
  void EnableHysteresis(bool enable_hysteresis = true) override
  {
    pin_map->_register[port_][pin_] =
        bit::Insert(pin_map->_register[port_][pin_],
                 enable_hysteresis, PinBitMap::kHysteresis, 1);
  }
  /// Set pin as Active low.
  ///
  /// @param set_as_active_low - if false, set pin as active high
  void SetAsActiveLow(bool set_as_active_low = true) override
  {
    pin_map->_register[port_][pin_] =
        bit::Insert(pin_map->_register[port_][pin_],
                 set_as_active_low, PinBitMap::kInputInvert, 1);
  }
  /// Set bit to 0 to enable analog mode
  ///
  /// @param set_as_analog - if false, set pin as digital
  void SetAsAnalogMode(bool set_as_analog = true) override
  {
    pin_map->_register[port_][pin_] =
        bit::Insert(pin_map->_register[port_][pin_],
                 !set_as_analog, PinBitMap::kAnalogDigitalMode, 1);
  }
  /// Enable by setting bit to 0 to enable digital filter.
  ///
  /// @param enable_digital_filter - if false, disable digital filter
  void EnableDigitalFilter(bool enable_digital_filter = true) override
  {
    pin_map->_register[port_][pin_] =
        bit::Insert(pin_map->_register[port_][pin_],
                 !enable_digital_filter, PinBitMap::kDigitalFilter, 1);
  }
  /// Enable Fast mode for pin.
  ///
  /// @param enable_fast_mode - if false, disable pin fast mode
  void EnableFastMode(bool enable_fast_mode = true) override
  {
    pin_map->_register[port_][pin_] = bit::Insert(
        pin_map->_register[port_][pin_], enable_fast_mode, PinBitMap::kSlew, 1);
  }
  /// Enable i2c high speed mode for pin
  ///
  /// @param enable_high_speed - if false, disable high speed I2c mode
  void EnableI2cHighSpeedMode(bool enable_high_speed = true) override
  {
    pin_map->_register[port_][pin_] =
        bit::Insert(pin_map->_register[port_][pin_],
                 !enable_high_speed, PinBitMap::kI2cHighSpeed, 1);
  }
  /// Enable high current i2c driver for pin
  ///
  /// @param enable_high_current - if false, disable high current i2c driver
  void EnableI2cHighCurrentDrive(bool enable_high_current = true) override
  {
    pin_map->_register[port_][pin_] =
        bit::Insert(pin_map->_register[port_][pin_],
                 enable_high_current, PinBitMap::kI2cHighCurrentDrive, 1);
  }
  /// Set pin as open drain
  ///
  /// @param set_as_open_drain - if false, do not set pin as open drain
  void SetAsOpenDrain(bool set_as_open_drain = true) override
  {
    pin_map->_register[port_][pin_] =
        bit::Insert(pin_map->_register[port_][pin_],
                 set_as_open_drain, PinBitMap::kOpenDrain, 1);
  }
  /// Enable dac capabilities on the pin.
  ///
  /// @param enable_dac - if false, disable dac capabilities of the pin.
  void EnableDac(bool enable_dac = true) override
  {
    pin_map->_register[port_][pin_] = bit::Insert(
        pin_map->_register[port_][pin_], enable_dac, PinBitMap::kDacEnable, 1);
  }
  uint8_t GetPort() const override
  {
    return port_;
  }
  uint8_t GetPin() const override
  {
    return pin_;
  }

 protected:
  uint8_t port_;
  uint8_t pin_;
};
