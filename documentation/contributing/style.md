# Style Guide

## C++ Style

SJSU-Dev2 mostly follows the
[Google C++ style guide](http://google.github.io/styleguide/cppguide.html).
Exceptions to the style guide are shown below.

## Usage of C files

Only on special cases should `.c` (C-only) source files be used. When
ever possible, be sure to use C++ rather than C.

## Never Include std C headers

Always opt to use the C++ version of the standard library headers. For
example, besides including `<stdlib.h>` use `<cstdlib>`.

## C++ Standard Libraries

Many of the C++ libraries are large, complex, and not optimal for the embedded
system projects. For the most part be careful when including std C++ libraries.
For example, including `#include <iostream>` will grow a binary by
**size by 150kB!!**

Bare in mind that the flash size for a lot of boards is anywhere from 32kB to
1MB. On a board with a heafty `512kB` flash memory `<iostream>` takes up
approximately **30%** of your flash size when it is included and NOT even used.

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
- C++ libraries that increase binary size and you should **NEVER** be
  included:
  - `<iostream>`

## Refrain from abbreviations

Choose the variable name "timer" over the name "tim". Choose InterruptHandler
over IHandler or IntHandler. FunctionPointer over FuncPtr. Be expressive with
your variables and make sure it is obvious what your variables do.

Exceptions are abbreviations known in the industry such as using `int i` within
a for loop. Another is the use of abbreviations like SPI and I2C where most
developers know the acronyms and what they mean, but rarely actually spell out
the acronym.

## Code expressions read nicely from left to right

When reading code, we read from left to right as we do any text. When writing
APIs think about how nicely your code reads from left to right.

Link to how this should look:
[Left to Right Technique](techniques/left-to-right.md).

## Curly Braces

SJSU-Dev2 will follow the Allman style of curly braces, where each curly
brace gets their own line.

```C++
if (IsValid())
{  // gets its own line

}
```

## Pausing Execution of a Program Indefinitely

Use `sjsu::Halt()`, found in the `library/utility/` folder rather than directly
with `for(;;)`, or `while (true)` etc...

This is helpful for testing, as, during a unit test, `sjsu::Halt()` simply
passes through, allowing the test to continue.

## Include Guards

The first non-comment line of every header file must be `#pragma once`.
**DO NOT** use the traditional macro include guards.

## Integer typing

Prefer to use the sign and width designated `<cstdint>` integers where the width
or the size of the integer matters. For example use `int32_t a = 5;` compared to
`int a = 5;` if you know you need a signed 32 bit number. Never assume size.

## Memory Allocation

SJSU-Dev2 libraries must NOT use dynamic memory such as with `malloc` or `new`.
All used memory must be allocated statically or use stack memory. This is to
ensure that the impact of code on RAM is known at compile time.

Code must also NOT use C++ libraries that allocate memory dynamically, such
as `std::string`.

## Pointer and Reference Character Placement

Pointer and reference symbols (asterisks and ampersand), must be in the
center of type and the variable name.

For example: `char * array;` and `PositronManager & pos;`

Prefer storing references over storing pointers. Use pointers only for
cases where the pointer's target may be changed or needs to point to
`nullptr`.

## Refrain from using preprocessor macros

Stay away from `#define` as much as possible. Use `constexpr` in place of
macros.

Only in the scenario where a C++ type solution cannot be found, but a macro
solution will solve the problem in a clean way, then macros can be used. Macro
definitions must be prefixed with `SJ2_` (public) or `_SJ2_` (private). When
prefixed with a `_`, that means that this macro is an internal macro and should
not be called used directly by application code.

## Preprocessor Semicolons

Macros must force the user to end them with a semicolon. Thus the contents of
the macro must be wrapped with `do { ... } while(0)` loop.

Another option is a to end with a throwaway `static_assert` statement if the
above solution does not work.

``` console
#define _SJ2_GENERATE_SOMETHING(name)     \
    bool factory_##name() { return val; } \
    static_assert(true)
```

## Preprocessor Conditionals

Refrain from using preprocessor conditionals. If it must be used prefer to use
`#if` rather than `#ifdef`. `#if` checks for definition as well as if the
expression evaluates to something truthful.

Every `#endif` must be commented with the expression used in the `#if`
statement.

## Hardware/Register Manipulation in Constructors

Hardware shall not be manipulated in an object's constructors. This is due to
the fact that statically allocated or global objects have an undefined order in
which their constructors are executed. In a lot of applications the order in
which hardware is initialized is important, thus a constructor initializing
hardware before the application has a chance to can lead to undefined behavior
and hard to debug bugs.

Every driver and class that needs to initialize hardware in any way must contain
an `Initialize(...)` method. This way the application developer must explicitly
`Initialize()` in order to initialize the hardware.

The constructor can and should be used to initialize class memory such as member
variables.

## Comment style

Comments should be directly above the line it means to comment.

Use `// ...` for all comments in the code except for in macros where `//` cannot
be used, thus `/* ... */` is acceptable only in this case.

`///` must be used for doxygen documentation comments.


## Documentation

Every global or public function, variable, and constant must be fully documented
using the doxygen style of documentation.

Code that is none trivial to understand must be documented to describe it.
Prefer to write code that is sell documenting.

## Error Handling

To be added...

## File formatting

Every file must end with a newline character.

## Number formatting for bitwise operations

Never use decimal or octal when doing bitwise operations. You may use hex
`0x32`, or binary `0b0011'0010`.

**DO** use the single quote `'` to separate your numbers for example:
`0b0110'1000'0101'1110` and `12'000'000`.

Make sure that the radix of the number you are using matches the radix from data
sheets or user manuals. If the data sheet says that at address `0xABCD`, you
should also use `0xABCD` rather then converting it to `43981` or converting it
to the binary form.

## Bitwise Operators

Refrain from using bit wise operators directly. Prefer to use the bit API for
bit manipulation.

See the [bit API guide](../guides/bit.md) for more details.

## Use third party library "units" whenever appropriate

In order to handle unit conversions and to make passing values with an
associated unit attached to it easier use the units library.
To learn how to use the units library see:
[github.com/nholthaus/units](https://github.com/nholthaus/units).

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

The following below requires you to handle the unit conversions yourself.
With every implementation like this, mistakes will happen and the code size will
increase with each implementation of code doing the same operations but in
different locations.

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
