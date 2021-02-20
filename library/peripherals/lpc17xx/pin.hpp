#pragma once

#include <cstdio>

#include "platforms/targets/lpc17xx/LPC17xx.h"
#include "peripherals/pin.hpp"
#include "utility/math/bit.hpp"
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

  void ModuleInitialize() override
  {
    ConfigureFunction();
    ConfigurePullResistor();
    ConfigureAsOpenDrain();
  }

 private:
  void ConfigureFunction()
  {
    uint32_t pin_reg_select           = PinRegisterLookup();
    function_map->pin[pin_reg_select] = bit::Insert(
        function_map->pin[pin_reg_select], settings.function, kPinMask);
  }

  void ConfigurePullResistor()
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
                    kResistorModes[Value(settings.resistor)],
                    kPinMask);
  }

  void ConfigureAsOpenDrain()
  {
    open_drain_map->pin[GetPort()] = bit::Insert(open_drain_map->pin[GetPort()],
                                                 settings.open_drain,
                                                 {
                                                     .position = GetPin(),
                                                     .width    = 1,
                                                 });
  }

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
    uint32_t odd_register = (GetPin() > 15) ? 1 : 0;
    return (GetPort() * 2) + odd_register;
  }

  /// Bitmask for the pin
  const bit::Mask kPinMask;
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
}  // namespace lpc17xx
}  // namespace sjsu
