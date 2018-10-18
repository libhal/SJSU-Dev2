#pragma once

#include "L0_LowLevel/interrupt.hpp"
#include "L0_LowLevel/LPC40xx.h"
#include "L1_Drivers/system_clock.hpp"
#include "L1_Drivers/system_timer.hpp"

extern SystemClock system_clock;
extern SystemTimer system_timer;

extern const IsrPointer kInterruptVectorTable[];

// These are defined after the compilation of the FreeRTOS port for Cortex M4F
// These will link to those definitions.
extern "C" void xPortPendSVHandler(void);   // NOLINT
extern "C" void vPortSVCHandler(void);      // NOLINT
extern "C" void xPortSysTickHandler(void);  // NOLINT
