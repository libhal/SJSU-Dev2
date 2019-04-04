#pragma once

#include <cstdint>

#include "L0_LowLevel/LPC40xx.h"
#include "L0_LowLevel/system_controller.hpp"
#include "L1_Drivers/pin.hpp"
#include "utility/log.hpp"

class AdcInterface
{
 public:
  virtual void Initialize(uint32_t adc_clk_hz = 1'000'000) = 0;
  virtual void Conversion()                                = 0;
  virtual uint16_t ReadResult()                            = 0;
  virtual bool FinishedConversion()                        = 0;
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

  union ControlRegister_t {
    uint32_t data;
    struct
    {
      unsigned channel_enable : 8;
      unsigned clock_divider : 8;
      unsigned enable_burst : 1;
      unsigned reserved0 : 4;
      unsigned power_enable : 1;
      unsigned reserved1 : 2;
      unsigned start_conversion_code : 3;
      unsigned start_conversion_on_falling_edge : 1;
    } bits;
  };

  union GlobalDataRegister_t {
    uint32_t data;
    struct
    {
      unsigned reserved0 : 4;
      unsigned result : 12;
      unsigned reserved1 : 8;
      unsigned converted_channel : 3;
      unsigned reserved2 : 3;
      unsigned overrun : 1;
      unsigned done : 1;
    } bits;
  };

  static volatile ControlRegister_t * GetControlRegister()
  {
    return reinterpret_cast<volatile ControlRegister_t *>(&adc_base->CR);
  }
  static volatile GlobalDataRegister_t * GetGlobalDataRegister()
  {
    return reinterpret_cast<volatile GlobalDataRegister_t *>(&adc_base->GDR);
  }
  static void BurstMode(bool burst_mode_is_on = true)
  {
    GetControlRegister()->bits.enable_burst = burst_mode_is_on;
  }
  explicit constexpr Adc(Channel channel)
      : adc_(&adc_pin_),
        adc_pin_(Pin(kAdcPorts[util::Value(channel)],
                     kAdcPins[util::Value(channel)])),
        channel_(util::Value(channel))
  {
  }
  // unit test constructor
  explicit constexpr Adc(PinInterface * adc_pin, Channel channel)
      : adc_(adc_pin),
        adc_pin_(Pin::CreateInactivePin()),
        channel_(util::Value(channel))
  {
  }
  void Initialize(uint32_t adc_clock_freq = 1'000'000) override
  {
    constexpr uint32_t kMaxAdcClock = 12'000'000;
    SJ2_ASSERT_FATAL(adc_clock_freq < kMaxAdcClock,
                     "Adc clock has to be less than or equal to 12MHz");

    PowerUpPeripheral(Lpc40xxSystemController::Peripherals::kAdc);

    if (channel_ < 4)
    {
      adc_->SetPinFunction(util::Value(AdcMode::kCh0123Pins));
    }
    if (channel_ >= 4)
    {
      adc_->SetPinFunction(util::Value(AdcMode::kCh4567Pins));
    }
    adc_->SetAsAnalogMode(true);
    adc_->SetMode(PinInterface::Mode::kInactive);

    ControlRegister_t control;

    control.data = adc_base->CR;
    control.bits.channel_enable =
        (control.bits.channel_enable | (1 << channel_)) & 0xFF;
    control.bits.power_enable = true;
    control.bits.clock_divider =
        (GetPeripheralFrequency() / adc_clock_freq) & 0xFF;

    adc_base->CR = control.data;
  }
  void Conversion() override
  {
    volatile ControlRegister_t * control = GetControlRegister();
    if (control->bits.enable_burst)
    {
      control->bits.start_conversion_code = 0;
    }
    else
    {
      // 0x01 = start code for "Start Conversion Now"
      control->bits.start_conversion_code = 0x01;
    }

    while (!(GetGlobalDataRegister()->bits.done))
    {
      continue;
    }
  }
  uint16_t ReadResult() override
  {
    return GetGlobalDataRegister()->bits.result;
  }
  bool FinishedConversion() override
  {
    return GetGlobalDataRegister()->bits.done;
  }

 private:
  PinInterface * adc_;
  Pin adc_pin_;

  uint8_t channel_;
};
