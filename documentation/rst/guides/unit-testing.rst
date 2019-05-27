Unit Testing Your Project
==========================

Unit Testing Tools
-------------------
SJSU-Dev2 uses Catch2_ (testing framework), Fakeit_ (struct & class mocking), and FFF_ (fake function framework) to assist in unit testing.

.. _Catch2: https://github.com/catchorg/Catch2
.. _FakeIt: https://github.com/eranpeer/FakeIt
.. _fff: https://github.com/meekrosoft/fff

Instrumentation added into Test Executables
--------------------------------------------
Unit tests are compiled with the following addons enabled:

* :code:`-fsanitize=address`
    * checks for out of bounds heap, stack or globals
    * Out-of-bounds accesses to heap, stack and globals
    * Use-after-free
    * Use-after-return (runtime flag
      ASAN_OPTIONS=detect_stack_use_after_return=1)
    * Use-after-scope (clang flag -fsanitize-address-use-after-scope)
    * Double-free, invalid free
    * Memory leaks (experimental)
    * Useful if your firmware is crashing and you cannot exactly figure out why.
    * To learn more see AddressSanitizer_
* :code:`-fcoverage-mapping`, :code:`-ftest-coverage`
    * When the test is run, code coverage files are generated, which are used
      after a :code:`make run-test` to generate code coverage html reports.
* :code:`-D HOST_TEST=1`
    * This defines a macro :code:`HOST_TEST` which can be used within your
      source files remove or add source code that can be used for tests only.

.. _AddressSanitizer: https://clang.llvm.org/docs/AddressSanitizer.html

Where to Put Project Unit Tests
--------------------------------
If your project directory does not have a :code:`test` folder, create one.

.. error::

  DO NOT IMPLEMENT a main function anywhere in your tests. This will cause
  linking issues since main is already defined for tests in the
  :code:`firmware/library/L4_Testing/main_test.cpp` file.

Adding Unit Tests Files to "User" Unit Tests
---------------------------------------------
Add a :code:`project.mk` file into your project folder if it doesn't already
exist. Within it, add your source and test source files like so:

  .. code-block:: make

    USER_TESTS += test/servo_controller_test.cpp
    USER_TESTS += test/guidance_system_test.cpp
    # etc ...

Compiling and Running Tests
----------------------------

Use :code:`make user-test` to compile your tests.
Use :code:`make run-test` to run user tests after they have been compiled.

Finding Example Test Files
----------------------------
If you are not sure how to start unit testing, search the library folder for
**test** folders. Within them should be test files that you can examine in order
to get an idea of how certain bits of code are tested.
