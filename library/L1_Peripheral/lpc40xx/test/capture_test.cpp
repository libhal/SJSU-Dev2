#include <cstdint>
#include "L1_Peripheral/cortex/interrupt.hpp"
#include "L0_Platform/lpc40xx/LPC40xx.h"
#include "L1_Peripheral/lpc40xx/timer.hpp"
#include "L1_Peripheral/lpc40xx/capture.hpp"
#include "L4_Testing/testing_frameworks.hpp"
#include "utility/enum.hpp"
#include "utility/status.hpp"


namespace sjsu::lpc40xx
{
EMIT_ALL_METHODS(Capture);

TEST_CASE("Testing lpc40xx Capture", "[lpc40xx-capture]")
{
    LPC_TIM_TypeDef test_timer_register;

    constexpr int channel_list_size = 2;
    constexpr Capture::CaptureChannelNumber channel_list[channel_list_size] = {
        Capture::kCaptureChannel0,
        Capture::kCaptureChannel1,
    };

    constexpr int mode_list_size = 4;
    constexpr Capture::CaptureEdgeMode mode_list[mode_list_size] = {
        Capture::kCaptureEdgeModeNone,
        Capture::kCaptureEdgeModeRising,
        Capture::kCaptureEdgeModeFalling,
        Capture::kCaptureEdgeModeBoth
    };

    IsrPointer test_timer_isr = nullptr;

    Timer::ChannelPartial_t kTestTimerPartial =
    {
        .timer_register = &test_timer_register,
        .power_id = sjsu::lpc40xx::SystemController::Peripherals::kTimer0,
        .irq = IRQn::TIMER0_IRQn,
        .user_callback = &test_timer_isr,
    };
    
    Timer::Channel_t kTestTimer =
    {
        .channel = kTestTimerPartial,
        .isr = nullptr,
    };


    memset(&test_timer_register, 0, sizeof(test_timer_register));

    Mock<sjsu::SystemController> mock_system_controller;    
    Fake(Method(mock_system_controller, PowerUpPeripheral));

    constexpr units::frequency::hertz_t kTestSystemFrequency = 4_MHz;
    constexpr int kTestPeripheralClockDivider = 1;
    
    When(Method(mock_system_controller, GetSystemFrequency)).AlwaysReturn(kTestSystemFrequency);
    When(Method(mock_system_controller, GetPeripheralClockDivider)).AlwaysReturn(kTestPeripheralClockDivider);

    Mock<sjsu::InterruptController> mock_interrupt_controller;
    Fake(Method(mock_interrupt_controller, Register));


    Capture test_subject(kTestTimer, mock_system_controller.get(), mock_interrupt_controller.get());

    SECTION("Capture Initialize")
    {
        constexpr units::frequency::hertz_t kTestFrequency = 1_MHz;

        test_subject.Initialize(kTestFrequency, nullptr);
        int prescaler = (kTestSystemFrequency.to<int32_t>() / kTestPeripheralClockDivider) / kTestFrequency.to<int32_t>();
        CHECK(test_timer_register.PR == prescaler);

        CHECK(bit::Read(test_timer_register.TCR, 0) == true);

    } // end section initialize

    SECTION("Capture Config")
    {
        for(int channel = 0; channel < channel_list_size; channel++)
        {
            int position = channel ? 3 : 0;
            for(int mode = 0; mode < mode_list_size; mode++)
            {
                test_subject.ConfigureCapture(channel_list[channel], mode_list[mode]);
                CHECK(bit::Extract(test_timer_register.CCR, position, 2) == mode_list[mode]);
            }
        }
        
    } // end section capture config

    SECTION("Capture Interrupt")
    {
        for(int channel = 0; channel < channel_list_size; channel++)
        {
            int position = channel ? 5 : 2;
            test_subject.EnableCaptureInterrupt(channel_list[channel], true);
            CHECK(bit::Read(test_timer_register.CCR, position) == true);
            test_subject.EnableCaptureInterrupt(channel_list[channel], false);
            CHECK(bit::Read(test_timer_register.CCR, position) == false);
        }    
    } // end section capture interrupt
   
} // end test case

} // namespace sjsu:lpc40xx


