#pragma once

#include <cstdint>

#include "L0_Platform/lpc40xx/LPC40xx.h"
#include "L1_Peripheral/interrupt.hpp"
#include "L1_Peripheral/lpc40xx/pin.hpp"
#include "L1_Peripheral/lpc40xx/system_controller.hpp"
#include "L1_Peripheral/lpc40xx/timer.hpp"
#include "L1_Peripheral/lpc40xx/pulse_capture.hpp"
#include "L1_Peripheral/timer.hpp"
#include "L1_Peripheral/pulse_capture.hpp"
#include "utility/bit.hpp"
#include "utility/log.hpp"
#include "utility/status.hpp"
#include "utility/units.hpp"

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
    sjsu::SystemController::PeripheralID power_id;
    /// Interrupt number associated with this timer
    IRQn irq;
    /// Callback invoked during a capture event
    CaptureIsr * user_callback;
    /// Pin corresponding to timer capture channel #0
    const sjsu::Pin & capture_pin0;
    /// Pin corresponding to timer capture channel #1
    const sjsu::Pin & capture_pin1;
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
    IsrPointer isr;
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
    inline static CaptureIsr timer0_isr = nullptr;
    inline static CaptureChannelNumber timer0_channel_number =
        CaptureChannelNumber::kChannel1;
    inline static const Pin kCapture0Channel0Pin               = Pin(1, 26);
    inline static const Pin kCapture0Channel1Pin               = Pin(1, 27);
    inline static const CaptureChannelPartial_t kTimerPartial0 = {
      .timer_register = LPC_TIM0,
      .power_id       = SystemController::Peripherals::kTimer0,
      .irq            = IRQn::TIMER0_IRQn,
      .user_callback  = &timer0_isr,
      .capture_pin0   = kCapture0Channel0Pin,
      .capture_pin1   = kCapture0Channel1Pin,
      .channel_number = &timer0_channel_number
    };

    inline static CaptureIsr timer1_isr = nullptr;
    inline static CaptureChannelNumber timer1_channel_number =
        CaptureChannelNumber::kChannel1;
    inline static const Pin kCapture1Channel0Pin               = Pin(1, 18);
    inline static const Pin kCapture1Channel1Pin               = Pin(1, 19);
    inline static const Pin kCapture1Pin                       = Pin(1, 14);
    inline static const CaptureChannelPartial_t kTimerPartial1 = {
      .timer_register = LPC_TIM1,
      .power_id       = SystemController::Peripherals::kTimer1,
      .irq            = IRQn::TIMER1_IRQn,
      .user_callback  = &timer1_isr,
      .capture_pin0   = kCapture1Channel0Pin,
      .capture_pin1   = kCapture1Channel1Pin,
      .channel_number = &timer1_channel_number
    };

    inline static CaptureIsr timer2_isr = nullptr;
    inline static CaptureChannelNumber timer2_channel_number =
        CaptureChannelNumber::kChannel1;
    inline static const Pin kCapture2Channel0Pin               = Pin(1, 14);
    inline static const Pin kCapture2Channel1Pin               = Pin(0, 5);
    inline static const CaptureChannelPartial_t kTimerPartial2 = {
      .timer_register = LPC_TIM2,
      .power_id       = SystemController::Peripherals::kTimer2,
      .irq            = IRQn::TIMER2_IRQn,
      .user_callback  = &timer2_isr,
      .capture_pin0   = kCapture2Channel0Pin,
      .capture_pin1   = kCapture2Channel1Pin,
      .channel_number = &timer2_channel_number
    };

    inline static CaptureIsr timer3_isr = nullptr;
    inline static CaptureChannelNumber timer3_channel_number =
        CaptureChannelNumber::kChannel1;
    inline static const Pin kCapture3Channel0Pin               = Pin(0, 23);
    inline static const Pin kCapture3Channel1Pin               = Pin(0, 24);
    inline static const CaptureChannelPartial_t kTimerPartial3 = {
      .timer_register = LPC_TIM3,
      .power_id       = SystemController::Peripherals::kTimer3,
      .irq            = IRQn::TIMER3_IRQn,
      .user_callback  = &timer3_isr,
      .capture_pin0   = kCapture3Channel0Pin,
      .capture_pin1   = kCapture3Channel1Pin,
      .channel_number = &timer3_channel_number
    };

   public:
    /// Structure that defines the capture channels associated with timer 0
    inline static const CaptureChannel_t kCaptureTimer0 = {
      .channel = kTimerPartial0, .isr = TimerHandler<kTimerPartial0>
    };

    /// Structure that defines the capture channels associated with timer 1
    inline static const CaptureChannel_t kCaptureTimer1 = {
      .channel = kTimerPartial1, .isr = TimerHandler<kTimerPartial1>
    };

    /// Structure that defines the capture channels associated with timer 2
    inline static const CaptureChannel_t kCaptureTimer2 = {
      .channel = kTimerPartial2, .isr = TimerHandler<kTimerPartial2>
    };

    /// Structure that defines the capture channels associated with timer 3
    inline static const CaptureChannel_t kCaptureTimer3 = {
      .channel = kTimerPartial3, .isr = TimerHandler<kTimerPartial3>
    };
  };  // struct Channel

  /// Handler used to collect capture event status and pass to a user callback
  static void TimerHandler(const CaptureChannelPartial_t & channel)
  {
    constexpr bit::Mask kCaptureInterruptFlagBit =
        bit::CreateMaskFromRange(4, 5);
    constexpr uint8_t kResetCaptureInterrupts = 0b11;

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

  /// Interrupt controller used to manage capture interrupts
  static constexpr sjsu::cortex::InterruptController kInterruptController =
      sjsu::cortex::InterruptController();

  /// Constructor for LPC40xx timer peripheral
  ///
  /// @param timer - timer to capture from
  /// @param channel - which channel of associated timer to capture from
  /// @param kFrequency - rate at which capture events are monitored
  /// @param system_controller - reference to system controller.
  ///        Uses the default LPC40xx system controller.
  /// @param interrupt_controller - reference to interrupt controller.
  ///        Uses the default LPC40xx interrupt controller.
  explicit constexpr PulseCapture(
      const CaptureChannel_t & timer,
      const CaptureChannelNumber & channel,
      const units::frequency::hertz_t kFrequency,
      const sjsu::SystemController & system_controller =
          DefaultSystemController(),
      const sjsu::InterruptController & interrupt_controller =
          kInterruptController)
      : timer_(timer),
        channel_(channel),
        frequency_(kFrequency),
        system_controller_(system_controller),
        interrupt_controller_(interrupt_controller)
  {
    *timer_.channel.channel_number = channel_;
  };

  /// This METHOD MUST BE EXECUTED before any othe rmethod can be called.
  /// Powers on the peripheral, configures the timer pins.
  /// See page 687 of the user manual UM10562 LPC408x/407x for more details.
  Status Initialize(CaptureIsr isr             = nullptr,
                    int32_t interrupt_priority = -1) const override
  {
    // NOTE: Same initialization as Timer library's Initialize()
    system_controller_.PowerUpPeripheral(timer_.channel.power_id);
    SJ2_ASSERT_FATAL(
        frequency_ != 0_Hz,
        "Cannot have zero ticks per microsecond, please choose 1 or more.");

    // Configure prescaler
    uint32_t prescaler =
        system_controller_.GetPeripheralFrequency(timer_.channel.power_id) /
        this->frequency_;
    timer_.channel.timer_register->PR = prescaler;

    // Enable timer
    constexpr bit::Mask kTimerEnableBit = bit::CreateMaskFromRange(0);
    timer_.channel.timer_register->TCR =
        bit::Set(timer_.channel.timer_register->TCR, kTimerEnableBit.position);

    // Disable capture channel
    ConfigureCapture(CaptureEdgeMode::kNone);
    EnableCaptureInterrupt(false);

    // Install capture ISR
    *timer_.channel.user_callback = isr;
    interrupt_controller_.Register(
        { .interrupt_request_number  = timer_.channel.irq,
          .interrupt_service_routine = timer_.isr,
          .enable_interrupt          = true,
          .priority                  = interrupt_priority });

    return Status::kSuccess;
  }

  /// Configures the edges to trigger a capture event on
  /// @param mode - desired edge to use
  void ConfigureCapture(CaptureEdgeMode mode) const override
  {
    if (channel_ == CaptureChannelNumber::kChannel1)
    {
      timer_.channel.capture_pin1.SetPinFunction(3);
    }
    else
    {
      timer_.channel.capture_pin0.SetPinFunction(3);
    }

    static constexpr bit::Mask kModeBits[2] = {
      bit::CreateMaskFromRange(0, 1), bit::CreateMaskFromRange(3, 4)
    };

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
    static constexpr bit::Mask kPendingBits[2] = {
      bit::CreateMaskFromRange(4), bit::CreateMaskFromRange(5)
    };

    static constexpr bit::Mask kEnableBits[2] = { bit::CreateMaskFromRange(2),
                                                  bit::CreateMaskFromRange(5) };

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
  const sjsu::SystemController & system_controller_;
  const sjsu::InterruptController & interrupt_controller_;
};  // class Capture
};  // namespace lpc40xx
};  // namespace sjsu
