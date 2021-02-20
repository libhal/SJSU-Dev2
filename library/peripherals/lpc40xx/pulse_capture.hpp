#pragma once

#include <cstdint>

#include "platforms/targets/lpc40xx/LPC40xx.h"
#include "peripherals/interrupt.hpp"
#include "peripherals/lpc40xx/pin.hpp"
#include "peripherals/lpc40xx/pulse_capture.hpp"
#include "peripherals/lpc40xx/system_controller.hpp"
#include "peripherals/lpc40xx/timer.hpp"
#include "peripherals/pulse_capture.hpp"
#include "peripherals/timer.hpp"
#include "utility/math/bit.hpp"
#include "utility/error_handling.hpp"
#include "utility/log.hpp"
#include "utility/math/units.hpp"

namespace sjsu
{
namespace lpc40xx
{
/// Pulse capture library
class PulseCapture final : public sjsu::PulseCapture
{
 public:
  /// List of available timers to capture from
  enum CaptureTimerNumber : uint8_t
  {
    kTimer0,
    kTimer1,
    kTimer2,
    kTimer3
  };

  /// List of available channels per timer to capture from
  enum CaptureChannelNumber : uint8_t
  {
    kChannel0,
    kChannel1
  };

  /// Structure used to define attributes of a timer capture channel

  struct CaptureChannelPartial_t
  {
    /// Pointer to memory-mapped timer peripheral registres
    LPC_TIM_TypeDef * timer_register;
    /// Peripheral ID of the timer peripheral to power on at initialization
    sjsu::SystemController::ResourceID id;
    /// Interrupt number associated with this timer
    IRQn irq;
    /// Callback invoked during a capture event
    CaptureCallback * user_callback;
    /// Pin corresponding to timer capture channel #0
    sjsu::Pin & capture_pin0;
    /// Pin corresponding to timer capture channel #1
    sjsu::Pin & capture_pin1;
    /// Pointer to variable containing capture channel number
    CaptureChannelNumber * channel_number;
  };

  /// Structure used to bundle an interrupt handler with a timer capture
  /// channel
  struct CaptureChannel_t
  {
    /// Structure defining low-level attributes of the capture channel
    const CaptureChannelPartial_t & channel;
    /// Callback used to invoke timer handler
    InterruptHandler handler;
  };

  /// Method used to define multiple timer handlers for each available timer
  template <const CaptureChannelPartial_t & port>
  static void TimerHandler()
  {
    TimerHandler(port);
  }

  /// Collection of timer and channel attributes for all available timers
  struct Channel  // NOLINT
  {
   private:
    inline static CaptureCallback timer0_callback = nullptr;
    inline static CaptureChannelNumber timer0_channel_number =
        CaptureChannelNumber::kChannel1;
    inline static Pin & capture0_channel0_pin = GetPin<1, 26>();
    inline static Pin & capture0_channel1_pin = GetPin<1, 27>();
    inline static const CaptureChannelPartial_t kTimerPartial0 = {
      .timer_register = LPC_TIM0,
      .id             = SystemController::Peripherals::kTimer0,
      .irq            = IRQn::TIMER0_IRQn,
      .user_callback  = &timer0_callback,
      .capture_pin0   = capture0_channel0_pin,
      .capture_pin1   = capture0_channel1_pin,
      .channel_number = &timer0_channel_number
    };

    inline static CaptureCallback timer1_callback = nullptr;
    inline static CaptureChannelNumber timer1_channel_number =
        CaptureChannelNumber::kChannel1;
    inline static Pin & capture1_channel0_pin = GetPin<1, 18>();
    inline static Pin & capture1_channel1_pin = GetPin<1, 19>();
    inline static Pin & capture1_pin          = GetPin<1, 14>();
    inline static const CaptureChannelPartial_t kTimerPartial1 = {
      .timer_register = LPC_TIM1,
      .id             = SystemController::Peripherals::kTimer1,
      .irq            = IRQn::TIMER1_IRQn,
      .user_callback  = &timer1_callback,
      .capture_pin0   = capture1_channel0_pin,
      .capture_pin1   = capture1_channel1_pin,
      .channel_number = &timer1_channel_number
    };

