#pragma once

#include <cstdint>

#include "L1_Peripheral/adc.hpp"

#include "L0_Platform/lpc40xx/LPC40xx.h"
#include "L1_Peripheral/lpc40xx/pin.hpp"
#include "L1_Peripheral/lpc40xx/system_controller.hpp"
#include "utility/log.hpp"
#include "utility/status.hpp"
#include "utility/units.hpp"

namespace sjsu
{
namespace lpc40xx
{
/// ADC driver for the LPC40xx and LPC17xx microcontrollers.
class Adc final : public sjsu::Adc
{
 public:
  /// Namespace containing the bitmask objects that are used to manipulate the
  /// lpc40xx's ADC Control register.
  struct Control  // NOLINT
  {
    /// Set which ADC channels are enabled. It bit position represents 1 channel
    /// with this 8 channel ADC.
    static constexpr bit::Mask kChannelEnable = bit::CreateMaskFromRange(0, 7);
    /// Sets the channel's clock divider. Potentially saving power if clock is
    /// reduced further.
    static constexpr bit::Mask kClockDivider = bit::CreateMaskFromRange(8, 15);
    /// Enable Burst Mode for the ADC. See BurstMode() method of this class to
    /// learn more about what it is and how it works.
    static constexpr bit::Mask kBurstEnable = bit::CreateMaskFromRange(16);
    /// Power on the ADC
    static constexpr bit::Mask kPowerEnable = bit::CreateMaskFromRange(21);
    /// In order to start a conversion a start code must be inserted into this
    /// bit location.
    static constexpr bit::Mask kStartCode = bit::CreateMaskFromRange(24, 26);
    /// Not used in this driver, but allows the use of an external pins to
    /// trigger a conversion. This flag indicates if rising or falling edges
    /// trigger the conversion.
    /// 1 = falling, 0 = rising.
    static constexpr bit::Mask kStartEdge = bit::CreateMaskFromRange(27);
  };
  /// Namespace containing the bitmask objects that are used to manipulate the
  /// lpc40xx's ADC Global Data register.
  struct GlobalData  // NOLINT
  {
    /// Result mask holds the latest result from the last ADC that was converted
    static constexpr bit::Mask kResult = bit::CreateMaskFromRange(4, 15);
    /// Converted channel mask indicates which channel was converted in the
    /// latest conversion.
    static constexpr bit::Mask kConvertedChannel =
        bit::CreateMaskFromRange(24, 26);
    /// Holds whether or not the ADC overran its conversion.
    static constexpr bit::Mask kOverrun = bit::CreateMaskFromRange(30);
    /// Indicates when the ADC conversion is complete.
    static constexpr bit::Mask kDone = bit::CreateMaskFromRange(31);
  };
  /// Structure that defines a channel's pin, pin's function code and channel
  /// number.
  ///
  /// Usage:
  ///
  /// ```
  /// sjsu::lpc40xx::Pin adc_pin(/* adc port number */, /* adc pin number */);
  /// const sjsu::lpc40xx::Adc::Channel_t kCustomChannelX = {
  ///   .adc_pin      = adc_pin,
  ///   .channel      = /* insert correct channel here */,
  ///   .pin_function = 0b101,
  /// };
  /// sjsu::lpc40xx::Adc channelX_adc(kCustomChannelX);
  /// ```
  struct Channel_t
  {
    /// Reference to the pin associated with the adc channel.
    const sjsu::Pin & adc_pin;
    /// Channel number
    uint8_t channel : 3;
    /// Which function code selects the ADC function for the pin, specified in
    /// the adc_pin field.
    uint8_t pin_function : 3;
  };
  /// Namespace containing predefined Channel_t description objects. These
  /// objects can be passed directly to the constructor of an lpc40xx::Adc
  /// object.
  ///
  /// Usage:
  ///
  /// ```
  /// sjsu::lpc40xx::Adc adc(sjsu::lpc40xx::Adc::Channel::kChannel0);
  /// ```
  struct Channel  // NOLINT
  {
   private:
    enum AdcMode : uint8_t
    {
      kCh0123Pins = 0b001,
      kCh4567Pins = 0b011
    };
    inline static const Pin kAdcPinChannel0 = Pin::CreatePin<0, 23>();
    inline static const Pin kAdcPinChannel1 = Pin::CreatePin<0, 24>();
    inline static const Pin kAdcPinChannel2 = Pin::CreatePin<0, 25>();
    inline static const Pin kAdcPinChannel3 = Pin::CreatePin<0, 26>();
    inline static const Pin kAdcPinChannel4 = Pin::CreatePin<1, 30>();
    inline static const Pin kAdcPinChannel5 = Pin::CreatePin<1, 31>();
    inline static const Pin kAdcPinChannel6 = Pin::CreatePin<0, 12>();
    inline static const Pin kAdcPinChannel7 = Pin::CreatePin<0, 13>();

