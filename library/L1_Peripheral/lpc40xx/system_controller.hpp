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
class SystemController final : public sjsu::SystemController
{
 public:
  enum class UsbSource : uint16_t
  {
    kBaseClock         = 0b00,
    kMainPllClock      = 0b01,
    kALternatePllClock = 0b10,
  };

  enum class SpifiSource : uint16_t
  {
    kBaseClock         = 0b00,
    kMainPllClock      = 0b01,
    kALternatePllClock = 0b10,
  };

  enum class OscillatorSource : uint16_t
  {
    kIrc  = 0b0,
    kMain = 0b1,
  };

  enum class MainClockSource : uint16_t
  {
    kBaseClock = 0b0,
    kPllClock  = 0b1,
  };

  struct PllInput  // NOLINT
  {
    static constexpr units::frequency::megahertz_t kIrc    = 12_MHz;
    static constexpr units::frequency::megahertz_t kF12MHz = 12_MHz;
    static constexpr units::frequency::megahertz_t kF16MHz = 16_MHz;
    static constexpr units::frequency::megahertz_t kF24MHz = 24_MHz;
  };

  enum class EmcDivider : uint8_t
  {
    kSameSpeedAsCpu    = 0,
    kHalfTheSpeedOfCpu = 1,
  };

  enum class UsbDivider : uint8_t
  {
    kDivideBy1 = 0,
    kDivideBy2,
    kDivideBy3,
    kDivideBy4,
  };

  // LPC40xx Peripheral Power On Values:
  // The kDeviceId of each peripheral corresponds to the peripheral's power on
  // bit position within the LPC40xx System Controller's PCONP register.
  class Peripherals
  {
   public:
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
  };

  static constexpr uint32_t kEnablePll              = (0b1 << 0);
  static constexpr uint32_t kPlock                  = 10;
  static constexpr uint32_t kClearPllMultiplier     = (0x1f << 0);
  static constexpr uint32_t kClearPllDivider        = (0b11 << 5);
  static constexpr uint32_t kClearPeripheralDivider = (0b1'1111);
  static constexpr units::frequency::megahertz_t kDefaultIRCFrequency = 12_MHz;
  static constexpr std::chrono::milliseconds kDefaultTimeout          = 1s;

  struct Oscillator  // NOLINT
  {
    static constexpr bit::Mask kSelect = bit::CreateMaskFromRange(0);
  };

  struct EmcClock  // NOLINT
  {
    static constexpr bit::Mask kDivider = bit::CreateMaskFromRange(0);
  };

  struct CpuClock  // NOLINT
  {
    static constexpr bit::Mask kDivider = bit::CreateMaskFromRange(0, 4);
    static constexpr bit::Mask kSelect  = bit::CreateMaskFromRange(8);
  };

  struct UsbClock  // NOLINT
  {
    static constexpr bit::Mask kDivider = bit::CreateMaskFromRange(0, 4);
    static constexpr bit::Mask kSelect  = bit::CreateMaskFromRange(8, 9);
  };

  struct PeripheralClock  // NOLINT
  {
    static constexpr bit::Mask kDivider = bit::CreateMaskFromRange(0, 4);
  };

  struct SpiFiClock  // NOLINT
  {
    static constexpr bit::Mask kDivider = bit::CreateMaskFromRange(0, 4);
    static constexpr bit::Mask kSelect  = bit::CreateMaskFromRange(8, 9);
  };

  inline static LPC_SC_TypeDef * system_controller = LPC_SC;

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

  /// Check if a peripheral is powered up by checking the power connection
  /// register. Should typically only be used for unit testing code and
  /// debugging.
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
    system_controller->CLKSRCSEL = bit::Insert(system_controller->CLKSRCSEL,
                                               static_cast<uint32_t>(source),
                                               Oscillator::kSelect);
  }
  void SelectMainClockSource(MainClockSource source) const
  {
    system_controller->CCLKSEL = bit::Insert(system_controller->CCLKSEL,
                                             static_cast<uint32_t>(source),
                                             CpuClock::kSelect);
  }
  void SelectUsbClockSource(UsbSource usb_clock) const
  {
    system_controller->USBCLKSEL = bit::Insert(system_controller->USBCLKSEL,
                                               static_cast<uint32_t>(usb_clock),
                                               UsbClock::kSelect);
  }
  void SelectSpifiClockSource(SpifiSource spifi_clock) const
  {
    system_controller->SPIFISEL =
        bit::Insert(system_controller->SPIFISEL,
                    static_cast<uint32_t>(spifi_clock),
                    SpiFiClock::kSelect);
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
      multiplier_value =
          static_cast<uint32_t>((desired_frequency / input_frequency) + 1);
    }
    else
    {
      multiplier_value =
          static_cast<uint32_t>(desired_frequency / input_frequency);
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
    // must subtract 1 from multiplier value as specified in datasheet
    system_controller->PLL0CFG =
        (system_controller->PLL0CFG & ~kClearPllMultiplier) |
        (multiplier_value - 1);
    system_controller->PLL0CFG =
        (system_controller->PLL0CFG & ~kClearPllDivider) | (divider_value << 5);
    system_controller->PLL0CON |= kEnablePll;
    // Necessary feed sequence to ensure the changes are intentional
    system_controller->PLL0FEED = 0xAA;
    system_controller->PLL0FEED = 0x55;

    while (!bit::Read(system_controller->PLL0STAT, kPlock))
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
    // must subtract 1 from multiplier value as specified in datasheet
    system_controller->PLL1CFG =
        (system_controller->PLL1CFG & ~kClearPllMultiplier) |
        (multiplier_value - 1);
    system_controller->PLL1CFG =
        (system_controller->PLL1CFG & ~kClearPllDivider) | (divider_value << 5);
    system_controller->PLL1CON |= kEnablePll;
    // Necessary feed sequence to ensure the changes are intentional
    system_controller->PLL1FEED = 0xAA;
    system_controller->PLL1FEED = 0x55;

    while (!bit::Read(system_controller->PLL1STAT, kPlock))
    {
      continue;
    }
  }

  void SetCpuClockDivider(uint8_t cpu_divider) const
  {
    SJ2_ASSERT_FATAL(cpu_divider < 32, "Divider mustn't exceed 32");

    system_controller->CCLKSEL = bit::Insert(
        system_controller->CCLKSEL, cpu_divider, CpuClock::kDivider);
  }

  void SetEmcClockDivider(EmcDivider emc_divider) const
  {
    system_controller->EMCCLKSEL =
        bit::Insert(system_controller->EMCCLKSEL,
                    static_cast<uint32_t>(emc_divider),
                    EmcClock::kDivider);
  }
  // TODO(#181): Set USB and Spifi clock rates
  inline static units::frequency::hertz_t speed_in_hertz = kDefaultIRCFrequency;
};

inline const sjsu::lpc40xx::SystemController & DefaultSystemController()
{
  static sjsu::lpc40xx::SystemController default_system_controller =
      sjsu::lpc40xx::SystemController();
  return default_system_controller;
}

}  // namespace lpc40xx
}  // namespace sjsu
