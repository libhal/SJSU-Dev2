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
/// System controller for the LPC17xx series of MCUs.
class SystemController final : public sjsu::SystemController
{
 public:
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
    //! @endcond
  };

  /// LPC17xx Peripheral Clock Selection Values:
  /// The `device_id` of each peripheral clock corresponds to the peripheral's
  /// clock selection bit position within the LPC17xx System Controller's
  /// PCLKSEL registers.
  class Clocks
  {
   public:
    //! @cond Doxygen_Suppress
    static constexpr auto kWdt                = PeripheralID::Define<0>();
    static constexpr auto kTimer0             = PeripheralID::Define<1>();
    static constexpr auto kTimer1             = PeripheralID::Define<2>();
    static constexpr auto kUart0              = PeripheralID::Define<3>();
    static constexpr auto kUart1              = PeripheralID::Define<4>();
    static constexpr auto kPwm1               = PeripheralID::Define<6>();
    static constexpr auto kI2c0               = PeripheralID::Define<7>();
    static constexpr auto kSpi                = PeripheralID::Define<8>();
    static constexpr auto kSsp1               = PeripheralID::Define<10>();
    static constexpr auto kDac                = PeripheralID::Define<11>();
    static constexpr auto kAdc                = PeripheralID::Define<12>();
    static constexpr auto kCan1               = PeripheralID::Define<13>();
    static constexpr auto kCan2               = PeripheralID::Define<14>();
    static constexpr auto kAcf                = PeripheralID::Define<15>();
    static constexpr auto kQuadratureEncoder  = PeripheralID::Define<16>();
    static constexpr auto kGpioInt            = PeripheralID::Define<17>();
    static constexpr auto kPowerControlBlock  = PeripheralID::Define<18>();
    static constexpr auto kI2c1               = PeripheralID::Define<19>();
    static constexpr auto kSsp0               = PeripheralID::Define<21>();
    static constexpr auto kTimer2             = PeripheralID::Define<22>();
    static constexpr auto kTimer3             = PeripheralID::Define<23>();
    static constexpr auto kUart2              = PeripheralID::Define<24>();
    static constexpr auto kUart3              = PeripheralID::Define<25>();
    static constexpr auto kI2c2               = PeripheralID::Define<26>();
    static constexpr auto kI2s                = PeripheralID::Define<27>();
    static constexpr auto kRit                = PeripheralID::Define<29>();
    static constexpr auto kSystemControlBlock = PeripheralID::Define<30>();
    static constexpr auto kMotorControlPwm    = PeripheralID::Define<31>();
    // Definitions not associated with a specific peripheral.
    static constexpr auto kCpu = PeripheralID::Define<32>();
    static constexpr auto kUsb = PeripheralID::Define<33>();
    //! @endcond
  };

  /// The available oscillators that can be used to drive the system clock and
  /// subsequently the CPU clock.
  ///
  /// @see 4.4.1 Clock Source Select register
  ///      https://www.nxp.com/docs/en/user-guide/UM10360.pdf#page=36
  enum class Oscillator : uint8_t
  {
    /// Internal RC oscillator as main.
    ///
    /// @warning This oscillator is not a very precise oscillator and cannot
    ///          be used for USB or high speed CAN > 100 kBits/s.
    kIrc = 0b00,
    /// The main oscillator (typically a crystal). This oscillator is connected
    /// externally and can have a frequency raning between 1 MHz to 25 MHz.
    kMain = 0b01,
    /// The RTC oscillator which has a frequency of approximately 32kHz.
    kRtc = 0b10,
  };

  /// The available CPU clock sources.
  enum class CpuClockSource : uint8_t
  {
    /// Uses the system clock.
    kSystemClock = 0b0,
    /// Uses the PLL0 clock.
    kPll0 = 0b1,
  };

  /// The available USB subsystem clock sources.
  ///
  /// @warning PLL0 should not be used as a source if PLL0 is driven by the
  ///          internal oscillator as the clock signal will not be precise
  ///          enough for the USB subsystem.
  enum class UsbClockSource : uint8_t
  {
    kPll0 = 0b0,
    kPll1 = 0b1,
  };

  /// The available USB clock dividers that can be selected when the USB clock
  /// is driven by PLL0. Since PLL0 has an output between 275 MHz to 550 MHz,
  /// only dividers values of 6, 8, and 10 are available in order to obtain a
  /// 48 MHz USB clock.
  enum class UsbClockDivider : uint8_t
  {
    /// The divider to use when PLL0 is configured to output 288 MHz.
    kDivideBy6 = 0b0101,
    /// The divider to use when PLL0 is configured to output 384 MHz.
    kDivideBy8 = 0b0111,
    /// The divider to use when PLL0 is configured to output 480 MHz.
    kDivideBy10 = 0b1001,
  };

  /// Namespace for the Clock Source Select register (CLKSRCSEL) bit masks. The
  /// CLKSRCSEL register is used to select the oscillator used to drive the
  /// system clock and PLL0.
  struct ClockSourceSelectRegister  // NOLINT
  {
    /// Clock source select bit mask.
    static constexpr auto kSelectMask = bit::MaskFromRange(0, 1);
  };

  /// Namespace containing the bit masks for the System Controls and Status
  /// register (SCS) used to configure the main oscillator.
  struct SystemControlsRegister  // NOLINT
  {
    /// Main oscillator frequency range select bit mask.
    static constexpr auto kOscillatorRangeMask = bit::MaskFromRange(4);
    /// Main oscillator enable bit mask.
    static constexpr auto kOscillatorEnableMask = bit::MaskFromRange(5);
    /// Main oscillator status bit mask.
    static constexpr auto kOscillatorStatusMask = bit::MaskFromRange(6);
  };

  /// Namespace containing definitions for PLL0 (Main PLL).
  struct Pll0  // NOLINT
  {
    /// Namespace containing the bit masks for the PLL0 Configuration register
    /// (PLL0CFG).
    struct ConfigurationRegister  // NOLINT
    {
      /// PLL0 multiplier bit mask.
      static constexpr auto kMultiplierMask = bit::MaskFromRange(0, 14);
      /// PLL0 pre-divider bit mask.
      static constexpr auto kPreDividerMask = bit::MaskFromRange(16, 23);
    };

    /// Namespace containing the bit masks for the PLL0 Status register
    /// (PLL0STAT).
    struct StatusRegister  // NOLINT
    {
      /// PLL0 multiplier bit mask.
      static constexpr auto kMultiplierMask = bit::MaskFromRange(0, 14);
      /// PLL0 pre-divider bit mask.
      static constexpr auto kPreDividerMask = bit::MaskFromRange(16, 23);
      /// PLL0 enable status bit mask.
      static constexpr auto kEnableMask = bit::MaskFromRange(24);
      /// PLL0 connection status bit mask.
      static constexpr auto kConnectMask = bit::MaskFromRange(25);
      /// PLL0 lock status bit mask.
      static constexpr auto kLockStatusMask = bit::MaskFromRange(26);
    };
  };

  /// Namespace containing definitions for PLL1 (USB PLL).
  struct Pll1  // NOLINT
  {
    /// The available PLL1 multipliers.
    enum class Multiplier : uint8_t
    {
      /// The multiplier to use when the main oscillator is 12 MHz.
      kMultiplyBy4 = 0b0'0011,
      /// The multiplier to use when the main oscillator is 16 MHz.
      kMultiplyBy3 = 0b0'0010,
      /// The multiplier to use when the main oscillator is 24 MHz.
      kMultiplyBy2 = 0b0'0001,
    };

    /// Namespace containing the bit masks for the PLL1 Configuration register
    /// (PLL1CFG).
    struct ConfigurationRegister  // NOLINT
    {
      /// PLL1 multiplier bit mask
      static constexpr auto kMultiplierMask = bit::MaskFromRange(0, 4);
      /// PLL1 pre-divider bit mask
      static constexpr auto kDividerMask = bit::MaskFromRange(5, 6);
    };

    /// Namespace containing the bit masks for the PLL1 Status register
    /// (PLL1STAT).
    struct StatusRegister  // NOLINT
    {
      /// PLL1 multiplier bit mask
      static constexpr auto kMultiplierMask = bit::MaskFromRange(0, 4);
      /// PLL1 pre-divider bit mask
      static constexpr auto kDividerMask = bit::MaskFromRange(5, 6);
      /// PLL1 enable status bit mask.
      static constexpr auto kEnableMask = bit::MaskFromRange(8);
      /// PLL1 connection status bit mask.
      static constexpr auto kConnectMask = bit::MaskFromRange(9);
      /// PLL1 lock status bit
      static constexpr auto kLockStatusMask = bit::MaskFromRange(10);
    };
  };

  /// Namespace containing the common bit masks for the PLL Control registers
  /// (PLL0CON and PLL1CON).
  struct PllControlRegister  // NOLINT
  {
    /// PLL enable bit mask.
    static constexpr auto kEnableMask = bit::MaskFromRange(0);
    /// PLL connect/disconnect bit mask.
    static constexpr auto kConnectMask = bit::MaskFromRange(1);
  };

  /// Namespace containing the bit masks for the CPU Clock Configuration
  /// register (CCLKCFG).
  struct CpuClockRegister  // NOLINT
  {
    /// The 8-bit CPU clock divider bitmask.
    static constexpr auto kDividerMask = bit::MaskFromRange(0, 7);
  };

  /// Namespace containing the bit masks for the USB Clock Configuration
  /// register (USBCLKCFG).
  struct UsbClockRegister  // NOLINT
  {
    /// The 4-bit USB clock divider bitmask.
    static constexpr auto kDividerMask = bit::MaskFromRange(0, 3);
  };

  /// @see 4.1 Summary of clocking and power control functions
  ///      https://www.nxp.com/docs/en/user-guide/UM10360.pdf#page=31
  struct ClockConfiguration_t
  {
    /// Configurations for the main oscillator. This oscillator can be used as a
    /// clock source to drive the system clock or PLL1.
    struct
    {
      /// Frequency of the main oscillator ranging between 1 MHz to 25 MHz.
      ///
      /// @note This value will vary and should be set based on the external
      ///       oscillator that is being used.
      units::frequency::hertz_t frequency = 0_MHz;
    } main_oscillator = {};

    /// Configurations for the system clock. This clock is used to drive the CPU
    /// clock directly or as an input to PLL0 which subsequently drives the CPU
    /// clock.
    struct
    {
      /// Clock source for the system clock.
      Oscillator clock_source = Oscillator::kIrc;
    } system = {};

    /// Configurations for PLL0 (Main PLL). The default configuration uses the
    /// 4 MHz internal RC as the system clock source to produce a current
    /// controlled oscillator frequency of 288 MHz.
    struct
    {
      /// The 15-bit PLL0 multiplier value ranging from 6 to 512.
      uint16_t multiplier = 36;
      /// The 8-bit PLL0 pre-divider value ranging from 1 to 32.
      uint8_t pre_divider = 1;
    } pll0 = {};

    /// Configurations for Pll1 (USB PLL).
    ///
    /// @see 4.6.9 Procedure for determining PLL1 settings
    ///      https://www.nxp.com/docs/en/user-guide/UM10360.pdf#page=52
    struct
    {
      /// The PLL1 multiplier select.
      Pll1::Multiplier multiplier = Pll1::Multiplier::kMultiplyBy4;
    } pll1 = {};

    /// Configurations for the CPU clock.
    struct
    {
      /// Maximum allowed CPU speed. This value will be 100 MHz or 120 MHz
      /// depending on the MCU in use.
      ///
      /// @see To determine the max CPU speed, see 1.4.1 Part options summary
      ///      https://www.nxp.com/docs/en/user-guide/UM10360.pdf#page=7
      units::frequency::hertz_t max_cpu_clock_rate = 100_MHz;
      /// Clock source for the CPU clock.
      CpuClockSource clock_source = CpuClockSource::kPll0;
      /// The 8-bit CPU clock divider. By default, the divider is set to 6 to
      /// obtain a CPU clock of 48 MHz using the default PLL0 configurations.
      uint8_t divider = 6;
    } cpu = {};

    /// Configurations for the USB subsystem clock.
    struct
    {
      /// Clock source for the USB subsystem clock.
      UsbClockSource clock_source = UsbClockSource::kPll0;
      /// Clock divider for the USB clock.
      ///
      /// @note This value is only used if the PLL0 output is used to drive the
      ///       USB subsystem.
      UsbClockDivider divider = UsbClockDivider::kDivideBy6;
    } usb = {};
  };

  /// Fixed Internal RC frequency.
  static constexpr units::frequency::hertz_t kIrcFrequency = 4_MHz;

  /// Fixed RTC frequency.
  static constexpr units::frequency::hertz_t kRTCFrequency = 32'768_Hz;

  /// The required frequency for the USB clock.
  static constexpr units::frequency::hertz_t kUsbClockFrequency = 48_MHz;

  /// Pointer reference to the system controller peripheral in memory.
  inline static LPC_SC_TypeDef * system_controller = LPC_SC;

  /// @param clock_configuration The desired clock configurations for the
  ///                            system.
  explicit constexpr SystemController(
      ClockConfiguration_t & clock_configuration)
      : clock_configuration_(clock_configuration)
  {
  }

  void Initialize() override
  {
    units::frequency::hertz_t osc_clk =
        clock_configuration_.main_oscillator.frequency;
    units::frequency::hertz_t sys_clk = 0_MHz;
    units::frequency::hertz_t pll_clk = 0_MHz;
    units::frequency::hertz_t usb_clk = 0_MHz;
    uint8_t cpu_clock_divider         = clock_configuration_.cpu.divider;

    // =========================================================================
    // Step 1. Set the internal RC as the clock source for all clocks.
    // =========================================================================
    // Disabling and disconnecting the PLLs sets the CPU and USB clock source to
    // be the system clock.
    DisableAndDisconnectPll(&system_controller->PLL0CON,
                            &system_controller->PLL0FEED);
    DisableAndDisconnectPll(&system_controller->PLL1CON,
                            &system_controller->PLL1FEED);
    // Disable the main oscillator.
    system_controller->SCS = bit::Clear(
        system_controller->SCS, SystemControlsRegister::kOscillatorEnableMask);
    SetSystemClockSource(Oscillator::kIrc);

    // =========================================================================
    // Step 2. Enable the main oscillator if it is used to drive the system
    //         clock and/or PLL1.
    // =========================================================================
    if ((clock_configuration_.usb.clock_source == UsbClockSource::kPll1) ||
        (clock_configuration_.system.clock_source == Oscillator::kMain))
    {
      EnableMainOscillator();
    }

    // =========================================================================
    // Step 3. Determine the running system clock rate based on the selected
    //         oscillator.
    // =========================================================================
    switch (clock_configuration_.system.clock_source)
    {
      case Oscillator::kIrc: sys_clk = kIrcFrequency; break;
      case Oscillator::kMain: sys_clk = osc_clk; break;
      case Oscillator::kRtc: sys_clk = kRTCFrequency; break;
    }

    // =========================================================================
    // Step 4. Configure the CPU clock by configuring and enabling PLL0 if
    //         necessary.
    // =========================================================================
    switch (clock_configuration_.cpu.clock_source)
    {
      case SystemController::CpuClockSource::kSystemClock:
      {
        SetSystemClockSource(clock_configuration_.system.clock_source);
        SetCpuClockDivider(cpu_clock_divider);
        pll_clk = sys_clk;
        break;
      }
      case SystemController::CpuClockSource::kPll0:
      {
        // Before configuring PLL0, verify the PLL0 configurations by checking
        // whether the expected PLL0 current controlled oscillator frequency and
        // the expected CPU frequency are within the valid frequency ranges.

        // Minimum / maximum current controlled oscillator frequencies of PLL0.
        constexpr units::frequency::hertz_t kMinFcco = 275_MHz;
        constexpr units::frequency::hertz_t kMaxFcco = 550_MHz;

        uint16_t multiplier = clock_configuration_.pll0.multiplier;
        uint8_t pre_divider = clock_configuration_.pll0.pre_divider;
        // Determine the running current controlled oscillator frequency based
        // on the configurations.
        pll_clk = (2 * sys_clk * multiplier) / pre_divider;

        SJ2_ASSERT_FATAL(
            (kMinFcco < pll_clk) && (pll_clk < kMaxFcco),
            "Invalid PLL0 multiplier and pre-divider configurations. The "
            "target current controlled oscillator frequency must be between "
            "275 MHz and 550 MHz.");

        auto max_cpu_clock_rate = clock_configuration_.cpu.max_cpu_clock_rate;

        SJ2_ASSERT_FATAL((pll_clk / cpu_clock_divider) < max_cpu_clock_rate,
                         "The configured CPU divider produces a CPU clock that "
                         "exceeds the maximum allowed CPU clock rate.");

        ConfigurePll0();
        break;
      }
    }

    // =========================================================================
    // Step 5. Configure the USB clock by configuring and enabling PLL1 if
    //         necessary.
    // =========================================================================
    switch (clock_configuration_.usb.clock_source)
    {
      case UsbClockSource::kPll0:
      {
        SJ2_ASSERT_FATAL(
            clock_configuration_.cpu.clock_source == CpuClockSource::kPll0,
            "The CPU clock source must be PLL0 in order to use PLL0 as a clock "
            "source for the USB clock.");

        uint8_t usb_clock_divider_select =
            Value(clock_configuration_.usb.divider);
        uint32_t usb_clock_divider = usb_clock_divider_select + 1;
        usb_clk                    = pll_clk / usb_clock_divider;

        SJ2_ASSERT_FATAL(usb_clk == kUsbClockFrequency,
                         "Attempting to use PLL0 to drive the USB clock, but "
                         "the PLL0 configuration and USB clock divider are not "
                         "configured to produce a frequency of 48 MHz.");

        system_controller->USBCLKCFG =
            bit::Insert(system_controller->USBCLKCFG, usb_clock_divider_select,
                        UsbClockRegister::kDividerMask);
        break;
      }
      case UsbClockSource::kPll1:
      {
        uint32_t multiplier = Value(clock_configuration_.pll1.multiplier) + 1;
        usb_clk             = osc_clk * multiplier;

        ConfigurePll1();
        break;
      }
    }

    // =========================================================================
    // Step 6. Set all peripheral clock dividers to 1.
    // =========================================================================
    // SEE: 4.7.3 Peripheral Clock Selection registers 0 and 1
    //      https://www.nxp.com/docs/en/user-guide/UM10360.pdf#page=58
    //
    // NOTE: The reserved bits are left as 0b00.
    system_controller->PCLKSEL0 = 0x5551'5155;
    system_controller->PCLKSEL0 = 0x5455'5455;

    // =========================================================================
    // Step 7. Determine the running CPU and USB clock rates.
    // =========================================================================
    cpu_clock_rate_ = pll_clk / cpu_clock_divider;
    usb_clock_rate_ = usb_clk;
  }

  void * GetClockConfiguration() override
  {
    return &clock_configuration_;
  }

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

  units::frequency::hertz_t GetClockRate(PeripheralID peripheral) const override
  {
    switch (peripheral.device_id)
    {
      case Clocks::kCpu.device_id:
      {
        return cpu_clock_rate_;
      }
      case Clocks::kUsb.device_id:
      {
        return usb_clock_rate_;
      }
      default:  // All other peripherals
      {
        return cpu_clock_rate_;
      }
    }
  }

 private:
  /// Configures the system clock source.
  ///
  /// @param source The oscillator used to drive the system clock.
  void SetSystemClockSource(Oscillator source) const
  {
    system_controller->CLKSRCSEL =
        bit::Insert(system_controller->CLKSRCSEL, Value(source),
                    ClockSourceSelectRegister::kSelectMask);
  }

  /// Configures the System Controls register (SCS) to enable the use of the
  /// main oscillator.
  ///
  /// @see https://www.nxp.com/docs/en/user-guide/UM10360.pdf#page=30
  void EnableMainOscillator() const
  {
    uint32_t system_controls = system_controller->SCS;
    auto frequency           = clock_configuration_.main_oscillator.frequency;

    constexpr units::frequency::hertz_t kMinFrequency = 1_MHz;
    constexpr units::frequency::hertz_t kMaxFrequency = 25_MHz;
    SJ2_ASSERT_FATAL(
        (kMinFrequency <= frequency) && (frequency <= kMaxFrequency),
        "The frequency of the main oscillator must be between 1 "
        "MHz and 25 MHz.");

    if (1_MHz <= frequency && frequency <= 15_MHz)
    {
      system_controls = bit::Clear(
          system_controls, SystemControlsRegister::kOscillatorRangeMask);
    }
    else if (15_MHz <= frequency && frequency <= 25_MHz)
    {
      system_controls = bit::Set(system_controls,
                                 SystemControlsRegister::kOscillatorRangeMask);
    }
    system_controls = bit::Set(system_controls,
                               SystemControlsRegister::kOscillatorEnableMask);

    system_controller->SCS = system_controls;

    // Wait for the main oscillator to become stable before proceeding with any
    // other operations.
    while (!bit::Read(system_controller->SCS,
                      SystemControlsRegister::kOscillatorStatusMask))
    {
      continue;
    }
  }

  /// Disconnect and then disable the specified PLL.
  ///
  /// @param pll_control_register Address of the PLL control register.
  /// @param pll_feed_register Address of the PLL feed register.
  void DisableAndDisconnectPll(volatile uint32_t * pll_control_register,
                               volatile uint32_t * pll_feed_register) const
  {
    // NOTE: It is very important not to merge any steps.

    // =========================================================================
    // Step 1. Disconnect PLL0 with one feed sequence.
    // =========================================================================
    *pll_control_register =
        bit::Clear(*pll_control_register, PllControlRegister::kConnectMask);
    WritePllFeedSequence(pll_feed_register);

    // =========================================================================
    // Step 2. Disable PLL0 with one feed sequence.
    // =========================================================================
    *pll_control_register =
        bit::Clear(*pll_control_register, PllControlRegister::kEnableMask);
    WritePllFeedSequence(pll_feed_register);
  }

  /// Writes the feed sequence that is necessary to lock in any changes to the
  /// PLLCON and PLLCGG registers.
  ///
  /// @param pll_feed_register Address of the PLL feed register to write to.
  void WritePllFeedSequence(volatile uint32_t * pll_feed_register) const
  {
    *pll_feed_register = 0xAA;
    *pll_feed_register = 0x55;
  }

  /// Waits for the PLL to achieve a locked status by checking the specified PLL
  /// status register.
  ///
  /// @param pll_status_register Address of the PLL status register to write to.
  /// @param pll_lock_status_mask The PLL lock status mask to check.
  void WaitForPllLockStatus(volatile uint32_t * pll_status_register,
                            bit::Mask pll_lock_status_mask) const
  {
    while (!bit::Read(*pll_status_register, pll_lock_status_mask))
    {
      continue;
    }
  }

  /// Waits for the PLL to become enabled and connected by checking the
  /// specified PLL status register.
  ///
  /// @param pll_status_register Address of the PLL status register.
  /// @param pll_enable_status_mask The PLL enable status mask to check.
  /// @param pll_connection_status_mask The PLL connection status mask to check.
  void WaitForPllConnectionStatus(volatile uint32_t * pll_status_register,
                                  bit::Mask pll_enable_status_mask,
                                  bit::Mask pll_connection_status_mask) const
  {
    while (!bit::Read(*pll_status_register, pll_enable_status_mask) &&
           !bit::Read(*pll_status_register, pll_connection_status_mask))
    {
      continue;
    }
  }

  /// Configures PLL0 (Main PLL) to produce a desired current conttolled
  /// oscillator frequency ranging from 275 MHz to 550 MHz.
  ///
  /// @see 4.5.13 PLL0 setup sequence
  ///      https://www.nxp.com/docs/en/user-guide/UM10360.pdf#page=48
  void ConfigurePll0() const
  {
    volatile uint32_t * pll_feed_register   = &system_controller->PLL0FEED;
    volatile uint32_t * pll_status_register = &system_controller->PLL0STAT;

    // The following sequence is specified in the LPC176x/5x User Manual
    // Section 4.5.13 and must be followed step by step in order to have PLL0
    // initialized and running.

    // NOTE: It is very important not to merge any steps. For example, do not
    //       update the PLL0CFG and enable PLL0 simultaneously with the same
    //       feed sequence.

    // =========================================================================
    // Step 1. Disconnect PLL0 with one feed sequence if PLL0 is already
    //         connected.
    //
    // Step 2. Disable PLL0 with one feed sequence.
    // =========================================================================
    // NOTE: DisableAndDisconnectPll performs Steps 1 and 2.
    DisableAndDisconnectPll(&system_controller->PLL0CON, pll_feed_register);

    // =========================================================================
    // Step 3. Change the CPU Clock Divider setting to speed up operation
    //         without PLL0, if desired.
    // =========================================================================
    SetCpuClockDivider(1);

    // =========================================================================
    // Step 4. Write to the Clock Source Selection Control register to change
    //         the clock source if needed.
    // =========================================================================
    SetSystemClockSource(clock_configuration_.system.clock_source);

    // =========================================================================
    // Step 5. Write to the PLL0CFG and make it effective with one feed
    //         sequence. The PLL0CFG can only be updated when PLL0 is disabled.
    // =========================================================================
    uint16_t multiplier = clock_configuration_.pll0.multiplier;
    uint8_t pre_divider = clock_configuration_.pll0.pre_divider;
    system_controller->PLL0CFG =
        bit::Insert(system_controller->PLL0CFG, (multiplier - 1),
                    Pll0::ConfigurationRegister::kMultiplierMask);
    system_controller->PLL0CFG =
        bit::Insert(system_controller->PLL0CFG, (pre_divider - 1),
                    Pll0::ConfigurationRegister::kPreDividerMask);
    WritePllFeedSequence(pll_feed_register);

    // =========================================================================
    // Step 6. Enable PLL0 with one feed sequence.
    // =========================================================================
    system_controller->PLL0CON =
        bit::Set(system_controller->PLL0CON, PllControlRegister::kEnableMask);
    WritePllFeedSequence(pll_feed_register);

    // =========================================================================
    // Step 7. Change the CPU Clock Divider setting for the operation with PLL0.
    //         It is critical to do this before connecting PLL0.
    // =========================================================================
    SetCpuClockDivider(clock_configuration_.cpu.divider);

    // =========================================================================
    // Step 8. Wait for PLL0 to achieve lock by monitoring the PLOCK0 bit in the
    //         PLL0STAT register, or using the PLOCK0 interrupt, or wait for a
    //         fixed time when the input clock to PLL0 is slow (i.e. 32 kHz).
    // =========================================================================
    WaitForPllLockStatus(pll_status_register,
                         Pll0::StatusRegister::kLockStatusMask);

    // =========================================================================
    // Step 9. Connect PLL0 with one feed sequence.
    // =========================================================================
    system_controller->PLL0CON =
        bit::Set(system_controller->PLL0CON, PllControlRegister::kConnectMask);
    WritePllFeedSequence(pll_feed_register);

    WaitForPllConnectionStatus(pll_status_register,
                               Pll0::StatusRegister::kEnableMask,
                               Pll0::StatusRegister::kConnectMask);
  }

  /// Configures PLL1 (USB PLL) to produce the required 40 MHz clock to drive
  /// the USB subsystem.
  void ConfigurePll1() const
  {
    // NOTE: It is very important not to merge any steps below. For example, do
    //       not update the PLL1CFG and enable PLL1 simultaneously with the
    //       same feed sequence.

    volatile uint32_t * pll_feed_register   = &system_controller->PLL1FEED;
    volatile uint32_t * pll_status_register = &system_controller->PLL1STAT;

    // =========================================================================
    // Step 1. Disconnect PLL0 with one feed sequence if PLL0 is already
    //         connected.
    //
    // Step 2. Disable PLL0 with one feed sequence.
    // =========================================================================
    // NOTE: DisableAndDisconnectPll performs Steps 1 and 2.
    DisableAndDisconnectPll(&system_controller->PLL1CON, pll_feed_register);

    // =========================================================================
    // Step 3. Write to the PLL1CFG and make it effective with one feed
    //         sequence.
    // =========================================================================
    uint8_t multiplier = Value(clock_configuration_.pll1.multiplier);
    constexpr uint8_t kDividerSelect = 0b01;
    system_controller->PLL1CFG =
        bit::Insert(system_controller->PLL1CFG, multiplier,
                    Pll1::ConfigurationRegister::kMultiplierMask);
    system_controller->PLL1CFG =
        bit::Insert(system_controller->PLL1CFG, kDividerSelect,
                    Pll1::ConfigurationRegister::kDividerMask);
    WritePllFeedSequence(pll_feed_register);

    // =========================================================================
    // Step 4. Enable PLL1 with one feed sequence.
    // =========================================================================
    system_controller->PLL1CON =
        bit::Set(system_controller->PLL1CON, PllControlRegister::kEnableMask);
    WritePllFeedSequence(pll_feed_register);

    // =========================================================================
    // Step 5. Configurations to the PLL must be locked before it can be
    //         connected.
    // =========================================================================
    WaitForPllLockStatus(pll_status_register,
                         Pll1::StatusRegister::kLockStatusMask);

    // =========================================================================
    // Step 6. Connect PLL1 with one feed sequence.
    // =========================================================================
    system_controller->PLL1CON =
        bit::Set(system_controller->PLL1CON, PllControlRegister::kConnectMask);
    WritePllFeedSequence(pll_feed_register);

    WaitForPllConnectionStatus(pll_status_register,
                               Pll1::StatusRegister::kEnableMask,
                               Pll1::StatusRegister::kConnectMask);
  }

  /// Sets divider used for the CPU clock (CCLK).
  ///
  /// @note If PLL0 is connected, a divider value of 1 is not allowed since the
  ///       produced clock rate must not exceed the maximum allowed CPU clock.
  ///
  /// @param cpu_divider 8-bit divider ranging from 1 to 255.
  void SetCpuClockDivider(uint8_t cpu_divider) const
  {
    SJ2_ASSERT_FATAL(cpu_divider != 0, "The CPU clock cannot be divided by 0.");
    system_controller->CCLKCFG =
        bit::Insert(system_controller->CCLKCFG, (cpu_divider - 1),
                    CpuClockRegister::kDividerMask);
  }

  /// Clock system configurations.
  ClockConfiguration_t & clock_configuration_;
  /// The running clock rate of the CPU clock.
  units::frequency::hertz_t cpu_clock_rate_ = 0_MHz;
  /// The running clock rate of the USB subsystem clock.
  units::frequency::hertz_t usb_clock_rate_ = 0_MHz;
};
}  // namespace lpc17xx
}  // namespace sjsu
