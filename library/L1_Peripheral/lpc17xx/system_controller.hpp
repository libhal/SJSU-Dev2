// System Controller for the LPC176x/5x series MCUs.
// The controller allows configuration of the cpu clock speed as well as the
// configuration of various peripheral power controls and peripheral clock
// speeds.
#pragma once

#include "project_config.hpp"

#include "L0_Platform/lpc17xx/LPC17xx.h"
#include "L1_Peripheral/system_controller.hpp"
#include "utility/bit.hpp"
#include "utility/enum.hpp"
#include "utility/log.hpp"

namespace sjsu
{
namespace lpc17xx
{
/// System controller for the LPC40xx series of MCUs.
class SystemController final : public sjsu::SystemController
{
 public:
  /// System input oscillator source select contants
  enum class OscillatorSource : uint8_t
  {
    /// Internal RC oscillator as main. Not very precise oscillator cannot be
    /// used for USB or high speed CAN.
    kIrc = 0b00,
    /// Use external oscillator (typically a crystal). See PllInput for options
    /// that can be used for the oscillator.
    kExternal = 0b01,
    /// Use the RTC oscillator as the main oscillator, approximate 32kHz.
    kRtc = 0b10,
  };

  /// PLL codes for selecting the PLL to be connected to a clock input
  enum class PllSelect : uint8_t
  {
    /// PLL0, Main PLL code
    kMainPll = 0,
    /// PLL1: USB PLL code
    kUsbPll,
  };

  /// Available frequencies of the external oscillator for use by PLL1 to
  /// produce the required USB clock.
  enum class UsbPllInputFrequency : uint8_t
  {
    kF12MHz = 0,
    kF16MHz,
    kF24MHz,
  };

  /// USB PLL Multiplier codes
  enum class UsbPllMultiplier : uint8_t
  {
    kMultiplyBy4 = 0b0'0011,
    kMultiplyBy3 = 0b0'0010,
    kMultiplyBy2 = 0b0'0001,
  };

  /// USB PLL Divider codes
  enum class UsbPllDivider : uint8_t
  {
    kDivideBy1 = 0b00,
    kDivideBy2 = 0b01,
    kDivideBy4 = 0b10,
    kDivideBy8 = 0b11,
  };

  /// Used to hold calculated PLL settings
  struct PllSettings_t
  {
    /// Multiplies the input oscillator by this amount
    uint16_t multiplier;
    /// Pre-divide the input source to drive the multiplier even higher.
    uint8_t pre_divider;
    /// Final output divider to bring the frequency down to a reasonable level
    /// for the system.
    uint8_t cpu_divider;
  };

  /// LPC17xx Peripheral Power On Values:
  /// The kDeviceId of each peripheral corresponds to the peripheral's power on
  /// bit position within the LPC17xx System Controller's PCONP register.
  ///
  /// @note The following bits are reserved by the manufacturer:
  ///       0, 5, 11, 20, 28.
  class Peripherals
  {
   public:
    //! @cond Doxygen_Suppress
    static constexpr auto kTimer0            = PeripheralID::Define<1>();
    static constexpr auto kTimer1            = PeripheralID::Define<2>();
    static constexpr auto kUart0             = PeripheralID::Define<3>();
    static constexpr auto kUart1             = PeripheralID::Define<4>();
    static constexpr auto kPwm1              = PeripheralID::Define<6>();
    static constexpr auto kI2c0              = PeripheralID::Define<7>();
    static constexpr auto kSpi               = PeripheralID::Define<8>();
    static constexpr auto kRtc               = PeripheralID::Define<9>();
    static constexpr auto kSsp1              = PeripheralID::Define<10>();
    static constexpr auto kAdc               = PeripheralID::Define<12>();
    static constexpr auto kCan1              = PeripheralID::Define<13>();
    static constexpr auto kCan2              = PeripheralID::Define<14>();
    static constexpr auto kGpio              = PeripheralID::Define<15>();
    static constexpr auto kRit               = PeripheralID::Define<16>();
    static constexpr auto kMotorControlPwm   = PeripheralID::Define<17>();
    static constexpr auto kQuadratureEncoder = PeripheralID::Define<18>();
    static constexpr auto kI2c1              = PeripheralID::Define<19>();
    static constexpr auto kSsp0              = PeripheralID::Define<21>();
    static constexpr auto kTimer2            = PeripheralID::Define<22>();
    static constexpr auto kTimer3            = PeripheralID::Define<23>();
    static constexpr auto kUart2             = PeripheralID::Define<24>();
    static constexpr auto kUart3             = PeripheralID::Define<25>();
    static constexpr auto kI2c2              = PeripheralID::Define<26>();
    static constexpr auto kI2s               = PeripheralID::Define<27>();
    static constexpr auto kGpdma             = PeripheralID::Define<29>();
    static constexpr auto kEthernet          = PeripheralID::Define<30>();
    static constexpr auto kUsb               = PeripheralID::Define<31>();
    static constexpr auto kCpu               = PeripheralID::Define<32>();
    //! @endcond
  };

