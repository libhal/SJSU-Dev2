// This file is meant for general purpose macros that can be used across the
// SJSU-Dev2 environment.
#pragma once
#include "config.hpp"
#include "L2_Utilities/ansi_terminal_codes.hpp"
#include "L2_Utilities/backtrace.hpp"
#include "L2_Utilities/debug_print.hpp"
// SJ2_SECTION will place a variable or function within a given section of the
// executable. It uses both attribute "section" and "used". Section attribute
// places variable/function into that section and "used" labels the symbol as
// used to ensure that the compiler does remove this symbol at link time.
#define SJ2_SECTION(section_name) __attribute__((used, section(section_name)))
// SJ2_USED will use void casting as a means to convince the compiler that the
// variable has been used in the software, to remove compiler warnings about
// unused variables.
// NOTE: this will not stop the compiler from optimizing this variable out.
#define SJ2_USED(variable) (void)variable
// These macros are used to stringify define values. For example:
//
//      #define VALUE true
//      #define STRING "value = " STRINGIFY(s) => "value = true"
//
#define SJ2_STRINGIFY(s) SJ2_STRINGIFY2(s)
#define SJ2_STRINGIFY2(s) #s
// SJ2_PACKED give a specified type a packed attribute
#define SJ2_PACKED(type) type __attribute__((packed))
// Set a function as a "weak" function. This means that if there is another
// declaration of this exact function somewhere else in the software, the
// non-weak function will be used instead of the weak function.
#define SJ2_WEAK __attribute__((weak))
// Similar to the weak attribute, but also gives each function the
// implementation of the function f.
#define SJ2_ALIAS(f) __attribute__((weak, alias(#f)))  // NOLINT

#if defined SJ2_INCLUDE_BACKTRACE && SJ2_INCLUDE_BACKTRACE == true
#define SJ2_DUMP_BACKTRACE() PrintTrace()
#else
#define SJ2_DUMP_BACKTRACE() \
    do                       \
    {                        \
    } while (0)
#endif  // defined SJ2_INCLUDE_BACKTRACE && SJ2_INCLUDE_BACKTRACE == true

#define SJ2_ASSERT_WARNING(condition, warning_message, ...)          \
    do                                                               \
    {                                                                \
        if (!(condition))                                            \
        {                                                            \
            DEBUG_PRINT("\n" SJ2_BACKGROUND_RED                      \
                        "WARNING: " warning_message SJ2_COLOR_RESET, \
                        ##__VA_ARGS__);                              \
        }                                                            \
    } while (0)

#define SJ2_ASSERT_FATAL(condition, fatal_message, ...)                   \
    do                                                                    \
    {                                                                     \
        if (!(condition))                                                 \
        {                                                                 \
            DEBUG_PRINT("\n" SJ2_BACKGROUND_RED                           \
                        "ERROR: " fatal_message SJ2_COLOR_RESET,          \
                        ##__VA_ARGS__);                                   \
            printf("\nPrinting Stack Trace:\n");                          \
            SJ2_DUMP_BACKTRACE();                                         \
            printf(                                                       \
                "\nRun: the following command in your project directory"  \
                "\n\n    " SJ2_BOLD_WHITE                                 \
                "arm-none-eabi-addr2line -e build/binaries/firmware.elf " \
                "<insert pc>" SJ2_COLOR_RESET                             \
                "\n\n"                                                    \
                "This will report the file and line number associated "   \
                "with that program counter values above.");               \
            while (true)                                                  \
            {                                                             \
                continue;                                                 \
            }                                                             \
        }                                                                 \
    } while (0)
