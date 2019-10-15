# Presubmit Checks

Before you can merge anything to SJSU-Dev2 repo, you need to preform a
few presubmit checks.

The presubmit checks will build, test, and lint the code, as well as
check to make sure that the current commit stage is clean. The following
sections will go into detail about these processes and wha thtey do.

## Building & Testing

The purpose of this action for presubmit is to verify that any changes
made to the repository doesn't break the project's ability to build and
test software. Even small changes can break the whole system.

## Linting

Linting code is the process of running a program, in this case we use
Google's `cpplint.py` script, to evaluate each source file to make sure
that they conform to our coding style standards as well as do some
rudamentory checks for code pratices that may be dangerous or produce
bugs.

Cpplint follows the [Google C++ style
guide](https://google.github.io/styleguide/cppguide.html). The google
cpplint repository can be found
[here](https://github.com/google/styleguide).

To lint the source code of your project as well as the SJSU-Dev2
libraries, in your project directory, run `make lint`. If there are any
errors found you should go in and fix them yourself or you can use
`clang-format` to do much of the formatting work for you.

### Clang-format

Clang-format is a code formatting tool that will refactor your code for
you. Clang-format is installed by the setup script in SJSU-Dev2. If the
clang-format command is not present on your machine, try pulling the
latest master branch and try running the setup script again.

To refactor your code with clang-format, all you have to do is run the
following: `clang-format -i <path/to/file>`. The `-i` flag means you
want to do an in place refactor. If you want to run this command, but
you don't want this to change your file, you can omit the `-i` flag and
it will dump the refactored text to stdout. You can use io redirects to
dump to a file and review if the changes are to your liking.

## Documentation

The code base will be evaluated for missing documentation or incorrect
documentation syntax. This test does not appear in the presubmit checks, as it
would require that `setup.sh` install doxygen on every system. If you install
doxygen, then you can run this by running `tools/generate_doxygen_docs.sh`.
The program will list all of the documentation errors at the end of the program.

## Running presubmit checks

To run the whole set of presubmit checks, change into your project
directory and run `make presubmit`. If you see this:

``` console
============================
| Everything looks good here |
|                            |
|           (⌐▪_▪)           |
|                            |
|      You may commit        |
============================
```

Then you passed.

If you see this:

``` console
============================
|        None of this!       |
|                            |
|        (╯°□ °)╯︵ ┻━┻      |
|                            |
|      Don't commit this     |
============================
|                            |
| Code style must conform ✔  |
|   Commit must be clean ✔   |
|     Tests must pass ✘      |
|     Code must build ✔      |
|                            |
============================
```

Then it failed and each **X** represents an area that you need to fix.