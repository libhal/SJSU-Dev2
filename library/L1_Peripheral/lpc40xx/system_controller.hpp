// system clock class set clock outputs frequency and selects clock source
#pragma once

#include <cstdint>

#include "project_config.hpp"

#include "L0_Platform/lpc40xx/LPC40xx.h"
#include "L1_Peripheral/system_controller.hpp"
#include "utility/bit.hpp"
#include "utility/build_info.hpp"
#include "utility/enum.hpp"
#include "utility/log.hpp"
#include "utility/macros.hpp"
#include "utility/time.hpp"

namespace sjsu
{
namespace lpc40xx
{
/// System controller for the LPC40xx series of MCUs.
class SystemController final : public sjsu::SystemController
{
 public:
  /// System input oscillator source select contants
  enum class OscillatorSource : uint16_t
  {
    /// Internal RC oscillator as main. Not very precise oscillator cannot be
    /// used for USB or high speed CAN.
    kIrc = 0b0,
    /// Use external oscillator (typically a crystal). See PllInput for options
    /// that can be used for the oscillator.
    kMain = 0b1,
  };

  /// Main system clock source
  enum class MainClockSource : uint16_t
  {
    /// Use IRC clock directly, without using a PLL
    kBaseClock = 0b0,
    /// Use PLL Clock as clock source for the system clock
    kPllClock = 0b1,
  };

  /// USB oscillator source contants (not used)
  enum class UsbSource : uint16_t
  {
    /// Use IRC clock directly
    kBaseClock = 0b00,
    /// Use main PLL as the clock source
    kMainPllClock = 0b01,
    /// Use alternative PLL as the clock source
    kAlternatePllClock = 0b10,
  };

  /// USB Clock divider constants
  enum class UsbDivider : uint8_t
  {
    kDivideBy1 = 0,
    kDivideBy2,
    kDivideBy3,
    kDivideBy4,
  };

  /// SPIFI oscillator source contants (not used)
  enum class SpifiSource : uint16_t
  {
    /// Use IRC clock directly
    kBaseClock = 0b00,
    /// Use main PLL as the clock source
    kMainPllClock = 0b01,
    /// Use alternative PLL as the clock source
    kAlternatePllClock = 0b10,
  };

  /// EMC storage clock speed constants
  enum class EmcDivider : uint8_t
  {
    /// Same speed as CPU
    kSameSpeedAsCpu = 0,
    /// Cut speed in half
    kHalfTheSpeedOfCpu = 1,
  };

  /// Speed of possible clock frequencies that can be put into the PLLs
  struct PllInput  // NOLINT
  {
    /// Speed of the Internal RC oscillator
    static constexpr units::frequency::megahertz_t kIrc = 12_MHz;
    /// Allowable speed from an external clock source/crystal oscillator
    static constexpr units::frequency::megahertz_t kF12MHz = 12_MHz;
    /// Allowable speed from an external clock source/crystal oscillator
    static constexpr units::frequency::megahertz_t kF16MHz = 16_MHz;
    /// Allowable speed from an external clock source/crystal oscillator
    static constexpr units::frequency::megahertz_t kF24MHz = 24_MHz;
  };

