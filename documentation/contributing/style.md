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

## Refrain from abbreviations

Choose the variable name "timer" over the name "tim". Choose InterruptHandler
over IHandler or IntHandler. FunctionPointer over FuncPtr. Be expressive with
your variables and make sure it is obvious what your variable does.

Exceptions are abbreviations known in the industry such as using `int i` within
a for loop. Another is the use of abbreviations like SPI and I2C where most
people do not typically remember what the actual words for these abbreviations,
but understand how these protocols work.

# Code expressions read nicely from left to right

When reading code, we read from left to right as we do any text. When
writing APIs think about how nicely your code reads from left to right.

Link to how this should look:
[Left to Right Technique](techniques/left-to-right.md).

# Curly Braces

SJSU-Dev2 will follow the Allman style of curly braces, where each curly
brace gets their own line.

# Infinite Loops

Use `Halt()`, found in the `library/utility/` folder rather than
`for(;;)`, or `while (true)` etc...

# Include Guards

The first non-comment line of every header file must be `#pragma once`.
**DO NOT** use the traditional macro include guards.

# Integer typing

Prefer to use the sign and width designated `<cstdint>` integers. Prefer
`int32_t a = 5;` compared to `int a = 5;`.

# Memory Allocation

SJSU-Dev2 library code must NOT use dynamic memory such as with `malloc` or
`new`. All memory must be preallocated either as stack memory or via by global
storage. Code must NOT use C++ libraries that allocate memory dynamically, such
as `std::string`.

# Pointers and references

Pointer and reference symbols (asterisks and ampersand), must be in the
center of type and the variable name.

For example: `char * array;` and `PositronManager & pos;`

Prefer storing references over storing pointers. Use pointers only for
cases where the pointer's target may be changed or needs to point to
`nullptr`.

# Refrain from using preprocessor macros

Stay away from `#define` as much as possible. Use `constexpr` and inline
in place of macros.

If a C++ type solution cannot be found, but a macro solution can work, then they
may be used them, but they must be prefixed with `SJ2_` or `_SJ2_`. When
prefixed with a `_`, that means that this macro is an internal macro and should
not be called used directly by application code.

# Usage of `constexpr`

Prefer to use these whenever possible to do complex computations during compile
time rather than during runtime.

# Preprocessor Semicolons

Macros must force the user to end them with a semicolon. Thus the contents of
the macro must be wrapped with `do { ... } while(0)` loop.

Standalone macros must end with a throwaway `static_assert` statement.

``` console
#define _SJ2_GENERATE_SOMETHING(name)     \
    bool factory_##name() { return val; } \
    static_assert(true)
```

# Preprocessor Conditionals

Refrain from using the preprocessor conditionals, but if it must be used  prefer
to use `#if` rather than `#ifdef`. `#if` checks for definition as well as if the
expression evaluates to something truthful.

Every `#endif` must be commented with the expression used in the `#if`
statement.

# Hardware Manipulation in Constructors

Hardware shall not be manipulated in an object's constructors. This is due to
the fact that statically allocated or global objects have an undefined order in
which their constructors are executed. This can lead to various undefined
behavior resulting in hard to debug bugs.

Each driver must contain an `Initialize(...)` method that the developer
must explicitly call to manipulate hardware.

This will ensure that the system and C++ libraries have been initialized before
modifying hardware registers.

The constructor should be used to initialize class memory such as member
variables nothing more.

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

Make sure that the radix of the number you are using from data sheets or
manuals matching the radix in those manuals. If the data sheet says that
at address `0xABCD`, you should also use 0xABCD rather then converting
it to `43981`.

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
the code doing the same operations but in different locations.

Do not do the following below:

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
  // Distance units are to be in millimeters
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
