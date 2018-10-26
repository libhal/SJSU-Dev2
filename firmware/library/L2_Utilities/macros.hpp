// This file is meant for general purpose macros that can be used across the
// SJSU-Dev2 environment.
#pragma once
#include "config.hpp"
#include "L2_Utilities/ansi_terminal_codes.hpp"
#include "L2_Utilities/debug.hpp"
#include "L2_Utilities/log.hpp"
#include "L2_Utilities/time.hpp"
// SJ2_SECTION will place a variable or function within a given section of the
// executable. It uses both attribute "section" and "used". Section attribute
// places variable/function into that section and "used" labels the symbol as
// used to ensure that the compiler does remove this symbol at link time.
#if defined(__APPLE__)
#define SJ2_SECTION(section_name) \
  __attribute__((used, section("__TEXT," section_name)))
#else
#define SJ2_SECTION(section_name) __attribute__((used, section(section_name)))
#endif
// SJ2_USED will use void casting as a means to convince the compiler that the
// variable has been used in the software, to remove compiler warnings about
// unused variables.
// NOTE: this will not stop the compiler from optimizing this variable out.
#define SJ2_USED(variable) ((void)variable)
// SJ2_VARIADIC_USED can suppress unused parameter warnings within macros when
// supplied with __VA_ARGS__.
inline void UsedVariadicFunction(...) {}
#define SJ2_VARIADIC_USED(...)         \
  if (false)                           \
  {                                    \
    UsedVariadicFunction(__VA_ARGS__); \
  }
// These macros are used to stringify define values. For example:
//
//      #define VALUE true
//      #define STRING "value = " STRINGIFY(s) => "value = true"
//
#define SJ2_STRINGIFY(s) SJ2_STRINGIFY2(s)
#define SJ2_STRINGIFY2(s) #s
// Returns the length of an array
#define SJ2_ARRAY_LENGTH(array) sizeof(array) / sizeof(*array)
// SJ2_PACKED give a specified type a packed attribute
#define SJ2_PACKED(type) type __attribute__((packed))
// SJ2_DUMP_BACKTRACE will print a list of the called functions leading up to
// where this macro is called.
#if defined SJ2_INCLUDE_BACKTRACE && SJ2_INCLUDE_BACKTRACE == true
#define SJ2_DUMP_BACKTRACE() ::debug::PrintTrace()
#else
#define SJ2_DUMP_BACKTRACE()
#endif  // defined SJ2_INCLUDE_BACKTRACE && SJ2_INCLUDE_BACKTRACE == true
// SJ2_IGNORE_STACK_TRACE will remove function profiling for this
// specific function which means it will not be recoreded in the stack_trace
// array and will not show up when a SJ2_DUMP_BACKTRACE() is called.
#define SJ2_IGNORE_STACK_TRACE(function) \
  function __attribute__((no_instrument_function))
// Set a function as a "weak" function. This means that if there is another
// declaration of this exact function somewhere else in the software, the
// non-weak function will be used instead of the weak function.
#define SJ2_WEAK __attribute__((weak))
// Similar to the weak attribute, but also gives each function the
// implementation of the function f.
#if defined(__APPLE__)
#define SJ2_ALIAS(f) \
  {                  \
  }
#else
#define SJ2_ALIAS(f) \
  __attribute__((weak, alias(#f), no_instrument_function))  // NOLINT
#endif
// When the condition is false, issue a warning to the user with a warning
// message. Warning message format acts like printf.
#define SJ2_ASSERT_WARNING(condition, warning_message, ...)        \
  do                                                               \
  {                                                                \
    if (!(condition))                                              \
    {                                                              \
      LOG_WARNING(warning_message SJ2_COLOR_RESET, ##__VA_ARGS__); \
    }                                                              \
  } while (0)
// When the condition is false, issue a critical level message to the user and
// halt the processor.
#define SJ2_ASSERT_FATAL_WITH_DUMP(with_dump, condition, fatal_message, ...)  \
  do                                                                          \
  {                                                                           \
    if (!(condition))                                                         \
    {                                                                         \
      LOG_CRITICAL(fatal_message SJ2_COLOR_RESET, ##__VA_ARGS__);             \
      if ((with_dump))                                                        \
      {                                                                       \
        printf("\nPrinting Stack Trace:\n\n");                                \
        SJ2_DUMP_BACKTRACE();                                                 \
        printf(                                                               \
            "\nRun: the following command in your project directory"          \
            "\n\n    " SJ2_BOLD_WHITE                                         \
            "arm-none-eabi-addr2line -e build/binaries/firmware.elf "         \
            "<insert pc>" SJ2_COLOR_RESET                                     \
            "\n\n"                                                            \
            "This will report the file and line number associated with that " \
            "program counter values provided above in the backtrace.\n\n");   \
      }                                                                       \
      Halt();                                                                 \
    }                                                                         \
  } while (0)

#if defined HOST_TEST
#define SJ2_ASSERT_FATAL(condition, fatal_message, ...)               \
  /* Without the if statement using the (condition) and SJ2_USED() */ \
  /* the compiler may complain about unused variables.             */ \
  /* This serves to silence those warnings during host tests.      */ \
  if (condition)                                                      \
  {                                                                   \
    SJ2_USED(fatal_message);                                          \
  }
#else
#define SJ2_ASSERT_FATAL(condition, fatal_message, ...) \
  SJ2_ASSERT_FATAL_WITH_DUMP(true, (condition), fatal_message, ##__VA_ARGS__)
#endif  // defined HOST_TEST