    inline static CaptureCallback timer2_callback = nullptr;
    inline static CaptureChannelNumber timer2_channel_number =
        CaptureChannelNumber::kChannel1;
    inline static Pin & capture2_channel0_pin = GetPin<1, 14>();
    inline static Pin & capture2_channel1_pin = GetPin<0, 5>();
    inline static const CaptureChannelPartial_t kTimerPartial2 = {
      .timer_register = LPC_TIM2,
      .id             = SystemController::Peripherals::kTimer2,
      .irq            = IRQn::TIMER2_IRQn,
      .user_callback  = &timer2_callback,
      .capture_pin0   = capture2_channel0_pin,
      .capture_pin1   = capture2_channel1_pin,
      .channel_number = &timer2_channel_number
    };

    inline static CaptureCallback timer3_callback = nullptr;
    inline static CaptureChannelNumber timer3_channel_number =
        CaptureChannelNumber::kChannel1;
    inline static Pin & capture3_channel0_pin = GetPin<0, 23>();
    inline static Pin & capture3_channel1_pin = GetPin<0, 24>();
    inline static const CaptureChannelPartial_t kTimerPartial3 = {
      .timer_register = LPC_TIM3,
      .id             = SystemController::Peripherals::kTimer3,
      .irq            = IRQn::TIMER3_IRQn,
      .user_callback  = &timer3_callback,
      .capture_pin0   = capture3_channel0_pin,
      .capture_pin1   = capture3_channel1_pin,
      .channel_number = &timer3_channel_number
    };

   public:
    /// Structure that defines the capture channels associated with timer 0
    inline static const CaptureChannel_t kCaptureTimer0 = {
      .channel = kTimerPartial0,
      .handler = TimerHandler<kTimerPartial0>,
    };

    /// Structure that defines the capture channels associated with timer 1
    inline static const CaptureChannel_t kCaptureTimer1 = {
      .channel = kTimerPartial1,
      .handler = TimerHandler<kTimerPartial1>,
    };

    /// Structure that defines the capture channels associated with timer 2
    inline static const CaptureChannel_t kCaptureTimer2 = {
      .channel = kTimerPartial2,
      .handler = TimerHandler<kTimerPartial2>,
    };

    /// Structure that defines the capture channels associated with timer 3
    inline static const CaptureChannel_t kCaptureTimer3 = {
      .channel = kTimerPartial3,
      .handler = TimerHandler<kTimerPartial3>
    };
  };  // struct Channel

  /// Handler used to collect capture event status and pass to a user callback
  static void TimerHandler(const CaptureChannelPartial_t & channel)
  {
    constexpr bit::Mask kCaptureInterruptFlagBit = bit::MaskFromRange(4, 5);
    constexpr uint8_t kResetCaptureInterrupts    = 0b11;

    if (*channel.user_callback != nullptr)
    {
      CaptureStatus_t status;
      status.flags = channel.timer_register->IR;

      if (*channel.channel_number == CaptureChannelNumber::kChannel0)
      {
        status.count = channel.timer_register->CR0;
      }
      else
      {
        status.count = channel.timer_register->CR1;
      }

      (*channel.user_callback)(status);
    }
    channel.timer_register->IR = bit::Insert(channel.timer_register->IR,
                                             kResetCaptureInterrupts,
                                             kCaptureInterruptFlagBit);
  }

