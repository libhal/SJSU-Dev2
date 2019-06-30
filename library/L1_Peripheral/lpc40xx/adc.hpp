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
class Adc final : public sjsu::Adc
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

  struct Control  // NOLINT
  {
    static constexpr bit::Mask kChannelEnable = bit::CreateMaskFromRange(0, 7);
    static constexpr bit::Mask kClockDivider  = bit::CreateMaskFromRange(8, 15);
    static constexpr bit::Mask kBurstEnable   = bit::CreateMaskFromRange(16);
    static constexpr bit::Mask kPowerEnable   = bit::CreateMaskFromRange(21);
    static constexpr bit::Mask kStartCode = bit::CreateMaskFromRange(24, 26);
    static constexpr bit::Mask kStartEdge = bit::CreateMaskFromRange(27);
  };

  struct GlobalData  // NOLINT
  {
    static constexpr bit::Mask kResult = bit::CreateMaskFromRange(4, 15);
    static constexpr bit::Mask kConvertedChannel =
        bit::CreateMaskFromRange(24, 26);
    static constexpr bit::Mask kOverrun = bit::CreateMaskFromRange(30);
    static constexpr bit::Mask kDone    = bit::CreateMaskFromRange(31);
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

  static constexpr sjsu::lpc40xx::SystemController kLpc40xxSystemController =
      sjsu::lpc40xx::SystemController();

  static constexpr uint8_t kActiveBits = 12;

  static void BurstMode(bool burst_mode_is_on = true)
  {
    adc_base->CR =
        bit::Insert(adc_base->CR, burst_mode_is_on, Control::kBurstEnable);
  }

  explicit constexpr Adc(const Channel_t & channel,
                         const sjsu::SystemController & system_controller =
                             kLpc40xxSystemController)
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

    const uint32_t kPeripheralFrequency =
        system_controller_.GetPeripheralFrequency(
            sjsu::lpc40xx::SystemController::Peripherals::kAdc);
    uint32_t clock_divider = kPeripheralFrequency / kClockFrequency;

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
