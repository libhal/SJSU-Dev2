#pragma once

#include <cstdint>

#include "L0_Platform/msp432p401r/msp432p401r.h"
#include "L1_Peripheral/system_controller.hpp"
#include "utility/bit.hpp"
#include "utility/build_info.hpp"
#include "utility/enum.hpp"
#include "utility/log.hpp"

namespace sjsu
{
namespace msp432p401r
{
/// SystemController class used to manage power control and various clock system
/// resources on the MSP432P401R MCU.
/// @ingroup l1_peripheral
class SystemController final : public sjsu::SystemController
{
 public:
  /// The available internal oscillators for the clock system module.
  enum class Oscillator : uint8_t
  {
    /// Low frequency oscillator (LFXT) with frequency of 32.768 kHz.
    kLowFrequency = 0b000,
    /// Ultra low power oscillator (VLO) with typical frequency of 9.4 kHz.
    kVeryLowFrequency = 0b001,
    /// Low frequency reference oscillator (REFO) that can be configured to
    /// output 32.768 kHz or 128 kHz.
    kReference = 0b010,
    /// Digitally controlled oscillator (DCO) that can be configured to
    /// generate a frequency between 1 MHZ and 48 MHz.
    kDigitallyControlled = 0b011,
    /// Low power oscillator with a typical frequency of 25 MHz.
    kModule = 0b100,
    /// High frequency oscillator (HFXT) which can be driven by an external
    /// oscillator or external square wave with frequency ranging from 1 MHz to
    /// 48 Mhz.
    kHighFrequency = 0b101,
  };

  /// The available system clocks used to drive various peripheral modules where
  /// kAuxiliary, kMaster, kSubsystemMaster, kLowSpeedSubsystemMaster, and
  /// kBackup are the primary clock signals.
  ///
  /// @see Figure 6-1 in the MSP432P4xx Reference Manual.
  ///      https://www.ti.com/lit/ug/slau356i/slau356i.pdf#page=380
  enum class Clock : uint8_t
  {
    /// Auxiliary clock (ACLK) with a max frequency of 128 kHz.
    kAuxiliary = 0,
    /// Master clock (MCLK) that drives the CPU.
    kMaster,
    /// Subsystem master clock (HSMCLK).
    kSubsystemMaster,
    /// Low-speed subsystem master clock (SMCLK).
    kLowSpeedSubsystemMaster,
    /// Low speed backup domain clock (BCLK). The with maximum restricted
    /// frequency of 32.768 kHz.
    kBackup,
    /// Very low frequency low power clock (LFXTCLK). This clock can be driven
    /// by the LFXT oscillator or an external oscillator with a frequency
    /// of 32.768 kHz or less in bypass mode.
    kVeryLowFrequency,
    /// Low frequency clock (REFOCLK).
    kLowFrequencyReference,
    /// Low frequency low power clock (HFXT).
    kLowFrequency,
    /// Low power module clock (MODCLK).
    kModule,
    /// System oscillator clock (SYSCLK).
    kSystem,
  };

  /// Namespace containing the bit masks for the Key Register (CS) which locks
  /// or unlocks the other clock system registers.
  struct KeyRegister  // NOLINT
  {
   public:
    /// The CSKEY bit mask used for locking or unlocking the clock system
    /// registers.
    static constexpr bit::Mask kCsKey = bit::CreateMaskFromRange(0, 15);
  };

  /// Namespace containing the bit masks for the Control 0 Register (CSCTL0)
  /// which controls the configurations for the digitally controlled oscillator.
  struct Control0Register  // NOLINT
  {
   public:
    /// DCO tuning value bit mask.
    static constexpr bit::Mask kTuningSelect = bit::CreateMaskFromRange(0, 9);
    /// DCO frequency seelect bit mask.
    static constexpr bit::Mask kFrequencySelect =
        bit::CreateMaskFromRange(16, 18);
    /// DCO enable bit mask.
    static constexpr bit::Mask kEnable = bit::CreateMaskFromRange(23);
  };

