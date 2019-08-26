
// NOTE: Connect pin 2.0 to pin 1.14 with a jumper wire to run this test.

#include <cstdint>
#include "L1_Peripheral/example.hpp"
#include "L1_Peripheral/inactive.hpp"
#include "L1_Peripheral/timer.hpp"
#include "L1_Peripheral/capture.hpp"
#include "L1_Peripheral/pwm.hpp"
#include "L1_Peripheral/pin.hpp"
#include "L1_Peripheral/interrupt.hpp"
#include "L1_Peripheral/lpc40xx/pin.hpp"
#include "L1_Peripheral/lpc40xx/timer.hpp"
#include "L1_Peripheral/lpc40xx/capture.hpp"
#include "L1_Peripheral/lpc40xx/pwm.hpp"
#include "utility/log.hpp"
#include "utility/map.hpp"
#include "utility/time.hpp"
#include "utility/units.hpp"

sjsu::lpc40xx::Capture *capture_ref = nullptr;
sjsu::lpc40xx::Capture::CaptureStatus_t status;
volatile uint32_t prev_count;
volatile uint32_t curr_count;
volatile uint32_t delta_accumulator;
volatile uint32_t num_samples;

void CaptureISR(void)
{
  capture_ref->GetCaptureStatus(sjsu::Capture::kCaptureChannel0, status);
  prev_count = curr_count;
  curr_count = status.count;
  delta_accumulator += (curr_count - prev_count);
  ++num_samples;
}

int main()
{
  constexpr units::frequency::hertz_t kCaptureFrequency = 4_MHz;
  constexpr units::frequency::hertz_t kGeneratorFrequency = 10_Hz;
  constexpr float kGeneratorDutyCycle = 0.5f;

  // Configure P1.14 as T2_CAP0
  LOG_INFO("Configure P1.14 as capture input T2_CAP0");
  sjsu::lpc40xx::Pin capture_pin(1, 14);
  capture_pin.SetPinFunction(3);
  capture_pin.SetAsOpenDrain(false);

  // Configure Timer #2 capture unit, channel 0
  LOG_INFO("Configuring Timer 2 capture unit to sample Channel 0 at 4 MHz");
  sjsu::lpc40xx::Capture capture_unit(
    sjsu::lpc40xx::Capture::Channel::kCaptureTimer2);
  capture_ref = &capture_unit;

  // Have capture unit sample T2_CAP0 at 1 MHz
  capture_unit.Initialize(kCaptureFrequency, CaptureISR);

  // Create 60 Hz square wave on P2.0, that we can capture
  LOG_INFO("Creating PWM output on P2.0");
  sjsu::lpc40xx::Pwm signal_generator(sjsu::lpc40xx::Pwm::Channel::kPwm0);
  signal_generator.Initialize(kGeneratorFrequency);
  signal_generator.SetDutyCycle(kGeneratorDutyCycle);

  // Enable capture interrupt for both edges
  LOG_INFO("Enabling capture");
  capture_unit.ConfigureCapture(sjsu::Capture::kCaptureChannel0,
    sjsu::Capture::kCaptureEdgeModeBoth);
  capture_unit.EnableCaptureInterrupt(sjsu::Capture::kCaptureChannel0, true);

  while (1)
  {
    sjsu::Delay(1s);
    float average_count = static_cast<float>(delta_accumulator)
      / static_cast<float>(num_samples);
    double frequency = 1.0f / (average_count *
      (1.0f / kCaptureFrequency.to<float>())) / 2.0f;
    delta_accumulator = 0;
    num_samples = 0;
    LOG_INFO("Input frequency = %.3f Hz, Expcted = %d", frequency,
      kGeneratorFrequency.to<int>());
  }

  sjsu::Halt();
  return 0;
}
