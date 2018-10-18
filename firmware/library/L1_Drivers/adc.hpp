#pragma once

#include <cstdint>

#include "L0_LowLevel/LPC40xx.h"
#include "L1_Drivers/pin.hpp"
#include "L2_Utilities/macros.hpp"

class AdcInterface
{
 public:
  virtual void Initialize(uint32_t adc_clk_hz = 1'000'000) = 0;
  virtual void Conversion()                                = 0;
  virtual uint16_t ReadResult()                            = 0;
  virtual bool FinishedConversion()                        = 0;
};

class Adc : public AdcInterface
{
 public:
  static constexpr uint8_t kMaxNumOfPins = 8;
  enum class Channel : uint8_t
  {
    kChannel0 = 0,
    kChannel1,
    kChannel2,
    kChannel3,
    kChannel4,
    kChannel5,
    kChannel6,
    kChannel7
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

  const uint8_t kAdcPorts[kMaxNumOfPins] = { kChannel0Port, kChannel1Port,
                                             kChannel2Port, kChannel3Port,
                                             kChannel4Port, kChannel5Port,
                                             kChannel6Port, kChannel7Port };
  const uint8_t kAdcPins[kMaxNumOfPins]  = { kChannel0Pin, kChannel1Pin,
                                            kChannel2Pin, kChannel3Pin,
                                            kChannel4Pin, kChannel5Pin,
                                            kChannel6Pin, kChannel7Pin };

  static LPC_ADC_TypeDef * adc_base;
  static LPC_SC_TypeDef * sysclk_register;
  static void BurstMode(bool burst_mode_is_on = false)
  {
    burst_mode_is_on ? (adc_base->CR |= (1 << ControlBit::kBurstMode))
                     : (adc_base->CR &= ~(1 << ControlBit::kBurstMode));
  }
  explicit constexpr Adc(Adc::Channel channel_bit)
      : adc_(&adc_pin_),
        adc_pin_(Pin(kAdcPorts[static_cast<uint8_t>(channel_bit)],
                     kAdcPins[static_cast<uint8_t>(channel_bit)])),
        channel_(static_cast<uint8_t>(channel_bit))
  {
  }
  // unit test constructor
  explicit constexpr Adc(PinInterface * adc_pin)
      : adc_(adc_pin), adc_pin_(Pin::CreateInactivePin()), channel_(0)
  {
  }
  void Initialize(uint32_t adc_clk_hz = 1'000'000) override
  {
    constexpr uint32_t kMaxAdcClock     = 12'000'000;
    constexpr uint8_t kDivBy4           = 0b10001;
    constexpr uint8_t kAdcPeripheralBit = 12;
    constexpr uint8_t kClkDivBit        = 8;

    constexpr uint32_t kAdcClk = 12'000'000 / 4;
    SJ2_ASSERT_FATAL(adc_clk_hz < kMaxAdcClock,
                     "Adc clock has to be less than or equal to 12MHz");

    adc_base->CR &= ~(1 << ControlBit::kPowerUp);
    sysclk_register->PCONP &= ~(1 << kAdcPeripheralBit);
    sysclk_register->PCONP |= (1 << kAdcPeripheralBit);
    sysclk_register->PCLKSEL |= (1 << kDivBy4);
    adc_base->CR |= (1 << ControlBit::kPowerUp);
    adc_base->CR |= (1 << channel_);
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

    for (uint32_t i = 2; i < 255; i++)
    {
      if ((kAdcClk / i) < adc_clk_hz)
      {
        adc_base->CR |= (i << kClkDivBit);
        break;
      }
    }
  }
  void Conversion() override
  {
    if (adc_base->CR & (1 << ControlBit::kBurstMode))
    {
      // clear start bits for burst mode
      adc_base->CR &= ~(7 << ControlBit::kStart);
    }
    else
    {
      // enable start bit for non-burst mode
      adc_base->CR |= (1 << ControlBit::kStart);
    }

    while (!(adc_base->GDR & (1 << kDoneBit)))
    {
      continue;
    }
  }
  uint16_t ReadResult() override
  {
    constexpr uint16_t kResultMask = 0xFFF;
    uint16_t result = static_cast<uint16_t>((adc_base->GDR >> 4) & kResultMask);
    return result;
  }
  bool FinishedConversion() override
  {
    return ((adc_base->GDR >> kDoneBit) & 1);
  }

 private:
  PinInterface * adc_;
  Pin adc_pin_;

  uint8_t channel_;

  static constexpr uint8_t kDoneBit = 31;
};