  /// Constructor for LPC40xx timer peripheral
  ///
  /// @param timer - timer to capture from
  /// @param channel - which channel of associated timer to capture from
  /// @param kFrequency - rate at which capture events are monitored
  explicit constexpr PulseCapture(const CaptureChannel_t & timer,
                                  const CaptureChannelNumber & channel,
                                  const units::frequency::hertz_t kFrequency)
      : timer_(timer), channel_(channel), frequency_(kFrequency)
  {
    *timer_.channel.channel_number = channel_;
  };

  /// This METHOD MUST BE EXECUTED before any other method can be called.
  /// Powers on the peripheral, configures the timer pins.
  /// See page 687 of the user manual UM10562 LPC408x/407x for more details.
  /// NOTE: Same initialization as Timer library's Initialize()
  void Initialize(CaptureCallback callback   = nullptr,
                  int32_t interrupt_priority = -1) const override
  {
    if (frequency_ == 0_Hz)
    {
      throw Exception(
          std::errc::invalid_argument,
          "Cannot have zero ticks per microsecond, please choose 1 or more.");
    }

    auto & system = sjsu::SystemController::GetPlatformController();
    system.PowerUpPeripheral(timer_.channel.id);

    // Configure prescaler
    uint32_t prescaler = system.GetClockRate(timer_.channel.id) / frequency_;
    timer_.channel.timer_register->PR = prescaler;

    // Enable timer
    constexpr bit::Mask kTimerEnableBit = bit::MaskFromRange(0);
    timer_.channel.timer_register->TCR =
        bit::Set(timer_.channel.timer_register->TCR, kTimerEnableBit.position);

    // Disable capture channel
    ConfigureCapture(CaptureEdgeMode::kNone);
    EnableCaptureInterrupt(false);

    // Install capture ISR
    *timer_.channel.user_callback = callback;
    sjsu::InterruptController::GetPlatformController().Enable(
        { .interrupt_request_number = timer_.channel.irq,
          .interrupt_handler        = timer_.handler,
          .priority                 = interrupt_priority });
  }

  /// Configures the edges to trigger a capture event on
  /// @param mode - desired edge to use
  void ConfigureCapture(CaptureEdgeMode mode) const override
  {
    if (channel_ == CaptureChannelNumber::kChannel1)
    {
      timer_.channel.capture_pin1.settings.function = 3;
      timer_.channel.capture_pin1.Initialize();
    }
    else
    {
      timer_.channel.capture_pin0.settings.function = 3;
      timer_.channel.capture_pin0.Initialize();
    }

    static constexpr bit::Mask kModeBits[2] = { bit::MaskFromRange(0, 1),
                                                bit::MaskFromRange(3, 4) };

    int which = 0;
    if (channel_ == CaptureChannelNumber::kChannel1)
    {
      which = 1;
    }

    timer_.channel.timer_register->CCR =
        bit::Insert(timer_.channel.timer_register->CCR,
                    static_cast<uint32_t>(mode),
                    kModeBits[which]);
  }

  /// Enables or disables the capture event interrupt
  /// @param enabled - enable or disable the interrupt
  void EnableCaptureInterrupt(bool enabled) const override
  {
    static constexpr bit::Mask kPendingBits[2] = { bit::MaskFromRange(4),
                                                   bit::MaskFromRange(5) };

    static constexpr bit::Mask kEnableBits[2] = { bit::MaskFromRange(2),
                                                  bit::MaskFromRange(5) };

    int which = 0;
    if (channel_ == CaptureChannelNumber::kChannel1)
    {
      which = 1;
    }

    timer_.channel.timer_register->CCR =
        bit::Insert(timer_.channel.timer_register->CCR,
                    static_cast<uint32_t>(enabled),
                    kEnableBits[which]);
    timer_.channel.timer_register->IR = bit::Set(
        timer_.channel.timer_register->IR, kPendingBits[which].position);
  }

 private:
  const CaptureChannel_t & timer_;
  const CaptureChannelNumber channel_;         // NOLINT
  const units::frequency::hertz_t frequency_;  // NOLINT
};
}  // namespace lpc40xx
}  // namespace sjsu
