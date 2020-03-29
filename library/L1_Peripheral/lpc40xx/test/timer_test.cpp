#include "L4_Testing/testing_frameworks.hpp"
#include "L1_Peripheral/lpc40xx/timer.hpp"

namespace sjsu::lpc40xx
{
EMIT_ALL_METHODS(Timer);

TEST_CASE("Testing lpc40xx Timer", "[lpc40xx-timer]")
{
  LPC_TIM_TypeDef local_timer_registers;
  memset(&local_timer_registers, 0, sizeof(local_timer_registers));

  constexpr units::frequency::hertz_t kSystemControllerClockFrequency = 12_MHz;
  constexpr uint32_t kExpectedPeripheralClockDivider                  = 1;
  constexpr SystemController::PeripheralID kExpectedPeripheralId =
      SystemController::Peripherals::kTimer0;
  constexpr int kExpectedIrq                   = 0;
  constexpr size_t kExpectedMatchRegisterCount = 4;

  // Setup mock interrupt controller
  Mock<sjsu::InterruptController> mock_interrupt_controller;
  Fake(Method(mock_interrupt_controller, Enable));
  sjsu::InterruptController::SetPlatformController(
      &mock_interrupt_controller.get());

  // Setup mock system controller
  Mock<sjsu::SystemController> mock_system_controller;
  Fake(Method(mock_system_controller, PowerUpPeripheral));
  When(Method(mock_system_controller, GetSystemFrequency))
      .AlwaysReturn(kSystemControllerClockFrequency);
  When(Method(mock_system_controller, GetPeripheralClockDivider))
      .AlwaysReturn(kExpectedPeripheralClockDivider);
  sjsu::SystemController::SetPlatformController(&mock_system_controller.get());

  const Timer::Peripheral_t kTimerPeripheral = {
    .peripheral = &local_timer_registers,
    .id         = kExpectedPeripheralId,
    .irq        = kExpectedIrq,
  };
  Timer timer(kTimerPeripheral);

  SECTION("Initialize")
  {
    constexpr units::frequency::hertz_t kExpectedFrequency = 1_MHz;
    constexpr uint32_t kExpectedPrescaler =
        kSystemControllerClockFrequency / kExpectedFrequency;
    const Status kStatus = timer.Initialize(kExpectedFrequency);

    Verify(Method(mock_system_controller, PowerUpPeripheral)
               .Matching([](sjsu::SystemController::PeripheralID id) {
                 return kExpectedPeripheralId.device_id == id.device_id;
               }),
           Method(mock_interrupt_controller, Enable));
    CHECK(kStatus == Status::kSuccess);
    CHECK(local_timer_registers.PR == kExpectedPrescaler);
  }

  SECTION("SetMatchBehavior")
  {
    constexpr std::array<uint32_t, kExpectedMatchRegisterCount> kTicks = {
      1'000, 100'000, 100, 123
    };
    constexpr std::array kMatchActions = {
      Timer::MatchAction::kInterrupt,
      Timer::MatchAction::kRestart,
      Timer::MatchAction::kStop,
      Timer::MatchAction::kInterruptRestart,
      Timer::MatchAction::kInterruptStop,
      Timer::MatchAction::kRestartStop,
      Timer::MatchAction::kInterruptRestartStop
    };
    volatile uint32_t * match_register_pointer = &local_timer_registers.MR0;

    for (size_t i = 0; i < kExpectedMatchRegisterCount; i++)
    {
      const uint8_t kExpectedMatchRegister = i;
      for (size_t j = 0; j < kMatchActions.size(); j++)
      {
        const Timer::MatchAction kMatchAction = kMatchActions[j];
        const bit::Mask kMatchActionMask      = {
          .position = static_cast<uint8_t>(kExpectedMatchRegister * 3),
          .width    = 3,
        };

        INFO("ticks: " << static_cast<size_t>(kTicks[i]));
        INFO("action: " << static_cast<size_t>(kMatchActions[j]));
        INFO("match_register: " << i);

        timer.SetMatchBehavior(kTicks[i], kMatchActions[j],
                               kExpectedMatchRegister);

        CHECK(bit::Extract(local_timer_registers.MCR, kMatchActionMask) ==
              static_cast<uint8_t>(kMatchAction));
        CHECK(match_register_pointer[kExpectedMatchRegister] == kTicks[i]);
      }
    }
  }

  SECTION("GetAvailableMatchRegisters")
  {
    CHECK(timer.GetAvailableMatchRegisters() == kExpectedMatchRegisterCount);
  }

  SECTION("GetCount")
  {
    CHECK(local_timer_registers.TC == 0);
    CHECK(timer.GetCount() == 0);

    constexpr uint32_t kExpectedCount = 12'345;
    local_timer_registers.TC          = kExpectedCount;
    CHECK(timer.GetCount() == kExpectedCount);
  }

  SECTION("Start")
  {
    volatile bool timer_start = bit::Read(
        local_timer_registers.TCR, Timer::TimerControlRegister::kEnableBit);
    CHECK(timer_start == false);
    timer.Start();
    CHECK(bit::Read(local_timer_registers.TCR,
                    Timer::TimerControlRegister::kEnableBit));
  }

  SECTION("Stop")
  {
    local_timer_registers.TCR = bit::Set(
        local_timer_registers.TCR, Timer::TimerControlRegister::kEnableBit);
    CHECK(bit::Read(local_timer_registers.TCR,
                    Timer::TimerControlRegister::kEnableBit) == 0b1);
    timer.Stop();
    CHECK(bit::Read(local_timer_registers.TCR,
                    Timer::TimerControlRegister::kEnableBit) == 0b0);
  }
}
}  // namespace sjsu::lpc40xx
