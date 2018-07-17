#include "L0_LowLevel/LPC40xx.h"
#include "L1_Drivers/system_timer.hpp"

// Sys_tick structure defaults to the Core M4 SysTick register address found in
// L0_LowLevel/SystemFiles/core_m4.h which is included in LPC40xx.h
SysTick_Type * SystemTimer::sys_tick = SysTick;
// system_timer_isr defaults to nullptr. The actual SystemTickHandler should
// check if the isr is set to nullptr, and if it is, turn off the timer, if
// set a proper function then execute it.
void (*SystemTimer::system_timer_isr)(void) = nullptr;
