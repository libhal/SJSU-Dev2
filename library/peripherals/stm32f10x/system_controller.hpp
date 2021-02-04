#pragma once

#include <cstdint>

#include "platforms/targets/stm32f10x/stm32f10x.h"
#include "platforms/utility/startup.hpp"
#include "peripherals/system_controller.hpp"
#include "utility/math/bit.hpp"
#include "utility/enum.hpp"

namespace sjsu
{
namespace stm32f10x
{
/// System controller for stm32f10x that controls clock sources, clock speed,
/// clock outputs control, and peripheral enabling
///
/// @see 8 Connectivity line devices: reset and clock control (RCC)
///      https://www.st.com/resource/en/reference_manual/cd00171190-stm32f101xx-stm32f102xx-stm32f103xx-stm32f105xx-and-stm32f107xx-advanced-arm-based-32-bit-mcus-stmicroelectronics.pdf#page=123
class SystemController : public sjsu::SystemController
{
 public:
  /// Number of bits between each enable register
  static constexpr uint32_t kBits = 32;

  /// Namespace containing the set of all peripherals id on the stm32f4xx mcu
  /// family.
  class Peripherals
  {
   public:
    /// Bit position of AHB
    static constexpr uint32_t kAHB = kBits * 0;

    //! @cond Doxygen_Suppress
    static constexpr auto kDma1  = ResourceID::Define<kAHB + 0>();
    static constexpr auto kDma2  = ResourceID::Define<kAHB + 1>();
    static constexpr auto kSram  = ResourceID::Define<kAHB + 2>();
    static constexpr auto kFlitf = ResourceID::Define<kAHB + 4>();
    static constexpr auto kCrc   = ResourceID::Define<kAHB + 6>();
    static constexpr auto kFsmc  = ResourceID::Define<kAHB + 8>();
    static constexpr auto kSdio  = ResourceID::Define<kAHB + 10>();
    //! @endcond

    /// Bit position of APB1
    static constexpr uint32_t kAPB1 = kBits * 1;

    //! @cond Doxygen_Suppress
    static constexpr auto kTimer2         = ResourceID::Define<kAPB1 + 0>();
    static constexpr auto kTimer3         = ResourceID::Define<kAPB1 + 1>();
    static constexpr auto kTimer4         = ResourceID::Define<kAPB1 + 2>();
    static constexpr auto kTimer5         = ResourceID::Define<kAPB1 + 3>();
    static constexpr auto kTimer6         = ResourceID::Define<kAPB1 + 4>();
    static constexpr auto kTimer7         = ResourceID::Define<kAPB1 + 5>();
    static constexpr auto kTimer12        = ResourceID::Define<kAPB1 + 6>();
    static constexpr auto kTimer13        = ResourceID::Define<kAPB1 + 7>();
    static constexpr auto kTimer14        = ResourceID::Define<kAPB1 + 8>();
    static constexpr auto kWindowWatchdog = ResourceID::Define<kAPB1 + 11>();
    static constexpr auto kSpi2           = ResourceID::Define<kAPB1 + 14>();
    static constexpr auto kSpi3           = ResourceID::Define<kAPB1 + 15>();
    static constexpr auto kUsart2         = ResourceID::Define<kAPB1 + 17>();
    static constexpr auto kUsart3         = ResourceID::Define<kAPB1 + 18>();
    static constexpr auto kUart4          = ResourceID::Define<kAPB1 + 19>();
    static constexpr auto kUart5          = ResourceID::Define<kAPB1 + 20>();
    static constexpr auto kI2c1           = ResourceID::Define<kAPB1 + 21>();
    static constexpr auto kI2c2           = ResourceID::Define<kAPB1 + 22>();
    static constexpr auto kUsb            = ResourceID::Define<kAPB1 + 23>();
    static constexpr auto kCan1           = ResourceID::Define<kAPB1 + 25>();
    static constexpr auto kBackupClock    = ResourceID::Define<kAPB1 + 27>();
    static constexpr auto kPower          = ResourceID::Define<kAPB1 + 28>();
    static constexpr auto kDac            = ResourceID::Define<kAPB1 + 29>();
    //! @endcond

    /// Bit position of AHB2
    static constexpr uint32_t kAPB2 = kBits * 2;