  /// LPC40xx Peripheral Power On Values:
  /// The kDeviceId of each peripheral corresponds to the peripheral's power on
  /// bit position within the LPC40xx System Controller's PCONP register.
  class Peripherals
  {
   public:
    //! @cond Doxygen_Suppress
    static constexpr auto kLcd               = AddPeripheralID<0>();
    static constexpr auto kTimer0            = AddPeripheralID<1>();
    static constexpr auto kTimer1            = AddPeripheralID<2>();
    static constexpr auto kUart0             = AddPeripheralID<3>();
    static constexpr auto kUart1             = AddPeripheralID<4>();
    static constexpr auto kPwm0              = AddPeripheralID<5>();
    static constexpr auto kPwm1              = AddPeripheralID<6>();
    static constexpr auto kI2c0              = AddPeripheralID<7>();
    static constexpr auto kUart4             = AddPeripheralID<8>();
    static constexpr auto kRtc               = AddPeripheralID<9>();
    static constexpr auto kSsp1              = AddPeripheralID<10>();
    static constexpr auto kEmc               = AddPeripheralID<11>();
    static constexpr auto kAdc               = AddPeripheralID<12>();
    static constexpr auto kCan1              = AddPeripheralID<13>();
    static constexpr auto kCan2              = AddPeripheralID<14>();
    static constexpr auto kGpio              = AddPeripheralID<15>();
    static constexpr auto kSpifi             = AddPeripheralID<16>();
    static constexpr auto kMotorControlPwm   = AddPeripheralID<17>();
    static constexpr auto kQuadratureEncoder = AddPeripheralID<18>();
    static constexpr auto kI2c1              = AddPeripheralID<19>();
    static constexpr auto kSsp2              = AddPeripheralID<20>();
    static constexpr auto kSsp0              = AddPeripheralID<21>();
    static constexpr auto kTimer2            = AddPeripheralID<22>();
    static constexpr auto kTimer3            = AddPeripheralID<23>();
    static constexpr auto kUart2             = AddPeripheralID<24>();
    static constexpr auto kUart3             = AddPeripheralID<25>();
    static constexpr auto kI2c2              = AddPeripheralID<26>();
    static constexpr auto kI2s               = AddPeripheralID<27>();
    static constexpr auto kSdCard            = AddPeripheralID<28>();
    static constexpr auto kGpdma             = AddPeripheralID<29>();
    static constexpr auto kEthernet          = AddPeripheralID<30>();
    static constexpr auto kUsb               = AddPeripheralID<31>();
    //! @endcond
  };

  /// Namespace for PLL configuration bit masks
  struct Pll  // NOLINT
  {
    /// In PLLCON register: When 1, and after a valid PLL feed, this bit
    /// will activate the related PLL and allow it to lock to the requested
    /// frequency.
    static constexpr bit::Mask kEnable = bit::CreateMaskFromRange(0);
    /// In PLLCFG register: PLL multiplier value, the amount to multiply the
    /// input frequency by.
    static constexpr bit::Mask kMultiplier = bit::CreateMaskFromRange(0, 4);
    /// In PLLCFG register: PLL divider value, the amount to divide the output
    /// of the multiplier stage to bring the frequency down to a
    /// reasonable/usable level.
    static constexpr bit::Mask kDivider = bit::CreateMaskFromRange(5, 6);
    /// In PLLSTAT register: if set to 1 by hardware, the PLL has accepted
    /// the configuration and is locked.
    static constexpr bit::Mask kPllLockStatus = bit::CreateMaskFromRange(10);
  };

  /// Namespace of Oscillator register bitmasks
  struct Oscillator  // NOLINT
  {
    /// IRC or Main oscillator select bit
    static constexpr bit::Mask kSelect = bit::CreateMaskFromRange(0);
  };

  /// Namespace of Clock register bitmasks
  struct CpuClock  // NOLINT
  {
    /// CPU clock divider amount
    static constexpr bit::Mask kDivider = bit::CreateMaskFromRange(0, 4);
    /// CPU clock source select bit
    static constexpr bit::Mask kSelect = bit::CreateMaskFromRange(8);
  };

  /// Namespace of Peripheral register bitmasks
  struct PeripheralClock  // NOLINT
  {
    /// Main single peripheral clock divider shared across all peripherals,
    /// except for USB and SPIFI.
    static constexpr bit::Mask kDivider = bit::CreateMaskFromRange(0, 4);
  };

  /// Namespace of EMC register bitmasks
  struct EmcClock  // NOLINT
  {
    /// EMC Clock divider bit
    static constexpr bit::Mask kDivider = bit::CreateMaskFromRange(0);
  };

  /// Namespace of USB register bitmasks
  struct UsbClock  // NOLINT
  {
    /// USB clock divider constant
    static constexpr bit::Mask kDivider = bit::CreateMaskFromRange(0, 4);
    /// USB clock source select bit
    static constexpr bit::Mask kSelect = bit::CreateMaskFromRange(8, 9);
  };

