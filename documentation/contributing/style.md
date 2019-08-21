# Style Guide

## C++ Style

SJSU-Dev2 follows the Google C++ style guide with exception to the
subjects stated here. See:
<http://google.github.io/styleguide/cppguide.html>

## Usage of C files

Only on special cases should `.c` (C-only) source files be used. When
ever possible, be sure to use C++ rather than C.

## Including C headers

Always opt to use the C++ version of the standard library headers. For
example, besides including `<stdlib.h>` use `<cstdlib>`.

## C++ Standard Libraries

Many of the C++ libraries are large and complex and not optimal for the
SJSU-Dev2 project. For the most part stay away from using or including
std C++ libraries. For example, simply the simple `#include <iostream>`
will grow the executable's **size by 150kB!?** Bare in mind that the
flash size for the board is **512kB**. `<iostream>` will take up
approximately **30%** of your flash size when it is included and not
even used.

  - Here are a few standard C++ libraries you can include:

      - `<variant>`
      - `<utility>`
      - `<tuple>`
      - `<type_traits>`
      - `<limits>`
      - `<bitset>`
      - `<atomic>`
      - `<utility>`
      - `<algorithm>`
      - `<c*>` C standard libraries

  - C++ libraries that increase compile time and **should not** be
    included:

      - `<functional>`
      - `<chrono>`

  - C++ libraries that increase binary size and you should **NEVER** be
    included:

      - `<iostream>`

## Taking more than 4 arguments for a function

If a method or function takes more than 4 arguments, consider taking a
structure as an input rather than the seperate arguments.

Without structure parameter:

``` c++
void GeneratePulse(uint32_t frequency, uint32_t amplitude, uint32_t cycles, bool rise_in_volume);

// Used like:
GeneratePulse(2000, 5, 100, false);

/// OR

struct GeneratePulseParameter
{
  uint32_t frequency;
  uint32_t amplitude;
  uint32_t cycles;
  bool rise_in_volume;
};
void GeneratePulse(GeneratePulseParameter parameters);

// Used like:
GeneratePulse(2000, 5, 100, false);
```

## Refrain from abbreviations

Choose the variable name "timer" over the name "tim". Choose
InterruptHandler over IHandler or IntHandler. FunctionPointer over
FuncPtr. Be expressive with your variables and make sure it is obvious
what your variable does.

Exceptions are abbreviations known in the industry such as using `int i`
within a for loop. Another is the use of abbreviations like SPI and I2C
where most people do not typically remember what the actual words for
these abbreviations, but understand how these protocols work.

# Code expressions read nicely from left to right

When reading code, we read from left to right as we do any text. When
writing APIs think about how nicely your code reads from left to right.

Link to how this should look: [Left to Right
Technique](techniques/left-to-right.md).

# Curly Braces

SJSU-Dev2 will follow the Allman style of curly braces, where each curly
brace gets their own line.

# Infinite Loops

Use `Halt()`, found in the `library/utility/` folder rather than
`for(;;)`, or `while (true)` etc...

# Include Guards

The first non-comment line of every header file must be `#pragma once`.
**DO NOT** use the traditional macro include guards.

# Memory Allocation

SJSU-Dev2 code may NOT use dynamic memory such as with `malloc` or
`new`. All memory must be preallocated either as stack memory or via by
global storage. Code must NOT use C++ libraries that allocate memory,
such as `std::string`.

# Pointers and references

Pointer and reference symbols (asterisks and ampersand), must be in the
center of type and the variable name.

For example: `char * array;` and `PositronManager & pos;`

Prefer storing references over storing pointers. Use pointers only for
cases where the pointer's target may be changed or needs to point to
`nullptr`. References passed to functions must be passed as const.

# Using Preprocessor macro

Stay away from `#define` as much as possible. Use `constexpr` and inline
in place of macros.

If a macro solution is the most optimal solution, they may be used them,
but they must be prefixed with `SJ2_` or `_SJ2_`. When prefixed with a
`_`, that means that this macro is an internal macro and should not be
called used directly by application code.

# Usage of `constexpr`

Prefer to use these whenever possible to do complex computations during
compile time rather than during runtime.

# Preprocessor Semicolons

Macros must force the user to end them with a semicolon. Thus the
contents of the macro must be wrapped with `do { ... } while(0)` loop.

Standalone macros must end with a throwaway `static_assert` statement.

``` console
#define _SJ2_GENERATE_SOMETHING(name)     \
    bool factory_##name() { return val; } \
    static_assert(true)
```

# Preprocessor Conditionals

Prefer to use `#if` rather than `#ifdef`. `#if` checks for definition as
well as if the expression evaluates to something truthful.

Every `#endif` must be commented with the exprssion used in the `#if`
statement.


# Integer typing

Prefer to use the sign and width designated `<cstdint>` integers. Prefer
`int32_t a = 5;` compared to `int a = 5;`.

# Low Level Driver Constructors

Drivers must not use their constructors to initialize hardware
peripherals as the order in which constructors are executed is
undefined.

Each driver must contain an `Initialize(...)` method that the developer
must explicitly call to manipulate hardware.

