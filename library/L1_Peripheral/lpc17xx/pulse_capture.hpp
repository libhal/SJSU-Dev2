#pragma once

#include "L1_Peripheral/interrupt.hpp"
#include "L1_Peripheral/lpc17xx/pin.hpp"
#include "L1_Peripheral/lpc17xx/system_controller.hpp"
#include "L1_Peripheral/lpc17xx/timer.hpp"
#include "L1_Peripheral/lpc40xx/pulse_capture.hpp"

namespace sjsu::lpc17xx
{
// The LPC40xx driver is compatible with the lpc17xx peripheral
using ::sjsu::lpc40xx::PulseCapture;

/// Structure used as a namespace for predefined Channel definitions
struct PulseCaptureChannel  // NOLINT
{
 private:
  inline static lpc40xx::PulseCapture::CaptureCallback timer0_isr = nullptr;
  inline static lpc40xx::PulseCapture::CaptureChannelNumber
      timer0_channel_number =
          lpc40xx::PulseCapture::CaptureChannelNumber::kChannel1;
  inline static const Pin kCapture0Channel0Pin = Pin(1, 26);
  inline static const Pin kCapture0Channel1Pin = Pin(1, 27);
  inline static const lpc40xx::PulseCapture::CaptureChannelPartial_t
      kTimerPartial0 = { .timer_register =
                             reinterpret_cast<lpc40xx::LPC_TIM_TypeDef *>(
                                 LPC_TIM0),
                         .power_id = SystemController::Peripherals::kTimer0,
                         .irq = static_cast<lpc40xx::IRQn>(IRQn::TIMER0_IRQn),
                         .user_callback  = &timer0_isr,
                         .capture_pin0   = kCapture0Channel0Pin,
                         .capture_pin1   = kCapture0Channel1Pin,
                         .channel_number = &timer0_channel_number };

  inline static lpc40xx::PulseCapture::CaptureCallback timer1_isr = nullptr;
  inline static lpc40xx::PulseCapture::CaptureChannelNumber
      timer1_channel_number =
          lpc40xx::PulseCapture::CaptureChannelNumber::kChannel1;
  inline static const Pin kCapture1Channel0Pin = Pin(1, 18);
  inline static const Pin kCapture1Channel1Pin = Pin(1, 19);
  inline static const lpc40xx::PulseCapture::CaptureChannelPartial_t
      kTimerPartial1 = { .timer_register =
                             reinterpret_cast<lpc40xx::LPC_TIM_TypeDef *>(
                                 LPC_TIM1),
                         .power_id = SystemController::Peripherals::kTimer1,
                         .irq = static_cast<lpc40xx::IRQn>(IRQn::TIMER1_IRQn),
                         .user_callback  = &timer1_isr,
                         .capture_pin0   = kCapture1Channel0Pin,
                         .capture_pin1   = kCapture1Channel1Pin,
                         .channel_number = &timer1_channel_number };

  inline static lpc40xx::PulseCapture::CaptureCallback timer2_isr = nullptr;
  inline static lpc40xx::PulseCapture::CaptureChannelNumber
      timer2_channel_number =
          lpc40xx::PulseCapture::CaptureChannelNumber::kChannel1;
  inline static const Pin kCapture2Channel0Pin = Pin(0, 4);
  inline static const Pin kCapture2Channel1Pin = Pin(0, 5);
  inline static const lpc40xx::PulseCapture::CaptureChannelPartial_t
      kTimerPartial2 = { .timer_register =
                             reinterpret_cast<lpc40xx::LPC_TIM_TypeDef *>(
                                 LPC_TIM2),
                         .power_id = SystemController::Peripherals::kTimer2,
                         .irq = static_cast<lpc40xx::IRQn>(IRQn::TIMER2_IRQn),
                         .user_callback  = &timer2_isr,
                         .capture_pin0   = kCapture2Channel0Pin,
                         .capture_pin1   = kCapture2Channel1Pin,
                         .channel_number = &timer2_channel_number };

  inline static lpc40xx::PulseCapture::CaptureCallback timer3_isr = nullptr;
  inline static lpc40xx::PulseCapture::CaptureChannelNumber
      timer3_channel_number =
          lpc40xx::PulseCapture::CaptureChannelNumber::kChannel1;
  inline static const Pin kCapture3Channel0Pin = Pin(0, 23);
  inline static const Pin kCapture3Channel1Pin = Pin(0, 24);
  inline static const lpc40xx::PulseCapture::CaptureChannelPartial_t
      kTimerPartial3 = { .timer_register =
                             reinterpret_cast<lpc40xx::LPC_TIM_TypeDef *>(
                                 LPC_TIM3),
                         .power_id = SystemController::Peripherals::kTimer3,
                         .irq = static_cast<lpc40xx::IRQn>(IRQn::TIMER3_IRQn),
                         .user_callback  = &timer3_isr,
                         .capture_pin0   = kCapture3Channel0Pin,
                         .capture_pin1   = kCapture3Channel1Pin,
                         .channel_number = &timer3_channel_number };

 public:
  /// Structure that defines the capture channels associated with timer 0
  inline static const lpc40xx::PulseCapture::CaptureChannel_t kCaptureTimer0 = {
    .channel = kTimerPartial0,
    .handler = lpc40xx::PulseCapture::TimerHandler<kTimerPartial0>
  };
  /// Structure that defines the capture channels associated with timer 1
  inline static const lpc40xx::PulseCapture::CaptureChannel_t kCaptureTimer1 = {
    .channel = kTimerPartial1,
    .handler = lpc40xx::PulseCapture::TimerHandler<kTimerPartial1>
  };
  /// Structure that defines the capture channels associated with timer 2
  inline static const lpc40xx::PulseCapture::CaptureChannel_t kCaptureTimer2 = {
    .channel = kTimerPartial2,
    .handler = lpc40xx::PulseCapture::TimerHandler<kTimerPartial2>
  };
  /// Structure that defines the capture channels associated with timer 3
  inline static const lpc40xx::PulseCapture::CaptureChannel_t kCaptureTimer3 = {
    .channel = kTimerPartial3,
    .handler = lpc40xx::PulseCapture::TimerHandler<kTimerPartial3>
  };
};
};  // namespace sjsu::lpc17xx
