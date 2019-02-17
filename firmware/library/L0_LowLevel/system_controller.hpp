// system clock class set clock outputs frequency and selects clock source
#pragma once

#include <cstdint>

#include "project_config.hpp"

#include "L0_LowLevel/LPC40xx.h"
#include "utility/bit.hpp"
#include "utility/enum.hpp"
#include "utility/log.hpp"
#include "utility/macros.hpp"

class LpcPeripheral
{
 public:
  class ID
  {
   public:
    size_t device_id = -1;
  };
  template <size_t kDeviceId>
  class AddID : public ID
  {
   public:
    constexpr AddID()
    {
      device_id = kDeviceId;
    }
  };
};

class Lpc40xxSystemController
{
 public:
  using PeripheralID = LpcPeripheral::ID;

  enum class SpifiSource : uint16_t
  {
    kIrcDirect         = (0b00 << 8),
    kMainPllClock      = (0b01 << 8),
    kAlternatePllClock = (0b10 << 8)
  };

  enum class OscillatorSource : uint16_t
  {
    kIrc  = 0b0,
    kMain = 0b1
  };

  enum class MainClockSource : uint16_t
  {
    kIrcDirect = 0b0,
    kPllClock  = 0b1
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

  enum class UsbSource : uint16_t
  {
    kIrcDirect         = 0x0,
    kMainPllClock      = 0x1,
    kAlternatePllClock = 0x2
  };

  enum class UsbDivider : uint8_t
  {
    kDisable = 0,
    kDivideBy1 = 1,
    kDivideBy2 = 2,
    kDivideBy3 = 3
  };

  // LPC40xx Peripheral Power On Values:
  // The kDeviceId of each peripheral corresponds to the peripheral's power on
  // bit position within the LPC40xx System Controller's PCONP register.
  class Peripherals
  {
   public:
    static constexpr auto kLcd               = LpcPeripheral::AddID<0>();
    static constexpr auto kTimer0            = LpcPeripheral::AddID<1>();
    static constexpr auto kTimer1            = LpcPeripheral::AddID<2>();
    static constexpr auto kUart0             = LpcPeripheral::AddID<3>();
    static constexpr auto kUart1             = LpcPeripheral::AddID<4>();
    static constexpr auto kPwm0              = LpcPeripheral::AddID<5>();
    static constexpr auto kPwm1              = LpcPeripheral::AddID<6>();
    static constexpr auto kI2c0              = LpcPeripheral::AddID<7>();
    static constexpr auto kUart4             = LpcPeripheral::AddID<8>();
    static constexpr auto kRtc               = LpcPeripheral::AddID<9>();
    static constexpr auto kSsp1              = LpcPeripheral::AddID<10>();
    static constexpr auto kEmc               = LpcPeripheral::AddID<11>();
    static constexpr auto kAdc               = LpcPeripheral::AddID<12>();
    static constexpr auto kCan1              = LpcPeripheral::AddID<13>();
    static constexpr auto kCan2              = LpcPeripheral::AddID<14>();
    static constexpr auto kGpio              = LpcPeripheral::AddID<15>();
    static constexpr auto kSpifi             = LpcPeripheral::AddID<16>();
    static constexpr auto kMotorControlPwm   = LpcPeripheral::AddID<17>();
    static constexpr auto kQuadratureEncoder = LpcPeripheral::AddID<18>();
    static constexpr auto kI2c1              = LpcPeripheral::AddID<19>();
    static constexpr auto kSsp2              = LpcPeripheral::AddID<20>();
    static constexpr auto kSsp0              = LpcPeripheral::AddID<21>();
    static constexpr auto kTimer2            = LpcPeripheral::AddID<22>();
    static constexpr auto kTimer3            = LpcPeripheral::AddID<23>();
    static constexpr auto kUart2             = LpcPeripheral::AddID<24>();
    static constexpr auto kUart3             = LpcPeripheral::AddID<25>();
    static constexpr auto kI2c2              = LpcPeripheral::AddID<26>();
    static constexpr auto kI2s               = LpcPeripheral::AddID<27>();
    static constexpr auto kSdCard            = LpcPeripheral::AddID<28>();
    static constexpr auto kGpdma             = LpcPeripheral::AddID<29>();
    static constexpr auto kEthernet          = LpcPeripheral::AddID<30>();
    static constexpr auto kUsb               = LpcPeripheral::AddID<31>();
  };

