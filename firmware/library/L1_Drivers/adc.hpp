#pragma once

#include <cstdint>

#include "L0_LowLevel/LPC40xx.h"
#include "L0_LowLevel/system_controller.hpp"
#include "L1_Drivers/pin.hpp"
#include "utility/bit.hpp"
#include "utility/log.hpp"

class AdcInterface
{
 public:
  virtual void Initialize(uint32_t conversion_frequency = 1'000'000) = 0;
  virtual void Conversion()                                          = 0;
  virtual uint16_t ReadResult()                                      = 0;
  virtual bool FinishedConversion()                                  = 0;
};

class Adc final : public AdcInterface, protected Lpc40xxSystemController
{
 public:
  enum class Channel : uint8_t
  {
    kChannel0 = 0,
    kChannel1,
    kChannel2,
    kChannel3,
    kChannel4,
    kChannel5,
    kChannel6,
    kChannel7,
    kMaxNumOfPins
  };
  enum AdcPortsMap : uint8_t
  {
    kChannel0Port = 0,
    kChannel1Port = 0,
    kChannel2Port = 0,
    kChannel3Port = 0,
    kChannel4Port = 1,
    kChannel5Port = 1,
    kChannel6Port = 0,
    kChannel7Port = 0
  };
  enum AdcPinsMap : uint8_t
  {
    kChannel0Pin = 23,
    kChannel1Pin = 24,
    kChannel2Pin = 25,
    kChannel3Pin = 26,
    kChannel4Pin = 30,
    kChannel5Pin = 31,
    kChannel6Pin = 12,
    kChannel7Pin = 13
  };
  enum ControlBit : uint8_t
  {
    kBurstMode = 16,
    kPowerUp   = 21,
    kStart     = 24
  };
  enum AdcMode : uint8_t
  {
    kCh0123Pins = 0b001,
    kCh4567Pins = 0b011
  };

  static constexpr size_t kTableLength  = util::Value(Channel::kMaxNumOfPins);
  const uint8_t kAdcPorts[kTableLength] = { kChannel0Port, kChannel1Port,
                                            kChannel2Port, kChannel3Port,
                                            kChannel4Port, kChannel5Port,
                                            kChannel6Port, kChannel7Port };
  const uint8_t kAdcPins[kTableLength]  = { kChannel0Pin, kChannel1Pin,
                                           kChannel2Pin, kChannel3Pin,
                                           kChannel4Pin, kChannel5Pin,
                                           kChannel6Pin, kChannel7Pin };

  inline static LPC_ADC_TypeDef * adc_base = LPC_ADC;

  /// Enable or disable burst mode for analog to digital converter.
  /// With burst mode enabled, the ADC will continually perform
  /// analog to digital conversions meaning that the MCU will not have to wait
  /// for the latest conversion, it can simply read from the ADC conversion
  /// register for the latest conversion. This method will allow for faster
  /// reading of ADC voltages, but the continually work requires more power.
  ///
  /// Without burst mode, the CPU will have to drive the clock of the ADC
  /// converter, which will take a time to perform. This method only consumes
  /// power when you are performing a conversion.
  ///
  /// @param burst_mode_is_on - Whether you would like
  static void BurstMode(bool activate_burst_mode = true)
  {
    if (activate_burst_mode)
    {
      adc_base->CR |= (1 << ControlBit::kBurstMode);
    }
    else
    {
      adc_base->CR &= ~(1 << ControlBit::kBurstMode);
    }
  }
  /// @param channel - ADC channel to read from
  explicit constexpr Adc(Channel channel)
      : adc_(&adc_pin_),
        adc_pin_(Pin(kAdcPorts[util::Value(channel)],
                     kAdcPins[util::Value(channel)])),
        channel_(util::Value(channel))
  {
  }
  /// @param adc_pin - address to an initialized adc pin
  explicit constexpr Adc(PinInterface * adc_pin, Channel channel)
      : adc_(adc_pin),
        adc_pin_(Pin::CreateInactivePin()),
        channel_(util::Value(channel))
  {
  }
  /// Initializes ADC hardware.
  ///
  /// MUST be run before attempting to use the Conversion() or ReadResult()
  /// methods.
  ///
  /// @param conversion_frequency - The spead
  void Initialize(uint32_t conversion_frequency = 1'000'000) override
  {
    constexpr uint8_t kClkDivBit    = 8;

    SJ2_ASSERT_FATAL(conversion_frequency < GetPeripheralFrequency(),
                     "Adc clock has to be less than or equal to 12MHz");

    PowerUpPeripheral(Lpc40xxSystemController::Peripherals::kAdc);

    adc_base->CR |= (1 << ControlBit::kPowerUp);
    adc_base->CR |= (1 << channel_);

    if (adc_ == &adc_pin_)
    {
      if (channel_ < 4)
      {
        adc_->SetPinFunction(static_cast<uint8_t>(AdcMode::kCh0123Pins));
      }
      if (channel_ >= 4)
      {
        adc_->SetPinFunction(static_cast<uint8_t>(AdcMode::kCh4567Pins));
      }
      adc_->SetAsAnalogMode(true);
      adc_->SetMode(PinInterface::Mode::kInactive);
    }

    uint32_t clock_divider = GetPeripheralFrequency() / conversion_frequency;
    adc_base->CR = bit::Insert(adc_base->CR, clock_divider, kClkDivBit, 8);
  }
  /// Run an ADC conversion. Once this function returns you may use the
  /// ReadResults() method to get the ADC output.
  void Conversion() override
  {
    if (adc_base->CR & (1 << ControlBit::kBurstMode))
    {
      // clear start bits for burst mode
      adc_base->CR &= ~(0b111 << ControlBit::kStart);
    }
    else
    {
      // enable start bit for non-burst mode
      adc_base->CR |= (1 << ControlBit::kStart);
    }

    while (!FinishedConversion())
    {
      continue;
    }
  }
  /// Returns ADC result from the latest conversion
  uint16_t ReadResult() override
  {
    constexpr uint16_t kResultMask = 0xFFF;
    /// TODO(undef): This will not work multiple ADCs
    uint16_t result = static_cast<uint16_t>((adc_base->GDR >> 4) & kResultMask);
    return result;
  }
  /// Check if the ADC conversion is complete
  [[gnu::always_inline]] bool FinishedConversion() override {
    constexpr uint8_t kDoneBit = 31;
    return ((adc_base->GDR >> kDoneBit) & 1);
  }

  private : PinInterface * adc_;
  Pin adc_pin_;

  uint8_t channel_;

};
