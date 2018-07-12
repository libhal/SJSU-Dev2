Unit Testing
=============

Unit Testing Tools
-------------------
SJTwo uses Catch2, Fakeit and FFF (fake function framework).


Compiling and Running Tests
----------------------------

Make sure you are in a project directory and run :code:`make test` to compile,
run, evaluate using valgrind and produce code coverage for your project and
the library files.

Created additional unit tests
------------------------------
Simply end the name of your file :code:`<insert title here>_test.cpp` and it
will be included in the code testing. Do not name a file that is not meant to be
a test :code:`_test.cpp`.

.. error::

	DO NOT IMPLEMENT a main function anywhere in your tests. This will cause
	linking issues since main is already defined in the
	:code:`library/L5_Testing/main_test.cpp` file.
