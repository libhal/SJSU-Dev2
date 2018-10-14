// system clock class set clock outputs frequency and selects clock source
#pragma once
#include <cstdint>
#include "L0_LowLevel/delay.hpp"
#include "L0_LowLevel/LPC40xx.h"
#include "L2_Utilities/macros.hpp"

class SystemClockInterface
{
 public:
  static constexpr uint64_t kDefaultTimeout = 1000;  // units in milliseconds
  virtual void SetClockFrequency(uint8_t frequency_in_mhz) = 0;
  virtual uint32_t GetClockFrequency()                     = 0;
};

class SystemClock : public SystemClockInterface
{
 private:
  uint32_t speed_in_hertz_;

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
  static constexpr uint16_t kClearPeripheralDivider = (0x1F << 0);
  static constexpr uint16_t kClearUsbDivider        = (0x1F << 0);

  SystemClock()
  {
    speed_in_hertz_ = 12000000;  // default 12 MHz from IRC
  }

  void SelectOscillatorSource(OscillatorSource source)
  {
    LPC_SC->CLKSRCSEL = (LPC_SC->CLKSRCSEL & ~(kOscillatorSelect)) |
                        static_cast<uint32_t>(source);
  }

  void SelectMainClockSource(MainClockSource source)
  {
    LPC_SC->CCLKSEL =
        (LPC_SC->CCLKSEL & ~(kBaseClockSelect)) | static_cast<uint32_t>(source);
  }

  void SelectUsbClockSource(UsbSource usb_clock)
  {
    LPC_SC->USBCLKSEL = (LPC_SC->USBCLKSEL & ~(kUsbClockSource)) |
                        static_cast<uint32_t>(usb_clock);
  }

  void SelectSpifiClockSource(SpifiSource spifi_clock)
  {
    LPC_SC->SPIFISEL = (LPC_SC->SPIFISEL & ~(kSpifiClockSource)) |
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
    LPC_SC->PLL0CFG =
        (LPC_SC->PLL0CFG & ~kClearPllMultiplier) | (multiplier_value - 1);
    LPC_SC->PLL0CFG =
        (LPC_SC->PLL0CFG & ~kClearPllDivider) | (divider_value << 5);
    LPC_SC->PLL0CON |= kEnablePll;
    // nessecary feed sequence to ensure the changes are intentional
    LPC_SC->PLL0FEED = 0xAA;
    LPC_SC->PLL0FEED = 0x55;
    while (!(LPC_SC->PLL0STAT >> kPlock & 1) && (current_time < timeout_time))
    {
      current_time = Milliseconds();
    }
    if (!(LPC_SC->PLL0STAT >> kPlock & 1) && (current_time >= timeout_time))
    {
      SJ2_ASSERT_FATAL(false,
                       "PLL lock could not be established before timeout");
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
    LPC_SC->PLL1CFG =
        (LPC_SC->PLL1CFG & ~kClearPllMultiplier) | (multiplier_value - 1);
    LPC_SC->PLL1CFG =
        (LPC_SC->PLL1CFG & ~kClearPllDivider) | (divider_value << 5);
    LPC_SC->PLL1CON |= kEnablePll;
    // nessecary feed sequence to ensure the changes are intentional
    LPC_SC->PLL1FEED = 0xAA;
    LPC_SC->PLL1FEED = 0x55;
    while (!(LPC_SC->PLL1STAT >> kPlock & 1) && (current_time < timeout_time))
    {
      current_time = Milliseconds();
    }
    if (!(LPC_SC->PLL1STAT >> kPlock & 1) && (current_time >= timeout_time))
    {
      SJ2_ASSERT_FATAL(false,
                       "PLL lock could not be established before timeout");
    }
    return (actual_speed - desired_speed_in_mhz);
  }

  void SetCpuClockDivider(uint8_t cpu_divider)
  {
    SJ2_ASSERT_FATAL(cpu_divider < 32, "Divider mustn't exceed 32");
    LPC_SC->CCLKSEL = (LPC_SC->CCLKSEL & ~kClearCpuDivider) | cpu_divider;
  }

  void SetEmcClockDivider(EmcDivider emc_divider)
  {
    LPC_SC->EMCCLKSEL = (LPC_SC->EMCCLKSEL & ~kClearEmcDivider) |
                        static_cast<uint8_t>(emc_divider);
  }

  void SetPeripheralClockDivider(uint8_t peripheral_divider)
  {
    SJ2_ASSERT_FATAL(peripheral_divider < 32, "Divider mustn't exceed 32");
    LPC_SC->PCLKSEL =
        (LPC_SC->PCLKSEL & ~kClearPeripheralDivider) | peripheral_divider;
  }

  void SetClockFrequency(uint8_t frequency_in_mhz) override
  {
    SelectOscillatorSource(OscillatorSource::kIrc);
    if (frequency_in_mhz > 12)
    {
      SetMainPll(PllInput::kIrc, frequency_in_mhz);
      SelectMainClockSource(MainClockSource::kPllClock);
    }
    else
    {
      SelectMainClockSource(MainClockSource::kBaseClock);
      speed_in_hertz_ = 12000000;
    }
    SetCpuClockDivider(kDivideInputBy1);
    SetPeripheralClockDivider(kDivideInputBy1);
    SetEmcClockDivider(EmcDivider::kSameSpeedAsCpu);
  }

  uint32_t GetClockFrequency() override
  {
    return speed_in_hertz_;
  }

  void PowerPeripheral(PeripheralPowerUp peripheral_select)
  {
    LPC_SC->PCONP =
        LPC_SC->PCONP | (1 << static_cast<uint8_t>(peripheral_select));
  }
  // TODO(Zaaji #181): Set USB and Spifi clock rates
};
