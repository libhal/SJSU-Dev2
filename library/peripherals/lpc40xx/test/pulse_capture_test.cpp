#include "peripherals/lpc40xx/pulse_capture.hpp"

#include <cstdint>

#include "testing/testing_frameworks.hpp"
#include "utility/enum.hpp"
#include "utility/error_handling.hpp"
#include "utility/math/units.hpp"

namespace sjsu::lpc40xx
{
PulseCapture::CaptureStatus_t isr_result;
PulseCapture::CaptureCallback test_timer_callback = nullptr;

void DummyCallback(PulseCapture::CaptureStatus_t status)
{
  memcpy(&isr_result, &status, sizeof(PulseCapture::CaptureStatus_t));
}

TEST_CASE("Testing lpc40xx Pulse Capture")
{
  LPC_TIM_TypeDef test_timer_register;

  constexpr PulseCapture::CaptureEdgeMode kModeList[] = {
    PulseCapture::CaptureEdgeMode::kNone,
    PulseCapture::CaptureEdgeMode::kRising,
    PulseCapture::CaptureEdgeMode::kFalling,
    PulseCapture::CaptureEdgeMode::kBoth
  };

  static Mock<sjsu::Pin> mock_pin0;
  static Mock<sjsu::Pin> mock_pin1;

  Fake(Method(mock_pin0, Pin::ModuleInitialize),
       Method(mock_pin1, Pin::ModuleInitialize));

  sjsu::Pin & capture0_input_pin = mock_pin0.get();
  sjsu::Pin & capture1_input_pin = mock_pin1.get();

  PulseCapture::CaptureChannelNumber test_timer_channel_number0 =
      PulseCapture::kChannel0;
  PulseCapture::CaptureChannelNumber test_timer_channel_number1 =
      PulseCapture::kChannel1;

  static const PulseCapture::CaptureChannelPartial_t kTestTimerPartial0 = {
    .timer_register = &test_timer_register,
    .id             = SystemController::Peripherals::kTimer0,
    .irq            = IRQn::TIMER0_IRQn,
    .user_callback  = &test_timer_callback,
    .capture_pin0   = capture0_input_pin,
    .capture_pin1   = capture1_input_pin,
    .channel_number = &test_timer_channel_number0
  };

  static const PulseCapture::CaptureChannelPartial_t kTestTimerPartial1 = {
    .timer_register = &test_timer_register,
    .id             = SystemController::Peripherals::kTimer0,
    .irq            = IRQn::TIMER0_IRQn,
    .user_callback  = &test_timer_callback,
    .capture_pin0   = capture0_input_pin,
    .capture_pin1   = capture1_input_pin,
    .channel_number = &test_timer_channel_number1
  };

  const PulseCapture::CaptureChannel_t kTestTimerCh0 = {
    .channel = kTestTimerPartial0,
    .handler = []() { PulseCapture::TimerHandler(kTestTimerPartial0); }
  };
  const PulseCapture::CaptureChannel_t kTestTimerCh1 = {
    .channel = kTestTimerPartial1,
    .handler = []() { PulseCapture::TimerHandler(kTestTimerPartial1); }
  };

  testing::ClearStructure(&test_timer_register);

  constexpr units::frequency::hertz_t kTestSystemFrequency = 4_MHz;
  constexpr int kTestPeripheralClockDivider                = 1;

  Mock<sjsu::SystemController> mock_system_controller;
  Fake(Method(mock_system_controller, PowerUpPeripheral));
  When(Method(mock_system_controller, GetClockRate))
      .AlwaysReturn(kTestSystemFrequency);
  sjsu::SystemController::SetPlatformController(&mock_system_controller.get());

  Mock<sjsu::InterruptController> mock_interrupt_controller;
  Fake(Method(mock_interrupt_controller, Enable));
  Fake(Method(mock_interrupt_controller, Disable));
  sjsu::InterruptController::SetPlatformController(
      &mock_interrupt_controller.get());

  constexpr units::frequency::hertz_t kTestFrequency = 4_MHz;

  PulseCapture test_subject0(kTestTimerCh0,
                             PulseCapture::CaptureChannelNumber::kChannel0,
                             kTestFrequency);

  PulseCapture test_subject1(kTestTimerCh1,
                             PulseCapture::CaptureChannelNumber::kChannel1,
                             kTestFrequency);

  PulseCapture * test_subjects[2] = { &test_subject0, &test_subject1 };

  SECTION("Capture Initialize Channel 0")
  {
    test_subjects[0]->Initialize(&DummyCallback);
    int prescaler =
        (kTestSystemFrequency.to<int32_t>() / kTestPeripheralClockDivider) /
        kTestFrequency.to<int32_t>();

    Verify(
        Method(mock_interrupt_controller, Enable)
            .Matching([kTestTimerCh0](
                          sjsu::InterruptController::RegistrationInfo_t info) {
              return (info.interrupt_request_number ==
                      kTestTimerCh0.channel.irq) &&
                     (info.priority == -1);
            }));

    CHECK(test_timer_register.PR == prescaler);
    CHECK(bit::Read(test_timer_register.TCR, 0) == true);
  }  // end section initialize

  SECTION("Capture Initialize Channel 1")
  {
    test_subjects[1]->Initialize(&DummyCallback);
    int prescaler =
        (kTestSystemFrequency.to<int32_t>() / kTestPeripheralClockDivider) /
        kTestFrequency.to<int32_t>();

    Verify(
        Method(mock_interrupt_controller, Enable)
            .Matching([kTestTimerCh1](
                          sjsu::InterruptController::RegistrationInfo_t info) {
              return (info.interrupt_request_number ==
                      kTestTimerCh1.channel.irq) &&
                     (info.priority == -1);
            }));

    CHECK(test_timer_register.PR == prescaler);
    CHECK(bit::Read(test_timer_register.TCR, 0) == true);
  }  // end section initialize

  SECTION("Capture Config Channel 0")
  {
    int position = 0;
    for (size_t mode = 0; mode < std::size(kModeList); mode++)
    {
      test_subjects[0]->ConfigureCapture(kModeList[mode]);
      CHECK(bit::Extract(test_timer_register.CCR, position, 2) ==
            static_cast<uint32_t>(kModeList[mode]));
    }
  }  // end section capture config

  SECTION("Capture Config Channel 0")
  {
    int position = 3;
    for (size_t mode = 0; mode < std::size(kModeList); mode++)
    {
      test_subjects[1]->ConfigureCapture(kModeList[mode]);
      CHECK(bit::Extract(test_timer_register.CCR, position, 2) ==
            static_cast<uint32_t>(kModeList[mode]));
    }
  }  // end section capture config

  SECTION("Capture Interrupt Channel 0")
  {
    int position = 2;
    test_subjects[0]->EnableCaptureInterrupt(true);
    CHECK(bit::Read(test_timer_register.CCR, position) == true);
    test_subjects[0]->EnableCaptureInterrupt(false);
    CHECK(bit::Read(test_timer_register.CCR, position) == false);
  }  // end section capture interrupt

  SECTION("Capture Interrupt Channel 1")
  {
    int position = 5;
    test_subjects[1]->EnableCaptureInterrupt(true);
    CHECK(bit::Read(test_timer_register.CCR, position) == true);
    test_subjects[1]->EnableCaptureInterrupt(false);
    CHECK(bit::Read(test_timer_register.CCR, position) == false);
  }  // end section capture interrupt

  SECTION("Capture Interrupt Handler Acknowledge (Common)")
  {
    testing::ClearStructure(&isr_result);
    kTestTimerCh0.handler();
    CHECK(bit::Extract(test_timer_register.IR, 4, 2) == 0b11);
  }

  SECTION("Capture Interrupt Handler Channel 0")
  {
    constexpr uint32_t kCr0TestPattern = 0x22226666;
    testing::ClearStructure(&isr_result);
    test_timer_register.CR0 = kCr0TestPattern;
    kTestTimerCh0.handler();
    CHECK(isr_result.count == kCr0TestPattern);
  }  // end section capture interrupt handler

  SECTION("Capture Interrupt Handler Channel 0")
  {
    constexpr uint32_t kCr1TestPattern = 0x33337777;
    testing::ClearStructure(&isr_result);
    test_timer_register.CR1 = kCr1TestPattern;
    kTestTimerCh1.handler();
    CHECK(isr_result.count == kCr1TestPattern);
  }  // end section capture interrupt handler
}  // end test case
}  // namespace sjsu::lpc40xx
