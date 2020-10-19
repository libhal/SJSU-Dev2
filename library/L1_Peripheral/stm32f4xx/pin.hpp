#pragma once

#include <array>

#include "L0_Platform/stm32f4xx/stm32f4xx.h"
#include "L1_Peripheral/pin.hpp"
#include "L1_Peripheral/stm32f4xx/system_controller.hpp"
#include "utility/bit.hpp"
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
    if (!('A' <= port_ && port_ <= 'I'))
    {
      throw Exception(std::errc::invalid_argument,
                      "Port must be between 'A' and 'I'");
    }

    switch (port_)
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
  }

  void ModuleEnable(bool = true) override {}

  void ConfigureFunction(uint8_t function) override
  {
    if (function > 0b1111)
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
      .position = static_cast<uint8_t>((pin_ % 8) * 4),
      .width    = 4,
    };

    // Set alternative function code
    Port()->AFR[pin_ / 8] = bit::Insert(Port()->AFR[pin_ / 8], function, kMask);
  }

  void ConfigurePullResistor(Resistor resistor) override
  {
    uint8_t mask = 0;
    // 00: No pull-up, pull-down
    // 01: Pull-up
    // 10: Pull-down
    // 11: Reserved
    switch (resistor)
    {
      case Resistor::kNone: mask = 0b00; break;
      case Resistor::kPullUp: mask = 0b01; break;
      case Resistor::kPullDown: mask = 0b10; break;
      default:
        throw Exception(std::errc::not_supported, "Invalid resistor pull.");
    }

    Port()->PUPDR = bit::Insert(Port()->PUPDR, mask, Mask());
  }

  void ConfigureAsOpenDrain(bool set_as_open_drain = true) override
  {
    Port()->OTYPER = bit::Insert(Port()->OTYPER, set_as_open_drain, pin_, 1);
  }

  void ConfigureAsAnalogMode(bool set_as_analog = true) override
  {
    // RM0090 p.281
    //
    //     00: Input (reset state)
    //     01: General purpose output mode
    //     10: Alternate function mode
    // --> 11: Analog mode
    constexpr uint8_t kAnalogCode = 0b11;
    if (set_as_analog)
    {
      Port()->MODER = bit::Insert(Port()->MODER, kAnalogCode, Mask());
    }
  }

 private:
  GPIO_TypeDef * Port() const
  {
    return gpio[PortToIndex()];
  }

  bit::Mask Mask() const
  {
    return {
      .position = (pin_ * 2U),
      .width    = 2,
    };
  }

  uint8_t PortToIndex() const
  {
    return static_cast<uint8_t>(port_ - 'A');
  }

  friend class Gpio;
};
}  // namespace sjsu::stm32f4xx