    //! @cond Doxygen_Suppress
    static constexpr auto kAFIO    = ResourceID::Define<kAPB2 + 0>();
    static constexpr auto kGpioA   = ResourceID::Define<kAPB2 + 2>();
    static constexpr auto kGpioB   = ResourceID::Define<kAPB2 + 3>();
    static constexpr auto kGpioC   = ResourceID::Define<kAPB2 + 4>();
    static constexpr auto kGpioD   = ResourceID::Define<kAPB2 + 5>();
    static constexpr auto kGpioE   = ResourceID::Define<kAPB2 + 6>();
    static constexpr auto kGpioF   = ResourceID::Define<kAPB2 + 7>();
    static constexpr auto kGpioG   = ResourceID::Define<kAPB2 + 8>();
    static constexpr auto kAdc1    = ResourceID::Define<kAPB2 + 9>();
    static constexpr auto kAdc2    = ResourceID::Define<kAPB2 + 10>();
    static constexpr auto kTimer1  = ResourceID::Define<kAPB2 + 11>();
    static constexpr auto kSpi1    = ResourceID::Define<kAPB2 + 12>();
    static constexpr auto kTimer8  = ResourceID::Define<kAPB2 + 13>();
    static constexpr auto kUsart1  = ResourceID::Define<kAPB2 + 14>();
    static constexpr auto kAdc3    = ResourceID::Define<kAPB2 + 15>();
    static constexpr auto kTimer9  = ResourceID::Define<kAPB2 + 19>();
    static constexpr auto kTimer10 = ResourceID::Define<kAPB2 + 20>();
    static constexpr auto kTimer11 = ResourceID::Define<kAPB2 + 21>();
    //! @endcond

    /// Bit position of systems outside of any bus
    static constexpr uint32_t kBeyond = kBits * 3;

    //! @cond Doxygen_Suppress
    static constexpr auto kCpu         = ResourceID::Define<kBeyond + 0>();
    static constexpr auto kSystemTimer = ResourceID::Define<kBeyond + 1>();
    static constexpr auto kI2s         = ResourceID::Define<kBeyond + 2>();
    //! @endcond
  };

  /// List of all of the enable registers in the order corresponding to the
  /// their constants in the Peripherals namespace.
  static inline volatile uint32_t * enable[] = {
    &RCC->AHBENR,
    &RCC->APB1ENR,
    &RCC->APB2ENR,
  };

  /// Pointer to the Clock Control register
  static inline RCC_TypeDef * clock_control = RCC;

  /// Pointer to the flash control register
  static inline FLASH_TypeDef * flash = FLASH;

  /// Available dividers for the APB bus
  enum class APBDivider
  {
    kDivideBy1  = 0,
    kDivideBy2  = 0b100,
    kDivideBy4  = 0b101,
    kDivideBy8  = 0b110,
    kDivideBy16 = 0b111,
  };

  /// Available dividers for the AHB bus
  enum class AHBDivider
  {
    kDivideBy1   = 0,
    kDivideBy2   = 0b1000,
    kDivideBy4   = 0b1001,
    kDivideBy8   = 0b1010,
    kDivideBy16  = 0b1011,
    kDivideBy64  = 0b1100,
    kDivideBy128 = 0b1101,
    kDivideBy256 = 0b1110,
    kDivideBy512 = 0b1111,
  };

  /// Available dividers for the ADC bus
  enum class AdcDivider
  {
    kDivideBy2 = 0b00,
    kDivideBy4 = 0b01,
    kDivideBy6 = 0b10,
    kDivideBy8 = 0b11,
  };

  /// Available clock sources available for the system clock
  enum class SystemClockSelect
  {
    kHighSpeedInternal = 0b00,
    kHighSpeedExternal = 0b01,
    kPll               = 0b10,
  };

  /// Bit masks for the CFGR register
  struct ClockConfigurationRegisters  // NOLINT
  {
    /// @see 8.3.2 Clock configuration register (RCC_CFGR)
    ///      https://www.st.com/resource/en/reference_manual/cd00171190-stm32f101xx-stm32f102xx-stm32f103xx-stm32f105xx-and-stm32f107xx-advanced-arm-based-32-bit-mcus-stmicroelectronics.pdf#page=134
    ///
    /// @returns The RCC_CFGR bit register.
    static bit::Register<uint32_t> Register()
    {
      return bit::Register(&clock_control->CFGR);
    }

    /// Controls which clock signal is sent to the MCO pin
    static constexpr auto kMco = bit::MaskFromRange(24, 26);

    /// Sets the USB clock divider
    static constexpr auto kUsbPrescalar = bit::MaskFromRange(22);

    /// Sets the PLL multiplier
    static constexpr auto kPllMul = bit::MaskFromRange(18, 21);

    /// If set to 1, will divide the HSE signal by 2 before sending to PLL
    static constexpr auto kHsePreDivider = bit::MaskFromRange(17);

    /// Sets which source the PLL will take as input
    static constexpr auto kPllSource = bit::MaskFromRange(16);

    /// Sets the clock divider for the ADC peripherals
    static constexpr auto kAdcDivider = bit::MaskFromRange(14, 15);

    /// Sets the divider for peripherals on the APB2 bus
    static constexpr auto kAPB2Divider = bit::MaskFromRange(11, 13);

