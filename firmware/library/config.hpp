#pragma once
// Include using <> vs "" for  to make sure we only grab the project version of
// project_config.hpp
#include <project_config.hpp>
#include <cstdint>

namespace config
{
// How to properly use global configuration option:
// ================================================
// If you are doing a #if, then use the macro name directly. Otherwise always
// use the kConstant type. Since this section is namespaced, don't forget to
// include the config:: scope for the kConstant. Example: config::kConstant

// How to create a new global configuration option:
// ================================================
// 1) Check if the macro is already defined. The macros should only be changed
//    in the <project_config.hpp> file
// 2) If the macro is not defined, give it a default value here.
// 3) Generate a typed constexpr version of the macro using SJ2_DECLARE_CONSTANT
//    This will check that the desired typed variable, and the macro equal
//    each other. They wouldn't in cases where the type is bool and the user
//    uses the value 2 for the macro.
// 4) It is recommend, that if there exists an acceptable range for a constant,
//    then use a static_assert to check that the constant generated from
//    SJ2_DECLARE_CONSTANT, is within range. For example if kSystemClockRate
//    can only be between 1Hz and 100Mhz, then kSystemClockRate should be
//    checked if it is within range.

// Creates a typed constexpr version of the macro defintion which should be used
// rather than using the macro directly.
#define SJ2_DECLARE_CONSTANT(macro_name, type, constant_name) \
  constexpr type constant_name = SJ2_##macro_name;            \
  static_assert(constant_name == SJ2_##macro_name,            \
                "SJ2_" #macro_name " must be of type '" #type "'")

// Used to enable and disable the sending of ANSI color escape codes via the
// ansi_terminal_codes.hpp
#if !defined SJ2_ENABLE_ANSI_CODES
#define SJ2_ENABLE_ANSI_CODES true
#endif  // !defined SJ2_ENABLE_ANSI_CODES
SJ2_DECLARE_CONSTANT(ENABLE_ANSI_CODES, bool, kEnableAnsiCodes);

// Used to set the system clock speed for the LPC4078 in MHz
#if !defined SJ2_SYSTEM_CLOCK_RATE_MHZ
#define SJ2_SYSTEM_CLOCK_RATE_MHZ 48
#endif  // !defined SJ2_SYSTEM_CLOCK_RATE
SJ2_DECLARE_CONSTANT(SYSTEM_CLOCK_RATE_MHZ, uint8_t, kSystemClockRateMhz);
constexpr uint32_t kSystemClockRate = SJ2_SYSTEM_CLOCK_RATE_MHZ * 1'000'000;
static_assert(1 <= kSystemClockRateMhz && kSystemClockRateMhz <= 100,
              "SJ2_SYSTEM_CLOCK can only be between 1Hz and 100Mhz");
static_assert(1'000'000 <= kSystemClockRate && kSystemClockRate <= 100'000'000,
              "SJ2_SYSTEM_CLOCK can only be between 1Hz and 100Mhz");

// Used to set the FreeRTOS tick frequency defined in Hz
#if !defined SJ2_RTOS_FREQUENCY
#define SJ2_RTOS_FREQUENCY 1'000
#endif  // !defined SJ2_RTOS_FREQUENCY
SJ2_DECLARE_CONSTANT(RTOS_FREQUENCY, uint16_t, kRtosFrequency);
static_assert(1 <= kRtosFrequency && kRtosFrequency <= 10'000,
              "SJ2_RTOS_FREQUENCY can only be between 1,000Hz and 1Hz");

// Default baud rate of 38400 divides perfectly with the LPC17xx and LPC40xx
// UART clock dividers perfectly, where as all other standard baud rates
// do not.
#if !defined SJ2_BAUD_RATE
#define SJ2_BAUD_RATE 38'400
#endif  // !defined SJ2_BAUD_RATE
SJ2_DECLARE_CONSTANT(BAUD_RATE, uint32_t, kBaudRate);
static_assert(4'800 <= kBaudRate && kBaudRate <= 4'000'000 &&
                  kBaudRate <= kSystemClockRate / 16,
              "SJ2_BAUD_RATE must be between 4800 bits/s and 4 Mbits/s and "
              "less than the clock speed / 16 ");

// Used to dump all the call stack when "PrintBacktrace" is called or an assert
// using PrintBacktrace is occurs.
// Disable this to omit getting these logs and reduce the binary size by ~5kB.
#if !defined SJ2_INCLUDE_BACKTRACE
#define SJ2_INCLUDE_BACKTRACE true
#endif  // !defined SJ2_INCLUDE_BACKTRACE
SJ2_DECLARE_CONSTANT(INCLUDE_BACKTRACE, bool, kIncludeBacktrace);

#if !defined SJ2_DEBUG_PRINT_ENABLED
#define SJ2_DEBUG_PRINT_ENABLED true
#endif  // !defined SJ2_DEBUG_PRINT_ENABLED
SJ2_DECLARE_CONSTANT(DEBUG_PRINT_ENABLED, bool, kDebugPrintEnabled);

// Used to set the default scheduler size for the TaskScheduler.
#if !defined SJ2_TASK_SCHEDULER_SIZE
#define SJ2_TASK_SCHEDULER_SIZE 16
#endif  // !defined SJ2_TASK_SCHEDULER_SIZE
SJ2_DECLARE_CONSTANT(TASK_SCHEDULER_SIZE, uint8_t, kTaskSchedulerSize);

}  // namespace config
