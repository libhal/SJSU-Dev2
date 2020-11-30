#pragma once

#include "L1_Peripheral/lpc17xx/pin.hpp"
#include "L1_Peripheral/lpc40xx/adc.hpp"

namespace sjsu
{
namespace lpc17xx
{
/// The LPC40xx driver is compatible with the lpc17xx peripheral
using ::sjsu::lpc40xx::Adc;
/// Namespace containing predefined Channel_t description objects. These
/// objects can be passed directly to the constructor of an lpc40xx::Adc
/// object.
///
/// Usage:
///
/// ```
/// sjsu::lpc17xx::Adc adc(sjsu::lpc17xx::AdcChannel::kChannel0);
/// ```
struct AdcChannel  // NOLINT
{
 private:
  enum AdcMode : uint8_t
  {
    kCh0123Pins = 0b01,
    kCh45Pins   = 0b11,
    kCh67Pins   = 0b10,
  };

  inline static lpc17xx::Pin adc_pin_channel0 = lpc17xx::Pin(0, 23);
  inline static lpc17xx::Pin adc_pin_channel1 = lpc17xx::Pin(0, 24);
  inline static lpc17xx::Pin adc_pin_channel2 = lpc17xx::Pin(0, 25);
  inline static lpc17xx::Pin adc_pin_channel3 = lpc17xx::Pin(0, 26);
  inline static lpc17xx::Pin adc_pin_channel4 = lpc17xx::Pin(1, 30);
  inline static lpc17xx::Pin adc_pin_channel5 = lpc17xx::Pin(1, 31);
  inline static lpc17xx::Pin adc_pin_channel6 = lpc17xx::Pin(0, 3);
  inline static lpc17xx::Pin adc_pin_channel7 = lpc17xx::Pin(0, 2);

 public:
  /// Predefined channel information for channel 0.
  /// Pass this to the lpc17xx::Adc class to utilize adc channel0.
  inline static const sjsu::lpc40xx::Adc::Channel_t kChannel0 = {
    .adc_pin      = adc_pin_channel0,
    .channel      = 0,
    .pin_function = AdcMode::kCh0123Pins,
  };
  /// Predefined channel information for channel 1.
  /// Pass this to the lpc17xx::Adc class to utilize adc channel1.
  inline static const sjsu::lpc40xx::Adc::Channel_t kChannel1 = {
    .adc_pin      = adc_pin_channel1,
    .channel      = 1,
    .pin_function = AdcMode::kCh0123Pins,
  };
  /// Predefined channel information for channel 2.
  /// Pass this to the lpc17xx::Adc class to utilize adc channel2.
  inline static const sjsu::lpc40xx::Adc::Channel_t kChannel2 = {
    .adc_pin      = adc_pin_channel2,
    .channel      = 2,
    .pin_function = AdcMode::kCh0123Pins,
  };
  /// Predefined channel information for channel 3.
  /// Pass this to the lpc17xx::Adc class to utilize adc channel3.
  inline static const sjsu::lpc40xx::Adc::Channel_t kChannel3 = {
    .adc_pin      = adc_pin_channel3,
    .channel      = 3,
    .pin_function = AdcMode::kCh0123Pins,
  };
  /// Predefined channel information for channel 4.
  /// Pass this to the lpc17xx::Adc class to utilize adc channel4.
  inline static const sjsu::lpc40xx::Adc::Channel_t kChannel4 = {
    .adc_pin      = adc_pin_channel4,
    .channel      = 4,
    .pin_function = AdcMode::kCh45Pins,
  };
  /// Predefined channel information for channel 5.
  /// Pass this to the lpc17xx::Adc class to utilize adc channel5.
  inline static const sjsu::lpc40xx::Adc::Channel_t kChannel5 = {
    .adc_pin      = adc_pin_channel5,
    .channel      = 5,
    .pin_function = AdcMode::kCh45Pins,
  };
  /// Predefined channel information for channel 6.
  /// Pass this to the lpc17xx::Adc class to utilize adc channel6.
  inline static const sjsu::lpc40xx::Adc::Channel_t kChannel6 = {
    .adc_pin      = adc_pin_channel6,
    .channel      = 6,
    .pin_function = AdcMode::kCh67Pins,
  };
  /// Predefined channel information for channel 7.
  /// Pass this to the lpc17xx::Adc class to utilize adc channel7.
  inline static const sjsu::lpc40xx::Adc::Channel_t kChannel7 = {
    .adc_pin      = adc_pin_channel7,
    .channel      = 7,
    .pin_function = AdcMode::kCh67Pins,
  };
};
}  // namespace lpc17xx
}  // namespace sjsu
