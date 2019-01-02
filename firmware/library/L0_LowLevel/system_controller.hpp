// system clock class set clock outputs frequency and selects clock source
#pragma once

#include <cstdint>

#include "project_config.hpp"

#include "L0_LowLevel/LPC40xx.h"
#include "utility/enum.hpp"
#include "utility/log.hpp"
#include "utility/macros.hpp"

class Lpc40xxSystemController
{
 public:
  enum class UsbSource : uint16_t
  {
    kBaseClock         = (0b00 << 8),
    kMainPllClock      = (0b01 << 8),
    kALternatePllClock = (0b10 << 8)
  };

  enum class SpifiSource : uint16_t
  {
    kBaseClock         = (0b00 << 8),
    kMainPllClock      = (0b01 << 8),
    kALternatePllClock = (0b10 << 8)
  };

  enum class OscillatorSource : uint16_t
  {
    kIrc  = 0b0,
    kMain = 0b1
  };

  enum class MainClockSource : uint16_t
  {
    kBaseClock = (0b0 << 8),
    kPllClock  = (0b1 << 8)
  };

  enum class PllInput : uint16_t
  {
    kIrc    = 12,
    kF12MHz = 12,
    kF16MHz = 16,
    kF24MHz = 24
  };

  enum class EmcDivider : uint8_t
  {
    kSameSpeedAsCpu,
    kHalfTheSpeedOfCpu
  };

  enum class PeripheralPowerUp : uint8_t
  {
    kLcd = 0,
    kTimer0,
    kTimer1,
    kUart0,
    kUart1,
    kPwm0,
    kPwm1,
    kI2c0,
    kUart4,
    kRtc,
    kSsp1,
    kEmc,
    kAdc,
    kCan1,
    kCan2,
    kGpio,
    kSpifi,
    kMotorControlPwm,
    kQuadratureEncoderInterface,
    kI2c1,
    kSsp2,
    kSsp0,
    kTimer2,
    kTimer3,
    kUart2,
    kUart3,
    kI2c2,
    kI2s,
    kSdCard,
    kGpdma,
    kEthernetBlock,
    kUsb
  };

  enum class UsbDivider : uint8_t
  {
    kDivideBy1 = 0,
    kDivideBy2,
    kDivideBy3,
    kDivideBy4
  };

