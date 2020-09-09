#include "L1_Peripheral/lpc17xx/system_controller.hpp"
#include "L1_Peripheral/lpc17xx/pulse_capture.hpp"
#include "L1_Peripheral/lpc17xx/timer.hpp"
#include "L2_HAL/communication/tsop752.hpp"
#include "utility/infrared_algorithms.hpp"
#include "utility/log.hpp"

enum RemoteCode : uint16_t
{
  kPower           = 0x0A5F,
  kVolumeUp        = 0x32CD,
  kVolumeDown      = 0xB24D,
  kToggleBluetooth = 0x40BF,
};

/// @see https://www.sbprojects.net/knowledge/ir/nec.php
constexpr sjsu::infrared::PulseDurationConfiguration_t kNecConfiguration = {
  .header_mark_duration  = 9000us,
  .header_space_duration = 4500us,
  .data_duration         = 560us,
  .logic_high_duration   = 1690us,
  .logic_low_duration    = 560us,
  .encoding_type         = sjsu::infrared::PulseDurationType::kDistance,
  .tolerance             = 0.25f,  // value may vary based on device latency
  .uses_repeat_frames    = true,
  .header_repeat_space   = 2250us,
};

void HandleDataFrame(const sjsu::infrared::DataFrame_t * data_frame)
{
  // Attempt to decode the received frame using the NEC protocol.
  const sjsu::infrared::DecodedFrame_t kDecodedFrame =
      sjsu::infrared::Decode(data_frame, kNecConfiguration);
  // Do nothing if the frame is invalid or the command is from another remote
  if (!kDecodedFrame.is_valid)
  {
    return;
  }
  // Check whether a repeat frame was decoded. A repeat frame after the initial
  // frame is continuously sent while a button is held down.
  if (kDecodedFrame.is_repeat)
  {
    sjsu::LogInfo("Button is held down repeating previous action.");
  }
  else
  {
    constexpr auto kCommandMask = sjsu::bit::MaskFromRange(0, 15);
    const uint16_t kCommand     = static_cast<uint16_t>(
        sjsu::bit::Extract(kDecodedFrame.data, kCommandMask));
    switch (kCommand)
    {
      case RemoteCode::kPower: sjsu::LogInfo("Power Button Pressed"); break;
      case RemoteCode::kVolumeUp: sjsu::LogInfo("Increasing Volume"); break;
      case RemoteCode::kVolumeDown: sjsu::LogInfo("Decreasing Volume"); break;
      case RemoteCode::kToggleBluetooth:
        sjsu::LogInfo("Toggling Bluetooth Mode");
        break;
      default: sjsu::LogInfo("Button Pressed"); break;
    }
  }
}

int main()
{
  sjsu::LogInfo("Starting Tsop752 IR Receiver Example...");

  // Using timeout of 9ms since the header mark of the NEC remote is 9ms long.
  constexpr std::chrono::microseconds kTimeout               = 9ms;
  constexpr units::frequency::hertz_t kPulseCaptureFrequency = 1_MHz;

  sjsu::lpc17xx::PulseCapture pulse_capture(
      sjsu::lpc17xx::PulseCaptureChannel::kCaptureTimer1,
      sjsu::lpc17xx::PulseCapture::CaptureChannelNumber::kChannel0,
      kPulseCaptureFrequency);

  sjsu::lpc17xx::Timer timer(sjsu::lpc17xx::TimerPeripheral::kTimer2);

  sjsu::Tsop752 ir_receiver(pulse_capture, timer, kTimeout);

  ir_receiver.SetInterruptCallback(HandleDataFrame);
  ir_receiver.Initialize();

  sjsu::Halt();
  return 0;
}