    /// Sets the divider for peripherals on the APB1 bus
    static constexpr auto kAPB1Divider = bit::MaskFromRange(8, 10);

    /// Sets the divider for peripherals on the AHB bus
    static constexpr auto kAHBDivider = bit::MaskFromRange(4, 7);

    /// Used to check if the system clock has taken the new system clock
    /// settings.
    static constexpr auto kSystemClockStatus = bit::MaskFromRange(2, 3);

    /// Set which clock will be used for the system clock.
    static constexpr auto kSystemClockSelect = bit::MaskFromRange(0, 1);
  };

  /// Bit masks for the CR register
  struct ClockControlRegisters  // NOLINT
  {
    /// @see 8.3.1 Clock control register (RCC_CR)
    ///      https://www.st.com/resource/en/reference_manual/cd00171190-stm32f101xx-stm32f102xx-stm32f103xx-stm32f105xx-and-stm32f107xx-advanced-arm-based-32-bit-mcus-stmicroelectronics.pdf#page=132
    ///
    /// @returns The RCC_CR bit register.
    static bit::Register<uint32_t> Register()
    {
      return bit::Register(&clock_control->CR);
    }

    /// Indicates if the PLL is enabled and ready
    static constexpr auto kPllReady = bit::MaskFromRange(25);
    /// Used to enable the PLL
    static constexpr auto kPllEnable = bit::MaskFromRange(24);
    /// Indicates if the external oscillator is ready for use
    static constexpr auto kExternalOscReady = bit::MaskFromRange(17);
    /// Used to enable the external oscillator
    static constexpr auto kExternalOscEnable = bit::MaskFromRange(16);
  };

  /// PLL frequency multiplication options.
  enum class PllMultiply
  {
    kMultiplyBy2  = 0b0000,
    kMultiplyBy3  = 0b0001,
    kMultiplyBy4  = 0b0010,
    kMultiplyBy5  = 0b0011,
    kMultiplyBy6  = 0b0100,
    kMultiplyBy7  = 0b0101,
    kMultiplyBy8  = 0b0110,
    kMultiplyBy9  = 0b0111,
    kMultiplyBy10 = 0b1000,
    kMultiplyBy11 = 0b1001,
    kMultiplyBy12 = 0b1010,
    kMultiplyBy13 = 0b1011,
    kMultiplyBy14 = 0b1100,
    kMultiplyBy15 = 0b1101,
    kMultiplyBy16 = 0b1110,
  };

  /// Bitmasks for the BDCR register
  struct RtcRegisters  // NOLINT
  {
    /// @see 8.3.9 Backup domain control register (RCC_BDCR)
    ///      https://www.st.com/resource/en/reference_manual/cd00171190-stm32f101xx-stm32f102xx-stm32f103xx-stm32f105xx-and-stm32f107xx-advanced-arm-based-32-bit-mcus-stmicroelectronics.pdf#page=150
    ///
    /// @returns The RCC_BDCR bit register.
    static bit::Register<uint32_t> Register()
    {
      return bit::Register(&clock_control->BDCR);
    }

    /// Will reset all clock states for the RTC
    static constexpr auto kBackupDomainReset = bit::MaskFromRange(16);
    /// Enables the RTC clock
    static constexpr auto kRtcEnable = bit::MaskFromRange(15);
    /// Selects the clock source for the RTC
    static constexpr auto kRtcSourceSelect = bit::MaskFromRange(8, 9);
    /// Indicates if the LSE is ready for use
    static constexpr auto kLowSpeedOscReady = bit::MaskFromRange(1);
    /// Used to enable the LSE
    static constexpr auto kLowSpeedOscEnable = bit::MaskFromRange(0);
  };

  /// Available clock sources for the RTC
  enum class RtcSource
  {
    kNoClock                       = 0b00,
    kLowSpeedInternal              = 0b01,
    kLowSpeedExternal              = 0b10,
    kHighSpeedExternalDividedBy128 = 0b11,
  };

  /// Available clock sources for the PLL
  enum class PllSource
  {
    kHighSpeedInternal           = 0b0,
    kHighSpeedExternal           = 0b1,
    kHighSpeedExternalDividedBy2 = 0b11,
  };

  /// Available dividers for the USB peripheral
  enum class UsbDivider
  {
    kDivideBy1 = 1,
    /// Divide by 1.5
    kDivideBy1Point5 = 0,
  };

  /// @see Figure 11. Clock Tree
  ///      https://www.st.com/resource/en/reference_manual/cd00171190-stm32f101xx-stm32f102xx-stm32f103xx-stm32f105xx-and-stm32f107xx-advanced-arm-based-32-bit-mcus-stmicroelectronics.pdf#page=126
  struct ClockConfiguration  // NOLINT
  {
    /// Defines the frequency of the high speed external clock signal
    units::frequency::hertz_t high_speed_external = 0_MHz;

