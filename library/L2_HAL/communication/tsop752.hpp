#pragma once

#include "L1_Peripheral/pulse_capture.hpp"
#include "L1_Peripheral/timer.hpp"
#include "L2_HAL/communication/infrared_receiver.hpp"
#include "utility/error_handling.hpp"
#include "utility/units.hpp"

namespace sjsu
{
/// Tsop752 Infrared (IR) Receiver is a device optimized for remote control
/// applications with carrier frequencies ranging from 30kHz to 56kHz.The driver
/// uses pulse capture to receive incoming IR remote control signals.
class Tsop752 final : public InfraredReceiver
{
 public:
  /// @param capture Pulse capture peripheral used to capture the de-modulated
  ///                signal frame.
  /// @param timer   Timer peripheral used to detect then end of the data frame.
  /// @param timeout Timeout time with microsecond precision.
  explicit Tsop752(sjsu::PulseCapture & capture,
                   sjsu::Timer & timer,
                   std::chrono::microseconds timeout)
      : capture_(capture),
        timer_(timer),
        kTimeout(timeout),
        data_frame_({ {}, 0 }),
        is_start_of_new_frame_(true),
        last_received_timestamp_(0)
  {
  }

  /// Initializes the PulseCapture and Timer peripherals.
  void Initialize() override
  {
    // using 1 MHz for 1µs precision (1 / 1'000'000 Hz = 1µs)
    constexpr units::frequency::hertz_t kFrequency = 1_MHz;
    constexpr uint8_t kTimerMatchRegister          = 0;

    capture_.Initialize(
        [this](auto capture_status) { HandlePulseCaptured(capture_status); });

    capture_.ConfigureCapture(sjsu::PulseCapture::CaptureEdgeMode::kBoth);
    capture_.EnableCaptureInterrupt(true);

    timer_.Initialize(kFrequency, [this]() { HandleEndOfFrame(); });

    timer_.SetMatchBehavior(static_cast<uint32_t>(kTimeout.count()),
                            sjsu::Timer::MatchAction::kInterrupt,
                            kTimerMatchRegister);
  }

  /// @param handler User callback handler to invoke when a frame is received.
  void SetInterruptCallback(DataReceivedHandler handler) override
  {
    user_callback_ = handler;
  }

  /// Invoked when a pulse is captured.
  ///
  /// Example timing diagram:
  ///                           __     __    __
  ///   Remote Control:      __|  |___|  |__|  |___
  ///                        __    ___    __    ___
  ///   IR Receiver Output:    |__|   |__|  |__|
  ///   pulse_buffer_length: 0 1  2   3  4  5  6
  ///
  /// @param status Status of the interrupt.
  void HandlePulseCaptured(PulseCapture::CaptureStatus_t status)
  {
    timer_.Reset();
    if (is_start_of_new_frame_)
    {
      is_start_of_new_frame_ = false;
    }
    else
    {
      data_frame_.pulse_buffer[data_frame_.pulse_buffer_length++] =
          static_cast<uint16_t>(status.count - last_received_timestamp_);
    }
    last_received_timestamp_ = status.count;
    timer_.Start();

    if (data_frame_.pulse_buffer_length ==
        infrared::DataFrame_t::kMaxPulseBufferSize)
    {
      HandleEndOfFrame();
    }
  }

  /// Invoked when a data frame is currently being received and no new pulse
  /// edge is captured after a timeout period, signifying the end of the data
  /// frame.
  void HandleEndOfFrame()
  {
    timer_.Stop();
    if (user_callback_ != nullptr)
    {
      user_callback_(&data_frame_);
    }
    data_frame_.pulse_buffer_length = 0;
    is_start_of_new_frame_          = true;
  }

 private:
  /// Pulse capture peripheral used to capture incoming pulse edges.
  const sjsu::PulseCapture & capture_;
  /// Timer peripheral for keeping track of the timeout period in between
  /// captured pulses.
  const sjsu::Timer & timer_;
  /// Timeout period used determine if the end of a frame is reached.
  const std::chrono::microseconds kTimeout;
  /// Data frame containing an array of timestamps for captured pulse edges.
  infrared::DataFrame_t data_frame_;
  /// True if captured pulse is the first edge of a new frame.
  bool is_start_of_new_frame_;
  /// The timestamp of the most recent captured pulse edge.
  uint32_t last_received_timestamp_;
  /// The callback that is invoked when a data frame is received.
  DataReceivedHandler user_callback_ = nullptr;
};
}  // namespace sjsu
