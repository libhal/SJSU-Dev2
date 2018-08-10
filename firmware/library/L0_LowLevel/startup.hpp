#pragma once

#include "L0_LowLevel/LPC40xx.h"
#include "L1_Drivers/system_timer.hpp"
// Define an alias for the Isr function pointer.
using IsrPointer = void (*)(void);

extern SystemTimer system_timer;

extern IsrPointer dynamic_isr_vector_table[];

extern "C" void xPortPendSVHandler(void);  // NOLINT
extern "C" void vPortSVCHandler(void);  // NOLINT
extern "C" void xPortSysTickHandler(void);  // NOLINT
