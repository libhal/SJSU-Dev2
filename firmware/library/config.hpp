#pragma once

// Definition gaurge to keep user from including the project_config.hpp file
#define SJ2_CONFIG_FILE true
#include <project_config.hpp>
#undef SJ2_CONFIG_FILE
#define SJ2_CONFIG_FILE false

// Used to enable and disable the sending of ANSI color escape codes via the
// ansi_terminal_codes.hpp
#if defined SJ2_ENABLE_ANSI_CODES
static_assert(SJ2_ENABLE_ANSI_CODES == false || SJ2_ENABLE_ANSI_CODES == true,
    "SJ2_ENABLE_ANSI_CODES can only be true or false.");
#else
#define SJ2_ENABLE_ANSI_CODES false
#endif // defined SJ2_ENABLE_ANSI_CODES

// Used to set the system clock speed for the LPC4078
#if defined SJ2_SYSTEM_CLOCK
static_assert(
    1 <= SJ2_SYSTEM_CLOCK && SJ2_SYSTEM_CLOCK <= 100'000'000,
    "SJ2_SYSTEM_CLOCK can only be between 1Hz and 100Mhz");
#else
#define SJ2_SYSTEM_CLOCK 48'000'000
#endif // defined SJ2_SYSTEM_CLOCK

// Used to set the FreeRTOS tick frequency defined in Hz
#if defined SJ2_RTOS_FREQUENCY
static_assert(
    1'000 <= SJ2_RTOS_FREQUENCY && SJ2_RTOS_FREQUENCY <= 1,
    "SJ2_RTOS_FREQUENCY can only be between 1,000Hz and 1Hz");
#else
#define SJ2_RTOS_FREQUENCY 1'000
#endif // defined SJ2_RTOS_FREQUENCY
