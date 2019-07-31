# Design Philosphy

SJSU-Dev2 is a development platform, and not a software framework, not
just a set of libraries, but a platform for developing firmware for 1 or
more microcontrollers.

## Philosphy

1.  Simple
2.  Light Weight
3.  Multi Platform
4.  Fast Builds
5.  Tested & Testable
6.  Anti-Framework

## Simple

SJSU-Dev2 should be powerful yet simple. The code should be readable and
intuitive, to those who have developed firmware and easy to learn for
those who are not too familiar.

Thus the the stylistic choices in the style guide and layer guide were
made.

## Light Weight: "You ONLY Pay for what you use"

Techniques used in SJSU-Dev2 must always limit the amount of unused code
or data that gets linked into the final binary REGARDLESS of
optimization level.

One great example was the shift from look up tables (LUT) and predefined
structures. See technique document \<insert documentation of LUT -\>
Predef Struct\>.

One exception to this rule has to do with the L1 Peripheral layer's use
of a polymorphic API using virtual methods. The compiler must implement
such virtual methods in the code even if they are never called or
referenced in the code. Thus the interfaces are kept to a minimum to
reduce bloat.

This means that microcontrollers with special features will not appear
in the interface. See \<insert link here\> for more information about
adding custom methods to a L1\_Peripheral layer object.

## Multiplatform

The aim of SJSU-Dev2 is to be a development platform that works with
multiple micrcontrollers across various processors. Developers should be
able to readily and easily build firmware applications for various
microcontrollers. One can choose to focus a single project towards a
specific supported microcontoller or they can choose to write a single
project that can build for multiple microcontrollers. For an example of
writing a single application that can compile code for multiple
microcontrollers see: \<insert link\>.

The platform works across various systems due to its common set of
interfaces defined within the L1\_Peripheral layer of the libraries.
Implementations of these peripherals can be passed to hardware
abstractions in the L2\_HAL layer objects, allowing them to function
regardless of which microcontroller they are built for.

Current support is only for the LPC40xx, LPC17xx, and ARM processors. We
want to support RISC-V, MIPS in the future.

## Fast Builds

Decisions, such as using header-only implementations for library code,
were made to make the build system as fast as possible by reducing the
number of invocations of the GCC compiler.

The set of allowable headers, the reduced use of complex templates, and
the use of static precompiled libraries have been used to greatly reduce
the time required to compile code.

The faster you can build the faster you can prototype, the faster you
can hit your deadlines.

# Tested & Testable  
SJSU-Dev2's code base is and must stay throughoghly tested. This 
ensures that the behavior of the code does not change when changes to 
the codebase occur. This makes refactoring and optimzing code easier 
as an error made in refactoring will result in a failed test.

But not only can the code written within the development platform's
library be tested but so can the applications and projects. SJSU-Dev2
comes with a suite of testing frameworks that developers can use to test
their own application logic if they so choose. There is effort put in to
make this as easy as possible to embolden developers to test more of
their application code.

# Anti-Framework

SJSU-Dev2 aims to be an anti-framework. It does not aim to constrain you
to a specific design philsophy. You can choose to omit using an OS, you
can choose to build an event loop architecture (similiar to JS), or you
can choose to something more akin to a multi master bus system. Whatever
you'd like to use for your own design philsophy, you choose, and we will
try to design libraries that enable this.