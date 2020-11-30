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
  /// Pointer to the alternative function I/O register
  static inline AFIO_TypeDef * afio = AFIO;

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

  /// The GPIO pins PB3, PB4, and PA15 are default initalized to be used for
  /// JTAG purposes. If you are using SWD and want to use these pins as GPIO or
  /// as other alternative functions, this function MUST be called.
  static void ReleaseJTAGPins()
  {
    auto & system = SystemController::GetPlatformController();

    // Enable the usage of alternative pin configurations
    system.PowerUpPeripheral(stm32f10x::SystemController::Peripherals::kAFIO);

    // Set the JTAG Release
    afio->MAPR =
        sjsu::bit::Insert(afio->MAPR, 0b010, sjsu::bit::MaskFromRange(24, 26));
  }

  /// @param port - must be a capitol letter from 'A' to 'I'
  /// @param pin - must be between 0 to 15
  constexpr Pin(uint8_t port, uint8_t pin) : sjsu::Pin(port, pin) {}

  void ModuleInitialize() override
  {
    bool port_is_valid = ('A' <= port_ && port_ <= 'I');
    if (!port_is_valid)
    {
      throw Exception(std::errc::invalid_argument,
                      "Port must be between 'A' and 'I'");
    }

    auto & system = SystemController::GetPlatformController();

    // Enable the usage of alternative pin configurations
    system.PowerUpPeripheral(stm32f10x::SystemController::Peripherals::kAFIO);

    switch (port_)
    {
      case 'A':
        system.PowerUpPeripheral(
            stm32f10x::SystemController::Peripherals::kGpioA);
        break;
      case 'B':
        system.PowerUpPeripheral(
            stm32f10x::SystemController::Peripherals::kGpioB);
        break;
      case 'C':
        system.PowerUpPeripheral(
            stm32f10x::SystemController::Peripherals::kGpioC);
        break;
      case 'D':
        system.PowerUpPeripheral(
            stm32f10x::SystemController::Peripherals::kGpioD);
        break;
      case 'E':
        system.PowerUpPeripheral(
            stm32f10x::SystemController::Peripherals::kGpioE);
        break;
      case 'F':
        system.PowerUpPeripheral(
            stm32f10x::SystemController::Peripherals::kGpioF);
        break;
      case 'G':
        system.PowerUpPeripheral(
            stm32f10x::SystemController::Peripherals::kGpioG);
        break;
    }
  }

  /// Does nothing
  void ModuleEnable(bool = true) override {}

  /// Will not change the function of the pin but does change the
  /// pin to its alternative function mode, meaning it will no longer respond or
  /// operate based on the GPIO registers. Muxing pins to the correct function
  /// must be done by the individual driver as the STM32F10x alternative
  /// function registers do not follow any sort of consistent pattern.
  ///
  /// @param alternative_function - set to 0 for gpio mode and set to 1 for
  ///        alternative mode.
  void ConfigureFunction(uint8_t alternative_function) override
  {
    static constexpr auto kMode = bit::MaskFromRange(0, 1);
    static constexpr auto kCFN1 = bit::MaskFromRange(3);

    Initialize();

    if (alternative_function > 0b1)
    {
      throw Exception(std::errc::invalid_argument, "Invalid function.");
    }

    uint32_t config = 0;
    // Set the alternative bit flag
    config = bit::Insert(config, alternative_function, kCFN1);
    // Set output speed to 50 MHz RM008 page 161 Table 21.
    config = bit::Insert(config, 0b11, kMode);

    SetConfig(config);
  }

  /// Should only be used for inputs. This method will change the pin's mode
  /// form out to input.
  void ConfigurePullResistor(Resistor resistor) override
  {
    bool pull_up   = true;
    uint8_t config = 0;

    // Configuration for analog input mode. See Table 20 on page 161 on RM0008
    switch (resistor)
    {
      case Resistor::kNone: config = 0b0100; break;
      case Resistor::kPullDown: pull_up = false; [[fallthrough]];
      case Resistor::kPullUp: config = 0b1000; break;
      case Resistor::kRepeater:
      {
        throw Exception(std::errc::not_supported, "Repeater not supported");
      }
    }

    SetConfig(config);
    Port()->ODR = bit::Insert(Port()->ODR, pull_up, pin_);
  }

  /// This function MUST NOT be called for pins set as inputs.
  void ConfigureAsOpenDrain(bool set_as_open_drain = true) override
  {
    static constexpr auto kCFN0 = bit::MaskFromRange(2);

    uint32_t config = GetConfig();

    config = bit::Insert(config, set_as_open_drain, kCFN0);

    SetConfig(config);
  }

  /// This function can only be used to set the pin as analog.
  void ConfigureAsAnalogMode(bool = true) override
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