  /// Namespace of SPIFI register bitmasks
  struct SpiFiClock  // NOLINT
  {
    /// SPIFI clock divider constant
    static constexpr bit::Mask kDivider = bit::CreateMaskFromRange(0, 4);
    /// SPIFI clock source select bit
    static constexpr bit::Mask kSelect = bit::CreateMaskFromRange(8, 9);
  };

  /// Pointer to the system controller peripheral in memory.
  inline static LPC_SC_TypeDef * system_controller = LPC_SC;

  /// Internal RC oscillator fixed frequency
  static constexpr units::frequency::megahertz_t kDefaultIRCFrequency = 12_MHz;

  void SetSystemClockFrequency(
      units::frequency::megahertz_t frequency) const override
  {
    SelectOscillatorSource(OscillatorSource::kIrc);
    if (frequency > 12_MHz)
    {
      SetMainPll(PllInput::kIrc, frequency);
      SelectMainClockSource(MainClockSource::kPllClock);
      speed_in_hertz = frequency;
    }
    else
    {
      SelectMainClockSource(MainClockSource::kBaseClock);
      speed_in_hertz = kDefaultIRCFrequency;
    }
    SetCpuClockDivider(1);
    SetPeripheralClockDivider({}, 1);
    SetEmcClockDivider(EmcDivider::kSameSpeedAsCpu);
  }

  void SetPeripheralClockDivider(const PeripheralID &,
                                 uint8_t peripheral_divider) const override
  {
    SJ2_ASSERT_FATAL(peripheral_divider <= 4, "Divider mustn't exceed 32");
    system_controller->PCLKSEL = peripheral_divider;
  }

  uint32_t GetPeripheralClockDivider(const PeripheralID &) const override
  {
    return system_controller->PCLKSEL;
  }

  units::frequency::hertz_t GetSystemFrequency() const override
  {
    return speed_in_hertz;
  }

  bool IsPeripheralPoweredUp(
      const PeripheralID & peripheral_select) const override
  {
    bool peripheral_is_powered_on =
        system_controller->PCONP & (1 << peripheral_select.device_id);

    return peripheral_is_powered_on;
  }

  void PowerUpPeripheral(const PeripheralID & peripheral_select) const override
  {
    system_controller->PCONP =
        bit::Set(system_controller->PCONP, peripheral_select.device_id);
  }

  void PowerDownPeripheral(
      const PeripheralID & peripheral_select) const override
  {
    system_controller->PCONP =
        bit::Clear(system_controller->PCONP, peripheral_select.device_id);
  }

 private:
  void SelectOscillatorSource(OscillatorSource source) const
  {
    system_controller->CLKSRCSEL =
        bit::Insert(system_controller->CLKSRCSEL, static_cast<uint32_t>(source),
                    Oscillator::kSelect);
  }

  void SelectMainClockSource(MainClockSource source) const
  {
    system_controller->CCLKSEL =
        bit::Insert(system_controller->CCLKSEL, static_cast<uint32_t>(source),
                    CpuClock::kSelect);
  }

  void SelectUsbClockSource(UsbSource usb_clock) const
  {
    system_controller->USBCLKSEL =
        bit::Insert(system_controller->USBCLKSEL,
                    static_cast<uint32_t>(usb_clock), UsbClock::kSelect);
  }

  void SelectSpifiClockSource(SpifiSource spifi_clock) const
  {
    system_controller->SPIFISEL =
        bit::Insert(system_controller->SPIFISEL,
                    static_cast<uint32_t>(spifi_clock), SpiFiClock::kSelect);
  }

