#include <cstdint>

#include "L1_Peripheral/lpc40xx/pulse_capture.hpp"
#include "L1_Peripheral/lpc40xx/pwm.hpp"

uint32_t previous_count;
uint32_t current_count;
uint32_t delta_accumulator;
uint32_t num_samples;

void CaptureISR(sjsu::PulseCapture::CaptureStatus_t status)
{
  previous_count = current_count;
  current_count  = status.count;
  delta_accumulator += (current_count - previous_count);
  ++num_samples;
}

int main()
{
  /* Sample our PWM signal generator output at 4 MHz */
  constexpr units::frequency::hertz_t kCaptureFrequency = 4_MHz;

  /* Generate a 10 Hz square wave using PWM as a test signal generator */
  constexpr units::frequency::hertz_t kGeneratorFrequency = 10_Hz;

  /* Configure the test signal duty cycle to 50% */
  constexpr float kGeneratorDutyCycle = 0.5f;

  sjsu::lpc40xx::PulseCapture capture_unit(
      sjsu::lpc40xx::PulseCapture::Channel::kCaptureTimer2,
      sjsu::lpc40xx::PulseCapture::CaptureChannelNumber::kChannel0,
      kCaptureFrequency);

  // Configure P1.14 as T2_CAP0
  sjsu::LogInfo("Configure P1.14 as capture input T2_CAP0");

  // Have capture unit sample T2_CAP0 at 1 MHz
  capture_unit.Initialize(CaptureISR);

  // Create 60 Hz square wave on P2.0, that we can capture
  sjsu::LogInfo("Creating PWM output on P2.0");
  sjsu::lpc40xx::Pwm signal_generator(sjsu::lpc40xx::Pwm::Channel::kPwm0);
  signal_generator.Initialize();
  signal_generator.ConfigureFrequency(kGeneratorFrequency);
  signal_generator.Enable();
  signal_generator.SetDutyCycle(kGeneratorDutyCycle);

  // Enable capture interrupt for both edges
  sjsu::LogInfo("Enabling capture");
  capture_unit.ConfigureCapture(sjsu::PulseCapture::CaptureEdgeMode::kBoth);
  capture_unit.EnableCaptureInterrupt(true);

  while (1)
  {
    // Wait long enough for the capture interrupt to trigger at least once
    sjsu::Delay(1s);

    const float kDeltaAccumulator = static_cast<float>(delta_accumulator);
    const float kNumberOfSamples  = static_cast<float>(num_samples);
    const float kCaptureFloat     = kCaptureFrequency.to<float>();
    const float kAverageCount     = kDeltaAccumulator / kNumberOfSamples;

    double frequency = 1.0f / (kAverageCount * (1.0f / kCaptureFloat)) / 2.0f;

    sjsu::LogInfo("Input frequency = %.3f Hz, Expected = %d Hz",
                  frequency,
                  kGeneratorFrequency.to<int>());

    delta_accumulator = 0;
    num_samples       = 0;
  }

  return 0;
}
