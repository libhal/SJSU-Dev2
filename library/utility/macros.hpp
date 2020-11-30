/// @defgroup Macros Utility Macros
/// @brief General purpose macros that can be used across the SJSU-Dev2
/// environment.
/// @{
#pragma once
/// SJ2_SECTION will place a variable or function within a given section of the
/// executable. It uses both attribute "section" and "used". Section attribute
/// places variable/function into that section and "used" labels the symbol as
/// used to ensure that the compiler does remove this symbol at link time.
#if defined(__APPLE__)
#define SJ2_SECTION(section_name) \
  __attribute__((used, section("__TEXT," section_name)))
#else
#define SJ2_SECTION(section_name) __attribute__((used, section(section_name)))
#endif
/// DO NOT USE this macro directly. Use the C++17 attribute [[maybe_unused]]
/// instead.
/// _SJ2_USED will use void casting as a means to convince the compiler that the
/// variable has been used in the software, to remove compiler warnings about
/// unused variables.
/// NOTE: this will not stop the compiler from optimizing this variable out.
#define _SJ2_USED(variable) ((void)variable)
/// These macros are used to stringify define values. For example:
///
///      #define VALUE true
///      #define STRING "value = " STRINGIFY(s) => "value = true"
///
#define SJ2_STRINGIFY(s) SJ2_STRINGIFY2(s)
/// Helper macro for stringifying an expression
#define SJ2_STRINGIFY2(s) #s
/// SJ2_PACKED give a specified type a packed attribute
#define SJ2_PACKED(type) type [[gnu::packed]]
/// Set a function as a "weak" function. This means that if there is another
/// declaration of this exact function somewhere else in the software, the
/// non-weak function will be used instead of the weak function.
#define SJ2_WEAK(function) [[gnu::weak]] function
/// @}
