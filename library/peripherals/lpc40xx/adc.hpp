#pragma once

#include <cstdint>

#include "platforms/targets/lpc40xx/LPC40xx.h"
#include "peripherals/adc.hpp"
#include "peripherals/lpc40xx/pin.hpp"
#include "peripherals/lpc40xx/system_controller.hpp"
#include "utility/math/bit.hpp"
#include "utility/error_handling.hpp"
#include "utility/log.hpp"
#include "utility/math/units.hpp"

namespace sjsu
{
namespace lpc40xx
{
/// ADC driver for the LPC40xx and LPC17xx microcontrollers.
class Adc final : public sjsu::Adc
{
 public:
  /// Namespace containing the bitmask objects that are used to manipulate the
  /// lpc40xx ADC Control register.
  struct Control  // NOLINT
  {
    /// In burst mode, sets the ADC channels to be automatically converted.
    /// It bit position represents 1 channel with this 8 channel ADC.
    /// In software mode, this should hold only a single 1 for the single
    /// channel to be converted.
    static constexpr bit::Mask kChannelSelect = bit::MaskFromRange(0, 7);

    /// Sets the channel's clock divider. Potentially saving power if clock is
    /// reduced further.
    static constexpr bit::Mask kClockDivider = bit::MaskFromRange(8, 15);

    /// Enable Burst Mode for the ADC. See BurstMode() method of this class to
    /// learn more about what it is and how it works.
    static constexpr bit::Mask kBurstEnable = bit::MaskFromRange(16);

    /// Power on the ADC
    static constexpr bit::Mask kPowerEnable = bit::MaskFromRange(21);

    /// In order to start a conversion a start code must be inserted into this
    /// bit location.
    static constexpr bit::Mask kStartCode = bit::MaskFromRange(24, 26);

    /// Not used in this driver, but allows the use of an external pins to
    /// trigger a conversion. This flag indicates if rising or falling edges
    /// trigger the conversion.
    /// 1 = falling, 0 = rising.
    static constexpr bit::Mask kStartEdge = bit::MaskFromRange(27);
  };

  /// Namespace containing the bitmask objects that are used to manipulate the
  /// lpc40xx ADC Global Data register.
  struct DataRegister  // NOLINT
  {
    /// Result mask holds the latest result from the last ADC that was converted
    static constexpr bit::Mask kResult = bit::MaskFromRange(4, 15);

    /// Converted channel mask indicates which channel was converted in the
    /// latest conversion.
    static constexpr bit::Mask kConvertedChannel = bit::MaskFromRange(24, 26);

    /// Holds whether or not the ADC overran its conversion.
    static constexpr bit::Mask kOverrun = bit::MaskFromRange(30);

    /// Indicates when the ADC conversion is complete.
    static constexpr bit::Mask kDone = bit::MaskFromRange(31);
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
    sjsu::Pin & adc_pin;

    /// Channel number
    uint8_t channel;

    /// Which function code selects the ADC function for the pin, specified in
    /// the adc_pin field.
    uint8_t pin_function;
  };

  /// The default and highest frequency that the ADC can operate at.
  static constexpr units::frequency::hertz_t kClockFrequency = 1_MHz;

  /// A pointer holding the address to the LPC40xx ADC peripheral.
  /// This variable is a dependency injection point for unit testing thus it is
  /// public and mutable. This is needed to perform the "test by side effect"
  /// technique for this class.
  inline static LPC_ADC_TypeDef * adc_base = LPC_ADC;

  /// Number of active bits of the ADC. The ADC is a 12-bit ADC meaning that the
  /// largest value it can have is 2^12 = 4096
  static constexpr uint8_t kActiveBits = 12;

  /// @param channel Passed channel descriptor object. See Channel_t and
  ///        Channel documentation for more details about how to use this.
  explicit Adc(const Channel_t & channel) : channel_(channel) {}

  void ModuleInitialize() override
  {
    sjsu::SystemController::GetPlatformController().PowerUpPeripheral(
        sjsu::lpc40xx::SystemController::Peripherals::kAdc);

    // It is required for proper operation of analog pins for the LPC40xx that
    // the pins be floating.
    channel_.adc_pin.settings.function  = channel_.pin_function;
    channel_.adc_pin.settings.resistor  = PinSettings_t::Resistor::kNone;
    channel_.adc_pin.settings.as_analog = true;
    channel_.adc_pin.Initialize();

    const auto kPeripheralFrequency =
        sjsu::SystemController::GetPlatformController().GetClockRate(
            sjsu::lpc40xx::SystemController::Peripherals::kAdc);

    const uint32_t kClockDivider = kPeripheralFrequency / kClockFrequency;

    bit::Register(&adc_base->CR)
        .Insert(kClockDivider, Control::kClockDivider)
        .Set(Control::kBurstEnable)
        .Set(Control::kPowerEnable)
        .Save();

    bit::Register(&adc_base->CR)
        .Set(bit::MaskFromRange(channel_.channel))
        .Save();
  }

