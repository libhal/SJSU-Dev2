#include "L4_Application/globals.hpp"

#include <FreeRTOS.h>

#include "L1_Drivers/system_clock.hpp"
#include "L1_Drivers/system_timer.hpp"
#include "L1_Drivers/uart.hpp"

SystemTimer system_timer;
SystemClock system_clock;
Uart uart0(Uart::Channels::kUart0);
