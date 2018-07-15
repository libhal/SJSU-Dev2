#include "L0_LowLevel/LPC40xx.h"
#include "L0_LowLevel/core_cm4.h"
#include "L1_Drivers/system_timer.hpp"

SysTick_Type * SystemTimer::sys_tick = SysTick;

void (*SystemTimer::system_timer_isr)(void) = nullptr;
