#pragma once

#include <cstdint>

#include "L1_Peripheral/adc.hpp"

#include "L0_Platform/lpc40xx/LPC40xx.h"
#include "L1_Peripheral/lpc40xx/pin.hpp"
#include "L1_Peripheral/lpc40xx/system_controller.hpp"
#include "utility/log.hpp"
#include "utility/status.hpp"

namespace sjsu
{
namespace lpc40xx
{
class Adc final : public sjsu::Adc, protected sjsu::lpc40xx::SystemController
{
 public:
  enum ControlBit : uint8_t
  {
    kBurstMode = 16,
    kPowerUp   = 21,
    kStart     = 24
  };

  static constexpr uint32_t kClockFrequency = 1'000'000;

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

  struct Channel_t
  {
    const sjsu::Pin & adc_pin;
    uint8_t channel : 3;
    uint8_t pin_function : 3;
  };

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
    inline static const Channel_t kChannel0 = {
      .adc_pin      = kAdcPinChannel0,
      .channel      = 0,
      .pin_function = AdcMode::kCh0123Pins,
    };
    inline static const Channel_t kChannel1 = {
      .adc_pin      = kAdcPinChannel1,
      .channel      = 1,
      .pin_function = AdcMode::kCh0123Pins,
    };
    inline static const Channel_t kChannel2 = {
      .adc_pin      = kAdcPinChannel2,
      .channel      = 2,
      .pin_function = AdcMode::kCh0123Pins,
    };
    inline static const Channel_t kChannel3 = {
      .adc_pin      = kAdcPinChannel3,
      .channel      = 3,
      .pin_function = AdcMode::kCh0123Pins,
    };
    inline static const Channel_t kChannel4 = {
      .adc_pin      = kAdcPinChannel4,
      .channel      = 4,
      .pin_function = AdcMode::kCh4567Pins,
    };
    inline static const Channel_t kChannel5 = {
      .adc_pin      = kAdcPinChannel5,
      .channel      = 5,
      .pin_function = AdcMode::kCh4567Pins,
    };
    inline static const Channel_t kChannel6 = {
      .adc_pin      = kAdcPinChannel6,
      .channel      = 6,
      .pin_function = AdcMode::kCh4567Pins,
    };
    inline static const Channel_t kChannel7 = {
      .adc_pin      = kAdcPinChannel7,
      .channel      = 7,
      .pin_function = AdcMode::kCh4567Pins,
    };
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

  explicit constexpr Adc(const Channel_t & channel) : channel_(channel) {}
  Status Initialize() const override
  {
    PowerUpPeripheral(sjsu::lpc40xx::SystemController::Peripherals::kAdc);

    channel_.adc_pin.SetPinFunction(channel_.pin_function);
    channel_.adc_pin.SetMode(sjsu::Pin::Mode::kInactive);
    channel_.adc_pin.SetAsAnalogMode(true);

    ControlRegister_t control;

    control.data = adc_base->CR;
    control.bits.channel_enable =
        (control.bits.channel_enable | (1 << channel_.channel)) & 0xFF;
    control.bits.power_enable = true;
    control.bits.clock_divider =
        (GetPeripheralFrequency() / kClockFrequency) & 0xFF;

    adc_base->CR = control.data;

    return Status::kSuccess;
  }
  void Conversion() const override
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
  uint16_t Read() const override
  {
    return GetGlobalDataRegister()->bits.result;
  }
  bool HasConversionFinished() const override
  {
    return GetGlobalDataRegister()->bits.done;
  }

 private:
  const Channel_t & channel_;
};
}  // namespace lpc40xx
}  // namespace sjsu
