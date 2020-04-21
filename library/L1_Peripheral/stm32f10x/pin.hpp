#pragma once

#include <array>

#include "L0_Platform/stm32f10x/stm32f10x.h"
#include "L1_Peripheral/pin.hpp"
#include "L1_Peripheral/stm32f10x/system_controller.hpp"
#include "utility/bit.hpp"
#include "utility/log.hpp"

namespace sjsu::stm32f10x
{
/// Implementation of a Pin in stm32f10x
class Pin final : public sjsu::Pin
{
 public:
  /// List of GPIO peripherals
  static inline std::array<GPIO_TypeDef *, 7> gpio = {
    GPIOA,  // => 'A'
    GPIOB,  // => 'B'
    GPIOC,  // => 'C'
    GPIOD,  // => 'D'
    GPIOE,  // => 'E'
    GPIOF,  // => 'F'
    GPIOG,  // => 'G'
  };

  /// @param port - must be a capitol letter from 'A' to 'I'
  /// @param pin - must be between 0 to 15
  constexpr Pin(uint8_t port, uint8_t pin) : sjsu::Pin(port, pin) {}

  void Initialize() const override
  {
    SJ2_ASSERT_FATAL('A' <= port_ && port_ <= 'I',
                     "Invalid port choosen for this pin implementation");
    switch (port_)
    {
      case 'A':
        SystemController::GetPlatformController().PowerUpPeripheral(
            stm32f10x::SystemController::Peripherals::kGpioA);
        break;
      case 'B':
        SystemController::GetPlatformController().PowerUpPeripheral(
            stm32f10x::SystemController::Peripherals::kGpioB);
        break;
      case 'C':
        SystemController::GetPlatformController().PowerUpPeripheral(
            stm32f10x::SystemController::Peripherals::kGpioC);
        break;
      case 'D':
        SystemController::GetPlatformController().PowerUpPeripheral(
            stm32f10x::SystemController::Peripherals::kGpioD);
        break;
      case 'E':
        SystemController::GetPlatformController().PowerUpPeripheral(
            stm32f10x::SystemController::Peripherals::kGpioE);
        break;
      case 'F':
        SystemController::GetPlatformController().PowerUpPeripheral(
            stm32f10x::SystemController::Peripherals::kGpioF);
        break;
      case 'G':
        SystemController::GetPlatformController().PowerUpPeripheral(
            stm32f10x::SystemController::Peripherals::kGpioG);
        break;
    }
  }

  /// Will not change the function of the pin but does change the
  /// pin to its alternative function mode, meaning it will no longer respond or
  /// operate based on the GPIO registers. Muxing pins to the correct function
  /// must be done by the individual driver as the STM32F10x alternative
  /// function registers do not follow any sort of consistent pattern.
  ///
  /// @param alternative_function - set to 0 for gpio mode and set to 1 for
  ///        alternative mode.
  void SetPinFunction(uint8_t alternative_function) const override
  {
    static constexpr auto kMode = bit::CreateMaskFromRange(0, 1);
    static constexpr auto kCFN1 = bit::CreateMaskFromRange(3);

    Initialize();

    uint32_t config = 0;
    // Set the alternative bit flag
    config = bit::Insert(config, alternative_function, kCFN1);
    // Set output speed to 50 MHz RM008 page 161 Table 21.
    config = bit::Insert(config, 0b11, kMode);

    SetConfig(config);
  }

  /// Should only be used for inputs. This method will change the pin's mode
  /// form out to input.
  void SetPull(Resistor resistor) const override
  {
    bool pull_up   = true;
    uint8_t config = 0;

    // Configuration for analog input mode. See Table 20 on page 161 on RM0008
    switch (resistor)
    {
      case Resistor::kNone: config = 0b0100; break;
      case Resistor::kPullDown: pull_up = false; [[fallthrough]];
      case Resistor::kPullUp: config = 0b1000; break;
      default: sjsu::LogInfo("Invalid pull resistor for this pin"); return;
    }

    SetConfig(config);
    Port()->ODR = bit::Insert(Port()->ODR, pull_up, pin_);
  }

  /// This function MUST NOT be called for pins set as inputs.
  void SetAsOpenDrain(bool set_as_open_drain = true) const override
  {
    static constexpr auto kCFN0 = bit::CreateMaskFromRange(2);

    uint32_t config = GetConfig();

    config = bit::Insert(config, set_as_open_drain, kCFN0);

    SetConfig(config);
  }

  /// This function can only be used to set the pin as analog.
  void SetAsAnalogMode(bool = true) const override
  {
    // Configuration for analog input mode. See Table 20 on page 161 on RM0008
    SetConfig(0b0100);
  }

 private:
  /// Returns the a pointer the gpio port.
  GPIO_TypeDef * Port() const
  {
    return gpio[port_ - 'A'];
  }

  /// Returns a bit mask indicating where the config bits are in the config
  /// registers.
  bit::Mask Mask() const
  {
    return {
      .position = static_cast<uint32_t>((pin_ * 4) % 32),
      .width    = 4,
    };
  }

  /// Returns the configuration control register for the specific pin.
  /// Pins 0 - 7 are in CRL and Pins 8 - 15 are in CRH.
  volatile uint32_t * Config() const
  {
    volatile uint32_t * config = &Port()->CRL;

    if (pin_ > 7)
    {
      config = &Port()->CRH;
    }

    return config;
  }

  /// @return the 4 bits of this ports config.
  uint32_t GetConfig() const
  {
    return bit::Extract(*Config(), Mask());
  }

  /// Set this ports 4 bits configuration.
  void SetConfig(uint32_t value) const
  {
    *Config() = bit::Insert(*Config(), value, Mask());
  }

  friend class Gpio;
};
}  // namespace sjsu::stm32f10x