    /// Defines the frequency of the low speed external clock signal.
    units::frequency::hertz_t low_speed_external = 0_MHz;

    /// Defines the configuration of the PLL
    struct
    {
      bool enable          = false;
      PllSource source     = PllSource::kHighSpeedInternal;
      PllMultiply multiply = PllMultiply::kMultiplyBy2;
      struct
      {
        UsbDivider divider = UsbDivider::kDivideBy1Point5;
      } usb = {};
    } pll = {};

    /// Defines which clock source will be use for the system.
    /// @warning System will lock up in the following situations:
    ///          - Select PLL, but PLL is not enabled
    ///          - Select PLL, but PLL frequency is too high
    ///          - Select High Speed External, but the frequency is kept at
    ///            0_Mhz.
    SystemClockSelect system_clock = SystemClockSelect::kHighSpeedInternal;

    /// Defines the configuration for the RTC
    struct
    {
      bool enable      = false;
      RtcSource source = RtcSource::kLowSpeedInternal;
    } rtc = {};

    /// Defines the configuration of the dividers beyond system clock mux.
    struct
    {
      AHBDivider divider = AHBDivider::kDivideBy1;
      /// Maximum rate of 36 MHz
      struct
      {
        APBDivider divider = APBDivider::kDivideBy1;
      } apb1 = {};

      struct
      {
        APBDivider divider = APBDivider::kDivideBy1;
        struct
        {
          /// Maximum of 14 MHz
          AdcDivider divider = AdcDivider::kDivideBy2;
        } adc = {};
      } apb2 = {};
    } ahb = {};
  };

  /// Constant for the frequency of the LSE
  static constexpr auto kLowSpeedInternal = 20_kHz;

  /// Constant for the frequency of the HSE
  static constexpr auto kHighSpeedInternal = 8_MHz;

  /// Constant for the frequency of the Flash Controller
  static constexpr auto kFlashClock = kHighSpeedInternal;

  /// Constant for the frequency of the Watch Dog Peripheral
  static constexpr auto kWatchdogClockRate = kLowSpeedInternal;

  /// @param config - clock configuration structure
  explicit constexpr SystemController(ClockConfiguration & config)
      : config_(config)
  {
  }