  /// Namespace containing the bit masks for the Control 1 Register (CSCTL1)
  /// which controls the configurations for selecting the oscillator source and
  /// clock divider for the primary clock signals.
  struct Control1Register  // NOLINT
  {
   public:
    /// Master clock source select bit mask.
    static constexpr bit::Mask kMasterClockSourceSelect =
        bit::CreateMaskFromRange(0, 2);
    /// Subsystem master clock source select bit mask.
    static constexpr bit::Mask kSubsystemClockSourceSelect =
        bit::CreateMaskFromRange(4, 6);
    /// Auxiliary clock source select bit mask.
    static constexpr bit::Mask kAuxiliaryClockSourceSelect =
        bit::CreateMaskFromRange(8, 10);
    /// Backup clock source select bit mask.
    static constexpr bit::Mask kBackupClockSourceSelect =
        bit::CreateMaskFromRange(12);
    /// Master clock divider select bit mask.
    static constexpr bit::Mask kMasterClockDividerSelect =
        bit::CreateMaskFromRange(16, 18);
    /// Subsystem master clock divider select bit mask.
    static constexpr bit::Mask kSubsystemClockDividerSelect =
        bit::CreateMaskFromRange(20, 22);
    /// Auxiliary clock divider select bit mask.
    static constexpr bit::Mask kAuxiliaryClockDividerSelect =
        bit::CreateMaskFromRange(24, 26);
    /// Low speed subsystem master clock divider select bit mask.
    static constexpr bit::Mask kLowSpeedSubsystemClockDividerSelect =
        bit::CreateMaskFromRange(28, 30);
  };

  /// Structure containing the necessary DCO configurations to generate a
  /// desired frequency.
  struct DcoConfiguration_t
  {
    /// The 3-bit select code to select the target DCO frequency range.
    uint8_t frequency_select;
    /// The 10-bit signed tuning value used to obtain the target DCO frequency.
    int16_t tuning_value;
  };

  /// Reference to the structure containing the clock system control registers.
  inline static CS_Type * clock_system = msp432p401r::CS;

  /// Reference to the device descriptor tag-length-value (TLV) structure
  /// containing the clock system calibration constants.
  inline static TLV_Type * device_descriptors = msp432p401r::TLV;

  /// Configures the digitally controlled clock (DCO) to produce the desired
  /// target nominal frequency to drive the master clock.
  ///
  /// @param frequency The desired target frequency between 1 MHz and 48 MHz.
  void SetSystemClockFrequency(
      units::frequency::megahertz_t frequency) const override
  {
    const DcoConfiguration_t kDcoConfiguration =
        CalculateDcoConfiguration(frequency);

    UnlockClockSystemRegisters();
    {
      clock_system->CTL0 = bit::Insert(clock_system->CTL0,
                                       kDcoConfiguration.tuning_value,
                                       Control0Register::kTuningSelect);
      clock_system->CTL0 = bit::Insert(clock_system->CTL0,
                                       kDcoConfiguration.frequency_select,
                                       Control0Register::kFrequencySelect);
      clock_system->CTL0 =
          bit::Set(clock_system->CTL0, Control0Register::kEnable);
    }
    LockClockSystemRegisters();

    WaitForClockReadyStatus(Clock::kMaster);
    SetClockSource(Clock::kMaster, Oscillator::kDigitallyControlled);
    SetClockSource(Clock::kSubsystemMaster, Oscillator::kDigitallyControlled);

    speed_in_hertz = frequency;
  }

  /// Sets the divider for the subsystem master clock (HSMCLK).
  ///
  /// @param peripheral_divider Divider value to set. Only the following
  ///                           dividers are available: 1, 2, 4, 8, 16, 32, 64,
  ///                           128.
  void SetPeripheralClockDivider(const PeripheralID &,
                                 uint8_t peripheral_divider) const override
  {
    SetClockDivider(Clock::kSubsystemMaster, peripheral_divider);
  }

  /// @returns The current clock divider used for the subsystem master clock.
  uint32_t GetPeripheralClockDivider(const PeripheralID &) const override
  {
    constexpr uint32_t kDividerValues[] = { 1, 2, 4, 8, 16, 32, 64, 128 };
    const uint32_t kDividerSelect =
        bit::Extract(clock_system->CTL1,
                     Control1Register::kSubsystemClockDividerSelect);
    return kDividerValues[kDividerSelect];
  }

  /// @returns The current system clock frequency.
  units::frequency::hertz_t GetSystemFrequency() const override
  {
    return speed_in_hertz;
  }

