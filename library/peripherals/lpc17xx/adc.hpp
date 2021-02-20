#pragma once

#include "peripherals/lpc17xx/pin.hpp"
#include "peripherals/lpc40xx/adc.hpp"

namespace sjsu
{
namespace lpc17xx
{
/// The LPC40xx driver is compatible with the lpc17xx peripheral
using ::sjsu::lpc40xx::Adc;

template <int channel>
static Adc & GetAdc()
{
  enum AdcMode : uint8_t
  {
    kCh0123Pins = 0b01,
    kCh45Pins   = 0b11,
    kCh67Pins   = 0b10,
  };

  if constexpr (channel == 0)
  {
    static auto & adc_pin_channel0 = lpc17xx::GetPin<0, 23>();
    static const sjsu::lpc40xx::Adc::Channel_t kChannel0 = {
      .adc_pin      = adc_pin_channel0,
      .channel      = 0,
      .pin_function = AdcMode::kCh0123Pins,
    };
    static Adc adc_channel0(kChannel0);
    return adc_channel0;
  }
  else if constexpr (channel == 1)
  {
    static auto & adc_pin_channel1 = lpc17xx::GetPin<0, 24>();
    static const sjsu::lpc40xx::Adc::Channel_t kChannel1 = {
      .adc_pin      = adc_pin_channel1,
      .channel      = 1,
      .pin_function = AdcMode::kCh0123Pins,
    };
    static Adc adc_channel1(kChannel1);
    return adc_channel1;
  }
  else if constexpr (channel == 2)
  {
    static auto & adc_pin_channel2 = lpc17xx::GetPin<0, 25>();
    static const sjsu::lpc40xx::Adc::Channel_t kChannel2 = {
      .adc_pin      = adc_pin_channel2,
      .channel      = 2,
      .pin_function = AdcMode::kCh0123Pins,
    };
    static Adc adc_channel2(kChannel2);
    return adc_channel2;
  }
  else if constexpr (channel == 3)
  {
    static auto & adc_pin_channel3 = lpc17xx::GetPin<0, 26>();
    static const sjsu::lpc40xx::Adc::Channel_t kChannel3 = {
      .adc_pin      = adc_pin_channel3,
      .channel      = 3,
      .pin_function = AdcMode::kCh0123Pins,
    };
    static Adc adc_channel3(kChannel3);
    return adc_channel3;
  }
  else if constexpr (channel == 4)
  {
    static auto & adc_pin_channel4 = lpc17xx::GetPin<1, 30>();
    static const sjsu::lpc40xx::Adc::Channel_t kChannel4 = {
      .adc_pin      = adc_pin_channel4,
      .channel      = 4,
      .pin_function = AdcMode::kCh45Pins,
    };
    static Adc adc_channel4(kChannel4);
    return adc_channel4;
  }
  else if constexpr (channel == 5)
  {
    static auto & adc_pin_channel5 = lpc17xx::GetPin<1, 31>();
    static const sjsu::lpc40xx::Adc::Channel_t kChannel5 = {
      .adc_pin      = adc_pin_channel5,
      .channel      = 5,
      .pin_function = AdcMode::kCh45Pins,
    };

    static Adc adc_channel5(kChannel5);
    return adc_channel5;
  }
  else if constexpr (channel == 6)
  {
    static auto & adc_pin_channel6 = lpc17xx::GetPin<0, 3>();
    static const sjsu::lpc40xx::Adc::Channel_t kChannel6 = {
      .adc_pin      = adc_pin_channel6,
      .channel      = 6,
      .pin_function = AdcMode::kCh67Pins,
    };

    static Adc adc_channel6(kChannel6);
    return adc_channel6;
  }
  else if constexpr (channel == 7)
  {
    static auto & adc_pin_channel7 = lpc17xx::GetPin<0, 2>();
    static const sjsu::lpc40xx::Adc::Channel_t kChannel7 = {
      .adc_pin      = adc_pin_channel7,
      .channel      = 7,
      .pin_function = AdcMode::kCh67Pins,
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
}  // namespace lpc17xx
}  // namespace sjsu
