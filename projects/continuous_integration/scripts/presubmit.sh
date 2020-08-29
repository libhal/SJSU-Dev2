#!/bin/bash

BUILD_CAPTURE=0
LINT_CAPTURE=0
TIDY_CAPTURE=0
TEST_CAPTURE=0

# Get base path
COMMON_SCRIPT_DIRECTORY=$(dirname "$0")
SJBASE=$(cd "${COMMON_SCRIPT_DIRECTORY}/../../../" ; pwd -P)

. ${COMMON_SCRIPT_DIRECTORY}/common.sh

function check
{
  if [ $1 -ne 0 ]
    then
    lint="$(print_status $LINT_CAPTURE)"
    tidy="$(print_status $TIDY_CAPTURE)"
    test="$(print_status $TEST_CAPTURE)"
    build="$(print_status $BUILD_CAPTURE)"
    printf "\e[0;31m ================================ \e[0m\n"
    printf "\e[1;31m|        None of this!           |\e[0m\n"
    printf "\e[1;31m|                                |\e[0m\n"
    printf "\e[1;31m|        (╯°□ °)╯︵ ┻━┻          |\e[0m\n"
    printf "\e[1;31m|                                |\e[0m\n"
    printf "\e[1;31m|      Don't even PUSH!          |\e[0m\n"
    printf "\e[0;31m ================================ \e[0m\n"
    printf "\e[0;31m|                                |\e[0m\n"
    printf "\e[0;31m| Code must be lint free ..... %b |\e[0m\n" $lint
    printf "\e[0;31m| Code must be tidy .......... %b |\e[0m\n" $tidy
    printf "\e[0;31m| Tests must pass ............ %b |\e[0m\n" $test
    printf "\e[0;31m| Code must build ............ %b |\e[0m\n" $build
    printf "\e[0;31m|                                |\e[0m\n"
    printf "\e[0;31m ================================ \e[0m\n"
    exit 1
  else
    printf "\e[0;32m ============================ \e[0m\n"
    printf "\e[0;32m| Everything looks good here |\e[0m\n"
    printf "\e[0;32m|                            |\e[0m\n"
    printf "\e[0;32m|           (•_•)            |\e[0m\r"
    sleep .5
    printf "\e[0;32m|           ( •_•)>⌐▪-▪      |\e[0m\r"
    sleep .5
    printf "\e[0;32m|           (⌐▪_▪)           |\e[0m\n"
    sleep .5
    printf "\e[0;32m|                            |\e[0m\n"
    printf "\e[0;32m|       You may commit       |\e[0m\n"
    printf "\e[0;32m ============================ \e[0m\n"
    exit 0
  fi
}

# Return to home project
cd $SJBASE/projects/continuous_integration

# ==============================================================================
# Lint Check
# ==============================================================================
print_divider "Executing 'lint' check"

time make -s lint 1> /dev/null
LINT_CAPTURE=$?
print_status $LINT_CAPTURE
echo ""

# ==============================================================================
# Clang Tidy Check
# ==============================================================================
print_divider "Executing 'commit-tidy' check"

time make -s commit-tidy -j4
TIDY_CAPTURE=$?
print_status $TIDY_CAPTURE
echo ""

# ==============================================================================
# Unit Test Check
# ==============================================================================
print_divider "Building and running unit tests"

time make -s test -j4 WARNING_BECOME_ERRORS=-Werror
TEST_CAPTURE=$?
print_status $TEST_CAPTURE
echo ""

# ==============================================================================
# Build All Projects
# ==============================================================================
time make --no-print-directory all-projects
BUILD_CAPTURE=$?

# Check if there were any errors. For this to succeed, this value should be 0
check $(($BUILD_CAPTURE+$LINT_CAPTURE+$TIDY_CAPTURE+$TEST_CAPTURE))
