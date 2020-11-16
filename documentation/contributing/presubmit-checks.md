# Presubmit Checks

Before anything is merged into SJSU-Dev2, various health and system checks are
preformed. These are called "presubmit" checks.

The presubmit checks will build, test, check the style of the code in order to
prevent regressions and keep the code clean and consistent in an automated way.

## Running presubmit checks

To run the whole set of presubmit checks, you MUST be in the
`SJSU-Dev2/projects/continuous_integration` project. Then from there, run
`make presubmit`. If you see this:

```console
============================
| Everything looks good here |
|                            |
|           (⌐▪_▪)           |
|                            |
|      You may commit        |
============================
```

Then you passed.

If you see something like this:

```console
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

## Project Building

Build all projects and verify that any changes to the code do not break projects
at compile time. This cannot verify if the project works as expected.

## Unit Testing

Build and run every unit test in SJSU-Dev2 to verify that the logic and behavior
of each library works as intended and has not been broken by a change in the
code.

## Linting

Linting code is the process of running a program, in this case we use
Google's `cpplint.py` script, to evaluate each source file to make sure
that they conform to our coding style standards as well as do some
rudimentary checks for code practices that may be dangerous or produce
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
