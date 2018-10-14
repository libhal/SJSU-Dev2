// Pin abstracts the process of changing the mode and attributes of a
// pin on the LPC40xx series of chips.
//   Usage:
//      Pin P0_0(0, 0);
//      P0_0.SetAsActiveLow();
//      P0_0.SetMode(PinInterface::Mode::pullUp);
#pragma once

#include <cstdio>

#include "L0_LowLevel/LPC40xx.h"
#include "L2_Utilities/macros.hpp"

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

  static PinMap_t * pin_map;
  // Compile time Pin factory that test the port and pin variables to make sure
  // they are within bounds of the pin_config_register.
  template <unsigned port, unsigned pin>
  static constexpr Pin CreatePin()
  {
    static_assert(port <= 5, "Port must be between 0 and 5");
    static_assert(pin <= 31, "Pin must be between 0 and 31");
    static_assert(port < 5 || (port == 5 && pin <= 4),
                  "For port 5, the pin number must be equal to or below 4");
    return Pin(port, pin);
  }
  // Pin P5.4 is not featured on the LPC4078, so manipulating its bits has
  // no effect.
  static constexpr Pin CreateInactivePin()
  {
    return Pin(5, 4);
  }
  constexpr Pin(uint8_t port_number, uint8_t pin_number)
      : port(port_number), pin(pin_number)
  {
  }
  void SetPinFunction(uint8_t function) override
  {
    pin_map->_register[port][pin] =
        BitPlace(pin_map->_register[port][pin], PinBitMap::kFunction,
                 function & 0b111, 3);
  }
  void SetMode(PinInterface::Mode mode) override
  {
    uint8_t ui_mode               = static_cast<uint8_t>(mode);
    pin_map->_register[port][pin] = BitPlace(
        pin_map->_register[port][pin], PinBitMap::kMode, ui_mode & 0b11, 2);
  }
  void EnableHysteresis(bool enable_hysteresis = true) override
  {
    pin_map->_register[port][pin] =
        BitPlace(pin_map->_register[port][pin], PinBitMap::kHysteresis,
                 enable_hysteresis, 1);
  }
  void SetAsActiveLow(bool set_as_active_low = true) override
  {
    pin_map->_register[port][pin] =
        BitPlace(pin_map->_register[port][pin], PinBitMap::kInputInvert,
                 set_as_active_low, 1);
  }
  // Set bit to 0 to enable analog mode
  void SetAsAnalogMode(bool set_as_analog = true) override
  {
    pin_map->_register[port][pin] =
        BitPlace(pin_map->_register[port][pin], PinBitMap::kAnalogDigitalMode,
                 !set_as_analog, 1);
  }
  // Enable by setting bit to 0 to enable digital filter.
  void EnableDigitalFilter(bool enable_digital_filter = true) override
  {
    pin_map->_register[port][pin] =
        BitPlace(pin_map->_register[port][pin], PinBitMap::kDigitalFilter,
                 !enable_digital_filter, 1);
  }
  void EnableFastMode(bool enable_fast_mode = true) override
  {
    pin_map->_register[port][pin] = BitPlace(
        pin_map->_register[port][pin], PinBitMap::kSlew, enable_fast_mode, 1);
  }
  // Enable by setting bit to 0 for i2c high speed mode
  void EnableI2cHighSpeedMode(bool enable_high_speed = true) override
  {
    pin_map->_register[port][pin] =
        BitPlace(pin_map->_register[port][pin], PinBitMap::kI2cHighSpeed,
                 !enable_high_speed, 1);
  }
  void EnableI2cHighCurrentDrive(bool enable_high_current = true) override
  {
    pin_map->_register[port][pin] =
        BitPlace(pin_map->_register[port][pin], PinBitMap::kI2cHighCurrentDrive,
                 enable_high_current, 1);
  }
  void SetAsOpenDrain(bool set_as_open_drain = true) override
  {
    pin_map->_register[port][pin] =
        BitPlace(pin_map->_register[port][pin], PinBitMap::kOpenDrain,
                 set_as_open_drain, 1);
  }
  void EnableDac(bool enable_dac = true) override
  {
    pin_map->_register[port][pin] = BitPlace(
        pin_map->_register[port][pin], PinBitMap::kDacEnable, enable_dac, 1);
  }
  inline uint32_t BitPlace(uint32_t target, uint32_t position, uint32_t value,
                           uint32_t value_width)
  {
    // Generate mask with all 1s
    uint32_t mask = 0xFFFFFFFF >> (32 - value_width);
    target &= ~(mask << position);
    target |= (value & mask) << position;
    return target;
  }
  uint8_t GetPort()
  {
    return port;
  }
  uint8_t GetPin()
  {
    return pin;
  }

 protected:
  uint8_t port;
  uint8_t pin;
};