  /// @attention If configuration of the system clocks is desired, one should
  ///            consult the user manual of the target MCU in use to determine
  ///            the valid clock configuration values that can/should be used.
  ///            The Initialize() method is only responsible for configuring the
  ///            clock system based on configurations in the ClockConfiguration.
  ///            Incorrect configurations may result in a hard fault or cause
  ///            the clock system(s) to supply incorrect clock rate(s).
  ///
  /// @see Figure 11. Clock Tree
  ///      https://www.st.com/resource/en/reference_manual/cd00171190-stm32f101xx-stm32f102xx-stm32f103xx-stm32f105xx-and-stm32f107xx-advanced-arm-based-32-bit-mcus-stmicroelectronics.pdf#page=126
  void Initialize() override
  {
    units::frequency::hertz_t system_clock = 0_Hz;

    // =========================================================================
    // Step 1. Select internal clock source for everything.
    //         Make sure PLLs are not clock sources for everything.
    // =========================================================================
    // Step 1.1 Set SystemClock to HSI
    ClockConfigurationRegisters::Register()
        .Insert(Value(SystemClockSelect::kHighSpeedInternal),
                ClockConfigurationRegisters::kSystemClockSelect)
        .Save();

    // Step 1.4 Reset RTC clock registers
    RtcRegisters::Register().Set(RtcRegisters::kBackupDomainReset).Save();

    // Manually clear the RTC reset bit
    RtcRegisters::Register().Clear(RtcRegisters::kBackupDomainReset).Save();

    // =========================================================================
    // Step 2. Disable PLL and external clock sources
    // =========================================================================
    ClockControlRegisters::Register()
        // Step 2.1 Disable PLLs
        .Clear(ClockControlRegisters::kPllEnable)
        // Step 2.1 Disable External Oscillators
        .Clear(ClockControlRegisters::kExternalOscEnable)
        .Save();

    // =========================================================================
    // Step 3. Enable External Oscillators
    // =========================================================================
    // Step 3.1 Enable High speed external Oscillator
    if (config_.high_speed_external != 0_MHz)
    {
      ClockControlRegisters::Register()
          .Set(ClockControlRegisters::kExternalOscEnable)
          .Save();

      while (!ClockControlRegisters::Register().Read(
          ClockControlRegisters::kExternalOscReady))
      {
        continue;
      }
    }

    // Step 3.2 Enable Low speed external Oscillator
    if (config_.low_speed_external != 0_MHz)
    {
      RtcRegisters::Register().Set(RtcRegisters::kLowSpeedOscEnable).Save();

      while (!RtcRegisters::Register().Read(RtcRegisters::kLowSpeedOscReady))
      {
        continue;
      }
    }

    // =========================================================================
    // Step 4. Set oscillator source for PLLs
    // =========================================================================
    ClockConfigurationRegisters::Register()
        .Insert((config_.pll.source == PllSource::kHighSpeedExternalDividedBy2),
                ClockConfigurationRegisters::kHsePreDivider)
        .Insert(Value(config_.pll.source),
                ClockConfigurationRegisters::kPllSource)
        .Save();

    // =========================================================================
    // Step 5. Setup PLLs and enable them where necessary
    // =========================================================================
    if (config_.pll.enable)
    {
      ClockConfigurationRegisters::Register()
          .Insert(Value(config_.pll.multiply),
                  ClockConfigurationRegisters::kPllMul)
          .Save();

      ClockControlRegisters::Register()
          .Set(ClockControlRegisters::kPllEnable)
          .Save();

      while (!ClockControlRegisters::Register().Read(
          ClockControlRegisters::kPllReady))
      {
        continue;
      }

      switch (config_.pll.source)
      {
        case PllSource::kHighSpeedInternal:
          pll_clock_rate_ = kHighSpeedInternal / 2;
          break;
        case PllSource::kHighSpeedExternal:
          pll_clock_rate_ = config_.high_speed_external;
          break;
        case PllSource::kHighSpeedExternalDividedBy2:
          pll_clock_rate_ = config_.high_speed_external / 2;
          break;
      }

      // Multiply the PLL clock up to the correct rate.
      pll_clock_rate_ = pll_clock_rate_ * (Value(config_.pll.multiply) + 2);
    }

    // =========================================================================
    // Step 6. Setup peripheral dividers
    // =========================================================================
    ClockConfigurationRegisters::Register()
        // Step 6.1 Set USB divider
        .Insert(Value(config_.pll.usb.divider),
                ClockConfigurationRegisters::kUsbPrescalar)
        // Step 6.2 Set AHB divider
        .Insert(Value(config_.ahb.divider),
                ClockConfigurationRegisters::kAHBDivider)
        // Step 6.3 Set APB1 divider
        .Insert(Value(config_.ahb.apb1.divider),
                ClockConfigurationRegisters::kAPB1Divider)
        // Step 6.4 Set APB2 divider
        .Insert(Value(config_.ahb.apb2.divider),
                ClockConfigurationRegisters::kAPB2Divider)
        // Step 6.5 Set ADC divider
        .Insert(Value(config_.ahb.apb2.adc.divider),
                ClockConfigurationRegisters::kAdcDivider)
        .Save();

    // =========================================================================
    // Step 7. Set System Clock and RTC Clock
    // =========================================================================
    uint32_t target_clock_source = Value(config_.system_clock);

    // Step 7.1 Set the Flash wait states appropriately prior to setting the
    //          system clock frequency. Failure to do this will cause the system
    //          to be unable to read from flash, resulting in the platform
    //          locking up. See p.60 of RM0008 for the Flash ACR register
    if (config_.system_clock == SystemClockSelect::kPll)
    {
      if (pll_clock_rate_ <= 24_MHz)
      {
        // 0 Wait states
        flash->ACR = sjsu::bit::Insert(flash->ACR, 0b000,
                                       sjsu::bit::MaskFromRange(0, 2));
      }
      else if (24_MHz <= pll_clock_rate_ && pll_clock_rate_ <= 48_MHz)
      {
        // 1 Wait state
        flash->ACR = sjsu::bit::Insert(flash->ACR, 0b001,
                                       sjsu::bit::MaskFromRange(0, 2));
      }
      else
      {
        // 2 Wait states
        flash->ACR = sjsu::bit::Insert(flash->ACR, 0b010,
                                       sjsu::bit::MaskFromRange(0, 2));
      }
    }

    // Step 7.2 Set system clock source
    // NOTE: return error if clock = SystemClockSelect::kHighSpeedExternal and
    //       high speed external is not enabled.
    ClockConfigurationRegisters::Register()
        .Insert(Value(config_.system_clock),
                ClockConfigurationRegisters::kSystemClockSelect)
        .Save();

    while (ClockConfigurationRegisters::Register().Extract(
               ClockConfigurationRegisters::kSystemClockStatus) !=
           target_clock_source)
    {
      continue;
    }

    switch (config_.system_clock)
    {
      case SystemClockSelect::kHighSpeedInternal:
        system_clock = kHighSpeedInternal;
        break;
      case SystemClockSelect::kHighSpeedExternal:
        system_clock = config_.high_speed_external;
        break;
      case SystemClockSelect::kPll: system_clock = pll_clock_rate_; break;
    }

    RtcRegisters::Register()
        // Step 7.3 Set the RTC oscillator source
        .Insert(Value(config_.rtc.source), RtcRegisters::kRtcSourceSelect)
        // Step 7.4 Enable/Disable the RTC
        .Insert(config_.rtc.enable, RtcRegisters::kRtcEnable)
        .Save();

    // =========================================================================
    // Step 8. Define the clock rates for the system
    // =========================================================================
    switch (config_.ahb.divider)
    {
      case AHBDivider::kDivideBy1: ahb_clock_rate_ = system_clock / 1; break;
      case AHBDivider::kDivideBy2: ahb_clock_rate_ = system_clock / 2; break;
      case AHBDivider::kDivideBy4: ahb_clock_rate_ = system_clock / 4; break;
      case AHBDivider::kDivideBy8: ahb_clock_rate_ = system_clock / 8; break;
      case AHBDivider::kDivideBy16: ahb_clock_rate_ = system_clock / 16; break;
      case AHBDivider::kDivideBy64: ahb_clock_rate_ = system_clock / 64; break;
      case AHBDivider::kDivideBy128:
        ahb_clock_rate_ = system_clock / 128;
        break;
      case AHBDivider::kDivideBy256:
        ahb_clock_rate_ = system_clock / 256;
        break;
      case AHBDivider::kDivideBy512:
        ahb_clock_rate_ = system_clock / 512;
        break;
    }

    switch (config_.ahb.apb1.divider)
    {
      case APBDivider::kDivideBy1:
        apb1_clock_rate_ = ahb_clock_rate_ / 1;
        break;
      case APBDivider::kDivideBy2:
        apb1_clock_rate_ = ahb_clock_rate_ / 2;
        break;
      case APBDivider::kDivideBy4:
        apb1_clock_rate_ = ahb_clock_rate_ / 4;
        break;
      case APBDivider::kDivideBy8:
        apb1_clock_rate_ = ahb_clock_rate_ / 8;
        break;
      case APBDivider::kDivideBy16:
        apb1_clock_rate_ = ahb_clock_rate_ / 16;
        break;
    }

    switch (config_.ahb.apb2.divider)
    {
      case APBDivider::kDivideBy1:
        apb2_clock_rate_ = ahb_clock_rate_ / 1;
        break;
      case APBDivider::kDivideBy2:
        apb2_clock_rate_ = ahb_clock_rate_ / 2;
        break;
      case APBDivider::kDivideBy4:
        apb2_clock_rate_ = ahb_clock_rate_ / 4;
        break;
      case APBDivider::kDivideBy8:
        apb2_clock_rate_ = ahb_clock_rate_ / 8;
        break;
      case APBDivider::kDivideBy16:
        apb2_clock_rate_ = ahb_clock_rate_ / 16;
        break;
    }

    switch (config_.rtc.source)
    {
      case RtcSource::kNoClock: rtc_clock_rate_ = 0_Hz; break;
      case RtcSource::kLowSpeedInternal:
        rtc_clock_rate_ = kLowSpeedInternal;
        break;
      case RtcSource::kLowSpeedExternal:
        rtc_clock_rate_ = config_.low_speed_external;
        break;
      case RtcSource::kHighSpeedExternalDividedBy128:
        rtc_clock_rate_ = config_.high_speed_external / 128;
        break;
    }

    switch (config_.pll.usb.divider)
    {
      case UsbDivider::kDivideBy1: usb_clock_rate_ = pll_clock_rate_; break;
      case UsbDivider::kDivideBy1Point5:
        usb_clock_rate_ = (pll_clock_rate_ * 2) / 3;
        break;
    }

    switch (config_.ahb.apb1.divider)
    {
      case APBDivider::kDivideBy1:
        timer_apb1_clock_rate_ = apb1_clock_rate_;
        break;
      default: timer_apb1_clock_rate_ = apb1_clock_rate_ * 2; break;
    }

    switch (config_.ahb.apb2.divider)
    {
      case APBDivider::kDivideBy1:
        timer_apb2_clock_rate_ = apb2_clock_rate_;
        break;
      default: timer_apb2_clock_rate_ = apb2_clock_rate_ * 2; break;
    }

    switch (config_.ahb.apb2.adc.divider)
    {
      case AdcDivider::kDivideBy2:
        adc_clock_rate_ = apb2_clock_rate_ / 2;
        break;
      case AdcDivider::kDivideBy4:
        adc_clock_rate_ = apb2_clock_rate_ / 4;
        break;
      case AdcDivider::kDivideBy6:
        adc_clock_rate_ = apb2_clock_rate_ / 6;
        break;
      case AdcDivider::kDivideBy8:
        adc_clock_rate_ = apb2_clock_rate_ / 8;
        break;
    }
  }

