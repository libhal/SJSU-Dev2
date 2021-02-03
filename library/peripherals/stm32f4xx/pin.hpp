#pragma once

#include <array>

#include "platforms/targets/stm32f4xx/stm32f4xx.h"
#include "peripherals/pin.hpp"
#include "peripherals/stm32f4xx/system_controller.hpp"
#include "utility/math/bit.hpp"
#include "utility/log.hpp"

namespace sjsu::stm32f4xx
{
/// Implementation of a Pin in stm32f4xx
class Pin final : public sjsu::Pin
{
 public:
  /// List of GPIO peripherals
  static inline std::array<GPIO_TypeDef *, 9> gpio = {
    GPIOA,  // => 'A'
    GPIOB,  // => 'B'
    GPIOC,  // => 'C'
    GPIOD,  // => 'D'
    GPIOE,  // => 'E'
    GPIOF,  // => 'F'
    GPIOG,  // => 'G'
    GPIOH,  // => 'H'
    GPIOI,  // => 'I'
  };

  /// @param port - must be a capitol letter from 'A' to 'I'
  /// @param pin - must be between 0 to 15
  constexpr Pin(uint8_t port, uint8_t pin) : sjsu::Pin(port, pin) {}

  void ModuleInitialize() override
  {
    switch (GetPort())
    {
      case 'A':
        SystemController::GetPlatformController().PowerUpPeripheral(
            stm32f4xx::SystemController::Peripherals::kGpioA);
        break;
      case 'B':
        SystemController::GetPlatformController().PowerUpPeripheral(
            stm32f4xx::SystemController::Peripherals::kGpioB);
        break;
      case 'C':
        SystemController::GetPlatformController().PowerUpPeripheral(
            stm32f4xx::SystemController::Peripherals::kGpioC);
        break;
      case 'D':
        SystemController::GetPlatformController().PowerUpPeripheral(
            stm32f4xx::SystemController::Peripherals::kGpioD);
        break;
      case 'E':
        SystemController::GetPlatformController().PowerUpPeripheral(
            stm32f4xx::SystemController::Peripherals::kGpioE);
        break;
      case 'F':
        SystemController::GetPlatformController().PowerUpPeripheral(
            stm32f4xx::SystemController::Peripherals::kGpioF);
        break;
      case 'G':
        SystemController::GetPlatformController().PowerUpPeripheral(
            stm32f4xx::SystemController::Peripherals::kGpioG);
        break;
      case 'H':
        SystemController::GetPlatformController().PowerUpPeripheral(
            stm32f4xx::SystemController::Peripherals::kGpioH);
        break;
      case 'I':
        SystemController::GetPlatformController().PowerUpPeripheral(
            stm32f4xx::SystemController::Peripherals::kGpioI);
        break;
    }

    ConfigureFunction();
    ConfigurePullResistor();
    ConfigureAsOpenDrain();
    ConfigureAsAnalogMode();
  }

 private:
  void ConfigureFunction()
  {
    if (settings.function > 0b1111)
    {
      throw Exception(std::errc::invalid_argument,
                      "The function code must be a 4-bit code.");
    }

    // RM0090 p.281
    //
    //     00: Input (reset state)
    //     01: General purpose output mode
    // --> 10: Alternate function mode
    //     11: Analog mode
    Port()->MODER = bit::Insert(Port()->MODER, 0b10, Mask());

    // Create bitmask for the pin
    const bit::Mask kMask = {
      .position = static_cast<uint8_t>((GetPin() % 8) * 4),
      .width    = 4,
    };

    // Set alternative function code
    Port()->AFR[GetPin() / 8] =
        bit::Insert(Port()->AFR[GetPin() / 8], settings.function, kMask);
  }

  void ConfigurePullResistor()
  {
    uint8_t mask = 0;
    // 00: No pull-up, pull-down
    // 01: Pull-up
    // 10: Pull-down
    // 11: Reserved
    switch (settings.resistor)
    {
      case PinSettings_t::Resistor::kNone: mask = 0b00; break;
      case PinSettings_t::Resistor::kPullUp: mask = 0b01; break;
      case PinSettings_t::Resistor::kPullDown: mask = 0b10; break;
    }

    Port()->PUPDR = bit::Insert(Port()->PUPDR, mask, Mask());
  }

  void ConfigureAsOpenDrain()
  {
    Port()->OTYPER =
        bit::Insert(Port()->OTYPER, settings.open_drain, GetPin(), 1);
  }

  void ConfigureAsAnalogMode()
  {
    // RM0090 p.281
    //
    //     00: Input (reset state)
    //     01: General purpose output mode
    //     10: Alternate function mode
    // --> 11: Analog mode
    constexpr uint8_t kAnalogCode = 0b11;
    if (settings.as_analog)
    {
      Port()->MODER = bit::Insert(Port()->MODER, kAnalogCode, Mask());
    }
  }

  GPIO_TypeDef * Port() const
  {
    return gpio[PortToIndex()];
  }

  bit::Mask Mask() const
  {
    return {
      .position = (GetPin() * 2U),
      .width    = 2,
    };
  }

  uint8_t PortToIndex() const
  {
    return static_cast<uint8_t>(GetPort() - 'A');
  }

  friend class Gpio;
};

template <int port, int pin_number>
inline Pin & GetPin()
{
  static_assert(
      ('A' <= port && port <= 'I') && (0 <= pin_number && pin_number <= 15),
      SJ2_ERROR_MESSAGE_DECORATOR(
          "stm32f4xx: Port must be between 'A' and 'I' and pin must be between "
          "0 and 15!\n"));

  static Pin pin(port, pin_number);
  return pin;
}
}  // namespace sjsu::stm32f4xx
