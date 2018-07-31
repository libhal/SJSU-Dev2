#pragma once

#include "L1_Drivers/system_timer.hpp"

extern SystemTimer system_timer;

extern "C" void xPortPendSVHandler(void);  // NOLINT
extern "C" void vPortSVCHandler(void);  // NOLINT
extern "C" void xPortSysTickHandler(void);  // NOLINT
