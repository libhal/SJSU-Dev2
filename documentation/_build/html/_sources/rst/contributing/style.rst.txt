SJSU-Dev2 Style Guide
===========================

C++ Style
-----------
SJSU-Dev2 follows the Google C++ style guide with exception to the subjects
stated here. See: http://google.github.io/styleguide/cppguide.html

Usage of C files
------------------
Only on special cases should :code:`.c` (C-only) source files be used. When ever
possible, be sure to use C++ rather than C.

C headers
------------
Always opt to use the C++ versio of the standard library headers. For example,
besides using :code:`<stdlib.h>` use :code:`<cstdlib>`.

C++ Standard Libraries
------------------------
Many of the C++ libraries are large and complex and not optimal for the
SJSU-Dev2 project. For the most part stay away from using or including std C++
libraries. For example, simply the simple :code:`#include <iostream>` will grow the
executable's **size by 150kB!?** Bare in mind that the flash size for the board is
512kB, thats approximately **30%** to simply include it, and not even to use it.

Here are a few standard C++ libraries you can include:
    * <variant>
    * <utility>
    * <tuple>
    * <type_traits>
    * <limits>
    * <bitset>
    * <atomic>
    * <utility>
    * <algorithm>
    * <c*> C standard libraries

Curly Braces
-------------
SJSU-Dev2 will follow the Allman style of curly braces, where each curly brace
gets their own line.

A control statement can be done in 1 line, you must be surrounded by curly
braces. For example, :code:`if(check) { DoSomething(); }`

Infinite Loops
---------------
Use :code:`while(true)` rather than :code:`for(;;)`
Also, be sure to supply a :code:`continue` statement within your while loop to
explicitly show that you mean :code:`while(true) { continue; }`

Include Guards
------------------
The first non-comment line of every header file must be :code:`#pragma once`.
Do not use the traditional macro include guards.

Memory Allocation
------------------
SJSU-Dev2 code may NOT use dynamic memory such as with :code:`malloc` or
:code:`new`. All memory must be preallocated either via stack memory or by
global storage. Code must NOT use C++ libraries that allocate memory, such as
std::string.

Pointers and references
------------------------
Pointer and reference symbols (astricks and ampersand), must be in the center
of type and the variable name.

For example: :code:`char * array;` and :code:`PositronManager & pos;`

Prefer storing references over storing pointers. Use pointers only for cases
where the pointer's target may be changed or needs to point to nullptr.
References must be passed as const as a function/method parameter.

Using Preprocessor macro
-------------------------
Try to stay away from #define as much as possible. Use constexpr and inline in
place of macros.

If a macro solution is the most optimal solution, they may be used them, but
they must be prefixed with :code:`SJ2_` or :code:`_SJ2_`. When prefixed with a
:code:`_`, that means that this macro is an internal macro and should not be
called by the application.

Preprocessor Semicolons
------------------------
Macros must force the user to end them with a semicolon. Thus the contents of
the macro must be wrapped with :code:`do { ... } while(0)` loop.

Standalone macros must end with a throwaway :code:`static_assert` statement.

.. code-block:: console

    #define _SJ2_GENERATE_SOMETHING(name)     \
        bool factory_##name() { return val; } \
        static_assert(1 == 1)

Preprocessor Conditionals
--------------------------
Prefer ot use :code:`#if` rather than :code:`#ifdef`. :code:`#if` checks for
definition as well as if the expression evaluates to something truthful.

Every :code:`#endif` must be commented with the exprssion used in the
:code:`#if` statement.

Integer typing
---------------
Stay away from the use of :code:`int` type. Prefer to use the sign and width
designated :code:`<cstdint>` integers. Prefer :code:`int32_t a = 5;` to
:code:`int a = 5;`.

Low Level Driver Constructors
------------------------------
Drivers must not use their constructor to initialize hardware perpherials.
Initialization should be done explicitly at the application start point main()
using an init() method.

This is to ensure that the SystemIntialization and C++ libraries have been
initialized before modifing hardware.

The constructor should be used to initialize and set member variables and
nothing more.

Usage of :code:`constexpr`
----------------------------
Prefer to use these whenever possible to do complex computations during compile
time rather than during runtime.

File comments
--------------
The top of each file must include a comment explaining the purpose of the file.

Comment style
---------------
Prefer that comments be directly above the line it means to comment.

Use :code:`// ...` for all comments in the code except for in macros for which
:code:`//` cannot be used, thus :code:`/* ... */` is acceptable.

Assertive Software
--------------------
Software in SJSU-Dev2 should be as assertive, in that you should use the
:code:`static_assert()` compile time error function whenever the user performs
and illegal action. For example, lets say the developer put 0Hz for the
constructor of the SystemClock class. There should be a
:code:`static_assert()` check done the parameters to make sure that the value
is within a reasonable range.

.. code-block:: c++

    class SystemClock
    {
        constexpr SystemClock(uint32_t frequency)
        {
            static_assert(1 <= frequency && frequency <= 100000000,
                "SystemClock frequency must be between 1 and 100000000");
        }
    }

File formatting
----------------
Every file must end with a newline character.

Number formatting
-------------------
Never use decimal or octal when doing bitsise operations. You may use hex
:code:`0x32`, or binary :code:`0b0011'0010`.

Please **DO** use the single quote :code:`'` to seperate your numbers for example:
:code:`0b0110'1000'0101'1110` and :code:`12'000'000`.

Making the number segments more visible if there are obvious mistakes.
