#pragma once

#include "L1_Peripheral/lpc17xx/pin.hpp"
#include "L1_Peripheral/lpc40xx/adc.hpp"

namespace sjsu
{
namespace lpc17xx
{
// The LPC40xx driver is compatible with the lpc17xx peripheral
using ::sjsu::lpc40xx::Adc;

struct AdcChannel  // NOLINT
{
 private:
  enum AdcMode : uint8_t
  {
    kCh0123Pins = 0b01,
    kCh45Pins   = 0b11,
    kCh67Pins   = 0b10,
  };
  inline static const Pin kAdcPinChannel0 = Pin::CreatePin<0, 23>();
  inline static const Pin kAdcPinChannel1 = Pin::CreatePin<0, 24>();
  inline static const Pin kAdcPinChannel2 = Pin::CreatePin<0, 25>();
  inline static const Pin kAdcPinChannel3 = Pin::CreatePin<0, 26>();
  inline static const Pin kAdcPinChannel4 = Pin::CreatePin<1, 30>();
  inline static const Pin kAdcPinChannel5 = Pin::CreatePin<1, 31>();
  inline static const Pin kAdcPinChannel6 = Pin::CreatePin<0, 3>();
  inline static const Pin kAdcPinChannel7 = Pin::CreatePin<0, 2>();

 public:
  inline static const sjsu::lpc40xx::Adc::Channel_t kChannel0 = {
    .adc_pin      = kAdcPinChannel0,
    .channel      = 0,
    .pin_function = AdcMode::kCh0123Pins,
  };
  inline static const sjsu::lpc40xx::Adc::Channel_t kChannel1 = {
    .adc_pin      = kAdcPinChannel1,
    .channel      = 1,
    .pin_function = AdcMode::kCh0123Pins,
  };
  inline static const sjsu::lpc40xx::Adc::Channel_t kChannel2 = {
    .adc_pin      = kAdcPinChannel2,
    .channel      = 2,
    .pin_function = AdcMode::kCh0123Pins,
  };
  inline static const sjsu::lpc40xx::Adc::Channel_t kChannel3 = {
    .adc_pin      = kAdcPinChannel3,
    .channel      = 3,
    .pin_function = AdcMode::kCh0123Pins,
  };
  inline static const sjsu::lpc40xx::Adc::Channel_t kChannel4 = {
    .adc_pin      = kAdcPinChannel4,
    .channel      = 4,
    .pin_function = AdcMode::kCh45Pins,
  };
  inline static const sjsu::lpc40xx::Adc::Channel_t kChannel5 = {
    .adc_pin      = kAdcPinChannel5,
    .channel      = 5,
    .pin_function = AdcMode::kCh45Pins,
  };
  inline static const sjsu::lpc40xx::Adc::Channel_t kChannel6 = {
    .adc_pin      = kAdcPinChannel6,
    .channel      = 6,
    .pin_function = AdcMode::kCh67Pins,
  };
  inline static const sjsu::lpc40xx::Adc::Channel_t kChannel7 = {
    .adc_pin      = kAdcPinChannel7,
    .channel      = 7,
    .pin_function = AdcMode::kCh67Pins,
  };
};
}  // namespace lpc17xx
}  // namespace sjsu
