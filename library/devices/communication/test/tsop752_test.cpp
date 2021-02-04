#include <array>

#include "devices/communication/tsop752.hpp"
#include "testing/testing_frameworks.hpp"

namespace sjsu
{
TEST_CASE("Tsop752 Infrared Receiver Test")
{
  Mock<PulseCapture> mock_pulse_capture;
  Mock<Timer> mock_timer;

  Fake(Method(mock_pulse_capture, ConfigureCapture),
       Method(mock_pulse_capture, EnableCaptureInterrupt));
  Fake(Method(mock_timer, SetMatchBehavior), Method(mock_timer, Start),
       Method(mock_timer, Stop), Method(mock_timer, Reset));

  constexpr std::chrono::microseconds kTimeout = 5ms;

  Tsop752 ir_receiver(mock_pulse_capture.get(), mock_timer.get(), kTimeout);

  SECTION("Initialize")
  {
    // Setup
    Fake(Method(mock_timer, Initialize));
    Fake(Method(mock_pulse_capture, Initialize));

    // Exercise
    ir_receiver.Initialize();

    // Verify
    Verify(Method(mock_timer, Initialize)).Once();
    Verify(Method(mock_pulse_capture, Initialize)).Once();
  }

  SECTION("Both peripherals initialization success")
  {
    // Setup
    constexpr uint32_t kExpectedTimerMatchCount   = kTimeout.count();
    constexpr uint8_t kExpectedTimerMatchRegister = 0;

    Fake(Method(mock_pulse_capture, Initialize));
    Fake(Method(mock_timer, Initialize));

    // Exercise
    ir_receiver.Initialize();

    // Verify
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

  SECTION("Interrupt Callbacks")
  {
    constexpr std::array<uint32_t, 6> kTestTimestamps = {
      0, 9'000, 13'500, 14'060, 15'750, 16'310,
    };
    constexpr std::array<uint16_t, 5> kExpectedPulses = {
      9'000, 4'500, 560, 1'690, 560,
    };
    constexpr uint32_t kPulseCaptureFlags = 0x0;

    SECTION("Handle pulse captured")
    {
      for (size_t i = 0; i < kTestTimestamps.size(); i++)
      {
        ir_receiver.HandlePulseCaptured({
            .count = kTestTimestamps[i],
            .flags = kPulseCaptureFlags,
        });

        Verify(Method(mock_timer, Reset), Method(mock_timer, Start));
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
