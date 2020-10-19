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
/// Pin control driver for the LPC40xx and LPC17xx microcontrollers.
class Pin final : public sjsu::Pin
{
 public:
  /// The maximum number of pins for each port.
  static constexpr uint32_t kNumberOfPins = 32;

  /// Table of registers for the LPC17xx pins.
  struct [[gnu::packed]] PinTable_t
  {
    /// Because each port holds 32 pins, and most of the pin control registers
    /// use 2-bits for configuration, and we have about five and a half ports,
    /// this structure holds an array of 11 uint32_t.
    volatile uint32_t pin[11];
  };

  /// Pointer to the pin function code map
  inline static volatile PinTable_t * function_map =
      reinterpret_cast<volatile PinTable_t *>(&LPC_PINCON->PINSEL0);
  /// Pointer to the pin resistor control map
  inline static volatile PinTable_t * resistor_map =
      reinterpret_cast<volatile PinTable_t *>(&LPC_PINCON->PINMODE0);
  /// Pointer to the open drain control map
  inline static volatile PinTable_t * open_drain_map =
      reinterpret_cast<volatile PinTable_t *>(&LPC_PINCON->PINMODE_OD0);

  /// Construct a pin for the specified port and pin numbers.
  ///
  /// @param port - port number for the pin you want to construct.
  /// @param pin - pin number for the pin you want to construct.
  constexpr Pin(uint8_t port, uint8_t pin)
      : sjsu::Pin(port, pin), kPinMask(GetPinsBitmask(pin))
  {
  }

  /// NOTE: GPIO hardare is enabled and ready by default on reset. This
  /// initialize method only serves to check that the port and pin are correct,
  /// and throws if they are not.
  void ModuleInitialize() override
  {
    if (port_ > 5 && pin_ > 31)
    {
      throw Exception(
          std::errc::invalid_argument,
          "Port must be between 0 and 5 & Pin must be between 0 and 31");
    }
    if (port_ == 5 && pin_ > 4)
    {
      throw Exception(std::errc::invalid_argument,
                      "For port 5, the pin number must be equal to or below 4");
    }
  }

  /// Does nothing
  void ModuleEnable(bool = true) override {}

  void ConfigureFunction(uint8_t function) override
  {
    uint32_t pin_reg_select = PinRegisterLookup();
    function_map->pin[pin_reg_select] =
        bit::Insert(function_map->pin[pin_reg_select], function, kPinMask);
  }

  void ConfigurePullResistor(Resistor resistor) override
  {
    static constexpr uint8_t kResistorModes[4] = {
      0b10,  // kNone     [0]
      0b11,  // kPullDown [1]
      0b00,  // kPullUp   [2]
      0b01,  // kRepeater [3]
    };
    uint32_t pin_reg_select = PinRegisterLookup();
    resistor_map->pin[pin_reg_select] =
        bit::Insert(resistor_map->pin[pin_reg_select],
                    kResistorModes[Value(resistor)], kPinMask);
  }

  /// Implement ConfigureAsAnalogMode as deprecated and unsupported
  [[deprecated("Unsupported operation")]] void ConfigureAsAnalogMode(
      bool = true) override
  {
    throw Exception(std::errc::operation_not_supported, "");
  }

  void ConfigureAsOpenDrain(bool set_as_open_drain = true) override
  {
    open_drain_map->pin[port_] =
        bit::Insert(open_drain_map->pin[port_], set_as_open_drain,
                    {
                        .position = pin_,
                        .width    = 1,
                    });
  }

 private:
  /// Utility function for generating bitmasks for specific pins.
  static constexpr bit::Mask GetPinsBitmask(uint8_t pin)
  {
    return {
      .position = static_cast<uint8_t>((pin * 2) % kNumberOfPins),
      .width    = 2,
    };
  }

  /// Performs the necessary math to figure out which register corresponds to
  /// this objects port.
  ///
  /// @returns Index of register in PinTable_t map.
  uint32_t PinRegisterLookup() const
  {
    uint32_t odd_register = (pin_ > 15) ? 1 : 0;
    return (port_ * 2) + odd_register;
  }

  /// Bitmask for the pin
  const bit::Mask kPinMask;
};
}  // namespace lpc17xx
}  // namespace sjsu