  uint32_t CalculatePll(units::frequency::megahertz_t input_frequency,
                        units::frequency::megahertz_t desired_frequency) const
  {
    SJ2_ASSERT_FATAL(desired_frequency < 384_MHz && desired_frequency > 12_MHz,
                     "Frequency must be lower than 384 MHz"
                     "and greater than or equal to 12 MHz");
    bool calculating = true;
    uint32_t multiplier_value;
    if ((desired_frequency.to<uint32_t>() % input_frequency.to<uint32_t>()) > 0)
    {
      multiplier_value = (desired_frequency / input_frequency) + 1;
    }
    else
    {
      multiplier_value = desired_frequency / input_frequency;
    }

    uint16_t divider_value = 1;

    while (calculating)
    {
      uint16_t current_controlled_oscillator_frequency;
      current_controlled_oscillator_frequency = static_cast<uint16_t>(
          (static_cast<uint16_t>(input_frequency) * multiplier_value * 2) *
          divider_value);
      if (current_controlled_oscillator_frequency >= 156)
      {
        calculating = false;
      }
      else
      {
        divider_value = static_cast<uint16_t>(divider_value * 2);
        SJ2_ASSERT_FATAL(divider_value < 8,
                         "PLL divider value went out of bounds");
      }
    }

    return multiplier_value;
  }

  void SetMainPll(units::frequency::megahertz_t input_frequency,
                  units::frequency::megahertz_t desired_frequency) const
  {
    uint16_t divider_value = 1;
    uint32_t multiplier_value =
        CalculatePll(input_frequency, desired_frequency);
    // units::frequency::megahertz_t actual_speed =
    //     input_frequency * multiplier_value;
    // TO DO: use registers to retreive values
    SelectOscillatorSource(OscillatorSource::kIrc);
    SelectMainClockSource(MainClockSource::kBaseClock);
    SelectUsbClockSource(UsbSource::kBaseClock);
    SelectSpifiClockSource(SpifiSource::kBaseClock);

    uint32_t config = 0;
    // must subtract 1 from multiplier value as specified in datasheet
    config = bit::Insert(config, multiplier_value - 1, Pll::kMultiplier);
    config = bit::Insert(config, divider_value, Pll::kDivider);
    // Set the PLL multiply and divide values
    system_controller->PLL0CFG = config;

    // Enable PLL
    system_controller->PLL0CON =
        bit::Set(system_controller->PLL0CON, Pll::kEnable);

    // Necessary feed sequence to ensure the changes are intentional
    system_controller->PLL0FEED = 0xAA;
    system_controller->PLL0FEED = 0x55;

    while (!bit::Read(system_controller->PLL0STAT, Pll::kPllLockStatus))
    {
      continue;
    }
  }

  void SetAlternatePll(units::frequency::megahertz_t input_frequency,
                       units::frequency::megahertz_t desired_frequency) const
  {
    uint16_t divider_value = 1;
    uint32_t multiplier_value =
        CalculatePll(input_frequency, desired_frequency);
    SelectUsbClockSource(UsbSource::kBaseClock);
    SelectSpifiClockSource(SpifiSource::kBaseClock);

    uint32_t config = 0;
    // must subtract 1 from multiplier value as specified in datasheet
    config = bit::Insert(config, multiplier_value - 1, Pll::kMultiplier);
    config = bit::Insert(config, divider_value, Pll::kDivider);
    // Set the PLL multiply and divide values
    system_controller->PLL1CFG = config;

    // Enable PLL
    system_controller->PLL1CON =
        bit::Set(system_controller->PLL1CON, Pll::kEnable);

    // Necessary feed sequence to ensure the changes are intentional
    system_controller->PLL1FEED = 0xAA;
    system_controller->PLL1FEED = 0x55;

    while (!bit::Read(system_controller->PLL1STAT, Pll::kPllLockStatus))
    {
      continue;
    }
  }

  void SetCpuClockDivider(uint8_t cpu_divider) const
  {
    SJ2_ASSERT_FATAL(cpu_divider < 32, "Divider mustn't exceed 32");

    system_controller->CCLKSEL = bit::Insert(system_controller->CCLKSEL,
                                             cpu_divider, CpuClock::kDivider);
  }

  void SetEmcClockDivider(EmcDivider emc_divider) const
  {
    system_controller->EMCCLKSEL =
        bit::Insert(system_controller->EMCCLKSEL,
                    static_cast<uint32_t>(emc_divider), EmcClock::kDivider);
  }
  // TODO(#181): Set USB and Spifi clock rates
  inline static units::frequency::hertz_t speed_in_hertz = kDefaultIRCFrequency;
};
}  // namespace lpc40xx
}  // namespace sjsu
