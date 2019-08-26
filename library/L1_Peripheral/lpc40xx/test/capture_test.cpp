#include <cstdint>
#include "L1_Peripheral/cortex/interrupt.hpp"
#include "L1_Peripheral/interrupt.hpp"
#include "L0_Platform/lpc40xx/LPC40xx.h"
#include "L1_Peripheral/lpc40xx/timer.hpp"
#include "L1_Peripheral/lpc40xx/capture.hpp"
#include "L4_Testing/testing_frameworks.hpp"
#include "utility/enum.hpp"
#include "utility/status.hpp"
#include "utility/units.hpp"


namespace sjsu::lpc40xx
{
EMIT_ALL_METHODS(Capture);

TEST_CASE("Testing lpc40xx Capture", "[lpc40xx-capture]")
{
    LPC_TIM_TypeDef test_timer_register;

    constexpr int kChannelListSize = 2;
    constexpr Capture::CaptureChannelNumber kChannelList[kChannelListSize] = {
        Capture::kCaptureChannel0,
        Capture::kCaptureChannel1,
    };

    constexpr int kModeListSize = 4;
    constexpr Capture::CaptureEdgeMode kModeList[kModeListSize] = {
        Capture::kCaptureEdgeModeNone,
        Capture::kCaptureEdgeModeRising,
        Capture::kCaptureEdgeModeFalling,
        Capture::kCaptureEdgeModeBoth
    };

    IsrPointer test_timer_isr = nullptr;

    Timer::ChannelPartial_t test_timer_partial =
    {
        .timer_register = &test_timer_register,
        .power_id = SystemController::Peripherals::kTimer0,
        .irq = IRQn::TIMER0_IRQn,
        .user_callback = &test_timer_isr,
    };

    Timer::Channel_t test_timer =
    {
        .channel = test_timer_partial,
        .isr = nullptr,
    };


    memset(&test_timer_register, 0, sizeof(test_timer_register));

    Mock<sjsu::SystemController> mock_system_controller;
    Fake(Method(mock_system_controller, PowerUpPeripheral));

    constexpr units::frequency::hertz_t kTestSystemFrequency = 4_MHz;
    constexpr int kTestPeripheralClockDivider = 1;

    When(Method(mock_system_controller, GetSystemFrequency))
        .AlwaysReturn(kTestSystemFrequency);
    When(Method(mock_system_controller, GetPeripheralClockDivider))
        .AlwaysReturn(kTestPeripheralClockDivider);

    Mock<sjsu::InterruptController> mock_interrupt_controller;
    Fake(Method(mock_interrupt_controller, Register));


    Capture test_subject(test_timer,
        mock_system_controller.get(),
        mock_interrupt_controller.get());

    SECTION("Capture Initialize")
    {
        constexpr units::frequency::hertz_t kTestFrequency = 1_MHz;

        test_subject.Initialize(kTestFrequency, nullptr);
        int prescaler = (kTestSystemFrequency.to<int32_t>() /
                kTestPeripheralClockDivider) / kTestFrequency.to<int32_t>();
        CHECK(test_timer_register.PR == prescaler);
        CHECK(bit::Read(test_timer_register.TCR, 0) == true);
    }  // end section initialize

    SECTION("Capture Config")
    {
        for (int channel = 0; channel < kChannelListSize; channel++)
        {
            int position = channel ? 3 : 0;
            for (int mode = 0; mode < kModeListSize; mode++)
            {
                test_subject.ConfigureCapture(
                    kChannelList[channel],
                    kModeList[mode]);
                CHECK(bit::Extract(test_timer_register.CCR, position, 2) ==
                    kModeList[mode]);
            }
        }
    }  // end section capture config

    SECTION("Capture Interrupt")
    {
        for (int channel = 0; channel < kChannelListSize; channel++)
        {
            int position = channel ? 5 : 2;
            test_subject.EnableCaptureInterrupt(kChannelList[channel], true);
            CHECK(bit::Read(test_timer_register.CCR, position) == true);
            test_subject.EnableCaptureInterrupt(kChannelList[channel], false);
            CHECK(bit::Read(test_timer_register.CCR, position) == false);
        }
    }  // end section capture interrupt
}  // end test case
}  // namespace sjsu::lpc40xx