  /// @return the a pointer to the clock configuration object used to configure
  /// this system controller.
  void * GetClockConfiguration() override
  {
    return &config_;
  }

  /// @return the clock rate frequency of a peripheral
  units::frequency::hertz_t GetClockRate(ResourceID id) const override
  {
    switch (id.device_id)
    {
      case Peripherals::kI2s.device_id: return pll_clock_rate_;
      case Peripherals::kUsb.device_id: return usb_clock_rate_;
      case Peripherals::kFlitf.device_id: return kHighSpeedInternal;

      // Arm Cortex running clock rate.
      // This code does not utilize the /8 clock for the system timer, thus the
      // clock rate for that subsystem is equal to the CPU running clock.
      case Peripherals::kSystemTimer.device_id: [[fallthrough]];
      case Peripherals::kCpu.device_id: return ahb_clock_rate_;

      // APB1 Timers
      case Peripherals::kTimer2.device_id: [[fallthrough]];
      case Peripherals::kTimer3.device_id: [[fallthrough]];
      case Peripherals::kTimer4.device_id: [[fallthrough]];
      case Peripherals::kTimer5.device_id: [[fallthrough]];
      case Peripherals::kTimer6.device_id: [[fallthrough]];
      case Peripherals::kTimer7.device_id: [[fallthrough]];
      case Peripherals::kTimer12.device_id: [[fallthrough]];
      case Peripherals::kTimer13.device_id: [[fallthrough]];
      case Peripherals::kTimer14.device_id: return timer_apb1_clock_rate_;

      // APB2 Timers
      case Peripherals::kTimer1.device_id: [[fallthrough]];
      case Peripherals::kTimer8.device_id: [[fallthrough]];
      case Peripherals::kTimer9.device_id: [[fallthrough]];
      case Peripherals::kTimer10.device_id: [[fallthrough]];
      case Peripherals::kTimer11.device_id: return timer_apb2_clock_rate_;

      case Peripherals::kAdc1.device_id: [[fallthrough]];
      case Peripherals::kAdc2.device_id: [[fallthrough]];
      case Peripherals::kAdc3.device_id: return adc_clock_rate_;
    }

    if (id.device_id < Peripherals::kAPB1)
    {
      return ahb_clock_rate_;
    }

    if (Peripherals::kAPB1 <= id.device_id && id.device_id < Peripherals::kAPB2)
    {
      return apb1_clock_rate_;
    }

    if (Peripherals::kAPB2 <= id.device_id &&
        id.device_id < Peripherals::kBeyond)
    {
      return apb2_clock_rate_;
    }

    return 0_Hz;
  }

