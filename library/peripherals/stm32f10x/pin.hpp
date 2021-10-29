#pragma once

#include <array>

#include "platforms/targets/stm32f10x/stm32f10x.h"
#include "peripherals/pin.hpp"
#include "peripherals/stm32f10x/system_controller.hpp"
#include "utility/math/bit.hpp"
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

  /// Maps the CONFIG flags for each pin use case
  struct PinConfigMapping_t
  {
    /// Configuration bit 1
    uint8_t CNF1;
    /// Configuration bit 0
    uint8_t CNF0;
    /// Mode bits
    uint8_t MODE;
    /// Output data register
    uint8_t PxODR;
  };

  /// @param port - must be a capitol letter from 'A' to 'I'
  /// @param pin - must be between 0 to 15
  constexpr Pin(uint8_t port, uint8_t pin) : sjsu::Pin(port, pin) {}

  /// Will not change the function of the pin but does change the
  /// pin to its alternative function mode, meaning it will no longer respond or
  /// operate based on the GPIO registers. Muxing pins to the correct function
  /// must be done by the individual driver as the STM32F10x alternative
  /// function registers do not follow any sort of consistent pattern.
  ///
  /// Set settings.function to 0 for gpio mode and set to 1 for alternative
  /// mode.
  /// If settings.as_analog is set to true, all other fields are ignored and the
  /// pin is put into analog mode.
  void ModuleInitialize() override
  {
    if (settings.function > 2)
    {
      throw Exception(
          std::errc::invalid_argument,
          "Only functions 0 (meaning General Purpose Output) or 1 (meaning "
          "alternative function) or 2 (meaning General Purpose Floating Input) "
          "allowed!");
    }

    auto & system = SystemController::GetPlatformController();

    // Enable the usage of alternative pin configurations
    system.PowerUpPeripheral(stm32f10x::SystemController::Peripherals::kAFIO);

    switch (GetPort())
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

    static constexpr PinConfigMapping_t kPushPullGpioOutput = {
      .CNF1  = 0,
      .CNF0  = 0,
      .MODE  = 0b11,  // Default to high speed 50 MHz
      .PxODR = 0b0,   // Default to 0 LOW Voltage
    };

    static constexpr PinConfigMapping_t kOpenDrainGpioOutput = {
      .CNF1  = 0,
      .CNF0  = 1,
      .MODE  = 0b11,  // Default to high speed 50 MHz
      .PxODR = 0b0,   // Default to 0 LOW Voltage
    };

    static constexpr PinConfigMapping_t kPushPullAlternativeOutput = {
      .CNF1  = 1,
      .CNF0  = 0,
      .MODE  = 0b11,  // Default to high speed 50 MHz
      .PxODR = 0b0,   // Default to 0 LOW Voltage
    };

    static constexpr PinConfigMapping_t kOpenDrainAlternativeOutput = {
      .CNF1  = 1,
      .CNF0  = 1,
      .MODE  = 0b11,  // Default to high speed 50 MHz
      .PxODR = 0b0,   // Default to 0 LOW Voltage
    };

    static constexpr PinConfigMapping_t kInputAnalog = {
      .CNF1  = 0,
      .CNF0  = 0,
      .MODE  = 0b00,
      .PxODR = 0b0,  // Don't care
    };

    static constexpr PinConfigMapping_t kInputFloat = {
      .CNF1  = 0,
      .CNF0  = 1,
      .MODE  = 0b00,
      .PxODR = 0b0,  // Don't care
    };

    static constexpr PinConfigMapping_t kInputPullDown = {
      .CNF1  = 1,
      .CNF0  = 0,
      .MODE  = 0b00,
      .PxODR = 0b0,  // Pull Down
    };

    static constexpr PinConfigMapping_t kInputPullUp = {
      .CNF1  = 1,
      .CNF0  = 0,
      .MODE  = 0b00,
      .PxODR = 0b1,  // Pull Up
    };

    PinConfigMapping_t mapping;

    if (settings.as_analog)
    {
      mapping = kInputAnalog;
    }
    else if (settings.resistor == PinSettings_t::Resistor::kPullDown)
    {
      mapping = kInputPullDown;
    }
    else if (settings.resistor == PinSettings_t::Resistor::kPullUp)
    {
      mapping = kInputPullUp;
    }
    else if (settings.function == 0 && settings.open_drain == false)
    {
      mapping = kPushPullGpioOutput;
    }
    else if (settings.function == 0 && settings.open_drain == true)
    {
      mapping = kOpenDrainGpioOutput;
    }
    else if (settings.function == 1 && settings.open_drain == false)
    {
      mapping = kPushPullAlternativeOutput;
    }
    else if (settings.function == 1 && settings.open_drain == true)
    {
      mapping = kOpenDrainAlternativeOutput;
    }
    else if (settings.function == 2)
    {
      mapping = kInputFloat;
    }
    else
    {
      throw sjsu::Exception(
          std::errc::not_supported,
          "This pin configuration is not supported by the STM32F10x platform");
    }

    constexpr auto kCNF1 = bit::MaskFromRange(3);
    constexpr auto kCNF0 = bit::MaskFromRange(2);
    constexpr auto kMODE = bit::MaskFromRange(0, 1);

    uint32_t config = bit::Value(0)
                          .Insert(mapping.CNF1, kCNF1)
                          .Insert(mapping.CNF0, kCNF0)
                          .Insert(mapping.MODE, kMODE)
                          .To<uint32_t>();

    SetConfig(config);
    Port()->ODR = bit::Insert(Port()->ODR, mapping.PxODR, GetPin());
  }

 private:
  /// Returns the a pointer the gpio port.
  GPIO_TypeDef * Port() const
  {
    return gpio[GetPort() - 'A'];
  }

  /// Returns a bit mask indicating where the config bits are in the config
  /// registers.
  bit::Mask Mask() const
  {
    return {
      .position = static_cast<uint32_t>((GetPin() * 4) % 32),
      .width    = 4,
    };
  }

  /// Returns the configuration control register for the specific pin.
  /// Pins 0 - 7 are in CRL and Pins 8 - 15 are in CRH.
  volatile uint32_t * Config() const
  {
    volatile uint32_t * config = &Port()->CRL;

    if (GetPin() > 7)
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

template <int port, int pin_number>
inline Pin & GetPin()
{
  static_assert(
      ('A' <= port && port <= 'I') && (0 <= pin_number && pin_number <= 15),
      "\n\n"
      "SJSU-Dev2 Compile Time Error:\n"
      "    stm32f10x: Port must be between 'A' and 'I' and pin must be\n"
      "    between 0 and 15!\n"
      "\n");

  static Pin pin(port, pin_number);
  return pin;
}
}  // namespace sjsu::stm32f10x