  /// Namespace of Oscillator register bitmasks
  struct Oscillator  // NOLINT
  {
    /// IRC, main, or RTC oscillator select bits
    static constexpr bit::Mask kSelect = bit::CreateMaskFromRange(0, 1);
  };

  /// Namespace of Clock register bitmasks
  struct CpuClock  // NOLINT
  {
    /// CPU clock divider amount bitmask
    static constexpr bit::Mask kDivider = bit::CreateMaskFromRange(0, 7);
  };

  /// Bit masks for the Main PLL register
  struct MainPll  // NOLINT
  {
    // The following bit masks apply to the PLL0STAT and PLL0CFG registers
    /// PLL multiplier mask
    static constexpr bit::Mask kMultiplier = bit::CreateMaskFromRange(0, 14);
    /// PLL Pre-divider mask
    static constexpr bit::Mask kPreDivider = bit::CreateMaskFromRange(16, 23);

    // The following bit masks only apply to the PLL0STAT register
    /// PLL Mode control mask
    static constexpr bit::Mask kMode = bit::CreateMaskFromRange(24, 25);
    /// PLL Lock status bit
    static constexpr bit::Mask kLockStatus = bit::CreateMaskFromRange(26);
  };

  /// Bit masks for the USB PLL register
  struct UsbPll  // NOLINT
  {
    /// PLL multiplier mask
    static constexpr bit::Mask kMultiplier = bit::CreateMaskFromRange(0, 4);
    /// PLL Pre-divider mask
    static constexpr bit::Mask kDivider = bit::CreateMaskFromRange(5, 6);
    /// PLL Mode control mask
    static constexpr bit::Mask kMode = bit::CreateMaskFromRange(8, 9);
    /// PLL Lock status bit
    static constexpr bit::Mask kLockStatus = bit::CreateMaskFromRange(10);
  };

  /// Common bit masks across PLLs
  struct Pll  // NOLINT
  {
    /// PLL enable bit
    static constexpr bit::Mask kEnableBit = bit::CreateMaskFromRange(0);
    /// PLL connect/disconnect bit
    static constexpr bit::Mask kConnectBit = bit::CreateMaskFromRange(1);
  };

  /// Fixed IRC frequency
  static constexpr units::frequency::hertz_t kDefaultIRCFrequency = 4_MHz;

  /// Required frequency for the USB clock
  static constexpr units::frequency::hertz_t kUsbClockFrequency = 48_MHz;

  /// Fixed RTC frequency
  static constexpr units::frequency::hertz_t kRTCFrequency = 32'768_Hz;

  /// Pointer to the system controller peripheral in memory.
  inline static LPC_SC_TypeDef * system_controller = LPC_SC;

  // TODO(#1140): Migrate to SystemController V2.0
  void Initialize() override
  {
    return;
  }

  /// @return the a pointer to the clock configuration object used to configure
  /// this system controller.
  void * GetClockConfiguration() override
  {
    return nullptr;
  }

  /// @return the clock rate frequency of a peripheral
  units::frequency::hertz_t GetClockRate(PeripheralID peripheral) const override
  {
    return GetSystemFrequency() / GetPeripheralClockDivider(peripheral);
  }