  /// Configures the clock divider for one of the four primary clock signals
  /// (ACLK, MCLK, HSMCLK, or SMCLK).
  ///
  /// @param clock   The clock to configure.
  /// @param divider The desired clock divider value. Only the following
  ///                dividers are available: 1, 2, 4, 8, 16, 32, 64, 128.
  void SetClockDivider(Clock clock, uint8_t divider) const
  {
    SJ2_ASSERT_FATAL(
        Value(clock) <= Value(Clock::kLowSpeedSubsystemMaster),
        "Only the following clocks have a ready status: kAuxiliary, kMaster, "
        "kSubsystemMaster, or kLowSpeedSubsystemMaster.");

    uint8_t select_value = 0b000;
    switch (divider)
    {
      case 1: select_value = 0b000; break;
      case 2: select_value = 0b001; break;
      case 4: select_value = 0b010; break;
      case 8: select_value = 0b011; break;
      case 16: select_value = 0b100; break;
      case 32: select_value = 0b101; break;
      case 64: select_value = 0b110; break;
      case 128: select_value = 0b111; break;
      default:
        SJ2_ASSERT_FATAL(false,
                         "Divider must be 1, 2, 4, 8, 16, 32, 64, or 128");
        return;
    }
    constexpr bit::Mask kDividerSelectMasks[] = {
      Control1Register::kAuxiliaryClockDividerSelect,
      Control1Register::kMasterClockDividerSelect,
      Control1Register::kSubsystemClockDividerSelect,
      Control1Register::kLowSpeedSubsystemClockDividerSelect,
    };
    UnlockClockSystemRegisters();
    {
      clock_system->CTL1 = bit::Insert(
          clock_system->CTL1, select_value, kDividerSelectMasks[Value(clock)]);
    }
    LockClockSystemRegisters();
    WaitForClockReadyStatus(clock);
  }

  bool IsPeripheralPoweredUp(const PeripheralID &) const override
  {
    return false;
  }

  void PowerUpPeripheral(const PeripheralID &) const override
  {
    SJ2_ASSERT_FATAL(false, "This function is not implemented.");
  }

  void PowerDownPeripheral(const PeripheralID &) const override
  {
    SJ2_ASSERT_FATAL(false, "This function is not implemented.");
  }

 private:
  /// Unlocks the clock system registers by writing the necessary value to the
  /// CSKEY register.
  void UnlockClockSystemRegisters() const
  {
    constexpr uint16_t kUnlockKey = 0x695A;
    clock_system->KEY =
        bit::Insert(clock_system->KEY, kUnlockKey, KeyRegister::kCsKey);
  }

  /// Locks the clock system registers by writing the necessary value to the
  /// CSKEY register.
  void LockClockSystemRegisters() const
  {
    constexpr uint16_t kLockKey = 0x0000;
    clock_system->KEY =
        bit::Insert(clock_system->KEY, kLockKey, KeyRegister::kCsKey);
  }

  /// Checks and waits for a clock signal to become stable after a frequency or
  /// divider configuration.
  ///
  /// @note This feature is only available for the primary clock signals.
  ///
  /// @param clock The primary clock signal ready status to wait on.
  void WaitForClockReadyStatus(Clock clock) const
  {
    if constexpr (build::kPlatform == build::Platform::host)
    {
      return;
    }
    SJ2_ASSERT_FATAL(
        Value(clock) <= Value(Clock::kBackup),
        "Only the following clocks have a ready status: kAuxiliary, kMaster, "
        "kSubsystemMaster, kLowSpeedSubsystemMaster, or kBackup.");

    constexpr uint8_t kClockReadyBit = 24;
    const uint8_t kOffset            = Value(clock);
    const bit::Mask kReadyBitMask    = bit::CreateMaskFromRange(
        static_cast<uint8_t>(kClockReadyBit + kOffset));
    while (!bit::Read(clock_system->STAT, kReadyBitMask))
    {
      continue;
    }
  }

  /// Configures one of the five primary clock signals (ACLK, MCLK,
  /// HSMCLK / SMCLK, and BCLK) to be sourced by the specified oscillator.
  ///
  /// @note When selecting the oscillator source for either SMCLK or SMCLK, the
  ///       oscillator will applied to both clock signals.
  ///
  /// @see https://www.ti.com/lit/ug/slau356i/slau356i.pdf#page=397
  ///
  /// @param clock      The primary clock to configure.
  /// @param oscillator The oscillator used to source the clock.
  void SetClockSource(Clock clock, Oscillator source) const
  {
    constexpr bit::Mask kPrimaryClockSelectMasks[] = {
      Control1Register::kAuxiliaryClockSourceSelect,
      Control1Register::kMasterClockSourceSelect,
      Control1Register::kSubsystemClockSourceSelect,
      Control1Register::kSubsystemClockSourceSelect,
      Control1Register::kBackupClockSourceSelect,
    };
    uint8_t select_value = Value(source);
    switch (clock)
    {
      case Clock::kMaster: [[fallthrough]];
      case Clock::kSubsystemMaster: [[fallthrough]];
      case Clock::kLowSpeedSubsystemMaster: break;
      case Clock::kAuxiliary:
        SJ2_ASSERT_FATAL(
            Value(source) <= Value(Oscillator::kReference),
            "The auxiliary clock can only be driven by kLowFrequency, "
            "kVeryLowFrequency, or kReference.");
        break;
      case Clock::kBackup:
        switch (source)
        {
          case Oscillator::kLowFrequency: break;
          case Oscillator::kReference: select_value = 0b1; break;
          default:
            SJ2_ASSERT_FATAL(false,
                             "The backup clock can only be driven by the "
                             "kLowFrequency or kReference.");
            return;
        }
        break;
      default:
        SJ2_ASSERT_FATAL(
            false,
            "clock must be one of the five primary clocks: kAuxiliary, "
            "kMaster, kSubsystemMaster, kLowSpeedSubsystemMaster, or kBackup.");
        return;
    }
    clock_system->CTL1 = bit::Insert(clock_system->CTL1,
                                     select_value,
                                     kPrimaryClockSelectMasks[Value(clock)]);
  }

