# Unit Testing

INCOMPLETE: Requires more details about unit testing standards and style

## Unit Testing Tools

To assist in unit testing, SJSU-Dev2 uses:

- Unit Testing Framework: [DocTest](https://github.com/onqtam/doctest)
- Interface Mocking: [Fakeit](https://github.com/eranpeer/FakeIt)
- Fake Function Framework: [FFF](https://github.com/meekrosoft/fff)

One property of the SJSU-Dev2 testing framework is that it does not require
external (non C++ language) tools to work. None of the following libraries
attempt to read the code in some language like python, ruby, etc, in order to
convert/remove/replace code in order to work. They all work within the C++
language, thus, all that is needed in order to test is a C++ compiler.

## Where to Put Library Project Unit Tests

If your project directory does not have a `test` folder, create one.

!!! Error
    DO NOT IMPLEMENT a main function anywhere in your tests. This will cause
    linking issues since main is already defined for tests in the
    `library/testing/main_test.cpp` file.

## Adding Unit Tests Files to Project Unit Tests

Add a `project.mk` file into your project folder if it doesn't already
exist. Within it, add your source and test source files like so:

``` make
TESTS += test/servo_controller_test.cpp
TESTS += test/guidance_system_test.cpp
# etc ...
```

## Compiling and Running Tests

Use `make test` to compile and run tests within your current project.

### Adding and Running a Successful Test

<script id="asciicast-331086" src="https://asciinema.org/a/331086.js" async>
</script>

### Showing Test Failure

<script id="asciicast-331088" src="https://asciinema.org/a/331088.js" async>
</script>

!!! Warning
    If you are running on a mac and see an error like this:

    ```
    In file included from test.cpp:4:
    /usr/local/opt/llvm/bin/../include/c++/v1/stdlib.h:94:15: fatal error: 'stdlib.h' file not found
    #include_next <stdlib.h>
    ```

    Then run the following command:
    ```
    sudo installer -pkg /Library/Developer/CommandLineTools/Packages/macOS_SDK_headers_for_macOS_10.14.pkg -target /
    ```

## Finding Example Test Files

If you are not sure how to start unit testing, search the library folder for
**test** folders. Within them should be test files that you can examine in order
to get an idea of how certain bits of code are tested.

## Instrumentation added into Test Executables

Unit tests are compiled with the following addons enabled:

- `-fsanitize=address`
    - checks for out of bounds heap, stack or globals
    - Out-of-bounds accesses to heap, stack and globals
    - Use-after-free
    - Use-after-return (runtime flag
      ASAN_OPTIONS=detect_stack_use_after_return=1)
    - Use-after-scope (clang flag -fsanitize-address-use-after-scope)
    - Double-free, invalid free
    - Memory leaks (experimental)
    - Useful if your firmware is crashing and you cannot exactly
      figure out why.
    - To learn more see
      [AddressSanitizer](https://clang.llvm.org/docs/AddressSanitizer.html)
- `-fcoverage-mapping`, `-ftest-coverage`
    - When the test is run, code coverage files are generated,
      which are used after a `make run-test` to generate code
      coverage html reports.
- `-D PLATFORM=host`
    - This defines a macro `PLATFORM` which can be used within the
      `build_info.hpp` source files which can be used to remove or add source
      code that can be used for tests only using `if constexpr`.