  static constexpr uint32_t kOscillatorSelect       = (1 << 0);
  static constexpr uint32_t kIrcDirectSelect        = 8;
  static constexpr uint32_t kUsbClockSource         = (0b11 << 8);
  static constexpr uint32_t kSpifiClockSource       = (0b11 << 8);
  static constexpr uint32_t kEnablePll              = (0b1 << 0);
  static constexpr uint32_t kPlock                  = 10;
  static constexpr uint32_t kDivideInputBy1         = 1;
  static constexpr uint32_t kClearPllMultiplier     = (0x1f << 0);
  static constexpr uint32_t kClearPllDivider        = (0b11 << 5);
  static constexpr uint32_t kClearCpuDivider        = (0x1F << 0);
  static constexpr uint32_t kClearEmcDivider        = (1 << 0);
  static constexpr uint32_t kClearPeripheralDivider = (0b1'1111);
  static constexpr uint32_t kClearUsbDivider        = (0x1F << 0);
  static constexpr uint32_t kDefaultIRCFrequency    = 12'000'000;
  static constexpr uint32_t kDefaultTimeout         = 1'000;  // ms

  inline static LPC_SC_TypeDef * system_controller = LPC_SC;

  constexpr Lpc40xxSystemController() {}

  uint32_t SetClockFrequency(
      uint8_t frequency_in_mhz,
      OscillatorSource oscillator = OscillatorSource::kIrc,
      PllInput pll_input          = PllInput::kIrc)
  {
    uint32_t offset = 0;
    SelectOscillatorSource(oscillator);
    if (frequency_in_mhz > 12)
    {
      offset = SetMainPll(pll_input, frequency_in_mhz);
      SelectCpuClockSource(MainClockSource::kPllClock);
      speed_in_hertz = frequency_in_mhz * 1'000'000;
    }
    else
    {
      SelectCpuClockSource(MainClockSource::kIrcDirect);
      speed_in_hertz = kDefaultIRCFrequency;
    }
    SetCpuClockDivider(kDivideInputBy1);
    SetEmcClockDivider(EmcDivider::kSameSpeedAsCpu);
    SetPeripheralClockDivider(kDivideInputBy1);
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

  uint32_t GetSystemFrequency() const
  {
#if defined(HOST_TEST)
    return config::kSystemClockRate;
#else
    return speed_in_hertz;
#endif
  }

  uint32_t GetPeripheralFrequency() const
  {
    uint32_t peripheral_clock_divider = GetPeripheralClockDivider();
    uint32_t result = 0;  // return 0 if peripheral_clock_divider == 0
    if (peripheral_clock_divider != 0)
    {
      result = GetSystemFrequency() / peripheral_clock_divider;
    }
    return result;
  }

  void PowerUpPeripheral(const PeripheralID & peripheral_select)
  {
    auto power_connection_with_enabled_peripheral =
        system_controller->PCONP | (1 << peripheral_select.device_id);

    system_controller->PCONP = power_connection_with_enabled_peripheral;
  }

  void PowerDownPeripheral(const PeripheralID & peripheral_select)
  {
    auto power_connection_without_enabled_peripheral =
        system_controller->PCONP & (1 << peripheral_select.device_id);

    system_controller->PCONP = power_connection_without_enabled_peripheral;
  }

  void SelectOscillatorSource(OscillatorSource source)
  {
    uint32_t source_bit = static_cast<uint32_t>(source);
    system_controller->CLKSRCSEL =
        bit::Insert(system_controller->CLKSRCSEL, source_bit, 0, 1);
  }

  void SelectCpuClockSource(MainClockSource source)
  {
    system_controller->CCLKSEL =
        bit::Insert(system_controller->CCLKSEL, 1, 0, 5);
    system_controller->CCLKSEL =
        bit::Insert(system_controller->CCLKSEL, static_cast<uint32_t>(source),
                    kIrcDirectSelect, 1);
  }

  void SelectUsbClockSource(UsbSource usb_clock)
  {
    uint32_t clock_value = static_cast<uint32_t>(usb_clock);
    system_controller->USBCLKSEL =
        bit::Insert(system_controller->USBCLKSEL, clock_value, 8, 2);
  }

  void SelectUsbClockDivider(UsbDivider usb_divider)
  {
    uint32_t divider_value = static_cast<uint32_t>(usb_divider);
    system_controller->USBCLKSEL =
        bit::Insert(system_controller->USBCLKSEL, divider_value, 0, 5);
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
    SelectCpuClockSource(MainClockSource::kIrcDirect);
    SelectUsbClockSource(UsbSource::kIrcDirect);
    SelectSpifiClockSource(SpifiSource::kIrcDirect);
    // must subtract 1 from multiplier value as specified in datasheet
    system_controller->PLL0CFG =
        (system_controller->PLL0CFG & ~kClearPllMultiplier) |
        (multiplier_value - 1);
    system_controller->PLL0CFG =
        (system_controller->PLL0CFG & ~kClearPllDivider) | (divider_value << 5);
    // nessecary feed sequence to ensure the changes are intentional
    system_controller->PLL0CON  = kEnablePll;
    system_controller->PLL0FEED = 0xAA;
    system_controller->PLL0FEED = 0x55;
    while (!(system_controller->PLL0STAT >> kPlock & 1) &&
           (current_time < timeout_time))
    {
      current_time = Milliseconds();
    }
    if (current_time >= timeout_time)
    {
      SJ2_ASSERT_FATAL(false,
                       "PLL lock could not be established before timeout");
    }
    return (actual_speed - desired_speed_in_mhz);
  }

  struct SystemControlRegister
  {
    unsigned reserved0 : 4;
    uint8_t main_oscillator_freq_above_15Mhz : 1;
    uint8_t main_oscillator_enable : 1;
    uint8_t is_main_oscillator_enabled : 1;
    uint32_t reserved1 : 25;
  };

  uint32_t SetAlternatePll(PllInput input_frequency,
                           uint16_t desired_speed_in_mhz)
  {
    uint16_t divider_value = 1;
    uint64_t timeout_time  = Milliseconds() + (kDefaultTimeout);
    uint64_t current_time  = Milliseconds();
    uint32_t multiplier_value =
        CalculatePll(input_frequency, desired_speed_in_mhz);
    uint32_t actual_speed =
        static_cast<uint32_t>(input_frequency) * multiplier_value;

    LPC_SC->SCS |= 1 << 5;
    while(!(LPC_SC->SCS & (1 << 6)))
    {
      continue;
    }

    SelectUsbClockSource(UsbSource::kIrcDirect);
    SelectSpifiClockSource(SpifiSource::kIrcDirect);
    // must subtract 1 from multiplier value as specified in datasheet
    system_controller->PLL1CFG =
        (system_controller->PLL1CFG & ~kClearPllMultiplier) |
        (multiplier_value - 1);
    system_controller->PLL1CFG =
        (system_controller->PLL1CFG & ~kClearPllDivider) | (divider_value << 5);
    // Necessary feed sequence to ensure the changes are intentional
    __disable_irq();  // Set PRIMASK
    system_controller->PLL1CON  = 1;
    system_controller->PLL1FEED = 0xAA;
    system_controller->PLL1FEED = 0x55;
    __enable_irq();  // Clear PRIMASK
    while (!(system_controller->PLL1STAT & (1 << kPlock)) &&
           (current_time < timeout_time))
    {
      current_time = Milliseconds();
    }
    if (!(system_controller->PLL1STAT & (1 << kPlock)) && current_time >= timeout_time)
    {
      SJ2_ASSERT_FATAL(false,
                       "PLL lock could not be established before timeout");
    }
    SJ2_PRINT_VARIABLE(system_controller->PLL1STAT, "%08lX");
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

 protected:
  // TODO(Zaaji #181): Set USB and Spifi clock rates
  inline static uint32_t speed_in_hertz = kDefaultIRCFrequency;
};

// inline Lpc40xxSystemController system_clock;
