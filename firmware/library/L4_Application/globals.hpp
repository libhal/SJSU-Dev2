#pragma once

#include <FreeRTOS.h>

#include "L1_Drivers/system_clock.hpp"
#include "L1_Drivers/system_timer.hpp"
#include "L1_Drivers/uart.hpp"

extern SystemClock system_clock;
extern SystemTimer system_timer;
extern Uart uart0;