  /// Sets a desired CPU speed by using the internal RC as the oscillator source
  /// and configuring PLL0.
  ///
  /// @note The USB subsystem should be configured to obtain its input clock
  ///       from the USB PLL. The internal RC is used for PLL0 and will not
  ///       generate a precise enough clock to be used by the USB subsystem.
  ///
  /// @param frequency The desired CPU Clock frequency in megahertz.
  void SetSystemClockFrequency(units::frequency::megahertz_t frequency) const
  {
    // The following sequence is specified in the LPC176x/5x User Manual
    // Section 4.5.13 and must be followed step by step in order to have PLL0
    // initialized and running.

    // NOTE: It is very important not to merge any steps. For example, do not
    //       update the PLL0CFG and enable PLL0 simultaneously with the same
    //       feed sequence.

    // 1. Disconnect PLL0 with one feed sequence if PLL0 is already connected.
    system_controller->PLL0CON =
        bit::Clear(system_controller->PLL0CON, Pll::kConnectBit);
    WritePllFeedSequence(PllSelect::kMainPll);
    // 2. Disable PLL0 with one feed sequence.
    system_controller->PLL0CON =
        bit::Clear(system_controller->PLL0CON, Pll::kEnableBit);
    WritePllFeedSequence(PllSelect::kMainPll);
    // 3. Change the CPU Clock Divider setting to speed up operation without
    //    PLL0, if desired.
    SetCpuClockDivider(0);
    // 4. Write to the Clock Source Selection Control register to change the
    //    clock source if needed.
    //    The 4 MHz internal RC will be used until PLL0 achieves a lock and is
    //    connected.
    SelectOscillatorSource(OscillatorSource::kIrc);
    // 5. Write to the PLL0CFG and make it effective with one feed sequence.
    //    The PLL0CFG can only be updated when PLL0 is disabled.
    const PllSettings_t kPll0Settings =
        CalculatePll0(kDefaultIRCFrequency, frequency);
    system_controller->PLL0CFG =
        bit::Insert(system_controller->PLL0CFG, kPll0Settings.multiplier,
                    MainPll::kMultiplier);
    system_controller->PLL0CFG =
        bit::Insert(system_controller->PLL0CFG, kPll0Settings.pre_divider,
                    MainPll::kPreDivider);
    WritePllFeedSequence(PllSelect::kMainPll);
    // 6. Enable PLL0 with one feed sequence.
    system_controller->PLL0CON =
        bit::Set(system_controller->PLL0CON, Pll::kEnableBit);
    WritePllFeedSequence(PllSelect::kMainPll);
    // 7. Change the CPU Clock Divider setting for the operation with PLL0.
    //    It is critical to do this before connecting PLL0.
    SetCpuClockDivider(kPll0Settings.cpu_divider);
    // 8. Wait for PLL0 to achieve lock by monitoring the PLOCK0 bit in the
    //    PLL0STAT register, or using the PLOCK0 interrupt, or wait for a fixed
    //    time when the input clock to PLL0 is slow (i.e. 32 kHz).
    SJ2_ASSERT_FATAL(WaitForPllLockStatus(PllSelect::kMainPll),
                     "PLL0 lock could not be established before timeout");
    // 9. Connect PLL0 with one feed sequence.
    system_controller->PLL0CON =
        bit::Set(system_controller->PLL0CON, Pll::kConnectBit);
    WritePllFeedSequence(PllSelect::kMainPll);

    SJ2_ASSERT_FATAL(WaitForPllConnectionStatus(PllSelect::kMainPll),
                     "Failed to connect PLL0.");

    speed_in_hertz = frequency;
  }
  /// Configures the USB PLL (PLL1) to produce the required 48 MHz clock based
  /// on the input frequency provided to the PLL.
  ///
  /// @param frequency Frequency of the external oscillator used to drive PLL1.
  void SetUsbPllInputFrequency(UsbPllInputFrequency frequency)
  {
    // NOTE:  It is very important not to merge any steps below. For example, do
    //        not update the PLL1CFG and enable PLL1 simultaneously with the
    //        same feed sequence.

    // 1. Disconnect PLL1 with one feed sequence if PLL1 is already connected.
    system_controller->PLL1CON =
        bit::Clear(system_controller->PLL1CON, Pll::kConnectBit);
    WritePllFeedSequence(PllSelect::kUsbPll);
    // 2. Disable PLL1 with one feed sequence.
    system_controller->PLL1CON =
        bit::Clear(system_controller->PLL1CON, Pll::kEnableBit);
    WritePllFeedSequence(PllSelect::kUsbPll);
    // 3. Write to the PLL1CFG and make it effective with one feed sequence.
    constexpr uint8_t kDivider = Value(UsbPllDivider::kDivideBy1);
    // depending on the input frequency, the multiplier value will always be
    // either 2, 3, or 4
    const uint8_t kMultiplier[] = {
      Value(UsbPllMultiplier::kMultiplyBy4),
      Value(UsbPllMultiplier::kMultiplyBy3),
      Value(UsbPllMultiplier::kMultiplyBy2),
    };
    system_controller->PLL1CFG =
        bit::Insert(system_controller->PLL1CFG, kMultiplier[Value(frequency)],
                    UsbPll::kMultiplier);
    system_controller->PLL1CFG =
        bit::Insert(system_controller->PLL1CFG, kDivider, UsbPll::kDivider);
    WritePllFeedSequence(PllSelect::kUsbPll);
    // 4. Enable PLL1 with one feed sequence.
    system_controller->PLL1CON =
        bit::Set(system_controller->PLL1CON, Pll::kEnableBit);
    WritePllFeedSequence(PllSelect::kUsbPll);
    // 5. Configurations to the PLL must be locked before it can be connected.
    SJ2_ASSERT_FATAL(WaitForPllLockStatus(PllSelect::kUsbPll),
                     "PLL1 lock could not be established before timeout");
    // 6. Connect PLL1 with one feed sequence.
    system_controller->PLL1CON =
        bit::Set(system_controller->PLL1CON, Pll::kConnectBit);
    WritePllFeedSequence(PllSelect::kUsbPll);

    SJ2_ASSERT_FATAL(WaitForPllConnectionStatus(PllSelect::kUsbPll),
                     "Failed to connect PLL1.");
  }
  /// Sets the divider to control the desired peripheral clock rate (PCLK) for a
  /// specified peripheral where PCLK = CCLK / divider.
  ///
  /// The following dividers are supported for non-CAN peripherals: 1, 2, 4, 8.
  /// For CAN the following dividers are supported: 1, 2, 4, 6.
  ///
  /// @param peripheral_select  Peripheral to configure.
  /// @param peripheral_divider Peripheral clock divider value.
  void SetPeripheralClockDivider(PeripheralID peripheral_select,
                                 uint8_t peripheral_divider) const
  {
    const bool kIsCanPeripheral =
        peripheral_select.device_id == Peripherals::kCan1.device_id ||
        peripheral_select.device_id == Peripherals::kCan2.device_id;
    // Convert the divider value to corresponding 2-bit select value
    // The list of divider select values can be found in the LPC176x/5x User
    // Manual Table 42.
    uint8_t divider_select;
    switch (peripheral_divider)
    {
      case 1: divider_select = 0b01; break;
      case 2: divider_select = 0b10; break;
      case 4: divider_select = 0b00; break;
      case 6:
      {
        SJ2_ASSERT_FATAL(
            kIsCanPeripheral,
            "The divider value of 6 is only supported for CAN peripherals.");
        divider_select = 0b11;
      }
      break;
      case 8:
      {
        SJ2_ASSERT_FATAL(
            !kIsCanPeripheral,
            "The divider value of 8 is not supported for CAN peripherals.");
        divider_select = 0b11;
      }
      break;
      default:
        SJ2_ASSERT_FATAL(
            false,
            "Only the following peripheral divider values are supported: 1, 2, "
            "4, 8. The divider value of 6 is supported for CAN peripherals.");
        divider_select = -1;
    }
    volatile uint32_t * pclk_sel =
        GetPeripheralClockSelectRegister(peripheral_select);
    const bit::Mask kDividerMask =
        CalculatePeripheralClockDividerMask(peripheral_select);
    *pclk_sel = bit::Insert(*pclk_sel, divider_select, kDividerMask);
  }

