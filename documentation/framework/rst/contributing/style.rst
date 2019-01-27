SJSU-Dev2 Style Guide
===========================

C++ Style
----------
SJSU-Dev2 follows the Google C++ style guide with exception to the subjects
stated here. See: http://google.github.io/styleguide/cppguide.html

Usage of C files
-----------------
Only on special cases should :code:`.c` (C-only) source files be used. When ever
possible, be sure to use C++ rather than C.

Including C headers
--------------------
Always opt to use the C++ version of the standard library headers. For example,
besides including :code:`<stdlib.h>` use :code:`<cstdlib>`.

C++ Standard Libraries
-----------------------
Many of the C++ libraries are large and complex and not optimal for the
SJSU-Dev2 project. For the most part stay away from using or including std C++
libraries. For example, simply the simple :code:`#include <iostream>` will grow
the executable's **size by 150kB!?** Bare in mind that the flash size for the
board is **512kB**. :code:`<iostream>` will take up approximately **30%** of your flash size when it is included and not even used.

Here are a few standard C++ libraries you can include:
  * :code:`<variant>`
  * :code:`<utility>`
  * :code:`<tuple>`
  * :code:`<type_traits>`
  * :code:`<limits>`
  * :code:`<bitset>`
  * :code:`<atomic>`
  * :code:`<utility>`
  * :code:`<algorithm>`
  * :code:`<c*>` C standard libraries

C++ libraries that increase compile time and **should not** be included:
  * :code:`<functional>`
  * :code:`<chrono>`

C++ libraries that increase binary size and you should **NEVER** be included:
  * :code:`<iostream>`

Curly Braces
-------------
SJSU-Dev2 will follow the Allman style of curly braces, where each curly brace
gets their own line.

Returning values
-----------------
Prefer to only have 1 return statement within function and to have it return a result variable.

.. code-block:: cpp

    int function()
    {
      // Preload with a default return.
      int result = 0;
      //
      // Do some stuff in the function that any alter the result.
      //
      return result;
    }

How to Efficiently Return variables
++++++++++++++++++++++++++++++++++++
.. code-block:: cpp

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

Infinite Loops
---------------
Use :code:`Halt()`, found in the :code:`library/utility/` folder rather than :code:`for(;;)`, or :code:`while (true)` etc...

Include Guards
------------------
The first non-comment line of every header file must be :code:`#pragma once`.
**DO NOT** use the traditional macro include guards.

Memory Allocation
------------------
SJSU-Dev2 code may NOT use dynamic memory such as with :code:`malloc` or
:code:`new`. All memory must be preallocated either as stack memory or via by
global storage. Code must NOT use C++ libraries that allocate memory, such as
:code:`std::string`.

Pointers and references
------------------------
Pointer and reference symbols (asterisks and ampersand), must be in the center
of type and the variable name.

For example: :code:`char * array;` and :code:`PositronManager & pos;`

Prefer storing references over storing pointers. Use pointers only for cases
where the pointer's target may be changed or needs to point to :code:`nullptr`.
References passed to functions must be passed as const.

Using Preprocessor macro
-------------------------
Stay away from :code:`#define` as much as possible. Use :code:`constexpr` and
inline in place of macros.

If a macro solution is the most optimal solution, they may be used them, but
they must be prefixed with :code:`SJ2_` or :code:`_SJ2_`. When prefixed with a
:code:`_`, that means that this macro is an internal macro and should not be
called used directly by application code.

Usage of :code:`constexpr`
----------------------------
Prefer to use these whenever possible to do complex computations during compile
time rather than during runtime.

Preprocessor Semicolons
------------------------
Macros must force the user to end them with a semicolon. Thus the contents of
the macro must be wrapped with :code:`do { ... } while(0)` loop.

Standalone macros must end with a throwaway :code:`static_assert` statement.

.. code-block:: console

    #define _SJ2_GENERATE_SOMETHING(name)     \
        bool factory_##name() { return val; } \
        static_assert(true)

Preprocessor Conditionals
--------------------------
Prefer to use :code:`#if` rather than :code:`#ifdef`. :code:`#if` checks for
definition as well as if the expression evaluates to something truthful.

Every :code:`#endif` must be commented with the exprssion used in the
:code:`#if` statement.

Integer typing
---------------
Prefer to use the sign and width designated :code:`<cstdint>` integers. Prefer
:code:`int32_t a = 5;` compared to
:code:`int a = 5;`.

Low Level Driver Constructors
------------------------------
Drivers must not use their constructors to initialize hardware peripherals as the order in which constructors are executed is undefined.

Each driver must contain an :code:`Initialize(...)` method that the developer must explicitly call to manipulate hardware.

This is to ensure that the system and C++ libraries have been initialized
before modifying hardware registers.

The constructor should be used to initialize class memory such as member variables nothing more.

File comments
--------------
The top of each file must include a comment explaining the purpose of the file.

Comment style
---------------
Prefer that comments be directly above the line it means to comment.

Use :code:`// ...` for all comments in the code except for in macros for which
:code:`//` cannot be used, thus :code:`/* ... */` is acceptable only in this
case.

:code:`///` must be used for doxygen documentation comments.

Assertive Software
--------------------
Software in SJSU-Dev2 should be as assertive, in that you should use the
:code:`SJ2_ASSERT_FATAL(...)` or :code:`SJ2_ASSERT_WARNING(...)` at run time to either prevent or warn the developer about potentially dangerous operations. compile time error function whenever the user performs
and illegal action. For example, lets say the developer put 0Hz for the
constructor of the SystemClock class. There should be a
:code:`SJ2_ASSERT_FATAL()` check done the parameters to make sure that the value
is within a reasonable range.

If possible, prefer the compile time :code:`static_assert()`, since this can tell the user about a bug well before they have flashed their board.

.. code-block:: c++

    class SystemClock
    {
        constexpr SystemClock(uint32_t frequency)
        {
            SJ2_ASSERT_FATAL(1 <= frequency && frequency <= 100000000,
                "SystemClock frequency must be between 1 and 100000000");
        }
    }

File formatting
----------------
Every file must end with a newline character.

Number formatting
-------------------
Never use decimal or octal when doing bitwise operations. You may use hex
:code:`0x32`, or binary :code:`0b0011'0010`.

Please **DO** use the single quote :code:`'` to separate your numbers for example:
:code:`0b0110'1000'0101'1110` and :code:`12'000'000`.

Making the number segments more visible if there are obvious mistakes.

Make sure that the radix of the number you are using from datasheets or manuals
matching the radix in those manuals. If the datahsheet says that at address
:code:`0xABCD`, you should also use 0xABCD rather then converting it to
:code:`43981`.
