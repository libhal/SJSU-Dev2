#include <array>

#include "L2_HAL/communication/tsop752.hpp"
#include "L4_Testing/testing_frameworks.hpp"

namespace sjsu
{
EMIT_ALL_METHODS(Tsop752);

TEST_CASE("Tsop752 Infrared Receiver Test", "[tsop752]")
{
  Mock<PulseCapture> mock_pulse_capture;
  Mock<Timer> mock_timer;

  Fake(Method(mock_pulse_capture, ConfigureCapture),
       Method(mock_pulse_capture, EnableCaptureInterrupt));
  Fake(Method(mock_timer, SetMatchBehavior),
       Method(mock_timer, Start),
       Method(mock_timer, Stop),
       Method(mock_timer, Reset));

  const std::array kFailureStatus = {
    Status::kTimedOut,          Status::kBusError,
    Status::kDeviceNotFound,    Status::kInvalidSettings,
    Status::kNotImplemented,    Status::kNotReadyYet,
    Status::kInvalidParameters,
  };
  constexpr std::chrono::microseconds kTimeout = 5ms;

  Tsop752 ir_receiver(mock_pulse_capture.get(), mock_timer.get(), kTimeout);

  SECTION("Initialize")
  {
    SECTION("PulseCapture peripheral initialization failure")
    {
      mock_timer.ClearInvocationHistory();
      When(Method(mock_timer, Initialize)).AlwaysReturn(Status::kSuccess);
      for (size_t i = 0; i < kFailureStatus.size(); i++)
      {
        INFO(
            "Testing pulse capture initialization failure with failure status: "
            << i);
        const Status kExpectedStatus = kFailureStatus[i];
        mock_pulse_capture.ClearInvocationHistory();
        When(Method(mock_pulse_capture, Initialize))
            .AlwaysReturn(kExpectedStatus);
        // Upon failing to initialize the pulse capture peripheral, the failure
        // Status should be immediately returned.
        CHECK(ir_receiver.Initialize() == kExpectedStatus);
        Verify(Method(mock_pulse_capture, Initialize)).Once();
        VerifyNoOtherInvocations(mock_pulse_capture, mock_timer);
      }
    }
    SECTION("Timer peripheral initialization failure")
    {
      When(Method(mock_pulse_capture, Initialize))
          .AlwaysReturn(Status::kSuccess);

      for (size_t i = 0; i < kFailureStatus.size(); i++)
      {
        INFO("Testing timer initialization failure with failure status: " << i);
        const Status kExpectedStatus = kFailureStatus[i];
        mock_pulse_capture.ClearInvocationHistory();
        mock_timer.ClearInvocationHistory();
        When(Method(mock_timer, Initialize)).AlwaysReturn(kExpectedStatus);

        CHECK(ir_receiver.Initialize() == kExpectedStatus);
        Verify(Method(mock_pulse_capture, Initialize),
               Method(mock_pulse_capture, ConfigureCapture),
               Method(mock_pulse_capture, EnableCaptureInterrupt),
               Method(mock_timer, Initialize),
               Method(mock_timer, SetMatchBehavior))
            .Once();
      }
    }
    SECTION("Both peripherals initialization success")
    {
      constexpr Status kExpectedStatus              = Status::kSuccess;
      constexpr uint32_t kExpectedTimerMatchCount   = kTimeout.count();
      constexpr uint8_t kExpectedTimerMatchRegister = 0;

      mock_pulse_capture.ClearInvocationHistory();
      mock_timer.ClearInvocationHistory();
      When(Method(mock_pulse_capture, Initialize))
          .AlwaysReturn(kExpectedStatus);
      When(Method(mock_timer, Initialize)).AlwaysReturn(kExpectedStatus);

      CHECK(ir_receiver.Initialize() == kExpectedStatus);
      Verify(Method(mock_pulse_capture, Initialize)).Once();
      Verify(Method(mock_pulse_capture, ConfigureCapture)
                 .Using(sjsu::PulseCapture::CaptureEdgeMode::kBoth),
             Method(mock_pulse_capture, EnableCaptureInterrupt).Using(true))
          .Once();
      Verify(Method(mock_timer, Initialize),
             Method(mock_timer, SetMatchBehavior)
                 .Using(kExpectedTimerMatchCount,
                        sjsu::Timer::MatchAction::kInterrupt,
                        kExpectedTimerMatchRegister))
          .Once();
    }
  }

  SECTION("Interrupt Callbacks")
  {
    constexpr std::array<uint32_t, 6> kTestTimestamps = {
      0, 9'000, 13'500, 14'060, 15'750, 16'310
    };
    constexpr std::array<uint16_t, 5> kExpectedPulses = {
      9'000, 4'500, 560, 1'690, 560
    };
    constexpr uint32_t kPulseCaptureFlags = 0x0;

    SECTION("Handle pulse captured")
    {
      for (size_t i = 0; i < kTestTimestamps.size(); i++)
      {
        mock_timer.ClearInvocationHistory();
        ir_receiver.HandlePulseCaptured({
            .count = kTestTimestamps[i],
            .flags = kPulseCaptureFlags,
        });
        Verify(Method(mock_timer, Reset), Method(mock_timer, Start)).Once();
      }
    }
    SECTION("Handle end of frame")
    {
      bool user_callback_invoked = false;
      InfraredReceiver::DataReceivedHandler mock_user_callback =
          [&](auto data_frame) {
            user_callback_invoked = true;
            for (size_t i = 0; i < kExpectedPulses.size(); i++)
            {
              INFO("Handling end of frame pulse edge index " << i);
              CHECK(data_frame->pulse_buffer[i] == kExpectedPulses[i]);
            }
            CHECK(data_frame->pulse_buffer_length == kExpectedPulses.size());
          };

      for (size_t i = 0; i < kTestTimestamps.size(); i++)
      {
        ir_receiver.HandlePulseCaptured({
            .count = kTestTimestamps[i],
            .flags = kPulseCaptureFlags,
        });
      }
      ir_receiver.SetInterruptCallback(mock_user_callback);
      ir_receiver.HandleEndOfFrame();

      Verify(Method(mock_timer, Stop)).Once();
      CHECK(user_callback_invoked);
    }
  }
}
}  // namespace sjsu