  /// @returns The clock divider for the specified peripheral.
  uint32_t GetPeripheralClockDivider(
      PeripheralID peripheral_select) const
  {
    volatile uint32_t * pclk_sel =
        GetPeripheralClockSelectRegister(peripheral_select);
    const bit::Mask kDividerMask =
        CalculatePeripheralClockDividerMask(peripheral_select);
    const uint8_t kDividerSelect =
        static_cast<uint8_t>(bit::Extract(*pclk_sel, kDividerMask));

    uint8_t peripheral_clock_divider;
    // convert and return the actual peripheral divider value based on the 2-bit
    // divider select value
    switch (kDividerSelect)
    {
      case 0b00: peripheral_clock_divider = 4; break;
      case 0b01: peripheral_clock_divider = 1; break;
      case 0b10: peripheral_clock_divider = 2; break;
      case 0b11:
      {
        // 0b11 for CAN peripiherals use a divider of 6 while all others use a
        // divider of 8
        switch (peripheral_select.device_id)
        {
          case Peripherals::kCan1.device_id: [[fallthrough]];
          case Peripherals::kCan2.device_id:
            peripheral_clock_divider = 6;
            break;
          default: peripheral_clock_divider = 8; break;
        }
      }
    }
    return peripheral_clock_divider;
  }

