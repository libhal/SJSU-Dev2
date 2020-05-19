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
  /// System clock input oscillator options
  enum class OscillatorSource : uint16_t
  {
    /// Internal RC oscillator as main. Not very precise oscillator cannot be
    /// used for USB or high speed CAN.
    kIrc = 0b0,
    /// Use external oscillator (typically a crystal). See PllInput for options
    /// that can be used for the oscillator.
    kExternal = 0b1,
  };

  /// Clock source for the CPU and all Peripherals except for USB and SPIFI
  enum class CpuClockSelect : uint16_t
  {
    /// Use IRC or External oscillator directly
    kSystemClock = 0b00,
    /// Use PLL0 main PLL as the clock source
    kPll0 = 0b01,
  };

  /// USB oscillator source contants (not used)
  enum class UsbClockSelect : uint8_t
  {
    /// Use IRC or external oscillator directly
    kSystemClock = 0b00,
    /// Use PLL0 main PLL as the clock source
    kPll0 = 0b01,
    /// Use PLL1 alternative PLL as the clock source
    kPll1 = 0b10,
  };

  /// USB Clock divider constants
  enum class UsbDivider : uint8_t
  {
    kDivideBy1 = 0,
    kDivideBy2,
    kDivideBy3,
    kDivideBy4,
  };

  /// SPIFI clock options
  enum class SpifiClockSelect : uint8_t
  {
    /// Use IRC or external oscillator directly
    kSystemClock = 0b00,
    /// Use PLL0 main PLL as the clock source
    kPll0 = 0b01,
    /// Use PLL1 alternative PLL as the clock source
    kPll1 = 0b10,
  };

  /// EMC storage clock speed constants
  enum class EmcDivider : bool
  {
    kSameSpeedAsCpu    = 0,
    kHalfTheSpeedOfCpu = 1,
  };

  /// Internal RC oscillator fixed frequency
  static constexpr units::frequency::megahertz_t kDefaultIRCFrequency = 12_MHz;

  /// LPC40xx Peripheral Power On Values:
  /// The kDeviceId of each peripheral corresponds to the peripheral's power on
  /// bit position within the LPC40xx System Controller's PCONP register.
  class Peripherals
  {
   public:
    //! @cond Doxygen_Suppress
    static constexpr auto kLcd               = PeripheralID::Define<0>();
    static constexpr auto kTimer0            = PeripheralID::Define<1>();
    static constexpr auto kTimer1            = PeripheralID::Define<2>();
    static constexpr auto kUart0             = PeripheralID::Define<3>();
    static constexpr auto kUart1             = PeripheralID::Define<4>();
    static constexpr auto kPwm0              = PeripheralID::Define<5>();
    static constexpr auto kPwm1              = PeripheralID::Define<6>();
    static constexpr auto kI2c0              = PeripheralID::Define<7>();
    static constexpr auto kUart4             = PeripheralID::Define<8>();
    static constexpr auto kRtc               = PeripheralID::Define<9>();
    static constexpr auto kSsp1              = PeripheralID::Define<10>();
    static constexpr auto kEmc               = PeripheralID::Define<11>();
    static constexpr auto kAdc               = PeripheralID::Define<12>();
    static constexpr auto kCan1              = PeripheralID::Define<13>();
    static constexpr auto kCan2              = PeripheralID::Define<14>();
    static constexpr auto kGpio              = PeripheralID::Define<15>();
    static constexpr auto kSpifi             = PeripheralID::Define<16>();
    static constexpr auto kMotorControlPwm   = PeripheralID::Define<17>();
    static constexpr auto kQuadratureEncoder = PeripheralID::Define<18>();
    static constexpr auto kI2c1              = PeripheralID::Define<19>();
    static constexpr auto kSsp2              = PeripheralID::Define<20>();
    static constexpr auto kSsp0              = PeripheralID::Define<21>();
    static constexpr auto kTimer2            = PeripheralID::Define<22>();
    static constexpr auto kTimer3            = PeripheralID::Define<23>();
    static constexpr auto kUart2             = PeripheralID::Define<24>();
    static constexpr auto kUart3             = PeripheralID::Define<25>();
    static constexpr auto kI2c2              = PeripheralID::Define<26>();
    static constexpr auto kI2s               = PeripheralID::Define<27>();
    static constexpr auto kSdCard            = PeripheralID::Define<28>();
    static constexpr auto kGpdma             = PeripheralID::Define<29>();
    static constexpr auto kEthernet          = PeripheralID::Define<30>();
    static constexpr auto kUsb               = PeripheralID::Define<31>();
    static constexpr auto kEeprom            = PeripheralID::Define<32>();
    // Definitions not associated with a specific peripheral.
    static constexpr auto kCpu = PeripheralID::Define<33>();
    //! @endcond
  };

  /// Defines the codes for the flash access clock cycles required based on the
  /// CPU clocks speed.
  enum class FlashConfiguration : uint32_t
  {
    /// Flash accesses use 1 CPU clock. Use for up to 20 MHz CPU clock with
    /// power boost off.
    kClock1 = 0b0000 << 12,
    /// Flash accesses use 2 CPU clocks. Use for up to 40 MHz CPU clock with
    /// power boost off.
    kClock2 = 0b0001 << 12,
    /// Flash accesses use 3 CPU clocks. Use for up to 60 MHz CPU clock with
    /// power boost off.
    kClock3 = 0b0010 << 12,
    /// Flash accesses use 4 CPU clocks. Use for up to 80 MHz CPU clock with
    /// power boost off.
    kClock4 = 0b0011 << 12,
    /// Flash accesses use 5 CPU clocks. Use for up to 100 MHz CPU clock with
    /// power boost off. If CPU clock is above 100 MHz, use this but with power
    /// boost on.
    kClock5 = 0b0100 << 12,
    /// Flash accesses use 6 CPU clocks. "Safe" setting for any allowed
    /// conditions.
    kClock6 = 0b0101 << 12,
  };

  /// Namespace for PLL configuration bit masks
  struct PllRegister  // NOLINT
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
  struct OscillatorRegister  // NOLINT
  {
    /// IRC or Main oscillator select bit
    static constexpr bit::Mask kSelect = bit::CreateMaskFromRange(0);

    /// SCS: Main oscillator range select
    static constexpr bit::Mask kRangeSelect = bit::CreateMaskFromRange(4);
    /// SCS: Main oscillator enable
    static constexpr bit::Mask kExternalEnable = bit::CreateMaskFromRange(5);
    /// SCS: Main oscillator ready status
    static constexpr bit::Mask kExternalReady = bit::CreateMaskFromRange(6);
  };

  /// Namespace of Clock register bitmasks
  struct CpuClockRegister  // NOLINT
  {
    /// CPU clock divider amount
    static constexpr bit::Mask kDivider = bit::CreateMaskFromRange(0, 4);
    /// CPU clock source select bit
    static constexpr bit::Mask kSelect = bit::CreateMaskFromRange(8);
  };

  /// Namespace of Peripheral register bitmasks
  struct PeripheralClockRegister  // NOLINT
  {
    /// Main single peripheral clock divider shared across all peripherals,
    /// except for USB and SPIFI.
    static constexpr bit::Mask kDivider = bit::CreateMaskFromRange(0, 4);
  };

  /// Namespace of EMC register bitmasks
  struct EmcClockRegister  // NOLINT
  {
    /// EMC Clock Register divider bit
    static constexpr bit::Mask kDivider = bit::CreateMaskFromRange(0);
  };

  /// Namespace of USB register bitmasks
  struct UsbClockRegister  // NOLINT
  {
    /// USB clock divider constant
    static constexpr bit::Mask kDivider = bit::CreateMaskFromRange(0, 4);
    /// USB clock source select bit
    static constexpr bit::Mask kSelect = bit::CreateMaskFromRange(8, 9);
  };

  /// Namespace of SPIFI register bitmasks
  struct SpiFiClockRegister  // NOLINT
  {
    /// SPIFI clock divider constant
    static constexpr bit::Mask kDivider = bit::CreateMaskFromRange(0, 4);
    /// SPIFI clock source select bit
    static constexpr bit::Mask kSelect = bit::CreateMaskFromRange(8, 9);
  };

  /// Clock configuration structure for use the lpc40xx microcontrollers
  /// See page 21 of a diagram of the clock tree that this datastructure
  /// represents: datasheets/sjtwo/LPC40xx/UM10562.pdf
  struct ClockConfiguration  // NOLINT
  {
    // =========================================================================
    // Oscillator Settings
    // =========================================================================

    /// The frequency of the external oscillator crystal. This field must not be
    /// set to 0_Mhz if an external oscillator is to be used anywhere in the
    /// system.
    units::frequency::megahertz_t external_oscillator_frequency = 0_MHz;

    /// The choice for system oscillator. If the external oscillator is used,
    /// then the `external_oscillator_frequency` must be some non-zero value.
    OscillatorSource system_oscillator = OscillatorSource::kIrc;

    // =========================================================================
    // PLL Configuration
    // =========================================================================

    /// Represents the two PLLs in the clock tree
    struct
    {
      /// Whether the PLL should be enabled or not.
      bool enabled = false;
      /// The output rate multiplication factor of the PLL. If the input is 12
      /// MHz and the multiply constant is 2, then the output frequency will be
      /// 24 MHz. The limit for this can be found in the
      ///    datasheets/sjtwo/LPC40xx/UM10562.pdf
      ///     or
      ///    datasheets/lpc40xx/UM10562.pdf
      uint8_t multiply = 1;
    } pll[2] = {};

    // =========================================================================
    // Clock Sources and Dividers
    // =========================================================================

    /// Represents the configuration options for the CPU
    struct
    {
      CpuClockSelect clock = CpuClockSelect::kSystemClock;
      uint8_t divider      = 1;
    } cpu = {};

    /// Represents the configuration options for USB
    struct
    {
      UsbClockSelect clock = UsbClockSelect::kSystemClock;
      UsbDivider divider   = UsbDivider::kDivideBy1;
    } usb = {};

    /// Represents the configuration options for SPIFI
    struct
    {
      SpifiClockSelect clock = SpifiClockSelect::kSystemClock;
      uint8_t divider        = 1;
    } spifi = {};

    /// Defines the peripheral clock divider amount
    uint8_t peripheral_divider = 1;
    /// Defines the EMC clock divider amount
    EmcDivider emc_divider = EmcDivider::kSameSpeedAsCpu;
  };

  /// Pointer to the system controller peripheral in memory.
  inline static LPC_SC_TypeDef * system_controller = LPC_SC;

  /// @param clock_configuration - Must have a valid clock configuration object
  /// for lpc40xx passed to this object for it to work. A default initialized
  /// ClockConfiguration will work if passed.
  explicit constexpr SystemController(ClockConfiguration & clock_configuration)
      : clock_configuration_(clock_configuration)
  {
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
      case Peripherals::kUsb.device_id:
      {
        return usb_clock_rate_;
      }
      case Peripherals::kSpifi.device_id:
      {
        return spifi_clock_rate_;
      }
      case Peripherals::kEmc.device_id:
      {
        return emc_clock_rate_;
      }
      case Peripherals::kCpu.device_id:
      {
        return cpu_clock_rate_;
      }
      default:  // All other peripherals
      {
        return peripheral_clock_rate_;
      }
    }
  }

  void Initialize() override
  {
    LPC_SC_TypeDef * sys = system_controller;
    auto config          = clock_configuration_;

    units::frequency::hertz_t system_clock = 0_Hz;
    units::frequency::hertz_t pll0         = 0_Hz;
    units::frequency::hertz_t pll1         = 0_Hz;

    // Reset all of the cached clock frequency values
    units::frequency::hertz_t cpu   = 0_Hz;
    units::frequency::hertz_t usb   = 0_Hz;
    units::frequency::hertz_t spifi = 0_Hz;

    // =========================================================================
    // Step 1. Select IRC as clock source for everything.
    //         Make sure PLLs are not clock sources for everything.
    // =========================================================================
    // Set CPU clock to system clock
    sys->CCLKSEL =
        bit::Insert(sys->CCLKSEL, Value(CpuClockSelect::kSystemClock),
                    CpuClockRegister::kSelect);
    // Set USB clock to system clock
    sys->USBCLKSEL =
        bit::Insert(sys->USBCLKSEL, Value(UsbClockSelect::kSystemClock),
                    UsbClockRegister::kSelect);
    // Set SPIFI clock to system clock
    sys->SPIFISEL =
        bit::Insert(sys->SPIFISEL, Value(SpifiClockSelect::kSystemClock),
                    SpiFiClockRegister::kSelect);

    // Set the clock source to IRC and not external oscillator. The next phase
    // disables that clock source, which will stop the system if this is not
    // switched.
    sys->CLKSRCSEL = Value(OscillatorSource::kIrc);

    // =========================================================================
    // Step 2. Disable PLLs
    // =========================================================================
    // NOTE: The only bit in this register that is used is bit 0 which indicates
    // enabled or disabled status, thus a single assignment is needed.
    sys->PLL0CON = 0;
    sys->PLL1CON = 0;
    // Disabling external oscillator if it is not going to be used
    system_controller->SCS =
        bit::Clear(system_controller->SCS, OscillatorRegister::kExternalEnable);

    // =========================================================================
    // Step 3. Select oscillator source for System Clock and Main PLL
    // =========================================================================
    // Enable the external oscillator if we are using it, which would be the
    // case if the alternative PLL is enabled or external oscillator is
    // selected.
    if (config.system_oscillator == OscillatorSource::kExternal ||
        config.pll[1].enabled)
    {
      EnableExternalOscillator();
    }

    sys->CLKSRCSEL = Value(config.system_oscillator);

    switch (config.system_oscillator)
    {
      case OscillatorSource::kIrc: system_clock = kDefaultIRCFrequency; break;
      case OscillatorSource::kExternal:
        system_clock = config.external_oscillator_frequency;
        break;
    }

    // =========================================================================
    // Step 4. Configure PLLs
    // =========================================================================
    pll0 = SetupPll(&sys->PLL0CON, &sys->PLL0CFG, &sys->PLL0FEED,
                    &sys->PLL0STAT, 0);

    pll1 = SetupPll(&sys->PLL1CON, &sys->PLL1CFG, &sys->PLL1FEED,
                    &sys->PLL1STAT, 1);

    // =========================================================================
    // Step 5. Set clock dividers for each clock source
    // =========================================================================
    // Set CPU clock divider
    sys->CCLKSEL = bit::Insert(sys->CCLKSEL, config.cpu.divider,
                               CpuClockRegister::kDivider);
    // Set EMC clock divider
    sys->EMCCLKSEL = bit::Insert(sys->EMCCLKSEL, Value(config.emc_divider),
                                 EmcClockRegister::kDivider);
    // Set Peripheral clock divider
    sys->PCLKSEL = bit::Insert(sys->PCLKSEL, config.peripheral_divider,
                               PeripheralClockRegister::kDivider);
    // Set USB clock divider
    sys->USBCLKSEL = bit::Insert(sys->USBCLKSEL, Value(config.usb.divider),
                                 UsbClockRegister::kDivider);
    // Set SPIFI clock divider
    sys->SPIFISEL = bit::Insert(sys->SPIFISEL, config.spifi.divider,
                                SpiFiClockRegister::kDivider);

    switch (config.cpu.clock)
    {
      case CpuClockSelect::kSystemClock: cpu = system_clock; break;
      case CpuClockSelect::kPll0: cpu = pll0; break;
    }

    switch (config.usb.clock)
    {
      case UsbClockSelect::kSystemClock: usb = system_clock; break;
      case UsbClockSelect::kPll0: usb = pll0; break;
      case UsbClockSelect::kPll1: usb = pll1; break;
    }

    switch (config.spifi.clock)
    {
      case SpifiClockSelect::kSystemClock: spifi = system_clock; break;
      case SpifiClockSelect::kPll0: spifi = pll0; break;
      case SpifiClockSelect::kPll1: spifi = pll1; break;
    }

    cpu_clock_rate_        = cpu / config.cpu.divider;
    peripheral_clock_rate_ = cpu / config.peripheral_divider;
    emc_clock_rate_        = cpu / (Value(config.emc_divider) + 1);
    usb_clock_rate_        = usb / Value(config.usb.divider);
    spifi_clock_rate_      = spifi / config.spifi.divider;

    // =========================================================================
    // Step 6. Configure flash cycles per load
    // =========================================================================
    sys->PBOOST   = 0b00;

    if (cpu_clock_rate_ < 20_MHz)
    {
      sys->FLASHCFG = Value(FlashConfiguration::kClock1);
    }
    else if (20_MHz <= cpu_clock_rate_ && cpu_clock_rate_ < 40_MHz)
    {
      sys->FLASHCFG = Value(FlashConfiguration::kClock2);
    }
    else if (40_MHz <= cpu_clock_rate_ && cpu_clock_rate_ < 60_MHz)
    {
      sys->FLASHCFG = Value(FlashConfiguration::kClock3);
    }
    else if (60_MHz <= cpu_clock_rate_ && cpu_clock_rate_ < 80_MHz)
    {
      sys->FLASHCFG = Value(FlashConfiguration::kClock4);
    }
    else if (80_MHz <= cpu_clock_rate_ && cpu_clock_rate_ < 100_MHz)
    {
      sys->FLASHCFG = Value(FlashConfiguration::kClock5);
    }
    else if (cpu_clock_rate_ >= 100_MHz)
    {
      sys->FLASHCFG = Value(FlashConfiguration::kClock5);
      sys->PBOOST   = 0b11;
    }

    // =========================================================================
    // Step 7. Finally select the sources for each clock
    // =========================================================================
    // Set CPU clock the source defined in the configuration
    sys->CCLKSEL = bit::Insert(sys->CCLKSEL, Value(config.cpu.clock),
                               CpuClockRegister::kSelect);

    // Set USB clock the source defined in the configuration
    sys->USBCLKSEL = bit::Insert(sys->USBCLKSEL, Value(config.usb.clock),
                                 UsbClockRegister::kSelect);

    // Set SPIFI clock the source defined in the configuration
    sys->SPIFISEL = bit::Insert(sys->SPIFISEL, Value(config.spifi.clock),
                                SpiFiClockRegister::kSelect);
  }

 private:
  units::frequency::hertz_t SetupPll(volatile uint32_t * control,
                                     volatile uint32_t * config,
                                     volatile uint32_t * feed,
                                     volatile uint32_t * stat,
                                     int pll_index) const
  {
    const auto & pll_config = clock_configuration_.pll[pll_index];
    units::frequency::hertz_t fcco;

    if (pll_config.enabled)
    {
      *config = bit::Insert(*config, pll_config.multiply - 1,
                            PllRegister::kMultiplier);

      if (clock_configuration_.system_oscillator == OscillatorSource::kIrc &&
          pll_index == 0)
      {
        fcco = kDefaultIRCFrequency * pll_config.multiply;
      }
      else
      {
        fcco = clock_configuration_.external_oscillator_frequency *
               pll_config.multiply;
      }

      // In the datasheet this is the divider, but it acts to multiply the
      // frequency higher to a point where the fcco is stable.
      //
      // fcco must be between 156 MHz to 320 MHz.
      uint32_t fcco_divide = 0;
      for (int divide_codes : { 0, 1, 2, 3 })
      {
        // Multiply the fcco by 2^divide_code
        units::frequency::hertz_t final_fcco = fcco * (1 << divide_codes);
        if (156_MHz <= final_fcco && final_fcco <= 320_MHz)
        {
          fcco_divide = divide_codes;
          break;
        }
      }

      SJ2_ASSERT_FATAL(
          fcco_divide != 0,
          "The provided multiply value and oscillator choices results in "
          "FCCO frequency outside of 156 MHz and 320 MHz. See page 65 "
          "UM10562 LPC408x/407x User manual for more details.");

      *config = bit::Insert(*config, fcco_divide, PllRegister::kDivider);
      // Enable PLL
      *control = 1;
      // Feed PLL in order to start the locking process
      *feed = 0xAA;
      *feed = 0x55;

      while (!bit::Read(*stat, PllRegister::kPllLockStatus))
      {
        continue;
      }
    }

    return fcco;
  }

  void EnableExternalOscillator() const
  {
    auto frequency = clock_configuration_.external_oscillator_frequency;
    if (1_MHz <= frequency && frequency <= 20_MHz)
    {
      system_controller->SCS =
          bit::Clear(system_controller->SCS, OscillatorRegister::kRangeSelect);
    }
    else if (20_MHz <= frequency && frequency <= 25_MHz)
    {
      system_controller->SCS =
          bit::Set(system_controller->SCS, OscillatorRegister::kRangeSelect);
    }

    SJ2_ASSERT_FATAL(
        1_MHz <= frequency && frequency <= 25_MHz,
        "External Oscillator Frequency is outside of the the acceptable 1 "
        "MHz <--> 25 MHz.");

    system_controller->SCS =
        bit::Set(system_controller->SCS, OscillatorRegister::kExternalEnable);

    while (
        !bit::Read(system_controller->SCS, OscillatorRegister::kExternalReady))
    {
      continue;
    }
  }

  ClockConfiguration & clock_configuration_;
  units::frequency::hertz_t cpu_clock_rate_        = 0_Hz;
  units::frequency::hertz_t peripheral_clock_rate_ = 0_Hz;
  units::frequency::hertz_t emc_clock_rate_        = 0_Hz;
  units::frequency::hertz_t usb_clock_rate_        = 0_Hz;
  units::frequency::hertz_t spifi_clock_rate_      = 0_Hz;
};
}  // namespace lpc40xx
}  // namespace sjsu
