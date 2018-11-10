#pragma once

#include "L1_Drivers/system_clock.hpp"
#include "L1_Drivers/system_timer.hpp"
#include "L1_Drivers/uart.hpp"

// extern SystemClock system_clock;
// extern SystemTimer system_timer;
// extern Uart uart0;

inline SystemTimer system_timer;
inline SystemClock system_clock;
inline Uart uart0(Uart::Channels::kUart0);
