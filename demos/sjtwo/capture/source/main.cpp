
#include <cstdint>
#include "L1_Peripheral/example.hpp"
#include "L1_Peripheral/inactive.hpp"
#include "L1_Peripheral/timer.hpp"
#include "L1_Peripheral/capture.hpp"
#include "L1_Peripheral/pwm.hpp"
#include "L1_Peripheral/pin.hpp"
#include "L1_Peripheral/lpc40xx/pin.hpp"
#include "L1_Peripheral/lpc40xx/timer.hpp"
#include "L1_Peripheral/lpc40xx/capture.hpp"
#include "L1_Peripheral/lpc40xx/pwm.hpp"
#include "utility/log.hpp"
#include "utility/map.hpp"
#include "utility/time.hpp"

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
  const uint32_t CaptureFrequency = 4'000'000;
  const uint32_t GeneratorFrequency = 10;
  const float GeneratorDutyCycle = 0.5f;

  // Configure P1.14 as T2_CAP0
  LOG_INFO("Configure P1.14 as capture input T2_CAP0");
  sjsu::lpc40xx::Pin CapturePin(1, 14);
  CapturePin.SetPinFunction(3);
  CapturePin.SetAsOpenDrain(false);
  CapturePin.SetMode(sjsu::Pin::Mode::kInactive);
  
  // Configure Timer #2 capture unit, channel 0
  LOG_INFO("Configuring Timer 2 capture unit to sample Channel 0 at 4 MHz");
  sjsu::lpc40xx::Capture CaptureUnit(sjsu::lpc40xx::Capture::Channel::kCaptureTimer2);
  capture_ref = &CaptureUnit;

  // Have capture unit sample T2_CAP0 at 1 MHz
  CaptureUnit.Initialize(CaptureFrequency, CaptureISR);

  // Create 60 Hz square wave on P2.0, that we can capture
  LOG_INFO("Creating PWM output on P2.0");
  sjsu::lpc40xx::Pwm SignalGenerator(sjsu::lpc40xx::Pwm::Channel::kPwm0);
  SignalGenerator.Initialize(GeneratorFrequency);
  SignalGenerator.SetDutyCycle(GeneratorDutyCycle);

  // Enable capture interrupt for both edges
  LOG_INFO("Enabling capture");
  CaptureUnit.ConfigureCapture(sjsu::Capture::kCaptureChannel0, sjsu::Capture::kCaptureEdgeModeBoth);
  CaptureUnit.EnableCaptureInterrupt(sjsu::Capture::kCaptureChannel0, true);

  while(1)
  {
    sjsu::Delay(1000);
    float average_count = static_cast<float>(delta_accumulator) / static_cast<float>(num_samples);
    double frequency = 1.0f / (average_count * (1.0f / static_cast<float>(CaptureFrequency))) / 2.0f;
    delta_accumulator = 0;
    num_samples = 0;
    LOG_INFO("Input frequency = %.3f Hz, Expcted = %ld", frequency, GeneratorFrequency);
  }

  sjsu::Halt();
  return 0;
}