  /// Returns the last set frequency of the system
  units::frequency::hertz_t GetSystemFrequency() const
  {
    return speed_in_hertz;
  }

  /// Check if a peripheral is powered up by checking the power connection
  /// register. Should typically only be used for unit testing code and
  /// debugging.
  bool IsPeripheralPoweredUp(PeripheralID peripheral_select) const override
  {
    return bit::Read(system_controller->PCONP, peripheral_select.device_id);
  }

  void PowerUpPeripheral(PeripheralID peripheral_select) const override
  {
    system_controller->PCONP =
        bit::Set(system_controller->PCONP, peripheral_select.device_id);
  }

  void PowerDownPeripheral(PeripheralID peripheral_select) const override
  {
    system_controller->PCONP =
        bit::Clear(system_controller->PCONP, peripheral_select.device_id);
  }

 private:
  inline static units::frequency::hertz_t speed_in_hertz = kDefaultIRCFrequency;

  void SelectOscillatorSource(OscillatorSource source) const
  {
    system_controller->CLKSRCSEL =
        bit::Insert(system_controller->CLKSRCSEL, static_cast<uint32_t>(source),
                    Oscillator::kSelect);
  }

  /// Calculates the multiplier. pre-divider, and CPU clock divider required to
  /// generated the desired CPU clock with PLL0 based on the specified input
  /// frequency.
  ///
  /// @param input_frequency Input of PLL0 should be 32 kHz to 50 MHz.
  /// @param desired_speed   Desired CPU clock to achieve. Should not
  ///                        exceed the maximum allowed CPU clock.
  PllSettings_t CalculatePll0(units::frequency::hertz_t input_frequency,
                              units::frequency::hertz_t desired_speed) const
  {
    // minimum/maximum input and output frequencies of PLL0 in kHz
    constexpr units::frequency::hertz_t kMinimumPll0InputFrequency = 32_kHz;
    constexpr units::frequency::hertz_t kMaximumPll0InputFrequency = 50_MHz;
    constexpr units::frequency::hertz_t kMinimumPll0OuputFrequency = 275_MHz;
    constexpr units::frequency::hertz_t kMaximumPll0OuputFrequency = 550_MHz;

    // Maximum allowed CPU speed in kHz.
    // This value will be 100 MHz or 120 MHz depending on the MCU in use
    // For the SJOne, the max CPU speed for LPC1758 is 100 MHz.
    constexpr units::frequency::hertz_t kMaxCPUSpeed = 100_MHz;

    SJ2_ASSERT_FATAL(
        input_frequency > kMinimumPll0InputFrequency &&
            input_frequency < kMaximumPll0InputFrequency,
        "The input PLL0 frequency must be between 32kHz and 50MHz");
    SJ2_ASSERT_FATAL(
        desired_speed < kMaxCPUSpeed,
        "The desired CPU speed cannot exceed the maximum allow CPU speed.");
    // The supported pre-divider values ranges from 1 to 32 while the supported
    // multiplier values ranges from 6 to 512.
    // Since a small value for the pre-divider, n, is desired, we will iterate
    // through n starting from the lowest possible value of 1 in order to find a
    // suitable multiplier, m. The values of m and n are inversely proportional;
    // therefore, we start looking for the multiplier from its largest possible
    // value of 512.
    for (uint8_t n = 0; n < 32; n++)
    {
      for (uint16_t m = 511; m >= 6; m--)
      {
        // Current calculated controlled oscillator frequency, fcco, output of
        // PLL0 in kilohertz
        // Dividing by 1000 to scale down kFcco, as the multiplier for is
        // internally scaled by 1000.
        const units::frequency::hertz_t kFcco =
            (2 * (m + 1) * input_frequency) / (n + 1);
        if (kMinimumPll0OuputFrequency < kFcco &&
            kFcco < kMaximumPll0OuputFrequency)
        {
          // since PLL0 is in use, the cpu_divider values of 0 and 1 are not
          // allowed as the resulting CPU clock will always be above the maximum
          // allowed CPU speed
          for (uint16_t cpu_divider = 2; cpu_divider < 256; cpu_divider++)
          {
            // Get resulting CPU clock
            // Requires that we scale kFcco back to a proper frequency.
            const units::frequency::hertz_t kCpuClock =
                kFcco / (cpu_divider + 1);
            if (kCpuClock == desired_speed)
            {
              return PllSettings_t{ .multiplier  = m,
                                    .pre_divider = n,
                                    .cpu_divider =
                                        static_cast<uint8_t>(cpu_divider) };
            }
          }  // cpu_divider loop
        }
      }  // m for loop
    }    // n for loop
    SJ2_ASSERT_FATAL(
        false,
        "Failed to calculate the PLL0 settings for the desired frequency.");
    return PllSettings_t{ .multiplier = 0, .pre_divider = 0, .cpu_divider = 0 };
  }