  void ModulePowerDown() override
  {
    bit::Register(&adc_base->CR)
        .Clear(bit::MaskFromRange(channel_.channel))
        .Save();
  }

  uint32_t Read() override
  {
    return bit::Extract(adc_base->DR[channel_.channel], DataRegister::kResult);
  }

  uint8_t GetActiveBits() override
  {
    return kActiveBits;
  }

 private:
  const Channel_t & channel_;
};

template <int channel>
static Adc & GetAdc()
{
  enum AdcMode : uint8_t
  {
    kCh0123Pins = 0b001,
    kCh4567Pins = 0b011
  };

  if constexpr (channel == 0)
  {
    static auto & adc_pin_channel0        = GetPin<0, 23>();
    static const Adc::Channel_t kChannel0 = {
      .adc_pin      = adc_pin_channel0,
      .channel      = 0,
      .pin_function = AdcMode::kCh0123Pins,
    };
    static Adc adc_channel0(kChannel0);
    return adc_channel0;
  }
  else if constexpr (channel == 1)
  {
    static auto & adc_pin_channel1        = GetPin<0, 24>();
    static const Adc::Channel_t kChannel1 = {
      .adc_pin      = adc_pin_channel1,
      .channel      = 1,
      .pin_function = AdcMode::kCh0123Pins,
    };
    static Adc adc_channel1(kChannel1);
    return adc_channel1;
  }
  else if constexpr (channel == 2)
  {
    static auto & adc_pin_channel2        = GetPin<0, 25>();
    static const Adc::Channel_t kChannel2 = {
      .adc_pin      = adc_pin_channel2,
      .channel      = 2,
      .pin_function = AdcMode::kCh0123Pins,
    };
    static Adc adc_channel2(kChannel2);
    return adc_channel2;
  }
  else if constexpr (channel == 3)
  {
    static auto & adc_pin_channel3        = GetPin<0, 26>();
    static const Adc::Channel_t kChannel3 = {
      .adc_pin      = adc_pin_channel3,
      .channel      = 3,
      .pin_function = AdcMode::kCh0123Pins,
    };
    static Adc adc_channel3(kChannel3);
    return adc_channel3;
  }
  else if constexpr (channel == 4)
  {
    static auto & adc_pin_channel4        = GetPin<1, 30>();
    static const Adc::Channel_t kChannel4 = {
      .adc_pin      = adc_pin_channel4,
      .channel      = 4,
      .pin_function = AdcMode::kCh4567Pins,
    };
    static Adc adc_channel4(kChannel4);
    return adc_channel4;
  }
  else if constexpr (channel == 5)
  {
    static auto & adc_pin_channel5        = GetPin<1, 31>();
    static const Adc::Channel_t kChannel5 = {
      .adc_pin      = adc_pin_channel5,
      .channel      = 5,
      .pin_function = AdcMode::kCh4567Pins,
    };
    static Adc adc_channel5(kChannel5);
    return adc_channel5;
  }
  else if constexpr (channel == 6)
  {
    static auto & adc_pin_channel6        = GetPin<0, 12>();
    static const Adc::Channel_t kChannel6 = {
      .adc_pin      = adc_pin_channel6,
      .channel      = 6,
      .pin_function = AdcMode::kCh4567Pins,
    };
    static Adc adc_channel6(kChannel6);
    return adc_channel6;
  }
  else if constexpr (channel == 7)
  {
    static auto & adc_pin_channel7        = GetPin<0, 13>();
    static const Adc::Channel_t kChannel7 = {
      .adc_pin      = adc_pin_channel7,
      .channel      = 7,
      .pin_function = AdcMode::kCh4567Pins,
    };
    static Adc adc_channel7(kChannel7);
    return adc_channel7;
  }
  else
  {
    static_assert(InvalidOption<channel>,
                  "\n\n"
                  "SJSU-Dev2 Compile Time Error:\n"
                  "    LPC40xx only supports ADC channels from 0 to 7. \n"
                  "\n");
    return GetAdc<0>();
  }
}
}  // namespace lpc40xx
}  // namespace sjsu
