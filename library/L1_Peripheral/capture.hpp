#pragma once
#include <cstdint>
#include "utility/status.hpp"

namespace sjsu
{
    class Capture
    {
        public:

        // two capture channels per timer
        enum CaptureTimerNumber : uint8_t 
        {
            kTimer0,
            kTimer1,
            kTimer2,
            kTimer3
        };

        // two capture channels per timer
        enum CaptureChannelNumber : uint8_t 
        {
            kCaptureChannel0,
            kCaptureChannel1
        };

        enum CaptureEdgeMode : uint8_t
        {
            kCaptureEdgeModeNone,
            kCaptureEdgeModeRising,
            kCaptureEdgeModeFalling,
            kCaptureEdgeModeBoth
        };

        virtual Status Initialize(uint32_t frequency, IsrPointer isr = nullptr, int32_t priority = -1) const = 0;
        virtual void ConfigureCapture(CaptureChannelNumber channel, CaptureEdgeMode mode) const = 0;
        virtual void EnableCaptureInterrupt(CaptureChannelNumber channel, bool enabled) const = 0;
    };

} /* namespace sjsu */