This is to ensure that the system and C++ libraries have been
initialized before modifying hardware registers.

The constructor should be used to initialize class memory such as member
variables nothing more.

# File comments

The top of each file must include a comment explaining the purpose of
the file.

# Comment style

Prefer that comments be directly above the line it means to comment.

Use `// ...` for all comments in the code except for in macros for which
`//` cannot be used, thus `/* ... */` is acceptable only in this case.

`///` must be used for doxygen documentation comments.

# Assertive Software

Software in SJSU-Dev2 should be as assertive, in that you should use the
`SJ2_ASSERT_FATAL(...)` or `SJ2_ASSERT_WARNING(...)` at run time to
either prevent or warn the developer about potentially dangerous
operations. compile time error function whenever the user performs and
illegal action. For example, lets say the developer put 0Hz for the
constructor of the SystemClock class. There should be a
`SJ2_ASSERT_FATAL()` check done the parameters to make sure that the
value is within a reasonable range.

If possible, prefer the compile time `static_assert()`, since this can
tell the user about a bug well before they have flashed their board.

``` c++
class SystemClock
{
    constexpr SystemClock(uint32_t frequency)
    {
        SJ2_ASSERT_FATAL(1 <= frequency && frequency <= 100000000,
            "SystemClock frequency must be between 1 and 100000000");
    }
}
```

# File formatting

Every file must end with a newline character.

# Number formatting

Never use decimal or octal when doing bitwise operations. You may use
hex `0x32`, or binary `0b0011'0010`.

Please **DO** use the single quote `'` to separate your numbers for
example: `0b0110'1000'0101'1110` and `12'000'000`.

Making the number segments more visible if there are obvious mistakes.

Make sure that the radix of the number you are using from datasheets or
manuals matching the radix in those manuals. If the datahsheet says that
at address `0xABCD`, you should also use 0xABCD rather then converting
it to `43981`.

# Returning values

Prefer to only have 1 return statement within function and to have it
return a result variable.

``` c++
int function()
{
  // Prel
```

# Returning values

Prefer to only have 1 return statement within function and to have it
return a result variable.

``` c++
int function()
{
  // Preload with a default return.
  int result = 0;
  //
  // Do some stuff in the function that any alter the result.
  //
  return result;
}
```

## How to Efficiently Return variables

``` c++
int function(bool check1, bool check2)
{
  // Preload with a default return.
  int result = 1;
  if (check1 && check2)
  {
    result = 17;
  }
  else if (!check1 && check2)
  {
    result = 17;
  }
  if (check1 && !check2)
  {
    result = 17;
  }
  // As you can see, using 1 as the preloaded value was a poor choice because
  // we had to write 17, 3 times and the compiler may have had to write those
  // three assignments into assembly.
  return result;
}

int functionBetter(bool check1, bool check2)
{
  // A better alternative would be set the default value to 17, now we only
  // have to check 1 case, and all other are taken care of.
  int result = 17;
  if (!check1 && !check2)
  {
    result = 1;
  }
  return result;
}
```

## Use third party library "units" whenever appropriate

In order to handle unit conversions and to make passing values with an
associated unit attached to it easier use the units library.
See: https://github.com/nholthaus/units on how to use it.

Prefer an interface like:

``` c++
#include "utility/units.hpp"

class DistanceSensor
{
 public:
  virtual Status Initialize() const = 0;
  virtual Status GetDistance(units::length::millimeters_t * distance) const = 0;
  // This can stay a float from 0.0f <-> 1.0f because some datasheets do not
  // assign units to the strength, and keep it as a percentage.
  virtual Status GetSignalStrengthPercent(float * strength) const = 0;
};
```

Over the following that requires you to handle the unit conversions yourself.
If every engineer does this, there are is bound to mistakes and worse, we bloat
the code doing the same operations but in different locations:

``` c++
class DistanceSensor
{
 public:
  static constexpr float kConversionCM   = 10;
  static constexpr float kConversionInch = 25.4f;
  static constexpr float kConversionFt   = 304.8f;

  // ==============================
  // Required Methods for Subclasses
  // ==============================
  virtual Status Initialize() const = 0;
  // Distance units are to be in milimeters
  virtual Status GetDistance(uint32_t * distance) const           = 0;
  virtual Status GetSignalStrengthPercent(float * strength) const = 0;

  // ===============
  // Utility Methods
  // ===============
  Status GetDistanceCm(float * distance) const
  {
    uint32_t sensor_reading;
    Status sensor_status;
    sensor_status = GetDistance(&sensor_reading);
    *distance     = static_cast<float>(sensor_reading / kConversionCM);
    return sensor_status;
  }
  Status GetDistanceInch(float * distance) const
  {
    uint32_t sensor_reading;
    Status sensor_status;
    sensor_status = GetDistance(&sensor_reading);
    *distance     = static_cast<float>(sensor_reading / kConversionInch);
    return sensor_status;
  }
  Status GetDistanceFt(float * distance) const
  {
    uint32_t sensor_reading;
    Status sensor_status;
    sensor_status = GetDistance(&sensor_reading);
    *distance     = static_cast<float>(sensor_reading / kConversionFt);
    return sensor_status;
  }
};
```
