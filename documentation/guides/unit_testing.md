# Unit Testing Your Project

## Unit Testing Tools

SJSU-Dev2 uses [Catch2](https://github.com/catchorg/Catch2) (testing
framework), [Fakeit](https://github.com/eranpeer/FakeIt) (struct & class
mocking), and [FFF](https://github.com/meekrosoft/fff) (fake function
framework) to assist in unit testing.

## Instrumentation added into Test Executables

Unit tests are compiled with the following addons enabled:

  -   - `-fsanitize=address`
        
          - checks for out of bounds heap, stack or globals
          - Out-of-bounds accesses to heap, stack and globals
          - Use-after-free
          - Use-after-return (runtime flag
            ASAN\_OPTIONS=detect\_stack\_use\_after\_return=1)
          - Use-after-scope (clang flag
            -fsanitize-address-use-after-scope)
          - Double-free, invalid free
          - Memory leaks (experimental)
          - Useful if your firmware is crashing and you cannot exactly
            figure out why.
          - To learn more see
            [AddressSanitizer](https://clang.llvm.org/docs/AddressSanitizer.html)

  -   - `-fcoverage-mapping`, `-ftest-coverage`
        
          - When the test is run, code coverage files are generated,
            which are used after a `make run-test` to generate code
            coverage html reports.

  -   - `-D HOST_TEST=1`
        
          - This defines a macro `HOST_TEST` which can be used within
            your source files remove or add source code that can be used
            for tests only.

## Where to Put Project Unit Tests

If your project directory does not have a `test` folder, create one.

<div class="error">

<div class="admonition-title">

Error

</div>

DO NOT IMPLEMENT a main function anywhere in your tests. This will cause
linking issues since main is already defined for tests in the
`firmware/library/L4_Testing/main_test.cpp` file.

</div>

## Adding Unit Tests Files to "User" Unit Tests

Add a `project.mk` file into your project folder if it doesn't already
exist. Within it, add your source and test source files like so:

> 
> 
> ``` make
> USER_TESTS += test/servo_controller_test.cpp
> USER_TESTS += test/guidance_system_test.cpp
> # etc ...
> ```

## Compiling and Running Tests

Use `make user-test` to compile your tests. Use `make run-test` to run
user tests after they have been compiled.

## Finding Example Test Files

If you are not sure how to start unit testing, search the library folder
for **test** folders. Within them should be test files that you can
examine in order to get an idea of how certain bits of code are tested.