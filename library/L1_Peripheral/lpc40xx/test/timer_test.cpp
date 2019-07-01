#include "L0_Platform/lpc40xx/LPC40xx.h"
#include "L0_Platform/lpc40xx/interrupt.hpp"
#include "L0_Platform/interrupt.hpp"
#include "L1_Peripheral/lpc40xx/timer.hpp"
#include "L4_Testing/testing_frameworks.hpp"

namespace sjsu::lpc40xx
{
EMIT_ALL_METHODS(Timer);

TEST_CASE("Testing lpc40xx adc", "[lpc40xx-adc]")
{
  // Create local version of LPC_ADC

  LPC_TIM_TypeDef local_timer;
  // Clear local adc registers
  memset(&local_timer, 0, sizeof(local_timer));

  // Set this up later during the test
  static IsrPointer dummy_timer_isr = nullptr;

  static Timer::ChannelPartial_t kDummyTimerPartial = {
    .timer_register = &local_timer,
    .power_id       = sjsu::lpc40xx::SystemController::Peripherals::kTimer0,
    .irq            = IRQn::TIMER0_IRQn,
    .user_callback  = &dummy_timer_isr,
  };

  static Timer::Channel_t kDummyTimer = {
    .channel = kDummyTimerPartial,
    .isr     = Timer::TimerHandler<kDummyTimerPartial>,
  };
  // Set mock for sjsu::SystemController
  constexpr uint32_t kDummySystemControllerClockFrequency = 12'000'000;
  Mock<sjsu::SystemController> mock_system_controller;
  Fake(Method(mock_system_controller, PowerUpPeripheral));
  When(Method(mock_system_controller, GetPeripheralFrequency))
      .AlwaysReturn(kDummySystemControllerClockFrequency);

  // Create ports and pins to test and mock
  Timer test_subject(kDummyTimer, mock_system_controller.get());

  SECTION("Initialization")
  {
    // Source "UM10562 LPC408x/7x User Manual" table 678 page 805
    constexpr uint32_t kDummyFrequency = 1'000'000;
    auto dummy_isr = [](void) { return; };
    IsrPointer kDummyIsr = dummy_isr;
    IsrPointer *kDummyIsrPointer = &kDummyIsr;

    test_subject.Initialize(kDummyFrequency, kDummyIsr);

    Verify(Method(mock_system_controller, PowerUpPeripheral)
               .Matching([](sjsu::SystemController::PeripheralID id) {
                 return sjsu::lpc40xx::SystemController::Peripherals::kTimer0
                            .device_id == id.device_id;
               }));

    // Check if any bits in the clock divider are set given a frequency of
    uint32_t expected_divider =
        kDummySystemControllerClockFrequency / kDummyFrequency;
    CHECK(expected_divider == local_timer.PR);
    // Check first bit (enable bit)
    CHECK(1 == local_timer.TCR);
    // Check first bit (enable bit)
    CHECK(kDummyIsrPointer == kDummyTimer.channel.user_callback);
  }
  // SECTION("Conversion and finished conversion")
  // {
  //   // Source "UM10562 LPC408x/7x User Manual" table 678 page 805
  //   constexpr uint8_t kStartNoBurst = 0b1;
  //   constexpr uint8_t kStartBurst   = 0;
  //   constexpr uint8_t kStartBit     = 24;
  //   constexpr uint8_t kDone         = 0b1;
  //   constexpr uint8_t kDoneBit      = 31;

  //   // Check if bit 24 in local_adc.CR for the start bits is set
  //   local_adc.DR[kMockChannel1.channel] |= (1 << kDoneBit);
  //   channel0_mock.Conversion();
  //   CHECK(((local_adc.CR >> kStartBit) & 1) == kStartNoBurst);
  //   channel0_mock.HasConversionFinished();
  //   CHECK(((local_adc.DR[kMockChannel1.channel] >> kDoneBit) & 1) == kDone);

  //   // Check if bits 24 to 26 in local_adc.CR are cleared for burst mode
  //   // conversion
  //   channel0_mock.BurstMode(true);
  //   channel0_mock.Conversion();
  //   CHECK(((local_adc.CR >> kStartBit) & 0b111) == kStartBurst);

  //   // Check if done bit is set after conversion
  //   channel0_mock.HasConversionFinished();
  //   CHECK(((local_adc.DR[kMockChannel1.channel] >> kDoneBit) & 1) == kDone);
  // }
  // SECTION("Burst mode")
  // {
  //   // Source "UM10562 LPC408x/7x User Manual" table 678 page 805
  //   constexpr uint8_t kBurstOn  = 0b1;
  //   constexpr uint8_t kBurstOff = 0b0;

  //   // Only need to test if burst mode will turn on and off
  //   // Burst mode applies to all channels that are initialized
  //   channel0_mock.BurstMode(true);
  //   CHECK(((local_adc.CR >> kBurstBit) & 1) == kBurstOn);
  //   channel0_mock.BurstMode(false);
  //   CHECK(((local_adc.CR >> kBurstBit) & 1) == kBurstOff);
  // }
  // SECTION("Read result")
  // {
  //   constexpr uint16_t kResultMask = 0xfff;
  //   constexpr uint8_t kResultBit   = 4;

  //   // Check if there is any value in the global data reg
  //   channel0_mock.Read();
  //   CHECK(((local_adc.DR[kMockChannel1.channel] >> kResultBit) & kResultMask) ==
  //         0);
  // }
}
}  // namespace sjsu::lpc40xx