  static constexpr uint16_t kOscillatorSelect       = (1 << 0);
  static constexpr uint16_t kBaseClockSelect        = (1 << 8);
  static constexpr uint16_t kUsbClockSource         = (0b11 << 8);
  static constexpr uint16_t kSpifiClockSource       = (0b11 << 8);
  static constexpr uint16_t kEnablePll              = (0b1 << 0);
  static constexpr uint16_t kPlock                  = 10;
  static constexpr uint8_t kDivideInputBy1          = 1;
  static constexpr uint16_t kClearPllMultiplier     = (0x1f << 0);
  static constexpr uint16_t kClearPllDivider        = (0b11 << 5);
  static constexpr uint16_t kClearCpuDivider        = (0x1F << 0);
  static constexpr uint16_t kClearEmcDivider        = (1 << 0);
  static constexpr uint16_t kClearPeripheralDivider = (0b1'1111);
  static constexpr uint16_t kClearUsbDivider        = (0x1F << 0);
  static constexpr uint32_t kDefaultIRCFrequency    = 12'000'000;
  static constexpr uint32_t kDefaultTimeout         = 1'000;  // ms

  inline static LPC_SC_TypeDef * system_controller = LPC_SC;

  constexpr Lpc40xxSystemController() {}

  uint32_t SetClockFrequency(uint8_t frequency_in_mhz)
  {
    uint32_t offset = 0;
    SelectOscillatorSource(OscillatorSource::kIrc);
    if (frequency_in_mhz > 12)
    {
      offset = SetMainPll(PllInput::kIrc, frequency_in_mhz);
      SelectMainClockSource(MainClockSource::kPllClock);
      speed_in_hertz = frequency_in_mhz * 1'000'000;
    }
    else
    {
      SelectMainClockSource(MainClockSource::kBaseClock);
      speed_in_hertz = kDefaultIRCFrequency;
    }
    SetCpuClockDivider(kDivideInputBy1);
    SetPeripheralClockDivider(kDivideInputBy1);
    SetEmcClockDivider(EmcDivider::kSameSpeedAsCpu);
    return offset;
  }

  void SetPeripheralClockDivider(uint8_t peripheral_divider)
  {
    SJ2_ASSERT_FATAL(peripheral_divider <= 4, "Divider mustn't exceed 32");
    system_controller->PCLKSEL = peripheral_divider;
  }

  uint32_t GetPeripheralClockDivider() const
  {
    #if defined(HOST_TEST)
    return 1;
    #else
    return system_controller->PCLKSEL;
    #endif
  }

  uint32_t GetSystemFrequency()
  {
    #if defined(HOST_TEST)
    return config::kSystemClockRate;
    #else
    return speed_in_hertz;
    #endif
  }

  uint32_t GetPeripheralFrequency()
  {
    uint32_t peripheral_clock_divider = GetPeripheralClockDivider();
    uint32_t result = 0;  // return 0 if peripheral_clock_divider == 0
    if (peripheral_clock_divider != 0)
    {
      result = GetSystemFrequency() / peripheral_clock_divider;
    }
    return result;
  }

  void PowerUpPeripheral(PeripheralPowerUp peripheral_select)
  {
    auto power_connection_with_enabled_peripheral =
        system_controller->PCONP | (1 << util::Value(peripheral_select));

    system_controller->PCONP = power_connection_with_enabled_peripheral;
  }
  void PowerDownPeripheral(PeripheralPowerUp peripheral_select)
  {
    auto power_connection_without_enabled_peripheral =
        system_controller->PCONP & (1 << util::Value(peripheral_select));

    system_controller->PCONP = power_connection_without_enabled_peripheral;
  }

 private:
  void SelectOscillatorSource(OscillatorSource source)
  {
    system_controller->CLKSRCSEL =
        (system_controller->CLKSRCSEL & ~(kOscillatorSelect)) |
        static_cast<uint32_t>(source);
  }

  void SelectMainClockSource(MainClockSource source)
  {
    system_controller->CCLKSEL =
        (system_controller->CCLKSEL & ~(kBaseClockSelect)) |
        static_cast<uint32_t>(source);
  }

  void SelectUsbClockSource(UsbSource usb_clock)
  {
    system_controller->USBCLKSEL =
        (system_controller->USBCLKSEL & ~(kUsbClockSource)) |
        static_cast<uint32_t>(usb_clock);
  }

  void SelectSpifiClockSource(SpifiSource spifi_clock)
  {
    system_controller->SPIFISEL =
        (system_controller->SPIFISEL & ~(kSpifiClockSource)) |
        static_cast<uint32_t>(spifi_clock);
  }

  uint32_t CalculatePll(PllInput input_frequency, uint16_t desired_speed_in_mhz)
  {
    SJ2_ASSERT_FATAL(desired_speed_in_mhz < 384 && desired_speed_in_mhz > 12,
                     "Frequency must be lower than 384 MHz"
                     "and greater than or equal to 12 MHz");
    bool calculating = true;
    uint32_t multiplier_value;
    if ((desired_speed_in_mhz % static_cast<uint16_t>(input_frequency)) >= 1)
    {
      multiplier_value = static_cast<uint32_t>(
          (desired_speed_in_mhz / static_cast<uint16_t>(input_frequency)) + 1);
    }
    else
    {
      multiplier_value = static_cast<uint32_t>(
          desired_speed_in_mhz / static_cast<uint16_t>(input_frequency));
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

  uint32_t SetMainPll(PllInput input_frequency, uint16_t desired_speed_in_mhz)
  {
    uint16_t divider_value = 1;
    uint64_t timeout_time  = Milliseconds() + kDefaultTimeout;
    uint64_t current_time  = Milliseconds();
    uint32_t multiplier_value =
        CalculatePll(input_frequency, desired_speed_in_mhz);
    uint32_t actual_speed =
        static_cast<uint32_t>(input_frequency) * multiplier_value;
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
    // nessecary feed sequence to ensure the changes are intentional
    system_controller->PLL0FEED = 0xAA;
    system_controller->PLL0FEED = 0x55;
    while (!(system_controller->PLL0STAT >> kPlock & 1) &&
           (current_time < timeout_time))
    {
      current_time = Milliseconds();
    }
    if (!(system_controller->PLL0STAT >> kPlock & 1) &&
        (current_time >= timeout_time))
    {
      SJ2_ASSERT_FATAL(false,
                       "PLL lock could not be established before timeout");
      actual_speed = kDefaultIRCFrequency;
    }
    return (actual_speed - desired_speed_in_mhz);
  }

  uint32_t SetAlternatePll(PllInput input_frequency,
                           uint16_t desired_speed_in_mhz)
  {
    uint16_t divider_value = 1;
    uint64_t timeout_time  = Milliseconds() + kDefaultTimeout;
    uint64_t current_time  = Milliseconds();
    uint32_t multiplier_value =
        CalculatePll(input_frequency, desired_speed_in_mhz);
    uint32_t actual_speed =
        static_cast<uint32_t>(input_frequency) * multiplier_value;
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
    while (!(system_controller->PLL1STAT >> kPlock & 1) &&
           (current_time < timeout_time))
    {
      current_time = Milliseconds();
    }
    if (!(system_controller->PLL1STAT >> kPlock & 1) &&
        (current_time >= timeout_time))
    {
      SJ2_ASSERT_FATAL(false,
                       "PLL lock could not be established before timeout");
    }
    return (actual_speed - desired_speed_in_mhz);
  }

  void SetCpuClockDivider(uint8_t cpu_divider)
  {
    SJ2_ASSERT_FATAL(cpu_divider < 32, "Divider mustn't exceed 32");
    system_controller->CCLKSEL =
        (system_controller->CCLKSEL & ~kClearCpuDivider) | cpu_divider;
  }

  void SetEmcClockDivider(EmcDivider emc_divider)
  {
    system_controller->EMCCLKSEL =
        (system_controller->EMCCLKSEL & ~kClearEmcDivider) |
        static_cast<uint8_t>(emc_divider);
  }
  // TODO(Zaaji #181): Set USB and Spifi clock rates
  inline static uint32_t speed_in_hertz = kDefaultIRCFrequency;
};

// inline Lpc40xxSystemController system_clock;
