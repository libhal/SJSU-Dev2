#pragma once

#include <array>

#include "L0_Platform/msp432p401r/msp432p401r.h"
#include "L1_Peripheral/pin.hpp"
#include "L1_Peripheral/gpio.hpp"
#include "utility/bit.hpp"
#include "utility/log.hpp"

namespace sjsu
{
namespace msp432p401r
{
/// Pin control driver for MSP432P401R MCUs.
class Pin final : public sjsu::Pin
{
 public:
  /// The available driver strength for high driver strength I/Os.
  enum class DriveStrength : uint8_t
  {
    /// Regular drive strength.
    kRegular,
    /// High drive strength.
    kHigh,
  };

  /// Array containing the port structures that map P1 to P10. Each port
  /// structure contains an odd and even port. For example, PA contains P1 and
  /// P2, PB contains P3 and P4, and so on. The only exception is Port J which
  /// is its own port and only has pins 1-5.
  inline static std::array<DIO_PORT_Interruptable_Type *, 6> ports = {
    PA,  // P1 and P2
    PB,  // P3 and P4
    PC,  // P5 and P6
    PD,  // P7 and P8
    PE,  // P9 and P10
    reinterpret_cast<DIO_PORT_Interruptable_Type *>(PJ)
  };

  /// @param port The port number. The capitol letter 'J' should be used is the
  ///             desired port is port J.
  /// @param pin  The pin number.
  constexpr Pin(uint8_t port, uint8_t pin) : sjsu::Pin(port, pin) {}

  /// @note GPIO hardare is enabled and ready by default on reset.
  void ModuleInitialize() override
  {
    // NOTE: port can only be 1-10 or 'J'
    if (((port_ > 10) || (port_ == 0)) && (port_ != 'J'))
    {
      throw Exception(std::errc::invalid_argument, "Port must be 1-10 or J");
    }
    if (pin_ > 7)
    {
      throw Exception(std::errc::invalid_argument,
                      "Pin must be between 0 and 7");
    }
  }

  void ModuleEnable(bool = true) override {}

  /// Configures the pin's function mode based on the specified 3-bit function
  /// code. Where the most significant bit determines the pin direction.
  ///
  /// @see Port Function Tables in 6.12 Input/Output Diagrams
  ///      http://www.ti.com/lit/ds/symlink/msp432p401r.pdf#page=138
  ///
  /// @param function The 3-bit function code.
  void ConfigureFunction(uint8_t function) override
  {
    if (function > 0b111)
    {
      throw Exception(
          std::errc::invalid_argument,
          "The function code must be a 3-bit value between 0b000 and 0b111.");
    }

    constexpr auto kDirectionBit = bit::MaskFromRange(2);
    constexpr auto kSel1Bit      = bit::MaskFromRange(1);
    constexpr auto kSel0Bit      = bit::MaskFromRange(0);

    volatile uint8_t * select1_register = RegisterAddress(&Port()->SEL1);
    volatile uint8_t * select0_register = RegisterAddress(&Port()->SEL0);
    *select1_register                   = static_cast<uint8_t>(
        bit::Insert(static_cast<uint32_t>(*select1_register),
                    bit::Read(function, kSel1Bit), pin_));
    *select0_register = static_cast<uint8_t>(
        bit::Insert(static_cast<uint32_t>(*select0_register),
                    bit::Read(function, kSel0Bit), pin_));

    SetPinDirection(Gpio::Direction(bit::Read(function, kDirectionBit)));
  }

  /// Sets the pin's resistor pull either as pull up, pull down, or none.
  ///
  /// @see 12.2.4 Pullup or Pulldown Resistor Enable Registers (PxREN)
  ///      https://www.ti.com/lit/ug/slau356i/slau356i.pdf#page=678
  ///
  /// @param resistor The resistor to set.
  void ConfigurePullResistor(Resistor resistor) override
  {
    volatile uint8_t * resistor_enable = RegisterAddress(&Port()->REN);
    // The output register (OUT) is used to select the pull down or pull up
    // resistor when resistor is enabled.
    volatile uint8_t * resistor_select = RegisterAddress(&Port()->OUT);

    switch (resistor)
    {
      case Resistor::kNone:
        *resistor_enable = bit::Clear(*resistor_enable, pin_);
        break;
      case Resistor::kPullDown:
        *resistor_enable = bit::Set(*resistor_enable, pin_);
        *resistor_select = bit::Clear(*resistor_select, pin_);
        break;
      case Resistor::kPullUp:
        *resistor_enable = bit::Set(*resistor_enable, pin_);
        *resistor_select = bit::Set(*resistor_select, pin_);
        break;
      case Resistor::kRepeater:
        throw Exception(std::errc::not_supported,
                        "Repeater mode not supported");
    }
  }

  /// Sets the drive strength of the pin.
  ///
  /// @param drive_strength The drive strength to set.
  void SetDriveStrength(DriveStrength drive_strength)
  {
    volatile uint8_t * drive_strength_register = RegisterAddress(&Port()->DS);

    if (drive_strength == DriveStrength::kHigh)
    {
      *drive_strength_register = bit::Set(*drive_strength_register, pin_);
    }
    else
    {
      *drive_strength_register = bit::Clear(*drive_strength_register, pin_);
    }
  }

  void ConfigureAsOpenDrain(bool) override
  {
    throw Exception(std::errc::operation_not_supported, "");
  }

  /// @note This function does nothing as setting the analog mode is not
  ///       required to enable the use of the ADC.
  [[deprecated("Unsupported operation")]] void ConfigureAsAnalogMode(
      bool) override
  {
    throw Exception(std::errc::operation_not_supported, "");
  }

 private:
  /// Sets the pin direction as output or input.
  ///
  /// @note The pin direction can determine the function of the pin when the
  ///       pin's function is not in general purpose I/O mode.
  ///
  /// @param direction The direction to set.
  void SetPinDirection(sjsu::Gpio::Direction direction) const
  {
    volatile uint8_t * direction_register = RegisterAddress(&Port()->DIR);
    if (direction == Gpio::Direction::kOutput)
    {
      *direction_register = bit::Set(*direction_register, pin_);
    }
    else
    {
      *direction_register = bit::Clear(*direction_register, pin_);
    }
  }

  /// Helper function to calculate the register address depending on whether the
  /// port number is odd or even. For example, PA->DIR consist of PA->DIR_L
  /// (used for P1) and PA->DIR_H (used for P2).
  ///
  /// @param address The address of the 16-bit register that holds the odd and
  ///                even 8-bit registers.
  /// @returns The address of a desired 8-bit register.
  volatile uint8_t * RegisterAddress(volatile uint16_t * address) const
  {
    // If the port number is odd or is 'J', use the low register. Otherwise. if
    // the port number is even, use the high register.
    uint32_t offset = 1;
    if (static_cast<bool>(port_ & 0b1) || (port_ == 'J'))
    {
      offset = 0;
    }
    return reinterpret_cast<volatile uint8_t *>(address) + offset;
  }

  /// @returns The reference to the port structure base on the port number.
  DIO_PORT_Interruptable_Type * Port() const
  {
    if (port_ == 'J')
    {
      return ports[5];
    }
    else if (!static_cast<bool>(port_ & 0b1))  // Even port number.
    {
      return ports[(port_ - 1) / 2];
    }
    // Odd port number
    return ports[(port_ / 2)];
  }

  friend class Gpio;
};
}  // namespace msp432p401r
}  // namespace sjsu
