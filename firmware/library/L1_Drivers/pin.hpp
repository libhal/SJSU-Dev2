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
#include "utility/enum.hpp"

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
  virtual void SetPinFunction(uint8_t function) const                       = 0;
  virtual void SetMode(PinInterface::Mode mode) const                       = 0;
  virtual void EnableHysteresis(bool enable_hysteresis = true) const        = 0;
  virtual void SetAsActiveLow(bool set_as_active_low = true) const          = 0;
  virtual void SetAsAnalogMode(bool set_as_analog = true) const             = 0;
  virtual void EnableDigitalFilter(bool enable_digital_filter = true) const = 0;
  virtual void EnableFastMode(bool enable_fast_mode = true) const           = 0;
  virtual void EnableI2cHighSpeedMode(bool enable_high_speed = true) const  = 0;
  virtual void EnableI2cHighCurrentDrive(
      bool enable_high_current = true) const                       = 0;
  virtual void SetAsOpenDrain(bool set_as_open_drain = true) const = 0;
  virtual void EnableDac(bool enable_dac = true) const             = 0;
  virtual uint8_t GetPort() const                                  = 0;
  virtual uint8_t GetPin() const                                   = 0;
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

  struct [[gnu::packed]] PinMap_t
  {
    using pin_config_register_t = volatile uint32_t;
    pin_config_register_t _register[6][32];
  };

  inline static PinMap_t * pin_map = reinterpret_cast<PinMap_t *>(LPC_IOCON);
  // Compile time Pin factory that test the port and pin variables to make sure
  // they are within bounds of the pin_config_register.
  template <unsigned port_, unsigned pin_>
  static constexpr Pin CreatePin()
  {
    static_assert(port_ <= 5, "Port must be between 0 and 5");
    static_assert(pin_ <= 31, "Pin must be between 0 and 31");
    static_assert(port_ < 5 || (port_ == 5 && pin_ <= 4),
                  "For port 5, the pin number must be equal to or below 4");
    return Pin(port_, pin_);
  }
  // Pin P5.4 is not featured on the LPC4078, so manipulating its bits has
  // no effect.
  static constexpr Pin CreateInactivePin()
  {
    return Pin(5, 4);
  }
  constexpr Pin(uint8_t port_number, uint8_t pin_number) : pin_info_{}
  {
    pin_info_.port = port_number & 0b111;
    pin_info_.pin  = pin_number & 0b1'1111;
  }
  void SetPinFunction(uint8_t function) const override
  {
    *GetPinRegister() = bit::Insert(
        *GetPinRegister(), util::Value(PinBitMap::kFunction), function, 3);
  }
  void SetMode(PinInterface::Mode mode) const override
  {
    uint8_t ui_mode   = static_cast<uint8_t>(mode);
    *GetPinRegister() = bit::Insert(
        *GetPinRegister(), util::Value(PinBitMap::kMode), ui_mode & 0b11, 2);
  }
  void EnableHysteresis(bool enable_hysteresis = true) const override
  {
    *GetPinRegister() =
        bit::Insert(*GetPinRegister(), util::Value(PinBitMap::kHysteresis),
                    enable_hysteresis, 1);
  }
  void SetAsActiveLow(bool set_as_active_low = true) const override
  {
    *GetPinRegister() =
        bit::Insert(*GetPinRegister(), util::Value(PinBitMap::kInputInvert),
                    set_as_active_low, 1);
  }
  // Set bit to 0 to enable analog mode
  void SetAsAnalogMode(bool set_as_analog = true) const override
  {
    *GetPinRegister() = bit::Insert(*GetPinRegister(),
                                    util::Value(PinBitMap::kAnalogDigitalMode),
                                    !set_as_analog, 1);
  }
  // Enable by setting bit to 0 to enable digital filter.
  void EnableDigitalFilter(bool enable_digital_filter = true) const override
  {
    *GetPinRegister() =
        bit::Insert(*GetPinRegister(), util::Value(PinBitMap::kDigitalFilter),
                    !enable_digital_filter, 1);
  }
  void EnableFastMode(bool enable_fast_mode = true) const override
  {
    *GetPinRegister() = bit::Insert(
        *GetPinRegister(), util::Value(PinBitMap::kSlew), enable_fast_mode, 1);
  }
  // Enable by setting bit to 0 for i2c high speed mode
  void EnableI2cHighSpeedMode(bool enable_high_speed = true) const override
  {
    *GetPinRegister() =
        bit::Insert(*GetPinRegister(), util::Value(PinBitMap::kI2cHighSpeed),
                    !enable_high_speed, 1);
  }
  void EnableI2cHighCurrentDrive(bool enable_high_current = true) const override
  {
    *GetPinRegister() = bit::Insert(
        *GetPinRegister(), util::Value(PinBitMap::kI2cHighCurrentDrive),
        enable_high_current, 1);
  }
  void SetAsOpenDrain(bool set_as_open_drain = true) const override
  {
    *GetPinRegister() =
        bit::Insert(*GetPinRegister(), util::Value(PinBitMap::kOpenDrain),
                    set_as_open_drain, 1);
  }
  void EnableDac(bool enable_dac = true) const override
  {
    *GetPinRegister() = bit::Insert(
        *GetPinRegister(), util::Value(PinBitMap::kDacEnable), enable_dac, 1);
  }
  uint8_t GetPort() const override
  {
    return pin_info_.port;
  }
  uint8_t GetPin() const override
  {
    return pin_info_.pin;
  }

 protected:
  volatile uint32_t * GetPinRegister() const
  {
    return &pin_map->_register[pin_info_.port][pin_info_.pin];
  }
  struct [[gnu::packed]] PinStructure
  {
    unsigned port : 3;
    unsigned pin : 5;
  };
  PinStructure pin_info_;
};