  /// Determines the target frequency range select value and the DCO tuning
  /// value necessary to generate a desired target frequency.
  ///
  /// @see 6.2.8.3 DCO Ranges and Tuning in the MSP432P4xx Reference Manual.
  ///      https://www.ti.com/lit/ug/slau356i/slau356i.pdf#page=386
  ///
  /// @param frequency The target DCO frequency.
  /// @returns A structure containing the DCO configuration values.
  DcoConfiguration_t CalculateDcoConfiguration(
      units::frequency::kilohertz_t frequency) const
  {
    constexpr units::frequency::megahertz_t kMinimumDcoSpeed = 1_MHz;
    constexpr units::frequency::megahertz_t kMaximumDcoSpeed = 48_MHz;
    SJ2_ASSERT_FATAL(
        (kMinimumDcoSpeed <= frequency) && (frequency <= kMaximumDcoSpeed),
        "The desired frequency must be between 1 MHz and 48 MHz.");
    // Determine the DCO tuning configuration values by finding the DCO
    // frequency range, DCO constant, and DCO calibration values based on the
    // desired target frequency.
    constexpr units::frequency::kilohertz_t kDcoCenterFrequencies[] = {
      1500_kHz, 3_MHz, 6_MHz, 12_MHz, 24_MHz, 48_MHz
    };
    uint8_t dco_frequency_select = 0b000;
    float dco_constant =
        static_cast<float>(device_descriptors->DCOIR_CONSTK_RSEL04);
    int32_t dco_calibration =
        static_cast<int32_t>(device_descriptors->DCOIR_FCAL_RSEL04);

    if (frequency >= 32_MHz)
    {
      // target frequency is 32 MHz to 48 MHz
      dco_frequency_select = 0b101;
      dco_constant = static_cast<float>(device_descriptors->DCOIR_CONSTK_RSEL5);
      dco_calibration =
          static_cast<int32_t>(device_descriptors->DCOIR_FCAL_RSEL5);
    }
    else if (frequency >= 16_MHz)
    {
      // target frequency is 16 MHz to 32 MHz
      dco_frequency_select = 0b100;
    }
    else if (frequency >= 8_MHz)
    {
      // target frequency is 8 MHz to 16 MHz
      dco_frequency_select = 0b011;
    }
    else if (frequency >= 4_MHz)
    {
      // target frequency is 4 MHz to 8 MHz
      dco_frequency_select = 0b010;
    }
    else if (frequency >= 2_MHz)
    {
      // target frequency is 2 MHz to 4 MHz
      dco_frequency_select = 0b001;
    }
    else
    {
      // target frequency is 1 MHz to 2 MHz with center frequency of 1.5 MHz
      // use the default initialized values
    }
    // Calculate the signed 10-bit tuning value using Equation 6 from
    // https://www.ti.com/lit/ug/slau356i/slau356i.pdf#page=387
    const auto kCenterFrequency = kDcoCenterFrequencies[dco_frequency_select];
    const float kFrequencyDifference =
        (frequency - kCenterFrequency).to<float>();
    const float kCalibration =
        (1.0f + dco_constant * static_cast<float>(768 - dco_calibration));
    const int16_t kTuneValue =
        static_cast<int16_t>((kFrequencyDifference * kCalibration) /
                             (frequency.to<float>() * dco_constant));

    return DcoConfiguration_t{
      .frequency_select = dco_frequency_select,
      .tuning_value     = kTuneValue,
    };
  }

  /// Current CPU speed. On reset, the default MCLK has a frequency of 3 MHz.
  inline static units::frequency::hertz_t speed_in_hertz = 3_MHz;
};
}  // namespace msp432p401r
}  // namespace sjsu