  /// Writes the feed sequence that is necessary to lock in any changes to the
  /// PLLCON and PLLCGG registers.
  void WritePllFeedSequence(PllSelect pll) const
  {
    volatile uint32_t * pll_feed_registers[] = {
      &(system_controller->PLL0FEED),
      &(system_controller->PLL1FEED),
    };
    *(pll_feed_registers[Value(pll)]) = 0xAA;
    *(pll_feed_registers[Value(pll)]) = 0x55;
  }

  bool WaitForPllLockStatus(PllSelect pll) const
  {
    // Skip waiting for PLLSTAT register to update if running a host unit test
    if constexpr (build::kPlatform == build::Platform::host)
    {
      return true;
    }

    volatile uint32_t * pll_status_registers[] = {
      &(system_controller->PLL0STAT),  // NOLINT
      &(system_controller->PLL1STAT)
    };
    const bit::Mask kLockStatusMasks[]  = { MainPll::kLockStatus,
                                           UsbPll::kLockStatus };
    volatile uint32_t * status_register = pll_status_registers[Value(pll)];
    const bit::Mask kLockStatusMask     = kLockStatusMasks[Value(pll)];

    while (!bit::Read(*status_register, kLockStatusMask.position))
    {
      continue;
    }

    return true;
  }

  /// @returns Returns true if the enable and connect status bits in the PLL
  ///          status register are both 1.
  bool WaitForPllConnectionStatus(PllSelect pll) const
  {
    // Skip waiting for PLLSTAT register to update if running a host unit test
    if constexpr (build::kPlatform == build::Platform::host)
    {
      return true;
    }

    volatile uint32_t * pll_status_registers[] = {
      &(system_controller->PLL0STAT),  // NOLINT
      &(system_controller->PLL1STAT)
    };
    const bit::Mask kMasks[]            = { MainPll::kMode, UsbPll::kMode };
    volatile uint32_t * status_register = pll_status_registers[Value(pll)];
    const bit::Mask kPllModeMask        = kMasks[Value(pll)];

    while (!bit::Read(*status_register, kPllModeMask.position))
    {
      continue;
    }

    return true;
  }

  /// Sets divider used for the CPU clock (CCLK). The PLL0 clock (pllclk) will
  /// be divided by the cpu_divider + 1.
  /// For example, if cpu_divider = 1, then CCLK = pllclk / 2.
  ///              if cpu_divider = 255, then CCLK = pllclk / 256.
  ///
  /// @note If PLL0 is connected, divider values 0 and 1 are not allowed
  ///       since the the produced clock rate must not exceed the maximum
  ///       allowed CPU clock.
  ///
  /// @param cpu_divider 8-bit divider ranging from 0 - 255.
  void SetCpuClockDivider(uint8_t cpu_divider) const
  {
    system_controller->CCLKCFG = bit::Insert(system_controller->CCLKCFG,
                                             cpu_divider, CpuClock::kDivider);
  }

  /// @returns  Pointer to the PCLKSEL0 or PCLKSEL1 register based on the
  ///           peripheral's device_id.
  volatile uint32_t * GetPeripheralClockSelectRegister(
      PeripheralID peripheral_select) const
  {
    if (peripheral_select.device_id > 15)
    {
      return &(system_controller->PCLKSEL1);
    }
    return &(system_controller->PCLKSEL0);
  }

  /// @returns  The bit mask for the 2-bit position of the specified
  ///           peripheral's divider select in the PCLKSEL0 or PCLKSEL1
  ///           register.
  bit::Mask CalculatePeripheralClockDividerMask(
      PeripheralID peripheral_select) const
  {
    constexpr uint8_t kMaxBitWidth = 32;
    const uint8_t kLowBit  = (peripheral_select.device_id * 2) % kMaxBitWidth;
    const uint8_t kHighBit = static_cast<uint8_t>(kLowBit + 1);
    return bit::CreateMaskFromRange(kLowBit, kHighBit);
  }
};
}  // namespace lpc17xx
}  // namespace sjsu
