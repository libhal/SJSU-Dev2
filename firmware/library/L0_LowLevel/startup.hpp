#pragma once

#include "L1_Drivers/system_timer.hpp"

extern SystemTimer system_timer;

extern "C" void xPortPendSVHandler(void);
extern "C" void vPortSVCHandler(void);
extern "C" void xPortSysTickHandler(void);