  bool IsPeripheralPoweredUp(ResourceID id) const override
  {
    return bit::Read(*EnableRegister(id), EnableBitPosition(id));
  }

  void PowerUpPeripheral(ResourceID id) const override
  {
    *EnableRegister(id) = bit::Set(*EnableRegister(id), EnableBitPosition(id));
  }

  void PowerDownPeripheral(ResourceID id) const override
  {
    *EnableRegister(id) =
        bit::Clear(*EnableRegister(id), EnableBitPosition(id));
  }

 private:
  volatile uint32_t * EnableRegister(ResourceID id) const
  {
    return enable[id.device_id / kBits];
  }

  uint32_t EnableBitPosition(ResourceID id) const
  {
    return id.device_id % kBits;
  }

  ClockConfiguration & config_;
  units::frequency::hertz_t rtc_clock_rate_        = 0_Hz;
  units::frequency::hertz_t usb_clock_rate_        = 0_Hz;
  units::frequency::hertz_t pll_clock_rate_        = 0_Hz;
  units::frequency::hertz_t ahb_clock_rate_        = 0_Hz;
  units::frequency::hertz_t apb1_clock_rate_       = 0_Hz;
  units::frequency::hertz_t apb2_clock_rate_       = 0_Hz;
  units::frequency::hertz_t timer_apb1_clock_rate_ = 0_Hz;
  units::frequency::hertz_t timer_apb2_clock_rate_ = 0_Hz;
  units::frequency::hertz_t adc_clock_rate_        = 0_Hz;
};

/// Sets the clock rate for all clock systems to their maximum allowable by the
/// ST specification for the stm32f10x.
///
///     - System clock = 72 MHz (maximum)
///     - AHB clock    = 72 MHz (maximum)
///     - APB1 clock   = 36 MHz (maximum)
///     - APB2 clock   = 72 MHz (maximum)
///     - USB clock    = 48 MHz (proper)
///     - ADC clock    = 12 MHz (2 MHz below maximum of 14 MHz)
///
/// @note This will call InitializePlatform() at the end, thus this should not
///       be called within an implementation of `InitializePlatform()` otherwise
///       this will recurse and crash the application.
///
/// @note This function assumes a 8 MHz external crystal to be used.
inline void SetMaximumClockSpeed()
{
  using sjsu::stm32f10x::SystemController;
  auto & config =
      sjsu::SystemController::GetPlatformController()
          .GetClockConfiguration<SystemController::ClockConfiguration>();

  // Set the speed of the high speed external oscillator.
  // NOTE: Change this if its different for your board.
  config.high_speed_external = 8_MHz;
  // Set the source of the PLL's oscillator to 4MHz.
  // See page 93 in RM0008 to see that the internal high speed oscillator,
  // which is 8MHz, is divided by 2 before being fed to the PLL to get 4MHz.
  config.pll.source = SystemController::PllSource::kHighSpeedExternal;
  // Enable PLL to increase the frequency of the system
  config.pll.enable = true;
  // Multiply the 8MHz * 9 => 72 MHz
  config.pll.multiply = SystemController::PllMultiply::kMultiplyBy9;
  // Set the system clock to the PLL
  config.system_clock = SystemController::SystemClockSelect::kPll;
  // APB1's maximum frequency is 36 MHz, so we divide 72 MHz / 2 => 36 MHz.
  config.ahb.apb1.divider = SystemController::APBDivider::kDivideBy2;
  // Keep APB1's clock undivided as it can handle up to 72 MHz.
  config.ahb.apb2.divider = SystemController::APBDivider::kDivideBy1;
  // Maximum frequency for ADC is 14 MHz, thus the best we can do is divide
  // 72 MHz / 6 to get 12 MHz.
  config.ahb.apb2.adc.divider = SystemController::AdcDivider::kDivideBy6;
  // Set USB to divide by 1.5 which will give you 48 MHz which is needed for USB
  // usage.
  config.pll.usb.divider = SystemController::UsbDivider::kDivideBy1Point5;

  // Initialize platform with new clock configuration settings.
  sjsu::InitializePlatform();
}

/// Sets the clock rate for all clock systems to their maximum allowable by the
/// ST specification for the stm32f10x using the internal oscillator.
///
///     - System clock = 64 MHz (maximum)
///     - AHB clock    = 64 MHz (maximum)
///     - APB1 clock   = 32 MHz (maximum)
///     - APB2 clock   = 64 MHz (maximum)
///     - USB clock    = 42 MHz (cannot be used)
///     - ADC clock    = 12 MHz (2 MHz below maximum of 14 MHz)
///
/// @note This will call InitializePlatform() at the end, thus this should not
///       be called within an implementation of `InitializePlatform()` otherwise
///       this will recurse and crash the application.
///
/// @note This will direct everything to use the internal oscillator and will
///       make USB unusable.
inline void SetMaximumClockSpeedUsingInternalOscillator()
{
  using sjsu::stm32f10x::SystemController;
  auto & config =
      sjsu::SystemController::GetPlatformController()
          .GetClockConfiguration<SystemController::ClockConfiguration>();

  // Set the speed of the high speed external oscillator.
  // NOTE: Change this if its different for your board.
  config.high_speed_external = 0_MHz;
  // Set the source of the PLL's oscillator to 4MHz.
  // See page 93 in RM0008 to see that the internal high speed oscillator,
  // which is 8MHz, is divided by 2 before being fed to the PLL to get 4MHz.
  config.pll.source = SystemController::PllSource::kHighSpeedInternal;
  // Enable PLL to increase the frequency of the system
  config.pll.enable = true;
  // Multiply the 8MHz / 2 * 16 => 64 MHz
  config.pll.multiply = SystemController::PllMultiply::kMultiplyBy16;
  // Set the system clock to the PLL
  config.system_clock = SystemController::SystemClockSelect::kPll;
  // APB1's maximum frequency is 32 MHz, so we divide 64 MHz / 2 => 32 MHz.
  config.ahb.apb1.divider = SystemController::APBDivider::kDivideBy2;
  // Keep APB1's clock undivided as it can handle up to 64 MHz.
  config.ahb.apb2.divider = SystemController::APBDivider::kDivideBy1;
  // Maximum frequency for ADC is 14 MHz, thus we divide 64 MHz / 6 to get
  // 12.8 MHz.
  config.ahb.apb2.adc.divider = SystemController::AdcDivider::kDivideBy6;
  // Set USB to divide by 1.5 which will give you 48 MHz which is needed for USB
  // usage.
  config.pll.usb.divider = SystemController::UsbDivider::kDivideBy1Point5;

  // Initialize platform with new clock configuration settings.
  sjsu::InitializePlatform();
}
}  // namespace stm32f10x
}  // namespace sjsu