   public:
    /// Predefined channel information for channel 0.
    /// Pass this to the lpc17xx::Adc class to utilize adc channel0.
    inline static const Channel_t kChannel0 = {
      .adc_pin      = kAdcPinChannel0,
      .channel      = 0,
      .pin_function = AdcMode::kCh0123Pins,
    };
    /// Predefined channel information for channel 1.
    /// Pass this to the lpc17xx::Adc class to utilize adc channel1.
    inline static const Channel_t kChannel1 = {
      .adc_pin      = kAdcPinChannel1,
      .channel      = 1,
      .pin_function = AdcMode::kCh0123Pins,
    };
    /// Predefined channel information for channel 2.
    /// Pass this to the lpc17xx::Adc class to utilize adc channel2.
    inline static const Channel_t kChannel2 = {
      .adc_pin      = kAdcPinChannel2,
      .channel      = 2,
      .pin_function = AdcMode::kCh0123Pins,
    };
    /// Predefined channel information for channel 3.
    /// Pass this to the lpc17xx::Adc class to utilize adc channel3.
    inline static const Channel_t kChannel3 = {
      .adc_pin      = kAdcPinChannel3,
      .channel      = 3,
      .pin_function = AdcMode::kCh0123Pins,
    };
    /// Predefined channel information for channel 4.
    /// Pass this to the lpc17xx::Adc class to utilize adc channel4.
    inline static const Channel_t kChannel4 = {
      .adc_pin      = kAdcPinChannel4,
      .channel      = 4,
      .pin_function = AdcMode::kCh4567Pins,
    };
    /// Predefined channel information for channel 5.
    /// Pass this to the lpc17xx::Adc class to utilize adc channel5.
    inline static const Channel_t kChannel5 = {
      .adc_pin      = kAdcPinChannel5,
      .channel      = 5,
      .pin_function = AdcMode::kCh4567Pins,
    };
    /// Predefined channel information for channel 6.
    /// Pass this to the lpc17xx::Adc class to utilize adc channel6.
    inline static const Channel_t kChannel6 = {
      .adc_pin      = kAdcPinChannel6,
      .channel      = 6,
      .pin_function = AdcMode::kCh4567Pins,
    };
    /// Predefined channel information for channel 7.
    /// Pass this to the lpc17xx::Adc class to utilize adc channel7.
    inline static const Channel_t kChannel7 = {
      .adc_pin      = kAdcPinChannel7,
      .channel      = 7,
      .pin_function = AdcMode::kCh4567Pins,
    };
  };

  /// The default and highest frequence that the ADC can operate at.
  static constexpr units::frequency::hertz_t kClockFrequency = 1_MHz;
  /// A pointer holding the address to the LPC40xx ADC peripheral.
  /// This variable is a dependency injection point for unit testing thus it is
  /// public and mutable. This is needed to perform the "test by side effect"
  /// technique for this class.
  inline static LPC_ADC_TypeDef * adc_base = LPC_ADC;
  /// Number of active bits of the ADC. The ADC is a 12-bit ADC meaning that the
  /// largest value it can have is 2^12 = 4096
  static constexpr uint8_t kActiveBits = 12;
  /// Turn on or off burst mode for the whole ADC peripheral.
  /// Normally, and ADC conversion must be triggered. After triggering
  /// conversion, one must wait until the conversion is complete before
  /// retrieving the converted analog voltage to digital value.
  ///
  /// Burst mode is an ADC mode that makes the ADC peripheral continously sample
  /// it's input channels without CPU intervention, meaning that the CPU can
  /// simply read back the value in the conversion register to get the current
  /// converted voltage.
  ///
  /// @param turn_burst_mode_on: if true, will turn on burst mode.
  static void BurstMode(bool turn_burst_mode_on = true)
  {
    adc_base->CR =
        bit::Insert(adc_base->CR, turn_burst_mode_on, Control::kBurstEnable);
  }
  /// @param channel: Passed channel descriptor object. See Channel_t and
  ///        Channel documentation for more details about how to use this.
  ///
  /// @param system_controller: pass a system controller object to the ADC.
  ///        typically this parameter is used for testing this class and for
  ///        choosing the lpc17xx system controller when using this class on
  ///        that platform.
  explicit constexpr Adc(const Channel_t & channel,
                         const sjsu::SystemController & system_controller =
                             DefaultSystemController())
      : channel_(channel), system_controller_(system_controller)
  {
  }
  Status Initialize() const override
  {
    system_controller_.PowerUpPeripheral(
        sjsu::lpc40xx::SystemController::Peripherals::kAdc);

    channel_.adc_pin.SetPinFunction(channel_.pin_function);
    channel_.adc_pin.SetPull(sjsu::Pin::Resistor::kNone);
    channel_.adc_pin.SetAsAnalogMode(true);

    const units::frequency::hertz_t kPeripheralFrequency =
        system_controller_.GetPeripheralFrequency(
            sjsu::lpc40xx::SystemController::Peripherals::kAdc);
    uint32_t clock_divider =
        (kPeripheralFrequency / kClockFrequency).to<uint32_t>();

    uint32_t control = adc_base->CR;

    control = bit::Set(control, channel_.channel);
    control = bit::Set(control, Control::kPowerEnable.position);
    control = bit::Insert(control, clock_divider, Control::kClockDivider);

    adc_base->CR = control;

    return Status::kSuccess;
  }
  void Conversion() const override
  {
    if (bit::Read(adc_base->CR, Control::kBurstEnable.position))
    {
      // NOTE: If burst mode is enabled, conversion start must be set 0
      adc_base->CR = bit::Insert(adc_base->CR, 0, Control::kStartCode);
    }
    else
    {
      // NOTE: 0x01 = start code for "Start Conversion Now"
      adc_base->CR = bit::Insert(adc_base->CR, 1, Control::kStartCode);
    }

    while (!HasConversionFinished())
    {
      continue;
    }
  }
  uint32_t Read() const override
  {
    uint32_t result =
        bit::Extract(adc_base->DR[channel_.channel], GlobalData::kResult);
    return result;
  }
  bool HasConversionFinished() const override
  {
    return bit::Read(adc_base->DR[channel_.channel],
                     GlobalData::kDone.position);
  }
  uint8_t GetActiveBits() const override
  {
    return kActiveBits;
  }

 private:
  const Channel_t & channel_;
  const sjsu::SystemController & system_controller_;
};
}  // namespace lpc40xx
}  // namespace sjsu
