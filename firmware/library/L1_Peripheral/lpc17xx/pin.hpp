#pragma once

#include <cstdio>

#include "L1_Peripheral/pin.hpp"

#include "L0_Platform/lpc17xx/LPC17xx.h"
#include "utility/bit.hpp"
#include "utility/enum.hpp"
#include "utility/log.hpp"
#include "utility/macros.hpp"

namespace sjsu
{
namespace lpc17xx
{
class Pin final : public sjsu::Pin
{
 public:
  struct [[gnu::packed]] PinTable_t
  {
    volatile uint32_t pin[11];
  };

  inline static volatile PinTable_t * function_map =
      reinterpret_cast<volatile PinTable_t *>(&LPC_PINCON->PINSEL0);

  inline static volatile PinTable_t * mode_map =
      reinterpret_cast<volatile PinTable_t *>(&LPC_PINCON->PINMODE0);

  inline static volatile PinTable_t * open_drain_map =
      reinterpret_cast<volatile PinTable_t *>(&LPC_PINCON->PINMODE_OD0);
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
  // Pin P5.4 is not featured on the LPC17xx, so manipulating its bits has
  // no effect.
  static constexpr sjsu::lpc17xx::Pin CreateInactivePin()
  {
    return Pin(5, 4);
  }

  constexpr Pin(uint8_t port, uint8_t pin) : sjsu::Pin(port, pin) {}

  uint32_t PinSelectRegister() const
  {
    uint32_t odd_register = (pin_ > 15) ? 1 : 0;
    return (port_ * 2) + odd_register;
  }

  void SetPinFunction(uint8_t function) const override
  {
    uint32_t pin_reg_select = PinSelectRegister();
    function_map->pin[pin_reg_select] =
        bit::Insert(function_map->pin[pin_reg_select], function, pin_ * 2, 2);
  }
  static constexpr uint8_t kResistorModes[4] = {
    0b10,  // kInactive [0]
    0b11,  // kPullDown [1]
    0b00,  // kPullUp   [2]
    0b01,  // kRepeater [3]
  };
  void SetMode(Mode mode) const override
  {
    uint32_t pin_reg_select = PinSelectRegister();
    mode_map->pin[pin_reg_select] =
        bit::Insert(mode_map->pin[pin_reg_select],
                    kResistorModes[util::Value(mode)], pin_ * 2, 2);
  }
  [[deprecated]] void SetAsAnalogMode(
      [[maybe_unused]] bool set_as_analog = true) const override {
    LOG_WARNING("Unsupported operation");
  }

  void SetAsOpenDrain(bool set_as_open_drain = true) const override
  {
    open_drain_map->pin[port_] =
        bit::Insert(open_drain_map->pin[port_], set_as_open_drain, pin_, 1);
  }
};
}  // namespace lpc17xx
}  // namespace sjsu
