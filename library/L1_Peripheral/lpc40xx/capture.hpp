
#pragma once
#include <cstdint>

#include "L0_Platform/lpc40xx/LPC40xx.h"
#include "L0_Platform/interrupt.hpp"
#include "L1_Peripheral/lpc40xx/system_controller.hpp"
#include "L1_Peripheral/lpc40xx/timer.hpp"
#include "L1_Peripheral/lpc40xx/capture.hpp"
#include "L1_Peripheral/timer.hpp"
#include "L1_Peripheral/capture.hpp"
#include "utility/log.hpp"
#include "utility/status.hpp"

namespace sjsu
{
    namespace lpc40xx
    {
        class Capture final : public sjsu::Capture
        {
            public:

            template<const sjsu::lpc40xx::Timer::ChannelPartial_t &port>
            static void CaptureHandler()
            {
                CaptureHandler(port);
            }

            static void CaptureHandler(const sjsu::lpc40xx::Timer::ChannelPartial_t &channel)
            {
                if(*channel.user_callback != nullptr)
                {
                    (*channel.user_callback)();
                }
                channel.timer_register->IR |= 0x30;
            }

            typedef struct
            {
                uint32_t count;
                uint32_t flags;
            } CaptureStatus_t;          

            struct Channel
            {
                private:

                /* HACK FIXME: Needed for initializing .isr member due to const declaration */
                inline static const sjsu::lpc40xx::Timer::ChannelPartial_t &kTimerPartial0 = sjsu::lpc40xx::Timer::Channel::kTimer0.channel;
                inline static const sjsu::lpc40xx::Timer::ChannelPartial_t &kTimerPartial1 = sjsu::lpc40xx::Timer::Channel::kTimer1.channel;
                inline static const sjsu::lpc40xx::Timer::ChannelPartial_t &kTimerPartial2 = sjsu::lpc40xx::Timer::Channel::kTimer2.channel;
                inline static const sjsu::lpc40xx::Timer::ChannelPartial_t &kTimerPartial3 = sjsu::lpc40xx::Timer::Channel::kTimer3.channel;

                public:

                /* Identical to what's in timer.hpp but we use the CaptureHandler template instead */   
                inline static const sjsu::lpc40xx::Timer::Channel_t kCaptureTimer0 = 
                {
                    .channel = kTimerPartial0,
                    .isr = CaptureHandler<kTimerPartial0>
                };
                inline static const sjsu::lpc40xx::Timer::Channel_t kCaptureTimer1 = 
                {
                    .channel = kTimerPartial1,
                    .isr = CaptureHandler<kTimerPartial1>
                };
                inline static const sjsu::lpc40xx::Timer::Channel_t kCaptureTimer2 = 
                {
                    .channel = kTimerPartial2,
                    .isr = CaptureHandler<kTimerPartial2>
                };
                inline static const sjsu::lpc40xx::Timer::Channel_t kCaptureTimer3 = 
                {
                    .channel = kTimerPartial3,
                    .isr = CaptureHandler<kTimerPartial3>
                };

            }; // struct Channel

            static constexpr sjsu::lpc40xx::SystemController kLpc40xxSystemController = sjsu::lpc40xx::SystemController();

            explicit constexpr Capture(
                const sjsu::lpc40xx::Timer::Channel_t &timer,
                const sjsu::SystemController &system_controller = kLpc40xxSystemController
                )
                : timer_(timer), system_controller_(system_controller)
            {
            };

            Status Initialize(uint32_t frequency, IsrPointer isr = nullptr, int32_t priority = -1) const override
            {
                // NOTE: Same initialization as Timer library's Initialize()
                system_controller_.PowerUpPeripheral(timer_.channel.power_id);
                    SJ2_ASSERT_FATAL(
                        frequency != 0,
                        "Cannot have zero ticks per microsecond, please choose 1 or more.");
                // Set Prescale register for Prescale Counter to milliseconds
                uint32_t prescaler =
                    system_controller_.GetPeripheralFrequency() / frequency;
                timer_.channel.timer_register->PR = prescaler;
                timer_.channel.timer_register->TCR |= (1 << 0);
                *timer_.channel.user_callback = isr;
                RegisterIsr(timer_.channel.irq, timer_.isr, true, priority);

                // NOTE: New initialization for Capture library
                ConfigureCapture(kCaptureChannel0, kCaptureEdgeModeNone);
                ConfigureCapture(kCaptureChannel1, kCaptureEdgeModeNone);
                EnableCaptureInterrupt(kCaptureChannel0, false);
                EnableCaptureInterrupt(kCaptureChannel1, false);

                return Status::kSuccess;
            } 

            void GetCaptureStatus(CaptureChannelNumber channel, CaptureStatus_t &status)
            {
                status.flags = timer_.channel.timer_register->IR;

                status.count = (channel == kCaptureChannel0)
                    ? timer_.channel.timer_register->CR0
                    : timer_.channel.timer_register->CR1
                    ;
            }

            void ConfigureCapture(CaptureChannelNumber channel, CaptureEdgeMode mode) const override
            {
                uint32_t shift = (channel == kCaptureChannel1) ? 3 : 0;
                timer_.channel.timer_register->CCR &= ~(0x03 << shift);

                if(mode)
                    timer_.channel.timer_register->CCR |= (mode & 3) << shift;   
            }

            void EnableCaptureInterrupt(CaptureChannelNumber channel, bool enabled) const override
            {
                uint32_t shift = (channel == kCaptureChannel1) ? 3 : 0;
                timer_.channel.timer_register->CCR &= ~(0x04 << shift);

                if(enabled)
                {
                    timer_.channel.timer_register->CCR |= (0x04 << shift);         
                    timer_.channel.timer_register->IR |= (channel == kCaptureChannel1) ? 0x20 : 0x10;
                }

            }
            private:

            const sjsu::lpc40xx::Timer::Channel_t &timer_;
            const sjsu::SystemController & system_controller_;                

        }; // class Capture

    }; // namespace lpc40xx 

}; // namespace sjsu 