# Style Guide

## C++ Style

SJSU-Dev2 mostly follows the
[Google C++ style guide](http://google.github.io/styleguide/cppguide.html).
Exceptions to the style guide are shown below.

## Usage of C files

Only on special cases should `.c` (C-only) source files be used. When ever
possible, be sure to use C++ rather than C.

## Never Include std C headers

Always opt to use the C++ version of the standard library headers. For example,
besides including `<stdlib.h>` use `<cstdlib>`.

## C++ Standard Libraries

Many of the C++ libraries pull in additional dependencies and requirements that
are not suitable for embedded system projects. Most C++ libraries are fine to
include, but some like `#include <iostream>` will grow a binary by size by
**150kB**!

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

SJSU-Dev2 will follow the Allman style of curly braces, where each curly brace
gets their own line.

```C++
if (IsValid())
{  // gets its own line

}
```

## Allowing Main To Exit

It is allowed in SJSU-Dev2 to allow main to return. When main returns, it will
report the return code and will halt the CPU in an infinite loop.

## Include Guards

**DO NOT** use the traditional macro include guards. The first non-comment line
of every header file must be `#pragma once`.

## Integer typing

Prefer to use the sign and width designated `<cstdint>` integers where the width
or the size of the integer matters. For example use `int32_t a = 5;` compared to
`int a = 5;` if you know you need a signed 32 bit number. Never assume size.

## Memory Allocation

SJSU-Dev2 libraries must NOT use dynamic memory such as with `malloc` or `new`.
All used memory must be allocated statically or use stack memory. This is to
ensure that the impact of code on RAM is known at compile time.

Code must also NOT use C++ libraries that allocate memory dynamically, such as
`std::string`.

## Pointer and Reference Character Placement

Pointer and reference symbols (asterisks and ampersand), must be in the center
of type and the variable name.

For example: `char * array;` and `PositronManager & pos;`

## Refrain from using macros

Stay away from `#define` as much as possible. Use `constexpr` in place of
macros if it is possible.

Only as a final resort where a C++ type solution cannot be found then macros can
be used. Macro definitions must be prefixed with `SJ2_` (public) or `_SJ2_`
(private). When prefixed with a `_`, that means that this macro is an internal
macro and should not be called used directly by application code.

### Preprocessor Semicolons

Macros must force the user to end them with a semicolon. Thus the contents of
the macro must be wrapped with `do { ... } while(0)` loop.

Another option is a to end with a throwaway `static_assert` statement if the
above solution does not work.

```console
#define _SJ2_GENERATE_SOMETHING(name)     \
    bool factory_##name() { return val; } \
    static_assert(true)
```

### Preprocessor Conditionals

Refrain from using preprocessor conditionals. If it must be used prefer to use
`#if` rather than `#ifdef`. `#if` checks for definition as well as if the
expression evaluates to something truthful.

Every `#endif` must be commented with the expression used in the `#if`
statement.

## Hardware/Register Manipulation in Constructors

They MUST NEVER alter and manipulate hardware. This is due to the fact that
statically allocated or global objects have an undefined order in which their
constructors are executed. Most systems are sensitive to the order in which
hardware is initialized, thus a constructor initializing hardware before the
application has a chance to can lead to undefined behavior and hard to debug
bugs.

Every library in SJSU-Dev2 must inherit the `sjsu::Module` interface which
will supply the interface for `void Initialize()` and `void Enable(bool enable)`
which are used for initializing, enabling, and disabling systems.

The constructor CAN and SHOULD be used to initialize class memory such as member
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

SJSU-Dev2 utilizes exception handling for errors. For more details please see
[Error Handling Guide](../software_architecture/error_handling.md).

## File formatting

- Every file must end with a newline character.
- Every line in a file must stay within a 80 character limit.
  - Exceptions to this rule are allowed. Use `// NOLINT` in these cases

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

See the [bit API guide](../libraries/bit.md) for more details.

## Use third party library "units" whenever appropriate

In order to handle unit conversions and to make passing values with an
associated unit attached to it easier, use the units library. To learn how to
use the units library see:
[github.com/nholthaus/units](https://github.com/nholthaus/units).

Prefer an interface like:

```c++
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

Rather than this:

```c++
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

The later example requires every single library to handle conversions themselves
when the solution should be generic and implemented once. The later example, if
implemented throughout the code base would result in code bloat, longer compile
times, and more locations where unit testing must be used to ensure correctness.
