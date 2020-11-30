# Design Philosophy

SJSU-Dev2 is a development platform, and not a software framework, not
just a set of libraries, but a platform for developing firmware for 1 or
more microcontrollers.

## Philosophy

1. Simple
2. Light Weight
3. Multi Platform
4. Builds Fast
5. Tested & Testable
6. Anti-Framework

### Simple

SJSU-Dev2 should be powerful yet simple. The code should be:

- Readable
- Intuitive
- Easy to learn

Thus the the stylistic choices in the style guide and layer guides were made.

### Light Weight

> You ONLY pay for what you use!

Techniques used in SJSU-Dev2 must always limit the amount of unused code or data
that gets linked into the final binary REGARDLESS of optimization level.
Reducing such bloat can come in the form of optimizing the number of methods an
interface contains, or reducing the amount of data required to configure a
peripheral. See the techniques section for more various guides on how to do
this.

### Multiplatform

SJSU-Dev2 was built to allow development on any platform. This means any sort of
microcontroller, processor, or system. Developers should be able to readily and
easily build firmware applications for various microcontrollers in a single
project. One can choose to focus a single project towards a specific supported
microcontroller or they can choose to write a single project that can build for
multiple microcontrollers.

See `demos/multiplatform/dual_platforms` for an example of how to do this.

### Builds Fast

Decisions, such as using header-only implementations for library code, were made
to make the build system as fast as possible by reducing the number of
invocations of the GCC compiler.

The following have been used to greatly reduce the time required to compile
code.

- Reducing the allowable headers
- Reduced use of complex templates
- Use of static precompiled libraries

The faster you can build the faster you can prototype, the faster you can hit
your deadlines.

### Tested & Testable

SJSU-Dev2's code base is and must stay thoroughly tested. This ensures that the
behavior of the code does not change when changes to the codebase occur. This
makes refactoring and optimizing code easier as an error made in refactoring
will result in a failed test.

But not only can the SJSU-Dev2's libraries be tested but so can its projects.
SJSU-Dev2 comes with a testing suite that help developers test their
applications. There is effort put in to make this as easy as possible to
embolden developers to test more of their application code.

### Anti-Framework

SJSU-Dev2 aims to be an anti-framework. It does not aim to constrain you
to a specific design philosophy. You can choose to omit using an OS, you
can choose to build an event loop architecture (similar to JS), or you
can choose to something more akin to a multi master bus system. Whatever
you'd like to use for your own design philosophy, you choose, and we will
try to design libraries that enable this.
